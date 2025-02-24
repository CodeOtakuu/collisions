#include <windows.h>
#include <d2d1.h>
#include <math.h>
#include <vector>
#include <iostream>

#pragma comment(lib, "d2d1")

#define ID_TIMER 1
#define FORCE 50 

int wh = 500;
int ww = 500;
bool pUp = false;

struct Circle {
    D2D1::ColorF color;
    float x, y, radius;
    float mass;
    float vx, vy;
    float gravity;
    float bounce;
};

std::vector<Circle> circles = {
    { { 255, 0, 0 } , 200, 200, 50, 150, 0, 0, 0.5f, 0.8f},
    { { 0, 255, 0 } , 300, 200, 50, 100, 0, 0, 0.5f, 0.8f},
    { { 0 , 0 , 255 }, 400, 200, 50, 100, 0, 0, 0.5f, 0.8f}
};

ID2D1Factory* pFactory = NULL;
ID2D1HwndRenderTarget* pRenderTarget = NULL;
ID2D1SolidColorBrush* pPBrush = NULL;

std::vector<ID2D1SolidColorBrush*> colors(3, nullptr);

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

    for (int i = 0; i < colors.size(); i++) {

        pRenderTarget->CreateSolidColorBrush(circles[i].color, &colors[i]);

    }

    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pPBrush);
}

void HandleCollisions() {
    float restitution = 0.8f;

    for (size_t i = 0; i < circles.size(); ++i) {
        for (size_t j = i + 1; j < circles.size(); ++j) {
            float dx = circles[j].x - circles[i].x;
            float dy = circles[j].y - circles[i].y;
            float distance = sqrt(dx * dx + dy * dy);
            float minDist = circles[i].radius + circles[j].radius;

            if (distance < minDist) {
                float overlap = minDist - distance;
                float nx = dx / distance;
                float ny = dy / distance;

                if (pUp) {
                    circles[i].vx = 100;
                    circles[i].vy = 150;
                }
                float vx1 = circles[i].vx;
                float vy1 = circles[i].vy;
                float vx2 = circles[j].vx;
                float vy2 = circles[j].vy;

                float dotProduct = (vx1 - vx2) * nx + (vy1 - vy2) * ny;


                if (dotProduct < 0) {
                    float mass1 = circles[i].mass;
                    float mass2 = circles[j].mass;


                    float impulse = 2 * dotProduct / (mass1 + mass2);


                    float velocityChange1 = impulse * mass2 * restitution;
                    float velocityChange2 = impulse * mass1 * restitution;

                    circles[i].vx -= velocityChange1 * nx;
                    circles[i].vy -= velocityChange1 * ny;
                    circles[j].vx += velocityChange2 * nx;
                    circles[j].vy += velocityChange2 * ny;

                    circles[i].x -= nx * (overlap / 2);
                    circles[i].y -= ny * (overlap / 2);
                    circles[j].x += nx * (overlap / 2);
                    circles[j].y += ny * (overlap / 2);
                }
            }
        }
    }
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        pRenderTarget->BeginDraw();
        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

        for (int i = 1; i < circles.size(); i++) {
            DrawCircle(pRenderTarget, colors[i], circles[i]);
            DrawCircle(pRenderTarget, pPBrush, circles[0]);
        }



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
        for (auto& circle : circles) {
            circle.vy += circle.gravity;
            circle.x += circle.vx;
            circle.y += circle.vy;

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
        }
        HandleCollisions();
        InvalidateRect(hwnd, NULL, FALSE);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_LEFT) circles[0].vx += -FORCE;
        if (wParam == VK_RIGHT) circles[0].vx += FORCE;
        if (wParam == VK_UP) circles[0].vy += -FORCE;
        if (wParam == VK_DOWN) circles[0].vy += FORCE;
        if (wParam == VK_F1) pUp = !pUp;
        InvalidateRect(hwnd, NULL, FALSE);
        break;



    case WM_CREATE:
        InitD2D(hwnd);
        SetTimer(hwnd, ID_TIMER, 1, NULL);
        break;

    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER);
        for (auto& c : colors) {
            c->Release();
        }
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
