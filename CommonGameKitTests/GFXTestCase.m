/*
 *  Copyright (c) 2020, Stefan Johnson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list
 *     of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

@import Cocoa;

#import "GFXTestCase.h"
#import "GFX.h"
#import "Expression.h"
#import "AssetManager.h"
#import "Callbacks.h"
#import <CommonObjc/Common.h>

@implementation GFXTestCase

+(Class) lastGFXTestCase: (XCTestSuite*)suite
{
    Class Last = Nil;
    for (XCTest *Test in suite.tests)
    {
        if ([Test isKindOfClass: [self class]])
        {
            return [Test class];
        }
        
        else if ([Test isKindOfClass: [XCTestSuite class]])
        {
            Class Cls = [self lastGFXTestCase: (XCTestSuite*)Test];
            if (Cls) Last = Cls;
        }
    }
    
    return Last;
}

+(XCTestSuite*) defaultTestSuite
{
    XCTestSuite *Suite = [XCTestSuite testSuiteForTestCaseClass: [self class]];
    
    [Suite addTest: [self testCaseWithSelector: @selector(compareResults)]];
    
    return Suite;
}

static NSMutableDictionary<NSString *, NSMutableDictionary<NSString *, CCGenericContainer *> *> *Results = nil;
+(void) setUp
{
    [super setUp];
    
    if (!Results) Results = [NSMutableDictionary new];
    
    if ([self class] != [GFXTestCase class]) Results[NSStringFromClass([self class])] = [NSMutableDictionary new];
}

-(void) setUp
{
    [super setUp];
    
    GFXShaderLibraryForName = CCAssetManagerCreateShaderLibrary;
    CCTimestamp = CACurrentMediaTime;
    
    FSPath Path = FSPathCreate([[[[[NSBundle bundleWithIdentifier: @"io.scrimpycat.CommonGameKit"] resourceURL] URLByAppendingPathComponent: @"assets/shaders/main.gfxlib" isDirectory: NO] path] UTF8String]);
    CCExpression Lib = CCExpressionCreateFromSourceFile(Path);
    CCExpressionEvaluate(Lib);
    FSPathDestroy(Path);
    
    Path = FSPathCreate([[[[[NSBundle bundleWithIdentifier: @"io.scrimpycat.CommonGameKit"] resourceURL] URLByAppendingPathComponent: @"assets/shaders/main.asset" isDirectory: NO] path] UTF8String]);
    CCExpression Asset = CCExpressionCreateFromSourceFile(Path);
    CCExpressionEvaluate(Asset);
    FSPathDestroy(Path);
}

-(void) tearDown
{
    CCAssetManagerDeregisterAllTexture();
    CCAssetManagerDeregisterAllTextureStream();
    CCAssetManagerDeregisterAllShader();
    CCAssetManagerDeregisterAllShaderLibrary();
    
    [super tearDown];
}

-(void) performTest: (XCTestRun*)run
{
    if ([self class] != [GFXTestCase class]) [super performTest: run];
}

typedef struct {
    CCString name;
    GFXBufferFormat format;
    _Bool nonNeg; //only set if signed
    _Bool small;
} ShaderArgument;

void SetScaledValue(uint8_t *Data, GFXBufferFormat Format, size_t Offset, size_t Factor, _Bool NonNeg, _Bool Small)
{
    for (size_t Loop = 0, Count = GFXBufferFormatGetElementCount(Format); Loop < Count; Loop++)
    {
        const size_t Bits = GFXBufferFormatGetBitSize(Format);
        uint64_t Value = Factor + Loop + Offset;
        for (size_t Scale = 2; Scale < 20; Scale++) Value = (Value * ((Factor / Scale) + 1)) + (Value / (Scale + 30));
        
        if (NonNeg) Value &= CCBitSet(Bits - 1);
        
        if (GFXBufferFormatIsFloat(Format))
        {
            const uint64_t MSB = CCBitHighestSet(CCBitSet(Bits));
            if ((Value & (MSB >> 1)) && (!Small))
            {
                Value &= ~(MSB >> 2);
                Value &= ~(MSB >> 3);
                Value &= ~(MSB >> 4);
                Value &= ~(MSB >> 5);
            }
            
            else
            {
                Value &= ~(MSB >> 1);
                Value |= MSB >> 2;
                Value |= MSB >> 3;
                Value |= MSB >> 4;
            }
        }
        
        else if (Small)
        {
            Value &= 0x0f;
        }
        
        for (size_t Byte = 0, ByteCount = Bits / 8; Byte < ByteCount; Byte++) Data[(ByteCount * Loop) + Byte] = ((uint8_t*)&Value)[Byte];
    }
}

-(CCPixelData) get2DResultForShader: (CCString)shader WithVertexArg: (ShaderArgument*)vArg Count: (size_t)vCount FragArg: (ShaderArgument*)fArg Count: (size_t)fCount Factor: (size_t)factor Blending: (GFXBlend)blending
{
    size_t Width = 100, Height = 100;
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, Width, Height, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 0.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(shader);
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, Width, 0.0f, Height, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    for (size_t Loop = 0; Loop < fCount; Loop++)
    {
        const GFXBufferFormat Format = fArg[Loop].format;
        const size_t Size = GFXBufferFormatGetSize(Format);
        uint8_t Data[Size];
        SetScaledValue(Data, Format, Loop & 1, factor, fArg[Loop].nonNeg, fArg[Loop].small);
        
        GFXBuffer Buffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData, Size, Data);
        GFXDrawSetBuffer(Draw, fArg[Loop].name, Buffer);
        GFXBufferDestroy(Buffer);
    }
    
    size_t BufferVertexSize = 0;
    for (size_t Loop = 0; Loop < vCount; Loop++) BufferVertexSize += GFXBufferFormatGetSize(vArg[Loop].format);
    
    uint8_t BufferData[BufferVertexSize * 4];
    for (size_t Vert = 0; Vert < 4; Vert++)
    {
        uint8_t *Data = &BufferData[BufferVertexSize * Vert];
        for (size_t Loop = 0; Loop < vCount; Loop++)
        {
            const GFXBufferFormat Format = vArg[Loop].format;
            const size_t Size = GFXBufferFormatGetSize(Format);
            
            SetScaledValue(Data, Format, (Loop & 1) + Vert, factor, vArg[Loop].nonNeg, vArg[Loop].small);
            
            Data += Size;
        }
    }
    
    GFXBuffer Buffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, BufferVertexSize * 4, BufferData);
    for (size_t Loop = 0, Offset = 0; Loop < vCount; Loop++)
    {
        const GFXBufferFormat Format = vArg[Loop].format;
        GFXDrawSetVertexBuffer(Draw, vArg[Loop].name, Buffer, Format, BufferVertexSize, Offset);
        
        Offset += GFXBufferFormatGetSize(Format);
    }
    GFXBufferDestroy(Buffer);
    
    GFXDrawSetBlending(Draw, blending);
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, Width, Height, 1);
    
    GFXFramebufferDestroy(Framebuffer);
    
    return Pixels;
}

static struct {
    CCString name;
    ShaderArgument vArgs[16];
    ShaderArgument fArgs[16];
} Shaders[] = {
    {
        .name = CC_STRING("vertex-colour"),
        .vArgs = {
            { .name = CC_STRING("vPosition"),   .format = GFXBufferFormatFloat32x2,     .nonNeg = TRUE,     .small = FALSE },
            { .name = CC_STRING("vColour"),     .format = GFXBufferFormatFloat32x4,     .nonNeg = TRUE,     .small = TRUE }
        }
    },
    {
        .name = CC_STRING("fragment-colour"),
        .vArgs = {
            { .name = CC_STRING("vPosition"),   .format = GFXBufferFormatFloat32x2,     .nonNeg = TRUE,     .small = FALSE }
        },
        .fArgs = {
            { .name = CC_STRING("colour"),      .format = GFXBufferFormatFloat32x4,     .nonNeg = TRUE,     .small = TRUE }
        }
    },
    {
        .name = CC_STRING("rounded-rect"),
        .vArgs = {
            { .name = CC_STRING("vPosition"),   .format = GFXBufferFormatFloat32x2,     .nonNeg = TRUE,     .small = FALSE },
            { .name = CC_STRING("vColour"),     .format = GFXBufferFormatFloat32x4,     .nonNeg = TRUE,     .small = TRUE },
            { .name = CC_STRING("vCoord"),      .format = GFXBufferFormatFloat32x2,     .nonNeg = TRUE,     .small = TRUE }
        },
        .fArgs = {
            { .name = CC_STRING("radius"),      .format = GFXBufferFormatFloat32,       .nonNeg = TRUE,     .small = TRUE },
            { .name = CC_STRING("scale"),       .format = GFXBufferFormatFloat32x2,     .nonNeg = TRUE,     .small = TRUE }
        }
    }
};

-(void) compareResults
{
    NSString *Impl = NSStringFromClass([self class]);
    NSMutableDictionary<NSString *, CCGenericContainer *> *Store = Results[Impl];
    
    for (size_t Loop = 0; Loop < sizeof(Shaders) / sizeof(typeof(*Shaders)); Loop++)
    {
        size_t VertCount = 0, FragCount = 0;
        while ((Shaders[Loop].vArgs[VertCount].name) && (++VertCount < 16));
        while ((Shaders[Loop].fArgs[FragCount].name) && (++FragCount < 16));
        
        const GFXBlend BlendModes[] = {
            GFXBlendOpaque,
            GFXBlendTransparent,
            GFXBlendTransparentPremultiplied
        };
        
        for (size_t Loop2 = 0; Loop2 < 40 * sizeof(BlendModes) / sizeof(typeof(*BlendModes)); Loop2++)
        {
            NSString *Key = nil;
            CC_STRING_TEMP_BUFFER(Name, Shaders[Loop].name,
                                  Store[Key] = [CCGenericContainer containerWithCopiedData: &(CCPixelData){ NULL } OfSize: sizeof(CCPixelData)];
                                  continue;
                                  ) Key = [NSString stringWithFormat: @"%s.%zu", Name, Loop2];
            
            CCPixelData Pixels = [self get2DResultForShader: Shaders[Loop].name
                                              WithVertexArg: Shaders[Loop].vArgs
                                                      Count: VertCount
                                                    FragArg: Shaders[Loop].fArgs
                                                      Count: FragCount
                                                     Factor: (Loop2 % 40) + 10
                                                   Blending: BlendModes[Loop2 / 40]];
            
            Store[Key] = [CCGenericContainer containerWithData: Pixels
                                                        OfSize: sizeof(Pixels)
                                                 ReleaserBlock: ^(void *Data, size_t Size, bool IsCopied){ CCPixelDataDestroy(Data); }];
        }
    }
    
    if ([GFXTestCase lastGFXTestCase: [XCTestSuite defaultTestSuite]] == [self class])
    {
        for (NSString *Program in Store)
        {
            CCPixelData A = Store[Program].data;
            if (A)
            {
                size_t WidthA, HeightA, DepthA;
                CCPixelDataGetSize(A, &WidthA, &HeightA, &DepthA);
                const size_t SizeA = WidthA * HeightA * DepthA * CCColourFormatSampleSizeForPlanar(A->format, CCColourFormatChannelPlanarIndex0);
                
                for (NSString *Impl in Results)
                {
                    if (![Impl isEqualToString: NSStringFromClass([self class])])
                    {
                        CCPixelData B = Results[Impl][Program].data;
                        if (B)
                        {
                            size_t WidthB, HeightB, DepthB;
                            CCPixelDataGetSize(A, &WidthB, &HeightB, &DepthB);
                            const size_t SizeB = WidthB * HeightB * DepthB * CCColourFormatSampleSizeForPlanar(B->format, CCColourFormatChannelPlanarIndex0);
                            
                            XCTAssertEqual(A->format, B->format, @"Pixel formats should be the same (%@.%@ : %@.%@)", NSStringFromClass([self class]), Program, Impl, Program);
                            XCTAssertEqual(SizeA, SizeB, @"Pixel data should be the same (%@.%@ : %@.%@)", NSStringFromClass([self class]), Program, Impl, Program);
                            
                            size_t ErrorCount = 0, AccumPrecisionError = 0;
                            for (size_t y = 0; y < HeightA; y++)
                            {
                                for (size_t x = 0; x < WidthA; x++)
                                {
                                    CCColour ColourA = CCPixelDataGetColour(A, x, y, 0);
                                    CCColour ColourB = CCPixelDataGetColour(B, x, y, 0);
                                    
                                    size_t Diff = 0;
                                    Diff += ColourA.channel[0].u8 > ColourB.channel[0].u8 ? ColourA.channel[0].u8 - ColourB.channel[0].u8 : ColourB.channel[0].u8 - ColourA.channel[0].u8;
                                    Diff += ColourA.channel[1].u8 > ColourB.channel[1].u8 ? ColourA.channel[1].u8 - ColourB.channel[1].u8 : ColourB.channel[1].u8 - ColourA.channel[1].u8;
                                    Diff += ColourA.channel[2].u8 > ColourB.channel[2].u8 ? ColourA.channel[2].u8 - ColourB.channel[2].u8 : ColourB.channel[2].u8 - ColourA.channel[2].u8;
                                    Diff += ColourA.channel[3].u8 > ColourB.channel[3].u8 ? ColourA.channel[3].u8 - ColourB.channel[3].u8 : ColourB.channel[3].u8 - ColourA.channel[3].u8;
                                    
                                    if (Diff)
                                    {
                                        AccumPrecisionError += Diff;
                                        ErrorCount++;
                                    }
                                }
                            }
                            
                            if ((ErrorCount > 3) || (AccumPrecisionError > 5)) // Adjust these as needed
                            {
                                for (size_t y = 0; y < HeightA; y++)
                                {
                                    for (size_t x = 0; x < WidthA; x++)
                                    {
                                        CCColour ColourA = CCPixelDataGetColour(A, x, y, 0);
                                        CCColour ColourB = CCPixelDataGetColour(B, x, y, 0);
                                        
                                        XCTAssertEqual(ColourA.channel[0].u8, ColourB.channel[0].u8, @"Pixel data red channel should be the same (%@.%@ : %@.%@) @ (%zu, %zu)", NSStringFromClass([self class]), Program, Impl, Program, x, y);
                                        XCTAssertEqual(ColourA.channel[1].u8, ColourB.channel[1].u8, @"Pixel data green channel should be the same (%@.%@ : %@.%@) @ (%zu, %zu)", NSStringFromClass([self class]), Program, Impl, Program, x, y);
                                        XCTAssertEqual(ColourA.channel[2].u8, ColourB.channel[2].u8, @"Pixel data blue channel should be the same (%@.%@ : %@.%@) @ (%zu, %zu)", NSStringFromClass([self class]), Program, Impl, Program, x, y);
                                        XCTAssertEqual(ColourA.channel[3].u8, ColourB.channel[3].u8, @"Pixel data alpha channel should be the same (%@.%@ : %@.%@) @ (%zu, %zu)", NSStringFromClass([self class]), Program, Impl, Program, x, y);
                                    }
                                }
                            }
                        }
                        
                        else XCTAssert(0, @"No pixel data available for %@.%@", Impl, Program);
                    }
                }
            }
            
            else XCTAssert(0, @"No pixel data available for %@.%@", Impl, Program);
        }
        
        Results = nil;
    }
}

-(void) testVertexColour
{
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, 4, 4, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 1.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("vertex-colour"));
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, 4.0f, 0.0f, 4.0f, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    struct {
        CCVector2D position;
        CCColourRGBA colour;
    } Data[4] = {
        { CCVector2DMake(2, 2), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f) },
        { CCVector2DMake(4, 2), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f) },
        { CCVector2DMake(2, 4), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f) },
        { CCVector2DMake(4, 4), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f) }
    };
    GFXBuffer Rect = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, sizeof(Data), Data);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vPosition"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), 0);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColour"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), colour));
    GFXBufferDestroy(Rect);
    
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, 4, 4, 1);
    for (size_t Y = 0; Y < 2; Y++)
    {
        for (size_t X = 0; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 255);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, 255);
        }
    }
    
    for (size_t Y = 2; Y < 4; Y++)
    {
        for (size_t X = 0; X < 2; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 255);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, 255);
        }
    }
    
    for (size_t Y = 2; Y < 4; Y++)
    {
        for (size_t X = 2; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 255);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, 255);
        }
    }
    
    
    GFXFramebufferDestroy(Framebuffer);
}

-(void) testFragmentColour
{
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, 4, 4, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 1.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("fragment-colour"));
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, 4.0f, 0.0f, 4.0f, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    GFXBuffer Colour = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCColourRGBA), &(CCColourRGBA){ 0.0f, 0.0f, 1.0f, 1.0f });
    GFXDrawSetBuffer(Draw, CC_STRING("colour"), Colour);
    GFXBufferDestroy(Colour);
    
    GFXBuffer Rect = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, sizeof(CCVector2D) * 4, (CCVector2D[4]){
        CCVector2DMake(2, 2),
        CCVector2DMake(4, 2),
        CCVector2DMake(2, 4),
        CCVector2DMake(4, 4)
    });
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vPosition"), Rect, GFXBufferFormatFloat32x2, sizeof(CCVector2D), 0);
    GFXBufferDestroy(Rect);
    
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, 4, 4, 1);
    for (size_t Y = 0; Y < 2; Y++)
    {
        for (size_t X = 0; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 255);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, 255);
        }
    }
    
    for (size_t Y = 2; Y < 4; Y++)
    {
        for (size_t X = 0; X < 2; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 255);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, 255);
        }
    }
    
    for (size_t Y = 2; Y < 4; Y++)
    {
        for (size_t X = 2; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 255);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, 255);
        }
    }
    
    
    GFXFramebufferDestroy(Framebuffer);
}

-(void) testRoundedRect
{
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, 4, 4, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 1.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("rounded-rect"));
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, 4.0f, 0.0f, 4.0f, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    GFXBuffer Radius = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(float), &(float){ 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("radius"), Radius);
    GFXBufferDestroy(Radius);
    
    GFXBuffer Scale = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.5f, 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("scale"), Scale);
    GFXBufferDestroy(Scale);
    
    struct {
        CCVector2D position;
        CCColourRGBA colour;
        CCVector2D coord;
    } Data[4] = {
        { CCVector2DMake(0.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.0f, 0.0f) },
        { CCVector2DMake(4.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(1.0f, 0.0f) },
        { CCVector2DMake(0.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.0f, 1.0f) },
        { CCVector2DMake(4.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(1.0f, 1.0f) }
    };
    GFXBuffer Rect = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, sizeof(Data), Data);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vPosition"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), 0);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColour"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), colour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vCoord"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), offsetof(typeof(*Data), coord));
    GFXBufferDestroy(Rect);
    
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    uint32_t Match[4][4] = {
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00000000, 0x0000ffff, 0x0000ffff, 0x00000000 },
        { 0x00000000, 0x0000ffff, 0x0000ffff, 0x00000000 },
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
    };
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, 4, 4, 1);
    for (size_t Y = 0; Y < 4; Y++)
    {
        for (size_t X = 0; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, (Match[Y][X] >> 24) & 0xff, @"Incorrect red channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, (Match[Y][X] >> 16) & 0xff, @"Incorrect green channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, (Match[Y][X] >> 8) & 0xff, @"Incorrect blue channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, (Match[Y][X] >> 0) & 0xff, @"Incorrect alpha channel for pixel (%zu, %zu)", X, Y);
        }
    }
    
    GFXFramebufferDestroy(Framebuffer);
}

-(void) testRoundedRectWithBlending
{
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, 4, 4, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 1.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("rounded-rect"));
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, 4.0f, 0.0f, 4.0f, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    GFXBuffer Radius = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(float), &(float){ 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("radius"), Radius);
    GFXBufferDestroy(Radius);
    
    GFXBuffer Scale = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.5f, 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("scale"), Scale);
    GFXBufferDestroy(Scale);
    
    struct {
        CCVector2D position;
        CCColourRGBA colour;
        CCVector2D coord;
    } Data[4] = {
        { CCVector2DMake(0.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.0f, 0.0f) },
        { CCVector2DMake(4.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(1.0f, 0.0f) },
        { CCVector2DMake(0.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.0f, 1.0f) },
        { CCVector2DMake(4.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(1.0f, 1.0f) }
    };
    GFXBuffer Rect = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, sizeof(Data), Data);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vPosition"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), 0);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColour"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), colour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vCoord"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), offsetof(typeof(*Data), coord));
    GFXBufferDestroy(Rect);
    
    GFXDrawSetBlending(Draw, GFXBlendTransparentPremultiplied);
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    uint32_t Match[4][4] = {
        { 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff },
        { 0xff0000ff, 0x0000ffff, 0x0000ffff, 0xff0000ff },
        { 0xff0000ff, 0x0000ffff, 0x0000ffff, 0xff0000ff },
        { 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff }
    };
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, 4, 4, 1);
    for (size_t Y = 0; Y < 4; Y++)
    {
        for (size_t X = 0; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, (Match[Y][X] >> 24) & 0xff, @"Incorrect red channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, (Match[Y][X] >> 16) & 0xff, @"Incorrect green channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, (Match[Y][X] >> 8) & 0xff, @"Incorrect blue channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, (Match[Y][X] >> 0) & 0xff, @"Incorrect alpha channel for pixel (%zu, %zu)", X, Y);
        }
    }
    
    GFXFramebufferDestroy(Framebuffer);
}

-(void) testRoundedRectCurve
{
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, 4, 4, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 1.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("rounded-rect"));
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, 4.0f, 0.0f, 4.0f, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    GFXBuffer Radius = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(float), &(float){ 0.25f });
    GFXDrawSetBuffer(Draw, CC_STRING("radius"), Radius);
    GFXBufferDestroy(Radius);
    
    GFXBuffer Scale = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.5f, 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("scale"), Scale);
    GFXBufferDestroy(Scale);
    
    struct {
        CCVector2D position;
        CCColourRGBA colour;
        CCVector2D coord;
    } Data[4] = {
        { CCVector2DMake(0.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.0f, 0.0f) },
        { CCVector2DMake(4.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.5f, 0.0f) },
        { CCVector2DMake(0.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.0f, 0.5f) },
        { CCVector2DMake(4.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.5f, 0.5f) }
    };
    GFXBuffer Rect = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, sizeof(Data), Data);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vPosition"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), 0);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColour"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), colour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vCoord"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), offsetof(typeof(*Data), coord));
    GFXBufferDestroy(Rect);
    
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    uint32_t Match[4][4] = {
        { 0x00000000, 0x00000000, 0x0000ffff, 0x0000ffff },
        { 0x00000000, 0x0000ffff, 0x0000ffff, 0x0000ffff },
        { 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff },
        { 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff }
    };
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, 4, 4, 1);
    for (size_t Y = 0; Y < 4; Y++)
    {
        for (size_t X = 0; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, (Match[Y][X] >> 24) & 0xff, @"Incorrect red channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, (Match[Y][X] >> 16) & 0xff, @"Incorrect green channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, (Match[Y][X] >> 8) & 0xff, @"Incorrect blue channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, (Match[Y][X] >> 0) & 0xff, @"Incorrect alpha channel for pixel (%zu, %zu)", X, Y);
        }
    }
    
    GFXFramebufferDestroy(Framebuffer);
}

-(void) testRoundedRectCurveWithBlending
{
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, 4, 4, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 1.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("rounded-rect"));
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, 4.0f, 0.0f, 4.0f, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    GFXBuffer Radius = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(float), &(float){ 0.25f });
    GFXDrawSetBuffer(Draw, CC_STRING("radius"), Radius);
    GFXBufferDestroy(Radius);
    
    GFXBuffer Scale = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.5f, 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("scale"), Scale);
    GFXBufferDestroy(Scale);
    
    struct {
        CCVector2D position;
        CCColourRGBA colour;
        CCVector2D coord;
    } Data[4] = {
        { CCVector2DMake(0.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.0f, 0.0f) },
        { CCVector2DMake(4.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.5f, 0.0f) },
        { CCVector2DMake(0.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.0f, 0.5f) },
        { CCVector2DMake(4.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector2DMake(0.5f, 0.5f) }
    };
    GFXBuffer Rect = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, sizeof(Data), Data);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vPosition"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), 0);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColour"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), colour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vCoord"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), offsetof(typeof(*Data), coord));
    GFXBufferDestroy(Rect);
    
    GFXDrawSetBlending(Draw, GFXBlendTransparentPremultiplied);
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    uint32_t Match[4][4] = {
        { 0xff0000ff, 0xff0000ff, 0x0000ffff, 0x0000ffff },
        { 0xff0000ff, 0x0000ffff, 0x0000ffff, 0x0000ffff },
        { 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff },
        { 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff }
    };
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, 4, 4, 1);
    for (size_t Y = 0; Y < 4; Y++)
    {
        for (size_t X = 0; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, (Match[Y][X] >> 24) & 0xff, @"Incorrect red channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, (Match[Y][X] >> 16) & 0xff, @"Incorrect green channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, (Match[Y][X] >> 8) & 0xff, @"Incorrect blue channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, (Match[Y][X] >> 0) & 0xff, @"Incorrect alpha channel for pixel (%zu, %zu)", X, Y);
        }
    }
    
    GFXFramebufferDestroy(Framebuffer);
}

-(void) testOutlineRoundedRect
{
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, 4, 4, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 1.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("outline-rounded-rect"));
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, 4.0f, 0.0f, 4.0f, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    GFXBuffer Radius = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(float), &(float){ 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("radius"), Radius);
    GFXBufferDestroy(Radius);
    
    GFXBuffer Scale = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.5f, 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("scale"), Scale);
    GFXBufferDestroy(Scale);
    
    GFXBuffer Outline = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.1f, 0.1f });
    GFXDrawSetBuffer(Draw, CC_STRING("outline"), Outline);
    GFXBufferDestroy(Outline);
    
    struct {
        CCVector2D position;
        CCColourRGBA colour;
        CCColourRGBA outlineColour;
        CCVector2D coord;
    } Data[4] = {
        { CCVector2DMake(0.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.0f, 0.0f) },
        { CCVector2DMake(4.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(1.0f, 0.0f) },
        { CCVector2DMake(0.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.0f, 1.0f) },
        { CCVector2DMake(4.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(1.0f, 1.0f) }
    };
    GFXBuffer Rect = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, sizeof(Data), Data);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vPosition"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), 0);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColour"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), colour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColourOutline"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), outlineColour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vCoord"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), offsetof(typeof(*Data), coord));
    GFXBufferDestroy(Rect);
    
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    uint32_t Match[4][4] = {
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00000000, 0x00800080, 0x00800080, 0x00000000 },
        { 0x00000000, 0x00800080, 0x00800080, 0x00000000 },
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
    };
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, 4, 4, 1);
    for (size_t Y = 0; Y < 4; Y++)
    {
        for (size_t X = 0; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, (Match[Y][X] >> 24) & 0xff, @"Incorrect red channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, (Match[Y][X] >> 16) & 0xff, @"Incorrect green channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, (Match[Y][X] >> 8) & 0xff, @"Incorrect blue channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, (Match[Y][X] >> 0) & 0xff, @"Incorrect alpha channel for pixel (%zu, %zu)", X, Y);
        }
    }
    
    GFXFramebufferDestroy(Framebuffer);
}

-(void) testOutlineRoundedRectWithBlending
{
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, 4, 4, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 1.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("outline-rounded-rect"));
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, 4.0f, 0.0f, 4.0f, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    GFXBuffer Radius = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(float), &(float){ 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("radius"), Radius);
    GFXBufferDestroy(Radius);
    
    GFXBuffer Scale = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.5f, 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("scale"), Scale);
    GFXBufferDestroy(Scale);
    
    GFXBuffer Outline = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.1f, 0.1f });
    GFXDrawSetBuffer(Draw, CC_STRING("outline"), Outline);
    GFXBufferDestroy(Outline);
    
    struct {
        CCVector2D position;
        CCColourRGBA colour;
        CCColourRGBA outlineColour;
        CCVector2D coord;
    } Data[4] = {
        { CCVector2DMake(0.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.0f, 0.0f) },
        { CCVector2DMake(4.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(1.0f, 0.0f) },
        { CCVector2DMake(0.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.0f, 1.0f) },
        { CCVector2DMake(4.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(1.0f, 1.0f) }
    };
    GFXBuffer Rect = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, sizeof(Data), Data);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vPosition"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), 0);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColour"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), colour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColourOutline"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), outlineColour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vCoord"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), offsetof(typeof(*Data), coord));
    GFXBufferDestroy(Rect);
    
    GFXDrawSetBlending(Draw, GFXBlendTransparentPremultiplied);
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    uint32_t Match[4][4] = {
        { 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff },
        { 0xff0000ff, 0x808000ff, 0x808000ff, 0xff0000ff },
        { 0xff0000ff, 0x808000ff, 0x808000ff, 0xff0000ff },
        { 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff }
    };
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, 4, 4, 1);
    for (size_t Y = 0; Y < 4; Y++)
    {
        for (size_t X = 0; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, (Match[Y][X] >> 24) & 0xff, @"Incorrect red channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, (Match[Y][X] >> 16) & 0xff, @"Incorrect green channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, (Match[Y][X] >> 8) & 0xff, @"Incorrect blue channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, (Match[Y][X] >> 0) & 0xff, @"Incorrect alpha channel for pixel (%zu, %zu)", X, Y);
        }
    }
    
    GFXFramebufferDestroy(Framebuffer);
}

-(void) testOutlineRoundedRectCurve
{
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, 4, 4, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 1.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("outline-rounded-rect"));
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, 4.0f, 0.0f, 4.0f, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    GFXBuffer Radius = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(float), &(float){ 0.25f });
    GFXDrawSetBuffer(Draw, CC_STRING("radius"), Radius);
    GFXBufferDestroy(Radius);
    
    GFXBuffer Scale = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.5f, 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("scale"), Scale);
    GFXBufferDestroy(Scale);
    
    GFXBuffer Outline = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.1f, 0.1f });
    GFXDrawSetBuffer(Draw, CC_STRING("outline"), Outline);
    GFXBufferDestroy(Outline);
    
    struct {
        CCVector2D position;
        CCColourRGBA colour;
        CCColourRGBA outlineColour;
        CCVector2D coord;
    } Data[4] = {
        { CCVector2DMake(0.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.0f, 0.0f) },
        { CCVector2DMake(4.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.5f, 0.0f) },
        { CCVector2DMake(0.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.0f, 0.5f) },
        { CCVector2DMake(4.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.5f, 0.5f) }
    };
    GFXBuffer Rect = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, sizeof(Data), Data);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vPosition"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), 0);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColour"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), colour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColourOutline"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), outlineColour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vCoord"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), offsetof(typeof(*Data), coord));
    GFXBufferDestroy(Rect);
    
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    uint32_t Match[4][4] = {
        { 0x00000000, 0x00000000, 0x00800080, 0x00800080 },
        { 0x00000000, 0x00800080, 0x00640f73, 0x0000ffff },
        { 0x00800080, 0x00640f73, 0x0000ffff, 0x0000ffff },
        { 0x00800080, 0x0000ffff, 0x0000ffff, 0x0000ffff }
    };
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, 4, 4, 1);
    for (size_t Y = 0; Y < 4; Y++)
    {
        for (size_t X = 0; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, (Match[Y][X] >> 24) & 0xff, @"Incorrect red channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, (Match[Y][X] >> 16) & 0xff, @"Incorrect green channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, (Match[Y][X] >> 8) & 0xff, @"Incorrect blue channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, (Match[Y][X] >> 0) & 0xff, @"Incorrect alpha channel for pixel (%zu, %zu)", X, Y);
        }
    }
    
    GFXFramebufferDestroy(Framebuffer);
}

-(void) testOutlineRoundedRectCurveWithBlending
{
    GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | GFXTextureHintUsageColourRenderTarget | GFXTextureHintCPUReadMany | GFXTextureHintGPUReadMany | GFXTextureHintCPUWriteMany, CCColourFormatRGBA8Unorm, 4, 4, 1, NULL);
    
    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionClear, GFXFramebufferAttachmentActionStore, (CCColourRGBA){ 1.0f, 0.0f, 0.0f, 1.0f });
    GFXFramebuffer Framebuffer = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
    
    GFXCommandBuffer CommandBuffer = CCRetain(GFXCommandBufferCreate(CC_STD_ALLOCATOR));
    GFXCommandBufferRecord(CommandBuffer);
    
    GFXDraw Draw = GFXDrawCreate(CC_STD_ALLOCATOR);
    
    GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("outline-rounded-rect"));
    GFXDrawSetShader(Draw, Shader);
    GFXShaderDestroy(Shader);
    
    const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, 4.0f, 0.0f, 4.0f, -1.0f, 1.0f);
    GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
    GFXDrawSetBuffer(Draw, CC_STRING("modelViewProjectionMatrix"), Projection);
    GFXBufferDestroy(Projection);
    
    GFXBuffer Radius = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(float), &(float){ 0.25f });
    GFXDrawSetBuffer(Draw, CC_STRING("radius"), Radius);
    GFXBufferDestroy(Radius);
    
    GFXBuffer Scale = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.5f, 0.5f });
    GFXDrawSetBuffer(Draw, CC_STRING("scale"), Scale);
    GFXBufferDestroy(Scale);
    
    GFXBuffer Outline = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &(CCVector2D){ 0.1f, 0.1f });
    GFXDrawSetBuffer(Draw, CC_STRING("outline"), Outline);
    GFXBufferDestroy(Outline);
    
    struct {
        CCVector2D position;
        CCColourRGBA colour;
        CCColourRGBA outlineColour;
        CCVector2D coord;
    } Data[4] = {
        { CCVector2DMake(0.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.0f, 0.0f) },
        { CCVector2DMake(4.0f, 0.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.5f, 0.0f) },
        { CCVector2DMake(0.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.0f, 0.5f) },
        { CCVector2DMake(4.0f, 4.0f), CCVector4DMake(0.0f, 0.0f, 1.0f, 1.0f), CCVector4DMake(0.0f, 1.0f, 0.0f, 0.5f), CCVector2DMake(0.5f, 0.5f) }
    };
    GFXBuffer Rect = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex, sizeof(Data), Data);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vPosition"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), 0);
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColour"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), colour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vColourOutline"), Rect, GFXBufferFormatFloat32x4, sizeof(*Data), offsetof(typeof(*Data), outlineColour));
    GFXDrawSetVertexBuffer(Draw, CC_STRING("vCoord"), Rect, GFXBufferFormatFloat32x2, sizeof(*Data), offsetof(typeof(*Data), coord));
    GFXBufferDestroy(Rect);
    
    GFXDrawSetBlending(Draw, GFXBlendTransparentPremultiplied);
    GFXDrawSetFramebuffer(Draw, Framebuffer, 0);
    GFXDrawSubmit(Draw, GFXPrimitiveTypeTriangleStrip, 0, 4);
    
    GFXDrawDestroy(Draw);
    
    GFXCommandBufferCommit(CommandBuffer, FALSE);
    
    while (!GFXCommandBufferIsComplete(CommandBuffer)) CC_SPIN_WAIT();
    GFXCommandBufferDestroy(CommandBuffer);
    
    uint32_t Match[4][4] = {
        { 0xff0000ff, 0xff0000ff, 0x808000ff, 0x808000ff },
        { 0xff0000ff, 0x808000ff, 0x8c640fff, 0x0000ffff },
        { 0x808000ff, 0x8c640fff, 0x0000ffff, 0x0000ffff },
        { 0x808000ff, 0x0000ffff, 0x0000ffff, 0x0000ffff }
    };
    
    CCPixelData Pixels = GFXTextureRead(Texture, CC_STD_ALLOCATOR, CCColourFormatRGBA8Unorm, 0, 0, 0, 4, 4, 1);
    for (size_t Y = 0; Y < 4; Y++)
    {
        for (size_t X = 0; X < 4; X++)
        {
            CCColour Pixel = CCPixelDataGetColour(Pixels, X, Y, 0);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, (Match[Y][X] >> 24) & 0xff, @"Incorrect red channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, (Match[Y][X] >> 16) & 0xff, @"Incorrect green channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, (Match[Y][X] >> 8) & 0xff, @"Incorrect blue channel for pixel (%zu, %zu)", X, Y);
            XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelAlpha).u8, (Match[Y][X] >> 0) & 0xff, @"Incorrect alpha channel for pixel (%zu, %zu)", X, Y);
        }
    }
    
    GFXFramebufferDestroy(Framebuffer);
}

-(void) testTextureBounds
{
    GFXTexture Root = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension1D, CCColourFormatRGB8Unorm, 100, 1, 1, NULL);
    
    GFXTexture Sub = GFXTextureCreateSubTexture(CC_STD_ALLOCATOR, Root, 10, 0, 0, 5, 1, 1, NULL);
    
    GFXTexture Sub2 = GFXTextureCreateSubTexture(CC_STD_ALLOCATOR, Sub, 2, 0, 0, 2, 1, 1, NULL);
    
    CCVector3D Bottom, Top;
    GFXTextureGetBounds(Sub2, &Bottom, &Top);
    
    XCTAssertEqual(Bottom.x, 0.12f, @"Should have correct coord");
    XCTAssertEqual(Bottom.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Bottom.z, 0.0f, @"Should have correct coord");
    
    XCTAssertEqual(Top.x, 0.14f, @"Should have correct coord");
    XCTAssertEqual(Top.y, 1.0f, @"Should have correct coord");
    XCTAssertEqual(Top.z, 1.0f, @"Should have correct coord");
    
    
    GFXTextureGetBounds(Sub, &Bottom, &Top);
    
    XCTAssertEqual(Bottom.x, 0.1f, @"Should have correct coord");
    XCTAssertEqual(Bottom.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Bottom.z, 0.0f, @"Should have correct coord");
    
    XCTAssertEqual(Top.x, 0.15f, @"Should have correct coord");
    XCTAssertEqual(Top.y, 1.0f, @"Should have correct coord");
    XCTAssertEqual(Top.z, 1.0f, @"Should have correct coord");
    
    
    GFXTextureGetBounds(Root, &Bottom, &Top);
    
    XCTAssertEqual(Bottom.x, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Bottom.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Bottom.z, 0.0f, @"Should have correct coord");
    
    XCTAssertEqual(Top.x, 1.0f, @"Should have correct coord");
    XCTAssertEqual(Top.y, 1.0f, @"Should have correct coord");
    XCTAssertEqual(Top.z, 1.0f, @"Should have correct coord");
    
    GFXTextureDestroy(Sub2);
    GFXTextureDestroy(Sub);
    GFXTextureDestroy(Root);
}

-(void) testTextureCoordNormalization
{
    GFXTexture Root = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension1D, CCColourFormatRGB8Unorm, 100, 1, 1, NULL);
    
    GFXTexture Sub = GFXTextureCreateSubTexture(CC_STD_ALLOCATOR, Root, 10, 0, 0, 5, 1, 1, NULL);
    
    GFXTexture Root2 = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension1D, CCColourFormatRGB8Unorm, 50, 1, 1, NULL);
    
    CCVector3D Multiplier = GFXTextureGetMultiplier(Root), Coord = GFXTextureNormalizePoint(Root, 1, 0, 0);
    
    XCTAssertEqual(Multiplier.x, 0.01f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.y, 1.0f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.z, 1.0f, @"Should have correct multiplier");
    
    XCTAssertEqual(Coord.x, 0.01f, @"Should have correct coord");
    XCTAssertEqual(Coord.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Coord.z, 0.0f, @"Should have correct coord");
    
    
    Multiplier = GFXTextureGetMultiplier(Sub);
    Coord = GFXTextureNormalizePoint(Sub, 1, 0, 0);
    
    XCTAssertEqual(Multiplier.x, 0.01f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.y, 1.0f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.z, 1.0f, @"Should have correct multiplier");
    
    XCTAssertEqual(Coord.x, 0.11f, @"Should have correct coord");
    XCTAssertEqual(Coord.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Coord.z, 0.0f, @"Should have correct coord");
    
    
    Multiplier = GFXTextureGetMultiplier(Root2);
    Coord = GFXTextureNormalizePoint(Root2, 1, 0, 0);
    
    XCTAssertEqual(Multiplier.x, 0.02f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.y, 1.0f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.z, 1.0f, @"Should have correct multiplier");
    
    XCTAssertEqual(Coord.x, 0.02f, @"Should have correct coord");
    XCTAssertEqual(Coord.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Coord.z, 0.0f, @"Should have correct coord");
    
    GFXTextureDestroy(Root2);
    GFXTextureDestroy(Sub);
    GFXTextureDestroy(Root);
}

-(void) testStreams
{
    GFXTextureStream Stream = GFXTextureStreamCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension1D, CCColourFormatRGB8Unorm, 4, 1, 1);
    
    GFXTexture Texture1 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    GFXTexture Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 2, 1, 1, NULL);
    GFXTexture Texture3 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    GFXTexture Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    XCTAssertNotEqual(Texture1, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture3, NULL, "Should have enough room to create the texture");
    XCTAssertEqual(Texture4, NULL, "Should not have enough room to create the texture");
    
    size_t x;
    GFXTextureGetOffset(Texture1, &x, NULL, NULL);
    XCTAssertEqual(x, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture2, &x, NULL, NULL);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture3, &x, NULL, NULL);
    XCTAssertEqual(x, 3, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture2);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture4, NULL, "Should have enough room to create the texture");
    
    
    GFXTextureGetOffset(Texture2, &x, NULL, NULL);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture4, &x, NULL, NULL);
    XCTAssertEqual(x, 2, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture4);
    GFXTextureDestroy(Texture3);
    GFXTextureDestroy(Texture2);
    GFXTextureDestroy(Texture1);
    GFXTextureStreamDestroy(Stream);
    
    
    
    
    Stream = GFXTextureStreamCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D, CCColourFormatRGB8Unorm, 3, 2, 1);
    
    Texture1 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 2, 2, 1, NULL);
    Texture3 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    XCTAssertNotEqual(Texture1, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture3, NULL, "Should have enough room to create the texture");
    XCTAssertEqual(Texture4, NULL, "Should not have enough room to create the texture");
    
    size_t y;
    GFXTextureGetOffset(Texture1, &x, &y, NULL);
    XCTAssertEqual(x, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture2, &x, &y, NULL);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture3, &x, &y, NULL);
    XCTAssertEqual(x, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 1, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture2);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture4, NULL, "Should have enough room to create the texture");
    
    
    GFXTextureGetOffset(Texture2, &x, &y, NULL);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture4, &x, &y, NULL);
    XCTAssertEqual(x, 2, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture4);
    GFXTextureDestroy(Texture3);
    GFXTextureDestroy(Texture2);
    GFXTextureDestroy(Texture1);
    GFXTextureStreamDestroy(Stream);
    
    
    
    
    Stream = GFXTextureStreamCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension3D, CCColourFormatRGB8Unorm, 3, 2, 2);
    
    Texture1 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 2, NULL);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 2, 2, 2, NULL);
    Texture3 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 2, NULL);
    Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    XCTAssertNotEqual(Texture1, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture3, NULL, "Should have enough room to create the texture");
    XCTAssertEqual(Texture4, NULL, "Should not have enough room to create the texture");
    
    size_t z;
    GFXTextureGetOffset(Texture1, &x, &y, &z);
    XCTAssertEqual(x, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(z, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture2, &x, &y, &z);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(z, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture3, &x, &y, &z);
    XCTAssertEqual(x, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(z, 0, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture2);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 2, 2, 1, NULL);
    
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture4, NULL, "Should have enough room to create the texture");
    
    
    GFXTextureGetOffset(Texture2, &x, &y, &z);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(z, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture4, &x, &y, &z);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(z, 1, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture4);
    GFXTextureDestroy(Texture3);
    GFXTextureDestroy(Texture2);
    GFXTextureDestroy(Texture1);
    GFXTextureStreamDestroy(Stream);
    
    
    
    
    Stream = GFXTextureStreamCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension1D, CCColourFormatRGB8Unorm, 4, 1, 1);
    
    Texture1 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 2, 1, 1, NULL);
    Texture3 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    GFXTextureStreamDestroy(Stream);
    GFXTextureDestroy(Texture3);
    GFXTextureDestroy(Texture2);
    GFXTextureDestroy(Texture1);
}

@end
