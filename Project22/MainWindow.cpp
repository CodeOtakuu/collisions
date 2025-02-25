#include "MainWindow.h"

void MainWindow::DrawCircle(ID2D1HwndRenderTarget* pRT, ID2D1SolidColorBrush* pBrush, Circle& circle)
{
	pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(circle.x, circle.y), circle.radius, circle.radius), pBrush);
}

void MainWindow::InitD2D()
{
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
    RECT rc;
    GetClientRect(hWnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    pFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hWnd, size),
        &pRenderTarget);

    for (int i = 0; i < colors.size(); i++) {

        pRenderTarget->CreateSolidColorBrush(circles[i].color, &colors[i]);

    }

    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pPBrush);
}

void MainWindow::OnPaint()
{
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    for (int i = 1; i < circles.size(); i++) {
        DrawCircle(pRenderTarget, colors[i], circles[i]);
        DrawCircle(pRenderTarget, pPBrush, circles[0]);
    }



    pRenderTarget->EndDraw();
    EndPaint(hWnd, &ps);
}

void MainWindow::OnKeyDown(WPARAM wParam)
{
    if (wParam == VK_LEFT) circles[0].vx += -50;
    if (wParam == VK_RIGHT) circles[0].vx += 50;
    if (wParam == VK_UP) circles[0].vy += -50;
    if (wParam == VK_DOWN) circles[0].vy += 50;
    InvalidateRect(hWnd, NULL, FALSE);
}

void MainWindow::OnTimerClock()
{
    for (auto& circle : circles) {
        circle.vy += circle.gravity;
        circle.x += circle.vx;
        circle.y += circle.vy;

        RECT rect;
        GetClientRect(hWnd, &rect);
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
    InvalidateRect(hWnd, NULL, FALSE);
}

void MainWindow::OnSizeChanged()
{
    if (pRenderTarget) {
        RECT rc;
        GetClientRect(hWnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
        pRenderTarget->Resize(size);
    }
}

void MainWindow::HandleCollisions()
{
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

LRESULT MainWindow::HandleMessage(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    switch (uMessage)
    {
    case WM_CREATE:
        SetTimer(hWnd, 0, 1, NULL);
        InitD2D();
        break;
    case WM_PAINT:
        OnPaint();
        break;
    case WM_TIMER:
        OnTimerClock();
        break;
    case WM_SIZE:
        OnSizeChanged();
        break;
    case WM_KEYDOWN:
        OnKeyDown(wParam);
        break;
    case WM_DESTROY:
        for (auto& c : colors) {
            SafeRelease(&c);
        }
        SafeRelease(&pPBrush);
        SafeRelease(&pRenderTarget);
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMessage, wParam, lParam);;
    }
    return 0;
}
