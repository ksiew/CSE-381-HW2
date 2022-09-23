// ConvexHullAlgorithms.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ConvexHullAlgorithms.h"

#define MDIFFERENCE 50
#define MSUM 100
#define QHULL 150
#define PCHULL 200
#define GJK 250
#define MAX_LOADSTRING 100

// Global Variables:
int currAlgo = 0;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_CONVEXHULLALGORITHMS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CONVEXHULLALGORITHMS));

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

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CONVEXHULLALGORITHMS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HWND CreateButton(HWND m_hWnd, int algo){
    LPCWSTR text;
    switch (algo) {
        case MDIFFERENCE:
            text = L"MDIFFERENCE";
            break;

        case MSUM:
            text = L"MSUM";
            break;

        case QHULL:
            text = L"QUICK HULL";
            break;

        case PCHULL:
            text = L"PC HULL";
            break;

        case GJK:
            text = L"GJK";
            break;

        default:
            text = L"err";
            break;
    }

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        L"BUTTON",                     // Window class
        text,    // Window text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,           // Window style

        // Size and position
        10,         // x position 
        50 + algo,         // y position 
        40,        // Button width
        50,        // Button height

        m_hWnd,       // Parent window   
        (HMENU)algo,
        (HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE),
        NULL        // Additional application data
    );
    return hwnd;

}

void  createEllipses(HWND m_hWnd, int num) {
    for (int i = 0; i < num; i++) {
        createEllipse(m_hWnd);
    }
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

    case WM_CREATE:
        CreateButton(hWnd, MDIFFERENCE);
        CreateButton(hWnd, MSUM);
        CreateButton(hWnd, QHULL);
        CreateButton(hWnd, PCHULL);
        CreateButton(hWnd, GJK);
        break;

    case WM_COMMAND:
        {
            // Parse the menu selections:
            switch (message)
            {
            case BN_CLICKED:
                currAlgo = wParam;
                createEllipses(hWnd, 10);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
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
