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

#include "display_state.h"

WINDOWPLACEMENT g_wpPrev = {sizeof(g_wpPrev)};

void WndRestore(HWND hwnd, DWORD dwStyle)
{
  SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
  SetWindowPlacement(hwnd, &g_wpPrev);
  SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                   SWP_FRAMECHANGED);
}

void WndResizeFullScreen(HWND hwnd, DWORD dwStyle)
{
  MONITORINFO mi = {sizeof(mi)};
  if (GetWindowPlacement(hwnd, &g_wpPrev) &&
      GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi))
  {
    SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
    SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left,
                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  }
}

bool SetFullScreen(int width, int height)
{
  DEVMODE mode{};
  if (!EnumDisplaySettings(nullptr, 0, &mode))
    return false;

  mode.dmPelsWidth        = width;
  mode.dmPelsHeight       = height;
  mode.dmDisplayFrequency = 60;

  // Change display mode.
  auto r =
      ChangeDisplaySettingsEx(nullptr, &mode, nullptr, CDS_FULLSCREEN, nullptr);

  return r == DISP_CHANGE_SUCCESSFUL;
}

bool ExitFullScreen()
{
  // Restore default display mode.
  auto r = ChangeDisplaySettingsEx(nullptr, nullptr, nullptr, 0, nullptr);
  return r == DISP_CHANGE_SUCCESSFUL;
}
