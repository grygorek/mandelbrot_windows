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

// mandelbrot.cpp : Defines the entry point for the application.
//
#include <windows.h>
#include <gdiplus.h>
#include <objidl.h>

// remove windows crap
#ifdef min
#undef min
#endif

#include <thread>
#include <memory>
#include "framework.h"
#include "resource.h"
#include "display_state.h"
#include "image.h"
#include "mandel_algo.h"
#include "debug_output.h"

#pragma comment(lib, "Gdiplus.lib")


#define MAX_LOADSTRING 100
#define WM_REDRAW (WM_USER + 1)


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

Image g_image{ 800, 600 };             // rendered image
bool g_fImageReady{ false };
MandelbrotParams fractalParams;

extern std::unique_ptr<Gdiplus::Bitmap> g_bitmap; // see win_drawing.cpp


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void OnPaint(HDC& hnd, Image& image);

void RenderPicture(HWND hWnd) {
	Mandelbrot_Image(fractalParams, g_image.width, g_image.height,
		[](int x, int y, const Image::Colour& c) {
			g_image.Pixel(x, y, c);
		});

	g_fImageReady = true;
	PostMessage(hWnd, WM_REDRAW, 0, 0);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MANDELBROT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	HWND hWnd = InitInstance(hInstance, nCmdShow);
	if (!hWnd)
	{
		return FALSE;
	}

	// Initialize GDI+.
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	std::thread t{ RenderPicture, hWnd };

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MANDELBROT));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	t.join();

	g_bitmap = nullptr;

	Gdiplus::GdiplusShutdown(gdiplusToken);
	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MANDELBROT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;// MAKEINTRESOURCEW(IDC_MANDELBROT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, g_image.width,
		g_image.height, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return nullptr;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int zoom = 0;

void ZoomFractal(int x, int y, int zoom)
{
	// shift image vector in relation to middle of the screen
	auto vx = x - g_image.width / 2;
	auto vy = y - g_image.height / 2;

	// zoom in ratio of 80%
	auto tmp1 = fractalParams.x_range;
	auto tmp2 = fractalParams.y_range;
	fractalParams.x_range *= 0.8;
	fractalParams.y_range *= 0.8;

	// convert the shift vector from image resolution to the fractal 'resolution'
	auto dx = fractalParams.x_range / g_image.width * vx;
	auto dy = fractalParams.y_range / g_image.height * vy;

	// 
	tmp1 = (tmp1 - fractalParams.x_range) / 2;
	tmp2 = (tmp2 - fractalParams.y_range) / 2;
	fractalParams.x_start += tmp1 + dx;
	fractalParams.y_start += tmp2 + dy;
}

// Return current mouse cursor position in window coordinates
POINT MouseClick(HWND hWnd) {
	POINT pos{ 0,0 };
	if (FALSE == GetCursorPos(&pos))
		return pos;

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	POINT ptClientLR;
	ptClientLR.x = rcClient.left + 1;
	ptClientLR.y = rcClient.top + 1;
	ClientToScreen(hWnd, &ptClientLR);

	pos.x -= ptClientLR.x;
	pos.y -= ptClientLR.y;

	return pos;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_REDRAW:
		InvalidateRect(hWnd, nullptr, true);
		break;
	case WM_LBUTTONUP:
	{
		POINT pos = MouseClick(hWnd);

		ZoomFractal(pos.x, pos.y, 0);
		std::thread t{ RenderPicture, hWnd };
		t.detach();
		OutputDebugString(std::to_string(zoom) + "," + std::to_string(pos.x) + "," + std::to_string(pos.y) + "\n");

	}
	break;
	case WM_MOUSEWHEEL:
	{
		int(wParam) < 0 ? zoom-- : zoom++;
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		if (g_fImageReady)
		{
			OnPaint(hdc, g_image);
		}
		else
		{
			const std::string s{ "Rendering, plese wait..." };
			auto len = GetTabbedTextExtentA(hdc, s.c_str(), s.size(), 0, 0);
			auto w = len & 0xFFFF;
			auto h = len >> 16;
			TextOutA(hdc, (g_image.width - w) / 2, (g_image.height - h) / 2, s.c_str(), s.size());
		}
		EndPaint(hWnd, &ps);
		OutputDebugStringA("Paint\n");
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}