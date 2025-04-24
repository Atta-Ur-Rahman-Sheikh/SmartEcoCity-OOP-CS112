#include <iostream>
#include <windows.h>
#include <functional>
#include <thread>

class MouseInput {
private:
    HANDLE hStdin;
    bool listening = false;

    std::function<void(int x, int y)> onLeftClick;
    std::function<void(int x, int y)> onRightClick;
    std::function<void(int x, int y)> onMove;
    std::function<void(int x, int y)> onDrag;

public:
    MouseInput() {
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hStdin, &mode);
        mode |= ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT;
        SetConsoleMode(hStdin, mode);
    }

    void handleLeftClick(std::function<void(int, int)> callback) { onLeftClick = callback; }
    void handleRightClick(std::function<void(int, int)> callback) { onRightClick = callback; }
    void handleMove(std::function<void(int, int)> callback) { onMove = callback; }
    void handleDrag(std::function<void(int, int)> callback) { onDrag = callback; }

    void startListening(bool async = false) {
        listening = true;

        auto loop = [this]() {
            INPUT_RECORD inputRecord;
            DWORD events;

            while (listening) {
                ReadConsoleInput(hStdin, &inputRecord, 1, &events);

                if (inputRecord.EventType == MOUSE_EVENT) {
                    auto& m = inputRecord.Event.MouseEvent;
                    int x = m.dwMousePosition.X;
                    int y = m.dwMousePosition.Y;

                    if (m.dwEventFlags == 0) { // Click
                        if (m.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                            if (onLeftClick) onLeftClick(x, y);
                        }
                        if (m.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
                            if (onRightClick) onRightClick(x, y);
                        }
                    }
                    else if (m.dwEventFlags == MOUSE_MOVED) {
                        if (m.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                            if (onDrag) onDrag(x, y);
                        } else {
                            if (onMove) onMove(x, y);
                        }
                    }
                }
            }
        };

        if (async)
            std::thread(loop).detach();
        else
            loop();
    }

    void stopListening() {
        listening = false;
    }
};

int main() {
    MouseInput mouse;

    std::cout << "Click, move, or drag the mouse in this console window...\n";

    mouse.handleLeftClick([](int x, int y) {
        std::cout << "[Left Click] at X: " << x << ", Y: " << y << '\n';
    });

    mouse.handleRightClick([](int x, int y) {
        std::cout << "[Right Click] at X: " << x << ", Y: " << y << '\n';
    });

    mouse.handleMove([](int x, int y) {
        std::cout << "[Move] at X: " << x << ", Y: " << y << '\n';
    });

    mouse.handleDrag([](int x, int y) {
        std::cout << "[Drag] at X: " << x << ", Y: " << y << '\n';
    });

    mouse.startListening(); // Blocking version. Use true to run async.

    return 0;
}
