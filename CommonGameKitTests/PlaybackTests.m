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
#import "Playback.h"

@interface PlaybackTests : XCTestCase
@end

@implementation PlaybackTests

-(void) testProgressingPlayback
{
    CCPlayback Playback = {
        .position = 0,
        .start = 0,
        .end = 0,
        .speed = (CCDecimal){ .coef = 1, .exp = 0 },
        .loop = CCPlaybackLoopOnce,
        .direction = CCPlaybackDirectionForward,
        .playing = TRUE
    };
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 100;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 100;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.end = 1000;
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 100, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 100;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 200, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 200;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 1000, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 1000;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 900, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 900;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 800, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 800;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.start = 200;
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 300, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 300;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 400, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 400;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 1000, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 1000;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 900, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 900;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 800, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 800;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 200, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    
    Playback.loop = CCPlaybackLoopRepeatRoll;
    Playback.start = 0;
    Playback.end = 0;
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 100;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 100;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.end = 1000;
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 100, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 100;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 200, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 200;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 199, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 200;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1100);
    
    XCTAssertEqual(Playback.position, 299, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 1000;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 900, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 900;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 800, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 800;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 801, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 800;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 1100);
    
    XCTAssertEqual(Playback.position, 701, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.start = 200;
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 300, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 300;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 400, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 400;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 599, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 400;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1100);
    
    XCTAssertEqual(Playback.position, 699, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 400;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1400);
    
    XCTAssertEqual(Playback.position, 999, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 400;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1401);
    
    XCTAssertEqual(Playback.position, 1000, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 1000;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 900, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 900;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 800, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 800;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 601, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    
    Playback.loop = CCPlaybackLoopRepeatFlip;
    Playback.start = 0;
    Playback.end = 0;
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 100;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 100;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.end = 1000;
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 0);
    
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 100, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 100;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 200, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 200;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 800, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 200;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1100);
    
    XCTAssertEqual(Playback.position, 700, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 1000;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 900, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 900;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 800, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 800;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 200, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 800;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 1100);
    
    XCTAssertEqual(Playback.position, 300, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.start = 200;
    Playback.position = 0;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 300, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 300;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 400, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 400;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 600, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 400;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1100);
    
    XCTAssertEqual(Playback.position, 500, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 400;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1400);
    
    XCTAssertEqual(Playback.position, 200, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 400;
    Playback.direction = CCPlaybackDirectionForward;
    
    CCPlaybackAdvance(&Playback, 1401);
    
    XCTAssertEqual(Playback.position, 201, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    Playback.position = 1000;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 900, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 900;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 100);
    
    XCTAssertEqual(Playback.position, 800, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionBackward, @"should be unchanged");
    
    
    Playback.position = 800;
    Playback.direction = CCPlaybackDirectionBackward;
    
    CCPlaybackAdvance(&Playback, 1000);
    
    XCTAssertEqual(Playback.position, 600, @"should be the correct position");
    XCTAssertEqual(Playback.direction, CCPlaybackDirectionForward, @"should be unchanged");
    
    
    
#define SEQ_ONCE(n, end) \
SEQ_ONCE_(n, end, n, CCPlaybackDirectionForward,), \
SEQ_ONCE_(n, end, end, CCPlaybackDirectionBackward, ((end) + n) - )
    
#define SEQ_ROLL(n, end) \
SEQ_ROLL_(n, end, n, CCPlaybackDirectionForward,), \
SEQ_ROLL_(n, end, end, CCPlaybackDirectionBackward, ((end) + n) - )
    
#define SEQ_FLIP(n, end) \
SEQ_FLIP_(n, end, n, CCPlaybackDirectionForward,), \
SEQ_FLIP_(n, end, end, CCPlaybackDirectionBackward, ((end) + n) - )
    
    
#define SEQ_ONCE_(n, end, pos, dir, flip) \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, 0, { flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, 1, { flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, 2, { flip (2+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, 4, { flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, 5, { flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, 6, { flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, 8, { flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, 9, { flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, 10, { flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, 11, { flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopOnce, UINT64_MAX, { flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)), flip (4+(n)) } }
    
#define SEQ_ROLL_(n, end, pos, dir, flip) \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 0, { flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 1, { flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 2, { flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 4, { flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 5, { flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 6, { flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 7, { flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 8, { flip (3+(n)), flip (1+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (3+(n)), flip (1+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (3+(n)), flip (1+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (3+(n)), flip (1+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 9, { flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 10, { flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 11, { flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, 12, { flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (1+(n)), flip (3+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatRoll, UINT64_MAX, { flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)) } }
    
#define SEQ_FLIP_(n, end, pos, dir, flip) \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 0, { flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 1, { flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 2, { flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 4, { flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 5, { flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (4+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (0+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (4+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (0+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 6, { flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 7, { flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 8, { flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 9, { flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 10, { flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)), flip (2+(n)), flip (4+(n)), flip (2+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 11, { flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (4+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (0+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (4+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (0+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (4+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, 12, { flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)), flip (4+(n)), flip (0+(n)) } }, \
{ (pos), (n), (end), (dir), CCPlaybackLoopRepeatFlip, UINT64_MAX, { flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)), flip (3+(n)), flip (2+(n)), flip (1+(n)), flip (0+(n)), flip (1+(n)), flip (2+(n)), flip (3+(n)), flip (4+(n)) } }
    
    struct {
        CCPlaybackTime position;
        CCPlaybackTime start;
        CCPlaybackTime end;
        CCPlaybackDirection direction;
        CCPlaybackLoop loop;
        CCPlaybackTime time;
        CCPlaybackTime results[20];
    } Sequences[] = {
        SEQ_ONCE(0, 4),
        SEQ_ONCE(1, 5),
        SEQ_ONCE(UINT64_MAX - 4, UINT64_MAX),
        
        SEQ_ROLL(0, 4),
        SEQ_ROLL(1, 5),
        SEQ_ROLL(UINT64_MAX - 4, UINT64_MAX),
        
        SEQ_FLIP(0, 4),
        SEQ_FLIP(1, 5),
        SEQ_FLIP(UINT64_MAX - 4, UINT64_MAX),

    };
    
    for (size_t Loop = 0; Loop < sizeof(Sequences) / sizeof(*Sequences); Loop++)
    {
        Playback.position = Sequences[Loop].position;
        Playback.start = Sequences[Loop].start;
        Playback.end = Sequences[Loop].end;
        Playback.direction = Sequences[Loop].direction;
        Playback.loop = Sequences[Loop].loop;
        
        for (size_t Loop2 = 0; Loop2 < 20; Loop2++)
        {
            CCPlaybackAdvance(&Playback, Sequences[Loop].time);
            XCTAssertEqual(Playback.position, Sequences[Loop].results[Loop2], @"should be the correct position");
        }
    }
    
    
    
    Playback.position = 0;
    Playback.start = 0;
    Playback.end = 100;
    Playback.direction = CCPlaybackDirectionForward;
    Playback.loop = CCPlaybackLoopOnce;
    
    Playback.speed = (CCDecimal){ .coef = 0, .exp = 0 };
    CCPlaybackAdvance(&Playback, 1000);
    XCTAssertEqual(Playback.position, 0, @"should be the correct position");
    
    Playback.speed = (CCDecimal){ .coef = 5, .exp = -2 };
    CCPlaybackAdvance(&Playback, 1000);
    XCTAssertEqual(Playback.position, 50, @"should be the correct position");
    
    Playback.speed = (CCDecimal){ .coef = 1, .exp = -3 };
    CCPlaybackAdvance(&Playback, 1000);
    XCTAssertEqual(Playback.position, 51, @"should be the correct position");
    
    Playback.speed = (CCDecimal){ .coef = 1, .exp = -4 };
    CCPlaybackAdvance(&Playback, 1000);
    XCTAssertEqual(Playback.position, 51, @"should be the correct position");
    
    Playback.speed = (CCDecimal){ .coef = UINT64_MAX, .exp = -1 };
    CCPlaybackAdvance(&Playback, 1000);
    XCTAssertEqual(Playback.position, 100, @"should be the correct position");
}

@end
