#include <windows.h>
#include <d2d1.h>
#include <math.h>
#include <iostream>

#pragma comment(lib, "d2d1")

#define ID_TIMER 1
#define FORCE 50 

int wh = 500;
int ww = 500;

struct Circle {
    float x, y, radius;
    float vx, vy;
    float gravity;
    float bounce;
};

Circle circle = { 200, 200, 50, 0, 0, 0.5f, 0.8f };
Circle Usil = { 500, 300, 30, 0, 0, 0 };

ID2D1Factory* pFactory = NULL;
ID2D1HwndRenderTarget* pRenderTarget = NULL;
ID2D1SolidColorBrush* pBrush = NULL;

void DrawCircle(ID2D1HwndRenderTarget* pRT, ID2D1SolidColorBrush* pBrush, Circle& circle) {
    pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(circle.x, circle.y), circle.radius, circle.radius), pBrush);
}

void InitD2D(HWND hwnd) {
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    pFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hwnd, size),
        &pRenderTarget);

    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        pRenderTarget->BeginDraw();
        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
        DrawCircle(pRenderTarget, pBrush, circle);
        //DrawCircle(pRenderTarget, pBrush, Usil);
        pRenderTarget->EndDraw();
        EndPaint(hwnd, &ps);
    }
    break;

    case WM_SIZE:

        if (pRenderTarget) {
            RECT rc;
            GetClientRect(hwnd, &rc);
            D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
            pRenderTarget->Resize(size);
        }
        break;

    case WM_TIMER:

        circle.vy += circle.gravity;

        circle.x += (int)circle.vx;
        circle.y += (int)circle.vy;
        RECT rect;

        GetClientRect(hwnd, &rect);
        if (circle.x - circle.radius < 0) {
            circle.x = circle.radius;
            circle.vx = -circle.vx * circle.bounce;
        }
        if (circle.x + circle.radius > rect.right) {
            circle.x = rect.right - circle.radius;
            circle.vx = -circle.vx * circle.bounce;
        }


        if (circle.y - circle.radius < 0) {
            circle.y = circle.radius;
            circle.vy = -circle.vy * circle.bounce;
        }
        if (circle.y + circle.radius > rect.bottom) {
            circle.y = rect.bottom - circle.radius;
            circle.vy = -circle.vy * circle.bounce;
        }

        if (circle.x - circle.radius < 0) circle.x = circle.radius;
        if (circle.x + circle.radius > rect.right) circle.x = rect.right - circle.radius;
        if (circle.y - circle.radius < 0) circle.y = circle.radius;
        if (circle.y + circle.radius > rect.bottom) circle.y = rect.bottom - circle.radius;

        InvalidateRect(hwnd, NULL, FALSE);
        break;

    case WM_CREATE:
        InitD2D(hwnd);
        SetTimer(hwnd, ID_TIMER, 1, NULL);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_LEFT) circle.vx += -FORCE;
        if (wParam == VK_RIGHT) circle.vx += FORCE;
        if (wParam == VK_UP) circle.vy += -FORCE;
        if (wParam == VK_DOWN) circle.vy += FORCE;
        InvalidateRect(hwnd, NULL, FALSE);
        break;

    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER);
        if (pBrush) pBrush->Release();
        if (pRenderTarget) pRenderTarget->Release();
        if (pFactory) pFactory->Release();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int main() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"CircleApp";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, L"Direct2D Circle App", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, ww, wh, NULL, NULL, wc.hInstance, NULL);

    if (!hwnd) return -1;

    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
