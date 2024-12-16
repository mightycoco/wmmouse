#define _X86_
#define NOGDI

#include <stdio.h>
#include <windows.h>

int main() {
   // printf() displays the string inside quotation
   printf("Hello, World!");
   HWND mWindow;
   POINT btn;
   Sleep(5000);
   GetCursorPos(&btn);
   mWindow = GetFocus();
   btn.x += 5;
   btn.y += 5;
   SendMessage(mWindow,WM_MOUSEMOVE,0,MAKELPARAM(btn.x,btn.y));
   return 0;
}