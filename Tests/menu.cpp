#include <iostream>
#include <windows.h>
#include <functional>

class MouseInputHandler {
private:
    HANDLE hStdin;
    bool listening = false;

    std::function<void(int, int)> onLeftClick;
    std::function<void(int, int)> onRightClick;
    std::function<void(int, int)> onMove;
    std::function<void(int, int)> onDrag;

    bool dragging = false;

public:
    MouseInputHandler() {
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hStdin, &mode);
        mode |= ENABLE_MOUSE_INPUT;
        mode &= ~ENABLE_QUICK_EDIT_MODE; // Disable Quick Edit to allow mouse input
        SetConsoleMode(hStdin, mode);
    }

    void setLeftClickCallback(std::function<void(int, int)> callback) { onLeftClick = callback; }
    void setRightClickCallback(std::function<void(int, int)> callback) { onRightClick = callback; }
    void setMoveCallback(std::function<void(int, int)> callback) { onMove = callback; }
    void setDragCallback(std::function<void(int, int)> callback) { onDrag = callback; }

    void startListening() {
        INPUT_RECORD inputRecord;
        DWORD events;

        while (true) {
            ReadConsoleInput(hStdin, &inputRecord, 1, &events);

            if (inputRecord.EventType == MOUSE_EVENT) {
                auto& mouse = inputRecord.Event.MouseEvent;
                int x = mouse.dwMousePosition.X;
                int y = mouse.dwMousePosition.Y;

                if (mouse.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                    if (mouse.dwEventFlags == 0) {
                        if (onLeftClick) onLeftClick(x, y);
                        dragging = true;
                    } else if (mouse.dwEventFlags == MOUSE_MOVED && dragging) {
                        if (onDrag) onDrag(x, y);
                    }
                } else {
                    dragging = false;
                }

                if (mouse.dwEventFlags == MOUSE_MOVED && !dragging) {
                    if (onMove) onMove(x, y);
                }

                if (mouse.dwButtonState & RIGHTMOST_BUTTON_PRESSED && mouse.dwEventFlags == 0) {
                    if (onRightClick) onRightClick(x, y);
                }
            }
        }
    }
};

int main() {
    MouseInputHandler mouse;

    mouse.setLeftClickCallback([](int x, int y) {
        std::cout << "[Left Click] at X: " << x << ", Y: " << y << "\n";
    });

    mouse.setRightClickCallback([](int x, int y) {
        std::cout << "[Right Click] at X: " << x << ", Y: " << y << "\n";
    });

    mouse.setMoveCallback([](int x, int y) {
        std::cout << "[Move] at X: " << x << ", Y: " << y << "\n";
    });

    mouse.setDragCallback([](int x, int y) {
        std::cout << "[Dragging] at X: " << x << ", Y: " << y << "\n";
    });

    std::cout << "Mouse input system active! Move, click, or drag in this console...\n";
    mouse.startListening(); // Blocking call

    return 0;
}
