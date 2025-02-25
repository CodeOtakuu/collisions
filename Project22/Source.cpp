#include "MainWindow.h"

int main() {

	MainWindow wnd;

    if (!wnd.Create(L"Xz", WS_OVERLAPPEDWINDOW)) {
        MessageBox(NULL, L"Error :(", L"Error", MB_OK);
        return -1;
    }
    

    ShowWindow(wnd.Window(), SW_SHOW);
    UpdateWindow(wnd.Window());

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}