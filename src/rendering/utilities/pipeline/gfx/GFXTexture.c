/*
 *  Copyright (c) 2015, Stefan Johnson
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

#define CC_QUICK_COMPILE
#include "GFXTexture.h"
#include "GFXMain.h"

#pragma mark Texture Stream
typedef struct {
    size_t x, y, z;
    size_t width, height, depth;
} GFXTextureStreamRegion3D;

typedef struct GFXTextureStreamNode {
    struct GFXTextureStreamNode *child[2];
    GFXTexture texture;
    CCMemoryDestructorCallback destructor;
    GFXTextureStreamRegion3D region;
} GFXTextureStreamNode;

typedef struct GFXTextureStream {
    CCAllocatorType allocator;
    GFXTextureStreamNode root;
} GFXTextureStreamInfo;

static void GFXTextureStreamDestroyNode(GFXTextureStreamNode *Node);

static void GFXTextureStreamNodeDestructor(GFXTextureStreamNode *Node)
{
    if (Node->child[0]) GFXTextureStreamDestroyNode(Node->child[0]);
    if (Node->child[1]) GFXTextureStreamDestroyNode(Node->child[1]);
    
    if (Node->texture) GFXTextureDestroy(Node->texture);
}

static GFXTextureStreamNode *GFXTextureStreamCreateNode(CCAllocatorType Allocator, GFXTextureStreamRegion3D Region)
{
    GFXTextureStreamNode *Node = CCMalloc(Allocator, sizeof(GFXTextureStreamNode), NULL, CC_DEFAULT_ERROR_CALLBACK);
    
    if (Node)
    {
        *Node = (GFXTextureStreamNode){
            .child = { NULL, NULL },
            .texture = NULL,
            .destructor = NULL,
            .region = Region
        };
        
        CCMemorySetDestructor(Node, (CCMemoryDestructorCallback)GFXTextureStreamNodeDestructor);
    }
    
    return Node;
}

static void GFXTextureStreamDestroyNode(GFXTextureStreamNode *Node)
{
    CCAssertLog(Node, "Node must not be null");
    
    CCFree(Node);
}

static inline _Bool GFXTextureStreamNodeRegionFit(GFXTextureStreamRegion3D Container, size_t Width, size_t Height, size_t Depth)
{
    return (Width <= Container.width) && (Height <= Container.height) && (Depth <= Container.depth);
}

static inline _Bool GFXTextureStreamNodeRegionExactFit(GFXTextureStreamRegion3D Container, size_t Width, size_t Height, size_t Depth)
{
    return (Width == Container.width) && (Height == Container.height) && (Depth == Container.depth);
}

static inline _Bool GFXTextureStreamNodeHasChildren(GFXTextureStreamNode *Node)
{
    return Node->child[0] || Node->child[1];
}

static inline void GFXTextureStreamNodePurge(GFXTextureStreamNode *Node)
{
    if ((!GFXTextureStreamNodeHasChildren(Node->child[0]) && !Node->child[0]->texture) && (!GFXTextureStreamNodeHasChildren(Node->child[1]) && !Node->child[1]->texture))
    {
        GFXTextureStreamDestroyNode(Node->child[0]);
        GFXTextureStreamDestroyNode(Node->child[1]);
    }
}

static GFXTextureStreamNode *GFXTextureStreamNodeFindAvailable(GFXTextureStreamNode *Node, size_t Width, size_t Height, size_t Depth, CCAllocatorType Allocator, _Bool Root)
{
    if ((!Root) && (Node->texture)) return NULL;
    
    if (GFXTextureStreamNodeRegionFit(Node->region, Width, Height, Depth))
    {
        const _Bool Children = GFXTextureStreamNodeHasChildren(Node);
        if (Children) GFXTextureStreamNodePurge(Node);
        
        if (GFXTextureStreamNodeRegionExactFit(Node->region, Width, Height, Depth))
        {
            return Children ? NULL : Node;
        }
        
        else if (Children)
        {
            GFXTextureStreamNode *N = GFXTextureStreamNodeFindAvailable(Node->child[0], Width, Height, Depth, Allocator, FALSE);
            if (!N) N = GFXTextureStreamNodeFindAvailable(Node->child[1], Width, Height, Depth, Allocator, FALSE);
            
            return N;
        }
        
        else
        {
            const size_t DiffW = Node->region.width - Width, DiffH = Node->region.height - Height, DiffD = Node->region.depth - Depth;
            
            if ((DiffW > DiffH) && (DiffW > DiffD))
            {
                Node->child[0] = GFXTextureStreamCreateNode(Allocator, (GFXTextureStreamRegion3D){
                    .x = Node->region.x, .width = Width,
                    .y = Node->region.y, .height = Node->region.height,
                    .z = Node->region.z, .depth = Node->region.depth
                });
                Node->child[1] = GFXTextureStreamCreateNode(Allocator, (GFXTextureStreamRegion3D){
                    .x = Node->region.x + Width, .width = Node->region.width - Width,
                    .y = Node->region.y, .height = Node->region.height,
                    .z = Node->region.z, .depth = Node->region.depth
                });
            }
            
            else if (DiffH > DiffD)
            {
                Node->child[0] = GFXTextureStreamCreateNode(Allocator, (GFXTextureStreamRegion3D){
                    .x = Node->region.x, .width = Node->region.width,
                    .y = Node->region.y, .height = Height,
                    .z = Node->region.z, .depth = Node->region.depth
                });
                Node->child[1] = GFXTextureStreamCreateNode(Allocator, (GFXTextureStreamRegion3D){
                    .x = Node->region.x, .width = Node->region.width,
                    .y = Node->region.y + Height, .height = Node->region.height - Height,
                    .z = Node->region.z, .depth = Node->region.depth
                });
            }
            
            else
            {
                Node->child[0] = GFXTextureStreamCreateNode(Allocator, (GFXTextureStreamRegion3D){
                    .x = Node->region.x, .width = Node->region.width,
                    .y = Node->region.y, .height = Node->region.height,
                    .z = Node->region.z, .depth = Depth
                });
                Node->child[1] = GFXTextureStreamCreateNode(Allocator, (GFXTextureStreamRegion3D){
                    .x = Node->region.x, .width = Node->region.width,
                    .y = Node->region.y, .height = Node->region.height,
                    .z = Node->region.z + Depth, .depth = Node->region.depth - Depth
                });
            }
            
            return GFXTextureStreamNodeFindAvailable(Node->child[0], Width, Height, Depth, Allocator, FALSE);
        }
    }
    
    return NULL;
}

static void GFXTextureStreamDestructor(GFXTextureStream Stream)
{
    if (Stream->root.child[0]) GFXTextureStreamDestroyNode(Stream->root.child[0]);
    if (Stream->root.child[1]) GFXTextureStreamDestroyNode(Stream->root.child[1]);
    
    GFXTextureDestroy(Stream->root.texture);
}

GFXTextureStream GFXTextureStreamCreate(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth)
{
    GFXTextureStream Stream = CCMalloc(Allocator, sizeof(GFXTextureStreamInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Stream)
    {
        GFXTexture Texture = GFXTextureCreate(Allocator, Hint, Format, Width, Height, Depth, NULL);
        if (Texture)
        {
            CCMemorySetDestructor(Stream, (CCMemoryDestructorCallback)GFXTextureStreamDestructor);
            
            *Stream = (GFXTextureStreamInfo){
                .allocator = Allocator,
                .root = (GFXTextureStreamNode){
                    .child = { NULL, NULL },
                    .texture = Texture,
                    .destructor = NULL,
                    .region = (GFXTextureStreamRegion3D){
                        .x = 0, .width = Width,
                        .y = 0, .height = Height,
                        .z = 0, .depth = Depth
                    }
                }
            };
        }
        
        else
        {
            CC_LOG_ERROR("Failed to create texture stream: Couldn't create root texture");
            CCFree(Stream);
        }
    }
    
    else CC_LOG_ERROR("Failed to allocate memory for texture stream. Allocation size (%zu)", sizeof(GFXTextureStreamInfo));
    
    return Stream;
}

void GFXTextureStreamDestroy(GFXTextureStream Stream)
{
    CCAssertLog(Stream, "Stream must not be null");
    
    CCFree(Stream);
}

#pragma mark - Texture

GFXTexture GFXTextureCreate(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    return GFXMain->texture->create(Allocator, Hint, Format, Width, Height, Depth, Data);
}

GFXTexture GFXTextureCreateSubTexture(CCAllocatorType Allocator, GFXTexture Texture, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    size_t W, H, D;
    GFXTextureGetSize(Texture, &W, &H, &D);
    
    CCAssertLog(((X + Width) <= W) && ((Y + Height) <= H) && ((Z + Depth) <= D), "Sub texture must not exceed bounds of parent texture");
    CCAssertLog(((X + Width) > X) && ((Y + Height) > Y) && ((Z + Depth) > Z), "Sub texture bounds must be greater than 1 or must not overflow");
    
    return GFXMain->texture->createSub(Allocator, CCRetain(Texture), X, Y, Z, Width, Height, Depth, Data);
}

static void GFXTextureDestructor(GFXTexture Texture)
{
    GFXTextureStreamNode *Node = GFXMain->texture->getStream(Texture);
    
    CCAssertLog(Node->destructor, "Destructor must be set");
    
    Node->destructor(Node->texture);
    Node->texture = NULL;
    
    GFXTextureStreamDestroyNode(Node);
}

GFXTexture GFXTextureCreateFromStream(CCAllocatorType Allocator, GFXTextureStream Stream, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    CCAssertLog(Stream, "Stream must not be null");
    
    GFXTextureStreamNode *Node = GFXTextureStreamNodeFindAvailable(&Stream->root, Width, Height, Depth, Stream->allocator, TRUE);
    if (!Node) return NULL;
    
    Node->texture = GFXTextureCreateSubTexture(Allocator, Stream->root.texture, Node->region.x, Node->region.y, Node->region.z, Width, Height, Depth, Data);
    
    if (Node->texture)
    {
        GFXMain->texture->setStream(Node->texture, CCRetain(Node));
        Node->destructor = CCMemorySetDestructor(Node->texture, (CCMemoryDestructorCallback)GFXTextureDestructor);
    }
    
    return Node->texture;
}

void GFXTextureDestroy(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    GFXMain->texture->destroy(Texture);
}

GFXTexture GFXTextureGetParent(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    return GFXMain->texture->parent(Texture);
}

GFXTextureHint GFXTextureGetHints(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    return GFXMain->texture->hints(Texture);
}

CCPixelData GFXTextureGetData(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    return GFXMain->texture->data(Texture);
}

void GFXTextureGetOffset(GFXTexture Texture, size_t *X, size_t *Y, size_t *Z)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    GFXMain->texture->offset(Texture, X, Y, Z);
}

void GFXTextureGetInternalOffset(GFXTexture Texture, size_t *X, size_t *Y, size_t *Z)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    if (GFXMain->texture->optional.internalOffset) GFXMain->texture->optional.internalOffset(Texture, X, Y, Z);
    else
    {
        size_t RealX = 0, RealY = 0, RealZ = 0;
        while (Texture)
        {
            size_t CurX, CurY, CurZ;
            GFXTextureGetOffset(Texture, &CurX, &CurY, &CurZ);
            
            RealX += CurX;
            RealY += CurY;
            RealZ += CurZ;
            
            Texture = GFXTextureGetParent(Texture);
        }
        
        *X = RealX;
        *Y = RealY;
        *Z = RealZ;
    }
}

void GFXTextureGetSize(GFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    GFXMain->texture->size(Texture, Width, Height, Depth);
}

void GFXTextureGetInternalSize(GFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    if (GFXMain->texture->optional.internalSize) GFXMain->texture->optional.internalSize(Texture, Width, Height, Depth);
    else
    {
        GFXTexture Root = Texture;
        while (Texture)
        {
            Texture = GFXTextureGetParent(Texture);
            
            if (Texture) Root = Texture;
        }
        
        GFXTextureGetSize(Root, Width, Height, Depth);
    }
}

void GFXTextureGetBounds(GFXTexture Texture, CCVector3D *Bottom, CCVector3D *Top)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    size_t Width, Height, Depth;
    GFXTextureGetSize(Texture, &Width, &Height, &Depth);
    
    size_t RealX = 0, RealY = 0, RealZ = 0;
    size_t RealW = 0, RealH = 0, RealD = 0;
    
    if ((GFXMain->texture->optional.internalOffset) && (GFXMain->texture->optional.internalSize))
    {
        GFXTextureGetInternalOffset(Texture, &RealX, &RealY, &RealZ);
        GFXTextureGetInternalSize(Texture, &RealW, &RealH, &RealD);
    }
    
    else
    {
        while (Texture)
        {
            size_t X, Y, Z;
            GFXTextureGetOffset(Texture, &X, &Y, &Z);
            
            RealX += X;
            RealY += Y;
            RealZ += Z;
            
            GFXTextureGetSize(Texture, &RealW, &RealH, &RealD);
            
            Texture = GFXTextureGetParent(Texture);
        }
    }
    
    if (Bottom) *Bottom = CCVector3DMake((float)RealX / (float)RealW,
                                         (float)RealY / (float)RealH,
                                         (float)RealZ / (float)RealD);
    
    if (Top) *Top = CCVector3DMake((float)(RealX + Width) / (float)RealW,
                                   (float)(RealY + Height) / (float)RealH,
                                   (float)(RealZ + Depth) / (float)RealD);
}

CCVector3D GFXTextureGetMultiplier(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    size_t RealW = 0, RealH = 0, RealD = 0;
    GFXTextureGetInternalSize(Texture, &RealW, &RealH, &RealD);
    
    return CCVector3DMake(1.0f / (float)RealW,
                          1.0f / (float)RealH,
                          1.0f / (float)RealD);
}

CCVector3D GFXTextureNormalizePoint(GFXTexture Texture, size_t X, size_t Y, size_t Z)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    size_t BaseX = 0, BaseY = 0, BaseZ = 0;
    GFXTextureGetInternalOffset(Texture, &BaseX, &BaseY, &BaseZ);
    
    return CCVector3Mul(CCVector3DMake(BaseX + X, BaseY + Y, BaseZ + Z), GFXTextureGetMultiplier(Texture));
}

void GFXTextureSetFilterMode(GFXTexture Texture, GFXTextureHint FilterType, GFXTextureHint FilterMode)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    GFXMain->texture->setFilterMode(Texture, FilterType, FilterMode);
}

void GFXTextureSetAddressMode(GFXTexture Texture, GFXTextureHint Coordinate, GFXTextureHint AddressMode)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    GFXMain->texture->setAddressMode(Texture, Coordinate, AddressMode);
}

void GFXTextureInvalidate(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    if (GFXMain->texture->optional.invalidate) GFXMain->texture->optional.invalidate(Texture);
}

CCPixelData GFXTextureRead(GFXTexture Texture, CCAllocatorType Allocator, CCColourFormat Format, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth)
{
    /*
     TODO: To support async reads in the future, the CCPixelData (or CCData) could just implement a new deferred/future type.
     */
    CCAssertLog(Texture, "Texture must not be null");
    
    size_t W, H, D;
    GFXTextureGetSize(Texture, &W, &H, &D);
    
    CCAssertLog(((X + Width) <= W) && ((Y + Height) <= H) && ((Z + Depth) <= D), "Read must not exceed bounds of texture");
    CCAssertLog(((X + Width) > X) && ((Y + Height) > Y) && ((Z + Depth) > Z), "Read bounds must be greater than 1 or must not overflow");
    
    return GFXMain->texture->read(Texture, Allocator, Format, X, Y, Z, Width, Height, Depth);
}

void GFXTextureWrite(GFXTexture Texture, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    CCAssertLog(Texture, "Texture must not be null");
    CCAssertLog(Data, "Data must not be null");
    
    size_t W, H, D;
    GFXTextureGetSize(Texture, &W, &H, &D);
    
    CCAssertLog(((X + Width) <= W) && ((Y + Height) <= H) && ((Z + Depth) <= D), "Write must not exceed bounds of texture");
    CCAssertLog(((X + Width) > X) && ((Y + Height) > Y) && ((Z + Depth) > Z), "Write bounds must be greater than 1 or must not overflow");
    
    return GFXMain->texture->write(Texture, X, Y, Z, Width, Height, Depth, Data);
}
