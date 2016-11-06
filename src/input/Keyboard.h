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

#ifndef CommonGameKit_Keyboard_h
#define CommonGameKit_Keyboard_h

#include "GLSetup.h"
#include "Component.h"

/// The action type of the keyboard input event. This is compatible with GLFW actions.
typedef enum {
    CCKeyboardActionRelease,
    CCKeyboardActionPress,
    CCKeyboardActionRepeat
} CCKeyboardAction;

/// The modifiers applied to the keyboard input event. This is compatible with GLFW modifiers.
typedef enum {
    CCKeyboardModifierShift = (1 << 0),
    CCKeyboardModifierControl = (1 << 1),
    CCKeyboardModifierAlt = (1 << 2),
    CCKeyboardModifierSuper = (1 << 3)
} CCKeyboardModifier;

/// The keycode for the keyboard key the input event belongs to. This is compatible with GLFW keycode.
typedef enum {
    /* Printable keys */
    CCKeyboardKeycodeSpace =              32,
    CCKeyboardKeycodeApostrophe =         39  /* ' */,
    CCKeyboardKeycodeComma =              44  /* , */,
    CCKeyboardKeycodeMinus =              45  /* - */,
    CCKeyboardKeycodePeriod =             46  /* . */,
    CCKeyboardKeycodeSlash =              47  /* / */,
    CCKeyboardKeycode0 =                  48,
    CCKeyboardKeycode1 =                  49,
    CCKeyboardKeycode2 =                  50,
    CCKeyboardKeycode3 =                  51,
    CCKeyboardKeycode4 =                  52,
    CCKeyboardKeycode5 =                  53,
    CCKeyboardKeycode6 =                  54,
    CCKeyboardKeycode7 =                  55,
    CCKeyboardKeycode8 =                  56,
    CCKeyboardKeycode9 =                  57,
    CCKeyboardKeycodeSemicolon =          59  /* ; */,
    CCKeyboardKeycodeEqual =              61  /* = */,
    CCKeyboardKeycodeA =                  65,
    CCKeyboardKeycodeB =                  66,
    CCKeyboardKeycodeC =                  67,
    CCKeyboardKeycodeD =                  68,
    CCKeyboardKeycodeE =                  69,
    CCKeyboardKeycodeF =                  70,
    CCKeyboardKeycodeG =                  71,
    CCKeyboardKeycodeH =                  72,
    CCKeyboardKeycodeI =                  73,
    CCKeyboardKeycodeJ =                  74,
    CCKeyboardKeycodeK =                  75,
    CCKeyboardKeycodeL =                  76,
    CCKeyboardKeycodeM =                  77,
    CCKeyboardKeycodeN =                  78,
    CCKeyboardKeycodeO =                  79,
    CCKeyboardKeycodeP =                  80,
    CCKeyboardKeycodeQ =                  81,
    CCKeyboardKeycodeR =                  82,
    CCKeyboardKeycodeS =                  83,
    CCKeyboardKeycodeT =                  84,
    CCKeyboardKeycodeU =                  85,
    CCKeyboardKeycodeV =                  86,
    CCKeyboardKeycodeW =                  87,
    CCKeyboardKeycodeX =                  88,
    CCKeyboardKeycodeY =                  89,
    CCKeyboardKeycodeZ =                  90,
    CCKeyboardKeycodeLeftBracket =        91  /* [ */,
    CCKeyboardKeycodeBackslash =          92  /* \ */,
    CCKeyboardKeycodeRightBracket =       93  /* ] */,
    CCKeyboardKeycodeGraveAccent =        96  /* ` */,
    CCKeyboardKeycodeWorld1 =             161 /* non-US #1 */,
    CCKeyboardKeycodeWorld2 =             162 /* non-US #2 */,
    
    /* Function keys */
    CCKeyboardKeycodeEscape =             256,
    CCKeyboardKeycodeEnter =              257,
    CCKeyboardKeycodeTab =                258,
    CCKeyboardKeycodeBackspace =          259,
    CCKeyboardKeycodeInsert =             260,
    CCKeyboardKeycodeDelete =             261,
    CCKeyboardKeycodeRight =              262,
    CCKeyboardKeycodeLeft =               263,
    CCKeyboardKeycodeDown =               264,
    CCKeyboardKeycodeUp =                 265,
    CCKeyboardKeycodePageUp =             266,
    CCKeyboardKeycodePageDown =           267,
    CCKeyboardKeycodeHome =               268,
    CCKeyboardKeycodeEnd =                269,
    CCKeyboardKeycodeCapsLock =           280,
    CCKeyboardKeycodeScrollLock =         281,
    CCKeyboardKeycodeNumLock =            282,
    CCKeyboardKeycodePrintScreen =        283,
    CCKeyboardKeycodePause =              284,
    CCKeyboardKeycodeF1 =                 290,
    CCKeyboardKeycodeF2 =                 291,
    CCKeyboardKeycodeF3 =                 292,
    CCKeyboardKeycodeF4 =                 293,
    CCKeyboardKeycodeF5 =                 294,
    CCKeyboardKeycodeF6 =                 295,
    CCKeyboardKeycodeF7 =                 296,
    CCKeyboardKeycodeF8 =                 297,
    CCKeyboardKeycodeF9 =                 298,
    CCKeyboardKeycodeF10 =                299,
    CCKeyboardKeycodeF11 =                300,
    CCKeyboardKeycodeF12 =                301,
    CCKeyboardKeycodeF13 =                302,
    CCKeyboardKeycodeF14 =                303,
    CCKeyboardKeycodeF15 =                304,
    CCKeyboardKeycodeF16 =                305,
    CCKeyboardKeycodeF17 =                306,
    CCKeyboardKeycodeF18 =                307,
    CCKeyboardKeycodeF19 =                308,
    CCKeyboardKeycodeF20 =                309,
    CCKeyboardKeycodeF21 =                310,
    CCKeyboardKeycodeF22 =                311,
    CCKeyboardKeycodeF23 =                312,
    CCKeyboardKeycodeF24 =                313,
    CCKeyboardKeycodeF25 =                314,
    CCKeyboardKeycodeKeypad0 =            320,
    CCKeyboardKeycodeKeypad1 =            321,
    CCKeyboardKeycodeKeypad2 =            322,
    CCKeyboardKeycodeKeypad3 =            323,
    CCKeyboardKeycodeKeypad4 =            324,
    CCKeyboardKeycodeKeypad5 =            325,
    CCKeyboardKeycodeKeypad6 =            326,
    CCKeyboardKeycodeKeypad7 =            327,
    CCKeyboardKeycodeKeypad8 =            328,
    CCKeyboardKeycodeKeypad9 =            329,
    CCKeyboardKeycodeKeypadDecimal =      330,
    CCKeyboardKeycodeKeypadDivide =       331,
    CCKeyboardKeycodeKeypadMultiply =     332,
    CCKeyboardKeycodeKeypadSubtract =     333,
    CCKeyboardKeycodeKeypadAdd =          334,
    CCKeyboardKeycodeKeypadEnter =        335,
    CCKeyboardKeycodeKeypadEqual =        336,
    CCKeyboardKeycodeLeftShift =          340,
    CCKeyboardKeycodeLeftControl =        341,
    CCKeyboardKeycodeLeftAlt =            342,
    CCKeyboardKeycodeLeftSuper =          343,
    CCKeyboardKeycodeRightShift =         344,
    CCKeyboardKeycodeRightControl =       345,
    CCKeyboardKeycodeRightAlt =           346,
    CCKeyboardKeycodeRightSuper =         347,
    CCKeyboardKeycodeMenu =               348,
    
    CCKeyboardKeycodeCount,
    CCKeyboardKeycodeUnknown =            -1
} CCKeyboardKeycode;

typedef struct {
    double timestamp;
    _Bool down, repeat;
} CCKeyboardState;

typedef struct {
    CCKeyboardKeycode keycode;
    CCChar character;
    CCKeyboardModifier flags;
    CCKeyboardState state;
} CCKeyboardMap;

/*!
 * @brief Register a new keyboard input event.
 * @param Keycode The keycode the event belongs to.
 * @param Action The action of the event.
 * @param Mods The modifiers applied.
 */
void CCKeyboardInput(CCKeyboardKeycode Keycode, CCKeyboardAction Action, CCKeyboardModifier Mods);

/*!
 * @brief Register a keyboard character input event.
 * @description This should be called after @b CCKeyboardInput
 * @param Codepoint The character representation for the input.
 * @param Mods The modifiers applied.
 */
void CCKeyboardCharInput(CCChar Codepoint, CCKeyboardModifier Mods); //TODO: Combine with CCKeyboardInput and let caller handle separate callbacks

/*!
 * @brief Reset the state for all keys.
 */
void CCKeyboardStateReset(void);

/*!
 * @brief Get the keyboard state for the given component.
 * @param Component The input map keyboard component.
 * @return The keyboard state for the given component, or empty state if there is none available.
 */
CCKeyboardState CCKeyboardGetStateForComponent(CCComponent Component);

#endif
