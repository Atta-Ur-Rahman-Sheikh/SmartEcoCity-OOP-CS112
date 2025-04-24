#pragma once
#include <iostream>
#include <windows.h>
#include <functional>
#include <thread>

class MouseInput {
private:
    HANDLE hStdin;
    std::function<void(int x, int y)> clickCallback;
    bool listening;

public:
    MouseInput() : hStdin(GetStdHandle(STD_INPUT_HANDLE)), listening(false) {
        DWORD mode = ENABLE_EXTENDED_FLAGS;
        SetConsoleMode(hStdin, mode);
        mode = ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT;
        SetConsoleMode(hStdin, mode);
    }

    void onClick(std::function<void(int x, int y)> callback) {
        clickCallback = callback;
    }

    void startListening(bool async = false) {
        listening = true;
        auto listenFunc = [this]() {
            INPUT_RECORD inputRecord;
            DWORD events;
            while (listening) {
                ReadConsoleInput(hStdin, &inputRecord, 1, &events);
                if (inputRecord.EventType == MOUSE_EVENT) {
                    auto& mouse = inputRecord.Event.MouseEvent;
                    if (mouse.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                        int x = mouse.dwMousePosition.X;
                        int y = mouse.dwMousePosition.Y;
                        if (clickCallback) clickCallback(x, y);
                    }
                }
            }
        };

        if (async) std::thread(listenFunc).detach();
        else listenFunc();
    }

    void stopListening() {
        listening = false;
    }
};
