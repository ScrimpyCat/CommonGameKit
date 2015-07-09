//
//  InputSystem.h
//  Blob Game
//
//  Created by Stefan Johnson on 3/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputSystem_h
#define Blob_Game_InputSystem_h

#include "ComponentSystem.h"
#include "Entity.h"
#include "Keyboard.h"

//Note: Currently just a demo system

#define CC_INPUT_SYSTEM_ID 21
#define CC_INPUT_COMPONENT_FLAG 0x2000000

typedef enum {
    CCInputMapTypeNone = 1,
    CCInputMapTypeKeyboard,
    CCInputMapTypeMouse,
    CCInputMapTypeController,
    CCInputMapTypeGroup
} CCInputMapType;

typedef enum {
    CCInputStateInactive,
    CCInputStateActive
} CCInputState;

typedef void (*CCInputMapKeyboardCallback)(CCComponent Component, CCKeyboardMap State);

void CCInputSystemRegister(void);
void CCInputSystemDeregister(void);

/*!
 * @brief Get the input components registered by the system for a certain type.
 * @param InputType The type of components to retrieve.
 * @return The list of components. Note: This collection must be destroyed.
 */
CCCollection CCInputSystemGetComponents(CCInputMapType InputType);

/*!
 * @brief Gets the state of the action belonging to the entity.
 * @param Entity The entity that has the component attached to it.
 * @param Action The action to look for.
 * @return CCInputStateActive if the action is active, or CCInputStateInactive if it is not or does not exist.
 */
CCInputState CCInputSystemGetStateForAction(CCEntity Entity, const char *Action);

/*!
 * @brief Gets the pressure of the action belonging to the entity.
 * @discussion For binary inputs, the pressure will be calculated by ramping up relative to the timestamp.
 * @param Entity The entity that has the component attached to it.
 * @param Action The action to look for.
 * @return The amount of pressure currently applied to the action (0.0 - 1.0), where 0.0 is no pressure and 1.0
 *         is the maximum amount of pressure allowed.
 */
float CCInputSystemGetPressureForAction(CCEntity Entity, const char *Action);

/*!
 * @brief Gets the 2 axis pressure of the action belonging to the entity.
 * @discussion For binary inputs, the pressure will be calculated by ramping up relative to the timestamp.
 *             And for inputs without 2 axis, they can be simulated by placing them inside an input group
 *             component. If the group contains 2 components then they'll be assigned in order (0:x, 1:y),
 *             while if it contains 4 components then they'll be assigned (0:-x to 2:+x, 1:-y to 3:+y).
 *
 * @param Entity The entity that has the component attached to it.
 * @param Action The action to look for.
 * @return The amount of pressure currently applied to the action it wil either be in the form of (0.0 - 1.0),
 *         or (-1.0 - +1.0), where 0.0 is no pressure and -/+1.0 is the maximum amount of pressure allowed.
 */
CCVector2D CCInputSystemGetPressure2ForAction(CCEntity Entity, const char *Action);

/*!
 * @brief Gets the 3 axis pressure of the action belonging to the entity.
 * @discussion For binary inputs, the pressure will be calculated by ramping up relative to the timestamp.
 *             And for inputs without 3 axis, they can be simulated by placing them inside an input group
 *             component. If the group contains 3 components then they'll be assigned in order (0:x, 1:y, 2:z),
 *             while if it contains 6 components then they'll be assigned (0:-x to 3:+x, 1:-y to 4:+y, 2:-z to 5:+z).
 *
 * @param Entity The entity that has the component attached to it.
 * @param Action The action to look for.
 * @return The amount of pressure currently applied to the action it wil either be in the form of (0.0 - 1.0),
 *         or (-1.0 - +1.0), where 0.0 is no pressure and -/+1.0 is the maximum amount of pressure allowed.
 */
CCVector3D CCInputSystemGetPressure3ForAction(CCEntity Entity, const char *Action);

#endif
