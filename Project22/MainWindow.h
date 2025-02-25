#pragma once
#include "BaseWin.h"
#include <d2d1.h>
#include <vector>

#pragma comment(lib, "d2d1.lib")

struct Circle {
    D2D1::ColorF color;
    float x, y, radius;
    float mass;
    float vx, vy;
    float gravity;
    float bounce;
};

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

class MainWindow : public WindowBase<MainWindow>
{
	ID2D1Factory* pFactory;
	ID2D1HwndRenderTarget* pRenderTarget;
	ID2D1SolidColorBrush* pPBrush;
    std::vector<ID2D1SolidColorBrush*> colors = {
        {nullptr},
        {nullptr},
        {nullptr}
    };
    std::vector<Circle> circles = {
    { { 255, 0, 0 } , 200, 200, 50, 150, 0, 0, 0.5f, 0.8f},
    { { 0, 255, 0 } , 300, 200, 50, 100, 0, 0, 0.5f, 0.8f},
    { { 0 , 0 , 255 }, 400, 200, 50, 100, 0, 0, 0.5f, 0.8f}
    };

    void DrawCircle(ID2D1HwndRenderTarget* pRT, ID2D1SolidColorBrush* pBrush, Circle& circle);
    void InitD2D();
    void OnPaint();
    void OnKeyDown(WPARAM wParam);
    void OnTimerClock();
    void OnSizeChanged();
    void HandleCollisions();
    PCWSTR  ClassName() const { return L"Direct2D App"; }

public:

    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pPBrush(NULL) {};

    LRESULT HandleMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);

    ~MainWindow() {};

};

