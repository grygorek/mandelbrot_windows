/// Copyright 2022 Piotr Grygorczuk <grygorek@gmail.com>
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.

#ifndef DISPLAY_STATE_H_
#define DISPLAY_STATE_H_

#include <Windows.h>

/// Restore original window size
///
/// @param hwnd - handle to a window
/// @param dwStyle - result of GetWindowLong(hwnd, GWL_STYLE);
void WndRestore(HWND hwnd, DWORD dwStyle);

/// Resize window to screen resolution
void WndResizeFullScreen(HWND hwnd, DWORD dwStyle);

/// Switch screen mode to full screen with provided resolution
///
/// @retval true - switched sucessfully
/// @retval false - failed to switch
///
/// @remarks New resolution must be supported
bool SetFullScreen(int width, int height);

/// Return from full screen to default screen resolution
///
/// @returns true if success, false otherwise
bool ExitFullScreen();

#endif
