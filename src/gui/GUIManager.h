/*
 *  Copyright (c) 2016, Stefan Johnson
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

#ifndef Blob_Game_GUIManager_h
#define Blob_Game_GUIManager_h

#include "GUIObject.h"

/*!
 * @brief Creates the global GUI manager.
 */
void GUIManagerCreate(void);

/*!
 * @brief Destroys the global GUI manager.
 */
void GUIManagerDestroy(void);

/*!
 * @brief Updates the global GUI manager.
 * @warning This call must be made within a GUI manager lock.
 */
void GUIManagerUpdate(void);

/*!
 * @brief Render the global GUI manager.
 * @warning This call must be made within a GUI manager lock.
 * @param Framebuffer The framebuffer to render to.
 */
void GUIManagerRender(GFXFramebuffer Framebuffer);

//void GUIManagerResize(); //could be just an atomic swap?
/*!
 * @brief Submit an event to the GUI manager.
 * @param Event The GUI event to be submitted.
 */
void GUIManagerHandleEvent(GUIEvent Event);

/*!
 * @brief Adds a GUI object to be managed by the global GUI manager.
 * @param Object The GUI object being managed.
 */
void GUIManagerAddObject(GUIObject Object);

/*!
 * @brief Removes a GUI object from being managed by the global GUI manager.
 * @warning After this call it is no longer safe to reference the GUI.
 * @param Object The GUI object being managed.
 */
void GUIManagerRemoveObject(GUIObject Object);

/*!
 * @brief Get the current GUI objects from the manager.
 * @warning Should obtain locks to the GUI manager prior to calling this function.
 * @return The current GUI object list.
 */
CCCollection GUIManagerGetObjects(void);

/*!
 * @brief Attempt to lock the GUI manager.
 * @return True if was able to lock the GUI manager, otherwise false.
 */
_Bool GUIManagerTryLock(void);

/*!
 * @brief Lock the GUI manager.
 */
void GUIManagerLock(void);

/*!
 * @brief Unlock the GUI manager.
 */
void GUIManagerUnlock(void);

#endif
