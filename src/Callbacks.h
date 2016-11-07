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

/*!
 * @header
 * These callbacks should be set prior to any function from the library is called.
 */
#ifndef CommonGameKit_Callbacks_h
#define CommonGameKit_Callbacks_h

#include <CommonC/Common.h>

typedef int (*CCEngineMain)(int argc, const char *argv[]);

/*!
 * @brief Call the libraries initial setup/main entrypoint. This should be called
 *        inside your main function.
 *
 * @description All required callbacks should be set before this call and no library
 *              functions should be called prior to this function. After this call
 *              your engine is required to setup any of the required modules.
 *
 * @param Main The function to be called after CCMain has successfully been executed.
 * @param argc The number of entries in argv.
 * @param argv The program inputs.
 * @result The error code upon completion/exit of your program.
 */
int CCMain(CCEngineMain Main, int argc, const char *argv[]);

/*!
 * @brief The path for the core assets.
 * @description Use these paths to load the required assets. This will be set
 *              automatically in @b CCMain. However if the engine chooses to
 *              move the contents anywhere else, this path should be re-set. If
 *              re-setting, the current value should be destroyed.
 */
extern FSPath CCAssetPath;

/*!
 * @brief Set this to a function that can provide a high frequency timestamp.
 * @warning Must not be NULL.
 * @result The timestamp.
 */
extern double (*CCTimestamp)(void);

#endif
