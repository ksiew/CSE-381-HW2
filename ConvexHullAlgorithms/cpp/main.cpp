#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>

#include <list>
#include <memory>
using namespace std;

#pragma comment(lib, "d2d1")

#include "basewin.h"
#include "resource.h"

int currAlgo = 0;

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

class DPIScale
{
    static float scaleX;
    static float scaleY;

public:
    static void Initialize(ID2D1Factory *pFactory)
    {
        FLOAT dpiX, dpiY;
        pFactory->GetDesktopDpi(&dpiX, &dpiY);
        scaleX = dpiX/96.0f;
        scaleY = dpiY/96.0f;
    }

    template <typename T>
    static float PixelsToDipsX(T x)
    {
        return static_cast<float>(x) / scaleX;
    }

    template <typename T>
    static float PixelsToDipsY(T y)
    {
        return static_cast<float>(y) / scaleY;
    }
};

float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

struct MyEllipse
{
    D2D1_ELLIPSE    ellipse;
    D2D1_COLOR_F    color;

    void Draw(ID2D1RenderTarget *pRT, ID2D1SolidColorBrush *pBrush)
    {
        pBrush->SetColor(color);
        pRT->FillEllipse(ellipse, pBrush);
        pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
        pRT->DrawEllipse(ellipse, pBrush, 1.0f);
    }

    //checks if given coordinates overlap with this ellipse
    BOOL HitTest(float x, float y)
    {
        const float a = ellipse.radiusX;
        const float b = ellipse.radiusY;
        const float x1 = x - ellipse.point.x;
        const float y1 = y - ellipse.point.y;
        const float d = ((x1 * x1) / (a * a)) + ((y1 * y1) / (b * b));
        return d <= 1.0f;
    }

    D2D1_POINT_2F getPoint() {
        return ellipse.point;
    }
};

struct Edge
{
    MyEllipse    ellipse1;
    MyEllipse   ellipse2;
    D2D1_COLOR_F    color;

    void Draw(ID2D1RenderTarget* pRT, ID2D1SolidColorBrush* pBrush)
    {
        pBrush->SetColor(color);
        pRT->DrawLine(ellipse1.getPoint(), ellipse2.getPoint(),  pBrush, 0.5f);
    }

};

/*Struct for a graph
* 
* @param allEllipses: List of all ellipses in graph
* @param outerEllipses: List of all outer ellipses
* @param edges: List of all edges
* 
* @see EdgeSelection
*/
struct Graph {
    list<shared_ptr<MyEllipse>>             allEllipses;
    list<shared_ptr<MyEllipse>>::iterator   allSelection;

    list<shared_ptr<MyEllipse>>             outerEllipses;
    list<shared_ptr<MyEllipse>>::iterator   outerSelection;

    list< shared_ptr<Edge >>                     edges;
    list< shared_ptr<Edge >>::iterator   edgeSelection;

    shared_ptr<MyEllipse> AllSelection()
    {
        if (allSelection == allEllipses.end())
        {
            return nullptr;
        }
        else
        {
            return (*allSelection);
        }
    }
    
    shared_ptr<MyEllipse> OuterSelection()
    {
        if (outerSelection == outerEllipses.end())
        {
            return nullptr;
        }
        else
        {
            return (*outerSelection);
        }
    }

    shared_ptr<Edge> EdgeSelection()
    {
        if (edgeSelection == edges.end())
        {
            return nullptr;
        }
        else
        {
            return (*edgeSelection);
        }
    }

    /*Goes through list of all ellipses and adds whiceverh ones belong to the outer edge to OuterEllipses
    */
    void findOuter() {
        switch (currAlgo) {
            //TODO
        }
    }

    /*Goes through list of all Outer ellipses and creates edges using 
    */
    void findEdges() {
        switch (currAlgo) {
            //TODO
        }
    }

    void calculate() {
        findOuter();
        findEdges();

    }

    void clear() {
        allEllipses.clear();
        outerEllipses.clear();
        edges.clear();
    }

};

D2D1::ColorF::Enum colors[] = { D2D1::ColorF::LimeGreen };


class MainWindow : public BaseWindow<MainWindow>
{
    enum Mode
    {
        DrawMode,
        SelectMode,
        DragMode
    };

    HCURSOR                 hCursor;

    ID2D1Factory            *pFactory;
    ID2D1HwndRenderTarget   *pRenderTarget;
    ID2D1SolidColorBrush    *pBrush;
    D2D1_POINT_2F           ptMouse;

    Mode                    mode;
    size_t                  nextColor;

    //draw lists, everything in these will be drawn
    list<shared_ptr<MyEllipse>>             ellipses;
    list<shared_ptr<MyEllipse>>::iterator   selection;

    list< shared_ptr<Edge >>                     edges;
    list< shared_ptr<Edge >>::iterator   edgeSelection;

    //graphs to be used
    Graph graph1;
    Graph graph2;
    Graph graph3;
    Graph convexGraph;


     
    shared_ptr<MyEllipse> Selection() 
    { 
        if (selection == ellipses.end()) 
        { 
            return nullptr;
        }
        else
        {
            return (*selection);
        }
    }

    shared_ptr<Edge> EdgeSelection()
    {
        if (edgeSelection == edges.end())
        {
            return nullptr;
        }
        else
        {
            return (*edgeSelection);
        }
    }

    void    ClearSelection() { selection = ellipses.end(); }
    HRESULT InsertEllipse(float x, float y);
    void fillDraw(Graph* graph);
    HRESULT InsertEdge(MyEllipse p1, MyEllipse p2);
    HRESULT InsertEllipseGraph(Graph* graph, float x, float y);

    BOOL    HitTest(float x, float y);
    void    SetMode(Mode m);
    void    checkEdges(shared_ptr<MyEllipse>);
    void    MoveSelection(float x, float y);
    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    setAlgo(int algo);
    void    OnPaint();
    void    Resize();
    void    OnLButtonDown(int pixelX, int pixelY, DWORD flags);
    void    OnLButtonUp();
    void    OnMouseMove(int pixelX, int pixelY, DWORD flags);

public:

    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL), 
        ptMouse(D2D1::Point2F()), nextColor(0), selection(ellipses.end())
    {
    }

    PCWSTR  ClassName() const { return L"Circle Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

HRESULT MainWindow::CreateGraphicsResources()
{
    HRESULT hr = S_OK;
    if (pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &pRenderTarget);

        if (SUCCEEDED(hr))
        {
            const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
            hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
        }
    }
    return hr;
}

//deletes currently drawn images
void MainWindow::DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
    SafeRelease(&pBrush);
}

/*Adds all edges and ellipses of a graph to drawing ellipses and edges
* 
* @param Graph: pointer to graph whose ellipses and edges are being added
*/
void MainWindow::fillDraw(Graph* graph) {

    (*graph).calculate();
    for (shared_ptr<MyEllipse> p : graph->allEllipses) {
        selection = ellipses.insert(ellipses.end(), 1, p);
    }
    for (shared_ptr<Edge> p : graph->edges) {
        edgeSelection = edges.insert(edges.end(), 1, p);
    }
    
}

//tells D2D1 what needs to been drawn
void MainWindow::OnPaint()
{
    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr))
    {
        if (currAlgo == PCHULL) fillDraw(&convexGraph);

        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);
     
        pRenderTarget->BeginDraw();

        pRenderTarget->Clear( D2D1::ColorF(D2D1::ColorF::SkyBlue) );

        for (auto i = ellipses.begin(); i != ellipses.end(); ++i)
        {
            (*i)->Draw(pRenderTarget, pBrush);
        }

        for (auto i = edges.begin(); i != edges.end(); ++i)
        {
            (*i)->Draw(pRenderTarget, pBrush);
        }

        /*
        * not sure why this stuff is here
        if (Selection())
        {
            pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
            pRenderTarget->DrawEllipse(Selection()->ellipse, pBrush, 2.0f);
        }

        if (EdgeSelection())
        {
            pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
            pRenderTarget->DrawLine(EdgeSelection()->ellipse1.getPoint(), EdgeSelection()->ellipse2.getPoint(), pBrush, 2.0f);
        }
        */

        hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }
}

void MainWindow::Resize()
{
    if (pRenderTarget != NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        pRenderTarget->Resize(size);

        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}

//for dragging dots
void MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
    const float dipX = DPIScale::PixelsToDipsX(pixelX);
    const float dipY = DPIScale::PixelsToDipsY(pixelY);

    if (mode == DrawMode)
    {
     
    }
    else
    {
        ClearSelection();

        if (HitTest(dipX, dipY))
        {
            SetCapture(m_hwnd);

            ptMouse = Selection()->ellipse.point;
            ptMouse.x -= dipX;
            ptMouse.y -= dipY;

            SetMode(DragMode);
        }
    }
    InvalidateRect(m_hwnd, NULL, FALSE);
}

//tells window dot is no longer being dragged
void MainWindow::OnLButtonUp()
{
    if ((mode == DrawMode) && Selection())
    {
        ClearSelection();
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
    else if (mode == DragMode)
    {
        SetMode(SelectMode);
    }
    ReleaseCapture(); 
}


void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
    const float dipX = DPIScale::PixelsToDipsX(pixelX);
    const float dipY = DPIScale::PixelsToDipsY(pixelY);

    if ((flags & MK_LBUTTON) && Selection())
    { 
        if (mode == DragMode)
        {
            // Move the ellipse.
            Selection()->ellipse.point.x = dipX + ptMouse.x;
            Selection()->ellipse.point.y = dipY + ptMouse.y;
        }
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}



//this function checks all ellipses and inserts an edge if one is valid
void MainWindow::checkEdges(shared_ptr<MyEllipse> curr) {
    for (auto i = ellipses.rbegin(); i != ellipses.rend(); ++i) {
        if (*i != curr) {
            switch (currAlgo) {
            case MDIFFERENCE:

                break;

            case MSUM:

                break;

            case QHULL:

                break;

            case PCHULL:

                break;

            case GJK:

                break;
            }
        }
    }
}

//adds ellipse to draw list
HRESULT MainWindow::InsertEllipse(float x, float y)
{
    try
    {
        selection = ellipses.insert(
            ellipses.end(), 
            shared_ptr<MyEllipse>(new MyEllipse()));

        Selection()->ellipse.point = ptMouse = D2D1::Point2F(x, y);
        Selection()->ellipse.radiusX = Selection()->ellipse.radiusY = 10.0f;
        Selection()->color = D2D1::ColorF( colors[nextColor] );
        checkEdges(Selection());
        nextColor = (nextColor + 1) % ARRAYSIZE(colors);
    }
    catch (std::bad_alloc)
    {
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

/*inserts ellipse into a specific graph
* @param graph: pointer to graph ellipse is being inserted into
* @param x: x coordinate of ellipse
* @param y: y coordinate of ellipse

*/
HRESULT MainWindow::InsertEllipseGraph(Graph *graph, float x, float y)
{
    try
    {
        graph->allSelection = graph->allEllipses.insert(
            graph->allEllipses.end(),
            shared_ptr<MyEllipse>(new MyEllipse()));

        graph->AllSelection()->ellipse.point = ptMouse = D2D1::Point2F(x, y);
        graph->AllSelection()->ellipse.radiusX = graph->AllSelection()->ellipse.radiusY = 10.0f;
        graph->AllSelection()->color = D2D1::ColorF(colors[nextColor]);
        nextColor = (nextColor + 1) % ARRAYSIZE(colors);

    }
    catch (std::bad_alloc)
    {
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

//takes 2 ellipses and adds an edge into the draw list
HRESULT MainWindow::InsertEdge(MyEllipse p1, MyEllipse p2)
{
    try
    {
        edgeSelection = edges.insert(
            edges.end(),
            shared_ptr<Edge>(new Edge()));

        EdgeSelection()->ellipse1 = p1;
        EdgeSelection()->ellipse2 = p2;
        EdgeSelection()->color = D2D1::ColorF(colors[nextColor]);
        nextColor = (nextColor + 1) % ARRAYSIZE(colors);
    }
    catch (std::bad_alloc)
    {
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

//runs ellipse hittest function on all dots
BOOL MainWindow::HitTest(float x, float y)
{
    for (auto i = ellipses.rbegin(); i != ellipses.rend(); ++i)
    {
        if ((*i)->HitTest(x, y))
        {
            selection = (++i).base();
            return TRUE;
        }
    }
    return FALSE;
}

void MainWindow::MoveSelection(float x, float y)
{
    if ((mode == SelectMode) && Selection())
    {
        Selection()->ellipse.point.x += x;
        Selection()->ellipse.point.y += y;
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}

void MainWindow::SetMode(Mode m)
{
    mode = m;

    LPWSTR cursor;
    switch (mode)
    {
    case DrawMode:
        cursor = IDC_CROSS;
        break;

    case SelectMode:
        cursor = IDC_HAND;
        break;

    case DragMode:
        cursor = IDC_SIZEALL;
        break;
    }

    hCursor = LoadCursor(NULL, cursor);
    SetCursor(hCursor);
}
HWND CreateButton(HWND m_hWnd, int algo) {
    HFONT hFont = CreateFont(8, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
    LPCWSTR text;
    switch (algo) {
    case MDIFFERENCE:
        text = L"MDIF";
        break;

    case MSUM:
        text = L"MSUM";
        break;

    case QHULL:
        text = L"QH";
        break;

    case PCHULL:
        text = L"PCH";
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
        0,         // x position 
        50 + algo,         // y position 
        50,        // Button width
        50,        // Button height

        m_hWnd,       // Parent window   
        (HMENU)algo,
        (HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE),
        NULL        // Additional application data
    );
    return hwnd;

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    MainWindow win;

    if (!win.Create(L"Draw Circles", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL1));

    CreateButton(win.Window(), MDIFFERENCE);
    CreateButton(win.Window(), MSUM);
    CreateButton(win.Window(), QHULL);
    CreateButton(win.Window(), PCHULL);
    CreateButton(win.Window(), GJK);
    ShowWindow(win.Window(), nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(win.Window(), hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}

//Occurs when button is pressed, resets dots and changes alogrithm
 void MainWindow::setAlgo(int algo) {
    currAlgo = algo;
    ellipses.clear();
    edges.clear();
    graph1.clear();
    graph2.clear();
    graph3.clear();
    DiscardGraphicsResources();
    RECT rc;
    GetClientRect(m_hwnd, &rc);

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    float x = ((width/2 - 180) * ((float)rand() / RAND_MAX)) + 80;
    float y = ((height/2 - 60) * ((float)rand() / RAND_MAX)) + 10;

    InsertEllipseGraph(&graph1, x, y);
    x = ((width / 2 - 80) * ((float)rand() / RAND_MAX)) + 80;
    y = ((height / 2 - 10) * ((float)rand() / RAND_MAX)) + 10;
    InsertEllipseGraph(&graph1, x, y);
    x = ((width / 2 - 80) * ((float)rand() / RAND_MAX)) + 80;
    y = ((height / 2 - 10) * ((float)rand() / RAND_MAX)) + 10;
    InsertEllipseGraph(&graph2, x, y);
    x = ((width / 2 - 80) * ((float)rand() / RAND_MAX)) + 80;
    y = ((height / 2 - 10) * ((float)rand() / RAND_MAX)) + 10;
   InsertEllipseGraph(&graph3, x, y);
   //x = ((width / 2 - 80) * ((float)rand() / RAND_MAX)) + 80;
   //y = ((height / 2 - 10) * ((float)rand() / RAND_MAX)) + 10;
   // InsertEllipse(x, y);
    fillDraw(&graph1);
    fillDraw(&graph2);
    fillDraw(&graph3);
  

    InvalidateRect(m_hwnd, NULL, FALSE);
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        if (FAILED(D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;  // Fail CreateWindowEx.
        }
        DPIScale::Initialize(pFactory);
        SetMode(SelectMode);
        return 0;

    case WM_DESTROY:
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        OnPaint();
        return 0;

    case WM_SIZE:
        Resize();
        return 0;

    case WM_LBUTTONDOWN: 
        OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        return 0;

    case WM_LBUTTONUP: 
        OnLButtonUp();
        return 0;

    case WM_MOUSEMOVE: 
        OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        return 0;

    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT)
        {
            SetCursor(hCursor);
            return TRUE;
        }
        break;

    case WM_KEYDOWN:
        return 0;

    case WM_SETFONT:


    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case MDIFFERENCE:
            setAlgo( MDIFFERENCE);
            break;

        case MSUM:
            setAlgo(MSUM);
            break;

        case QHULL:
            setAlgo (QHULL);
            break;

        case PCHULL:
            setAlgo( PCHULL);
            break;

        case GJK:
            setAlgo( GJK);
            break;

        case ID_DRAW_MODE:
            SetMode(DrawMode);
            break;

        case ID_SELECT_MODE:
            SetMode(SelectMode);
            break;

        case ID_TOGGLE_MODE:
            if (mode == DrawMode)
            {
                SetMode(SelectMode);
            }
            else
            {
                SetMode(DrawMode);
            }
            break;
        }
        return 0;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
