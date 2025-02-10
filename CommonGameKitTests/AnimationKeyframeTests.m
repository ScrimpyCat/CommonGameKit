/*
 *  Copyright (c) 2025, Stefan Johnson
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

#import <XCTest/XCTest.h>
#import "AnimationKeyframe.h"

@interface AnimationKeyframeTests : XCTestCase
@end

@implementation AnimationKeyframeTests

-(void) testLengths
{
    CCAnimationKeyframe Animation = {
        .count = 7,
        .frames = (CCPlaybackTime[7]){ 10, 20, 30, 40, 50, 60, 70 }
    };
    
    XCTAssertEqual(CCAnimationKeyframeGetLength(&Animation), 70, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 0), 0, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 1), 10, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 2), 20, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 3), 30, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 4), 40, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 5), 50, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 6), 60, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 7), 70, @"should be the correct length");
    
    
    Animation.count = 3;
    
    XCTAssertEqual(CCAnimationKeyframeGetLength(&Animation), 30, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 0), 0, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 1), 10, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 2), 20, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 3), 30, @"should be the correct length");
    
    
    Animation.frames = (CCPlaybackTime[3]){ 5, 10, 50 };
    
    XCTAssertEqual(CCAnimationKeyframeGetLength(&Animation), 50, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 0), 0, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 1), 5, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 2), 10, @"should be the correct length");
    XCTAssertEqual(CCAnimationKeyframeGetFrameLength(&Animation, 3), 50, @"should be the correct length");
}

-(void) testGettingFrames
{
    CCAnimationKeyframe Animation = {
        .count = 7,
        .frames = (CCPlaybackTime[7]){ 2, 4, 6, 8, 10, 12, 14 },
        .jump = {
            .time = 2,
            .count = 7,
            .indexes = (size_t[]){ 0, 1, 2, 3, 4, 5, 6 }
        }
    };
    
    _Bool Finished;
    
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 0, &Finished), 0, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 1, &Finished), 0, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 2, &Finished), 1, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 3, &Finished), 1, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 4, &Finished), 2, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 5, &Finished), 2, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 6, &Finished), 3, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 7, &Finished), 3, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 8, &Finished), 4, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 9, &Finished), 4, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 10, &Finished), 5, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 11, &Finished), 5, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 12, &Finished), 6, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 13, &Finished), 6, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 14, &Finished), 6, @"should get the correct frame");
    XCTAssertTrue(Finished, @"should have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 15, &Finished), 6, @"should get the correct frame");
    XCTAssertTrue(Finished, @"should have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, UINT64_MAX, &Finished), 6, @"should get the correct frame");
    XCTAssertTrue(Finished, @"should have finished the animation");
    
    
    Animation.count = 3;
    Animation.frames = (CCPlaybackTime[7]){ 2, 6, 8 };
    Animation.jump.time = 2;
    Animation.jump.count = 4;
    Animation.jump.indexes = (size_t[]){ 0, 1, 1, 2 };
    
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 0, &Finished), 0, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 1, &Finished), 0, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 2, &Finished), 1, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 3, &Finished), 1, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 4, &Finished), 1, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 5, &Finished), 1, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 6, &Finished), 2, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 7, &Finished), 2, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 8, &Finished), 2, @"should get the correct frame");
    XCTAssertTrue(Finished, @"should have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 9, &Finished), 2, @"should get the correct frame");
    XCTAssertTrue(Finished, @"should have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, UINT64_MAX, &Finished), 2, @"should get the correct frame");
    XCTAssertTrue(Finished, @"should have finished the animation");
    
    
    Animation.jump.time = 1;
    Animation.jump.count = 7;
    Animation.jump.indexes = (size_t[]){ 0, 0, 1, 1, 1, 1, 2 };
    
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 0, &Finished), 0, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 1, &Finished), 0, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 2, &Finished), 1, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 3, &Finished), 1, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 4, &Finished), 1, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 5, &Finished), 1, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 6, &Finished), 2, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 7, &Finished), 2, @"should get the correct frame");
    XCTAssertFalse(Finished, @"should not have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 8, &Finished), 2, @"should get the correct frame");
    XCTAssertTrue(Finished, @"should have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, 9, &Finished), 2, @"should get the correct frame");
    XCTAssertTrue(Finished, @"should have finished the animation");
    XCTAssertEqual(CCAnimationKeyframeGetFrame(&Animation, UINT64_MAX, &Finished), 2, @"should get the correct frame");
    XCTAssertTrue(Finished, @"should have finished the animation");
}

-(void) testFrameTimes
{
    CCAnimationKeyframe Animation = {
        .count = 7,
        .frames = (CCPlaybackTime[7]){ 10, 20, 30, 40, 50, 60, 70 }
    };
    
    CCPlaybackTime Min = -1, Max = 0, Avg = 0;
    
    CCAnimationKeyframeGetFrameTimes(&Animation, &Min, &Max, &Avg);
    
    XCTAssertEqual(Min, 10, @"should get the correct time");
    XCTAssertEqual(Max, 10, @"should get the correct time");
    XCTAssertEqual(Avg, 10, @"should get the correct time");
    
    
    Animation.frames = (CCPlaybackTime[7]){ 1, 20, 30, 40, 50, 60, 65 };
    
    CCAnimationKeyframeGetFrameTimes(&Animation, &Min, &Max, &Avg);
    
    XCTAssertEqual(Min, 1, @"should get the correct time");
    XCTAssertEqual(Max, 19, @"should get the correct time");
    XCTAssertEqual(Avg, 9, @"should get the correct time");
}

-(void) testGenerateJumps
{
    CCAnimationKeyframe Animation = {
        .count = 7,
        .frames = (CCPlaybackTime[7]){ 10, 20, 30, 40, 50, 60, 70 }
    };
    
    CCPlaybackTime Min = -1, Max = 0, Avg = 0;
    
    CCAnimationKeyframeGetFrameTimes(&Animation, NULL, NULL, &Avg);
    
    size_t Count = CCAnimationKeyframeGetSampleCount(&Animation, Avg);
    
    XCTAssertEqual(Count, 7, @"should get the correct count");
    
    size_t Indexes[20];
    
    memset(Indexes, -1, sizeof(Indexes));
    
    CCAnimationKeyframeGenerateJumpIndexes(&Animation, Indexes, Count);
    
    XCTAssertEqual(Indexes[0], 0, @"should get the correct time");
    XCTAssertEqual(Indexes[1], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[2], 2, @"should get the correct time");
    XCTAssertEqual(Indexes[3], 3, @"should get the correct time");
    XCTAssertEqual(Indexes[4], 4, @"should get the correct time");
    XCTAssertEqual(Indexes[5], 5, @"should get the correct time");
    XCTAssertEqual(Indexes[6], 6, @"should get the correct time");
    XCTAssertEqual(Indexes[7], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[8], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[9], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[10], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[11], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[12], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[13], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[14], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[15], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[16], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[17], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[18], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[19], SIZE_MAX, @"should get the correct time");
    
    
    Animation.count = 4;
    Animation.frames = (CCPlaybackTime[7]){ 1, 10, 15, 18 };
    
    CCAnimationKeyframeGetFrameTimes(&Animation, &Min, &Max, &Avg);
    
    Count = CCAnimationKeyframeGetSampleCount(&Animation, Avg);
    
    XCTAssertEqual(Count, 4, @"should get the correct count");
    
    memset(Indexes, -1, sizeof(Indexes));
    
    CCAnimationKeyframeGenerateJumpIndexes(&Animation, Indexes, Count);
    
    XCTAssertEqual(Indexes[0], 0, @"should get the correct time");
    XCTAssertEqual(Indexes[1], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[2], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[3], 2, @"should get the correct time");
    XCTAssertEqual(Indexes[4], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[5], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[6], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[7], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[8], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[9], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[10], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[11], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[12], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[13], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[14], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[15], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[16], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[17], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[18], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[19], SIZE_MAX, @"should get the correct time");
    
    
    Count = CCAnimationKeyframeGetSampleCount(&Animation, Min);
    
    XCTAssertEqual(Count, 18, @"should get the correct count");
    
    memset(Indexes, -1, sizeof(Indexes));
    
    CCAnimationKeyframeGenerateJumpIndexes(&Animation, Indexes, Count);
    
    XCTAssertEqual(Indexes[0], 0, @"should get the correct time");
    XCTAssertEqual(Indexes[1], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[2], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[3], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[4], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[5], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[6], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[7], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[8], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[9], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[10], 2, @"should get the correct time");
    XCTAssertEqual(Indexes[11], 2, @"should get the correct time");
    XCTAssertEqual(Indexes[12], 2, @"should get the correct time");
    XCTAssertEqual(Indexes[13], 2, @"should get the correct time");
    XCTAssertEqual(Indexes[14], 2, @"should get the correct time");
    XCTAssertEqual(Indexes[15], 3, @"should get the correct time");
    XCTAssertEqual(Indexes[16], 3, @"should get the correct time");
    XCTAssertEqual(Indexes[17], 3, @"should get the correct time");
    XCTAssertEqual(Indexes[18], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[19], SIZE_MAX, @"should get the correct time");
    
    
    Count = CCAnimationKeyframeGetSampleCount(&Animation, Max);
    
    XCTAssertEqual(Count, 2, @"should get the correct count");
    
    memset(Indexes, -1, sizeof(Indexes));
    
    CCAnimationKeyframeGenerateJumpIndexes(&Animation, Indexes, Count);
    
    XCTAssertEqual(Indexes[0], 0, @"should get the correct time");
    XCTAssertEqual(Indexes[1], 1, @"should get the correct time");
    XCTAssertEqual(Indexes[2], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[3], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[4], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[5], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[6], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[7], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[8], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[9], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[10], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[11], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[12], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[13], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[14], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[15], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[16], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[17], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[18], SIZE_MAX, @"should get the correct time");
    XCTAssertEqual(Indexes[19], SIZE_MAX, @"should get the correct time");
}

@end
