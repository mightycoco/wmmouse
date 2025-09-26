/*
 * Copyright (C) 2023 Arm Limited
 */

#include <cstdio>
#include <cstring>
#include <ctime>
#include <windows.h>
#include <winuser.h>

void hideConsoleCursor() {
  CONSOLE_CURSOR_INFO cursorInfo;
  HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleCursorInfo(hStdOutput, &cursorInfo);
  cursorInfo.bVisible = false;
  SetConsoleCursorInfo(hStdOutput, &cursorInfo);
}

void simulateMouseMovement() {
  static INPUT input = {};
  ZeroMemory(&input, sizeof(input));
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_MOVE;

  if (SendInput(1, &input, sizeof(input)) != 1) {
    printf("Failed to SendInput, error=%#08lx\n", GetLastError());
  }
}

void simulateMouseMovement2() {
  POINT originalPos;
  if (GetCursorPos(&originalPos)) {
    // Generate random coordinates within the screen bounds
    int randomOffsetX = (rand() % 21) - 10; // Random value between -10 and 10
    int randomOffsetY = (rand() % 21) - 10; // Random value between -10 and 10
    int randomX = originalPos.x + randomOffsetX;
    int randomY = originalPos.y + randomOffsetY;

    // Ensure the coordinates are within screen bounds
    randomX = max(0, min(randomX, GetSystemMetrics(SM_CXSCREEN) - 1));
    randomY = max(0, min(randomY, GetSystemMetrics(SM_CYSCREEN) - 1));

    SetCursorPos(randomX, randomY);

    Sleep(1);

    SetCursorPos(originalPos.x, originalPos.y);
  } else {
    printf("Failed to get current mouse position, error=%#08lx\n",
           GetLastError());
  }
}

void simulateCtrlKeyPress() {
  INPUT input = {};
  input.type = INPUT_KEYBOARD;
  // Press the Ctrl key down
  input.ki.wVk = VK_CONTROL;
  input.ki.dwFlags = 0; // Key down
  if (SendInput(1, &input, sizeof(input)) != 1) {
    printf("Failed to SendInput, error=%#08lx\n", GetLastError());
  }

  Sleep(1);

  // Release the Ctrl key
  input.ki.dwFlags = KEYEVENTF_KEYUP; // Key up
  if (SendInput(1, &input, sizeof(input)) != 1) {
    printf("Failed to SendInput, error=%#08lx\n", GetLastError());
  }
}

// Global variable to store the activity ends hour
int activityEndsAt = 18;
int activityStartsAt = 7;

bool isActiveHours() {
  std::time_t t = std::time(nullptr);
  std::tm now;
  localtime_s(&now, &t);

  return now.tm_hour >= activityStartsAt && now.tm_hour < activityEndsAt;
}

// Parse time string in format "hh" or "hh:mm", returns hour (0-23) or -1 on
// error
int parseTimeString(const char *timeStr) {
  if (!timeStr)
    return -1;

  int hour = -1, minute = 0;

  // Try parsing "hh:mm" format first
  if (sscanf_s(timeStr, "%d:%d", &hour, &minute) == 2) {
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
      return -1;
    }
    return hour;
  }

  // Try parsing "hh" format
  if (sscanf_s(timeStr, "%d", &hour) == 1) {
    if (hour < 0 || hour > 23) {
      return -1;
    }
    return hour;
  }

  return -1;
}

int keyPressRnd = rand();
DWORD lastMouseMoveTime = 0;
DWORD nextMouseMoveTime = 0;
DWORD lastKeyPressTime = 0;
DWORD nextKeyPressTime = 0;

int parseArguments(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    if ((strcmp(argv[i], "--start") == 0 || strcmp(argv[i], "-s") == 0) &&
        i + 1 < argc) {
      int hour = parseTimeString(argv[i + 1]);
      if (hour != -1) {
        activityStartsAt = hour;
        i++; // Skip next argument as it's already processed
      } else {
        printf("Invalid time format for --start: %s\n", argv[i + 1]);
        return -1;
      }
    } else if ((strcmp(argv[i], "--end") == 0 || strcmp(argv[i], "-e") == 0) &&
               i + 1 < argc) {
      int hour = parseTimeString(argv[i + 1]);
      if (hour != -1) {
        activityEndsAt = hour;
        i++; // Skip next argument as it's already processed
      } else {
        printf("Invalid time format for --end: %s\n", argv[i + 1]);
        return -1;
      }
    } else {
      printf("Unknown argument: %s\n", argv[i]);
      return -1;
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (parseArguments(argc, argv) != 0) {
    return 1;
  }
  srand(static_cast<unsigned int>(time(nullptr)));
  hideConsoleCursor();

  const char spinner[] = "-\\|/";
  size_t spinnerIndex = 0;

  while (true) {
    if (!isActiveHours()) {
      printf("\rIdling                                                 \r");
      Sleep(60 * 1000); // Sleep for 1 minute
      continue;
    }

    printf("%c scanning\r", spinner[spinnerIndex]);
    spinnerIndex = (spinnerIndex + 1) % (sizeof(spinner) - 1);

    DWORD currentTime = GetTickCount();

    // Initialize next mouse move time if not set
    if (nextMouseMoveTime == 0) {
      // Random interval between 10 seconds and 2 minutes (10,000 to 120,000 ms)
      DWORD randomInterval = 10000 + (rand() % (120000 - 10000 + 1));
      nextMouseMoveTime = currentTime + randomInterval;
    }

    // Check if it's time for mouse movement
    if (currentTime >= nextMouseMoveTime) {
      simulateMouseMovement2();
      printf("\rmove                                                     \r");

      // Schedule next mouse movement: random interval between 10s and 2min
      DWORD randomInterval = 10000 + (rand() % (120000 - 10000 + 1));
      nextMouseMoveTime = currentTime + randomInterval;
    }

    // Initialize next key press time if not set
    if (nextKeyPressTime == 0) {
      // Random interval between 10 seconds and 2 minutes (10,000 to 120,000 ms)
      DWORD randomInterval = 10000 + (rand() % (120000 - 10000 + 1));
      nextKeyPressTime = currentTime + randomInterval;
    }

    // Check if it's time for key press
    if (currentTime >= nextKeyPressTime) {
      simulateCtrlKeyPress();
      printf("\rpress                                                    \r");

      // Schedule next key press: random interval between 10s and 2min
      DWORD randomInterval = 10000 + (rand() % (120000 - 10000 + 1));
      nextKeyPressTime = currentTime + randomInterval;
    }
    Sleep(500); // Sleep for 500 milliseconds
  }

  return 0;
}