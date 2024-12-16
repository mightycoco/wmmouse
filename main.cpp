// hide the console window
// #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Kernel32.lib")

#include <stdio.h>
#include <cstdlib>
#include <windows.h>
#include <ctime>

int main(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow = 0)
{
    // hide console cursor
    CONSOLE_CURSOR_INFO lpcci;
    HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hStdOutput, &lpcci);
    lpcci.bVisible = false;
    SetConsoleCursorInfo(hStdOutput, &lpcci);

    //auto hWnd = GetConsoleWindow();
    //SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 1);
    //SendMessage(hWnd, WM_SYSCOMMAND, SC_MONITORPOWER, 1);
    //SendMessage(hWnd, WM_SYSCOMMAND, SC_SCREENSAVE, 0);
    //Sleep(5000);

    printf("jiggling\n");
    char sp[] = "-\\|/";
    int csp = 0;

    while (true)
    {
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);

        if(now->tm_hour >= 18) {
            printf("s\r");
            Sleep(1000 * 60);
            continue;
        }
        printf("%c\r", sp[csp++]);
        if(csp >= 4) csp = 0;

        INPUT input;
        input.type = INPUT_MOUSE;
        input.mi.mouseData = 0;
        input.mi.time = 0;

        // input.mi.dx = std::rand() / ((RAND_MAX + 1u) / 6) - 3;
        // input.mi.dy = std::rand() / ((RAND_MAX + 1u) / 6) - 3;
        input.mi.dx = 0;
        input.mi.dy = 0;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        int r = SendInput(1, &input, sizeof(input));
        if (r != 1)
        {
            int err = GetLastError();
            printf("failed to SendInput, ret=%d, err=%#08x\n", r, err);
        }

        Sleep(500);
    }

    return 0;
}
