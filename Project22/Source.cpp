#include <windows.h>
#include <math.h>
#include <iostream>
#include <sstream>

#define ID_TIMER 1
#define FORCE 50 //def50

int wh = 500;
int ww = 500;

struct Circle {
    int x, y, radius;
    float vx, vy; 
    float gravity;  
    float bounce;  
};

Circle circle = { 200, 200, 50, 0, 0, 0.5f, 0.8f }; 
Circle Usil = { 500, 300, 30, 0, 0, 0 };

void DrawCircle(HDC hdc, Circle& circle) {
    Ellipse(hdc, circle.x - circle.radius, circle.y - circle.radius,
        circle.x + circle.radius, circle.y + circle.radius);
}

void SpeedUpIfHit(Circle& obj, Circle& target) {
   
    float dx = obj.x - target.x;
    float dy = obj.y - target.y;
    float distance = sqrt(dx * dx + dy * dy);  

  
    if (distance < obj.radius + target.radius) {
        obj.vy = 500;  
        obj.vx = 1000; 
    }
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    std::wstringstream ss; 

    switch (msg) {
    case WM_PAINT:
    {



        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC hdcBuffer = CreateCompatibleDC(hdc);
        HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, ww, wh);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);
    
        ss.str(L""); 
        ss << L"Position: (" << circle.x << L", " << circle.y << L")\n";
        ss << L"Velocity: (" << circle.vx << L", " << circle.vy << L")\n";
        ss << L"Bounce Coefficient: " << circle.bounce << L"\n";
        ss << L"Gravity: " << circle.gravity << L"\n";

       
        if (circle.vy < 0) {
            ss << L"Bounce Direction: Upward\n";
        }
        else if (circle.vy > 0) {
            ss << L"Bounce Direction: Downward\n";
        }

       
        if (circle.vx < 0) {
            ss << L"Moving: Left\n";
        }
        else if (circle.vx > 0) {
            ss << L"Moving: Right\n";
        }
        else {
            ss << L"Moving: Stationary (Horizontal)\n";
        }

        DrawCircle(hdcBuffer, circle);
        DrawCircle(hdcBuffer, Usil);
        SetBkColor(hdcBuffer, RGB(0, 0, 0));
        SetTextColor(hdcBuffer, RGB(0, 255, 0));
        TextOut(hdcBuffer, 10, 10, ss.str().c_str(), ss.str().length());
    

        BitBlt(hdc, 0, 0, ww, wh, hdcBuffer, 0, 0, SRCCOPY);

        SelectObject(hdcBuffer, hbmOld);
        DeleteObject(hbmBuffer);
        DeleteDC(hdcBuffer);
   
        EndPaint(hwnd, &ps);
    }
    break;

    case WM_SIZE:

        ww = LOWORD(lParam);  
        wh = HIWORD(lParam);



        break;

    case WM_TIMER:
    {



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

        SpeedUpIfHit(circle, Usil);

        InvalidateRect(hwnd, NULL, TRUE);
    }
    break;

    case WM_CREATE:
        SetTimer(hwnd, ID_TIMER, 1, NULL);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_LEFT) circle.vx = -FORCE;  
        if (wParam == VK_RIGHT) circle.vx = FORCE; 
        if (wParam == VK_UP) circle.y -= FORCE;
        if (wParam == VK_DOWN) circle.y += FORCE;

        RECT rect;
        GetClientRect(hwnd, &rect);
        if (circle.x - circle.radius < 0) circle.x = circle.radius;
        if (circle.x + circle.radius > rect.right) circle.x = rect.right - circle.radius;
        if (circle.y - circle.radius < 0) circle.y = circle.radius;
        if (circle.y + circle.radius > rect.bottom) circle.y = rect.bottom - circle.radius;

        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int main() {

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"CircleApp";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    if (!RegisterClass(&wc)) return -1;

    HWND hwnd = CreateWindow(wc.lpszClassName, L"Circle App", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, ww, wh, NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) return -1;

    ShowWindow(hwnd, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
