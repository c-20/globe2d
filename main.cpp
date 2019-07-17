#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>

#define WINVER  0x0500
#include <windows.h>

#include <stdio.h>

#include <math.h>
#include "globe2d.h"

#define GLOBE2DTITLETEXT    "globe2d"
#define GLOBE2DCLASSNAME    "globe2d"
#define GLOBE2DWINWIDTH     800
#define GLOBE2DWINHEIGHT    600

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T(GLOBE2DCLASSNAME);

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow) {
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T(GLOBE2DTITLETEXT), /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           GLOBE2DWINWIDTH,     /* The programs width */
           GLOBE2DWINHEIGHT,    /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
//    HDC dc = GetDC(hwnd);

    globe2dinit();

 //   SetCapture(hwnd);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0)) {
        InvalidateRect(hwnd, NULL, TRUE);
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);


//        globe2dupdate(dc);

        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    globe2dfree();

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  HDC dc, bmpdc;
  HGDIOBJ oldobj;
  HBITMAP bmph;
  PAINTSTRUCT ps;
  RECT client;
  int clientw, clienth;
  switch (message) {
    case WM_ERASEBKGND:
//      return 1;
//      dc = BeginPaint(hwnd, &ps);
    //  dc = GetDC(hwnd);
//      FillRect(dc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
//      EndPaint(hwnd, &ps);
      return 1;
    case WM_MOUSEMOVE:
      if (wParam == VK_ESCAPE)
        { PostQuitMessage(0); }
      break;
    case WM_PAINT:
//      Sleep(50); // 20fps limiter
//      InvalidateRect(hwnd, NULL, TRUE);
      GetClientRect(hwnd, &client);
      clientw = client.right - client.left;
      clienth = client.bottom - client.top;
      dc = BeginPaint(hwnd, &ps);
      bmpdc = CreateCompatibleDC(dc);
      bmph = CreateCompatibleBitmap(dc, clientw, clienth); // bmpdc gives B/W
      oldobj = SelectObject(bmpdc, bmph); // draw to bmp
      FillRect(bmpdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

      globe2dupdate(bmpdc, clientw, clienth);
//      SaveBitmap() ..considering run length encoding... + APNG converter

      BitBlt(dc, 0, 0, clientw, clienth, bmpdc, 0, 0, SRCCOPY);
      SelectObject(bmpdc, oldobj); // back to window
      DeleteObject(bmph);
      DeleteDC(bmpdc);
      EndPaint(hwnd, &ps);
      // paint as fast as possible - trigger another paint
      InvalidateRect(hwnd, NULL, FALSE);
      RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

/* save bitmap example
#include <vector>
#include <fstream>
#include <cstring>
#include <windows.h>
bool HDCToFile(const char* FilePath, HDC Context, RECT Area, uint16_t BitsPerPixel = 24)
{
    uint32_t Width = Area.right - Area.left;
    uint32_t Height = Area.bottom - Area.top;
    BITMAPINFO Info;
    BITMAPFILEHEADER Header;
    memset(&Info, 0, sizeof(Info));
    memset(&Header, 0, sizeof(Header));
    Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    Info.bmiHeader.biWidth = Width;
    Info.bmiHeader.biHeight = Height;
    Info.bmiHeader.biPlanes = 1;
    Info.bmiHeader.biBitCount = BitsPerPixel;
    Info.bmiHeader.biCompression = BI_RGB;
    Info.bmiHeader.biSizeImage = Width * Height * (BitsPerPixel > 24 ? 4 : 3);
    Header.bfType = 0x4D42;
    Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    char* Pixels = NULL;
    HDC MemDC = CreateCompatibleDC(Context);
    HBITMAP Section = CreateDIBSection(Context, &Info, DIB_RGB_COLORS, (void**)&Pixels, 0, 0);
    DeleteObject(SelectObject(MemDC, Section));
    BitBlt(MemDC, 0, 0, Width, Height, Context, Area.left, Area.top, SRCCOPY);
    DeleteDC(MemDC);
    std::fstream hFile(FilePath, std::ios::out | std::ios::binary);
    if (hFile.is_open())
    {
        hFile.write((char*)&Header, sizeof(Header));
        hFile.write((char*)&Info.bmiHeader, sizeof(Info.bmiHeader));
        hFile.write(Pixels, (((BitsPerPixel * Width + 31) & ~31) / 8) * Height);
        hFile.close();
        DeleteObject(Section);
        return true;
    }
    DeleteObject(Section);
    return false;
}
int main()
{
    HWND win = GetDesktopWindow();
    HDC dc = GetDC(win);
    HDCToFile("C:/.../Desktop/Foo.bmp", dc, {0, 0, 1366, 768});
    ReleaseDC(win, dc);
}
*/
