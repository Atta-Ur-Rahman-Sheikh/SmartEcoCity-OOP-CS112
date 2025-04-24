#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define B_BLACK "\033[90m"
#define B_RED "\033[91m"
#define B_GREEN "\033[92m"
#define B_YELLOW "\033[93m"
#define B_BLUE "\033[94m"
#define B_MAGENTA "\033[95m"
#define B_CYAN "\033[96m"
#define B_WHITE "\033[97m"
#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"
#define BG_B_BLACK "\033[100m"
#define BG_B_RED "\033[101m"
#define BG_B_GREEN "\033[102m"
#define BG_B_YELLOW "\033[103m"
#define BG_B_BLUE "\033[104m"
#define BG_B_MAGENTA "\033[105m"
#define BG_B_CYAN "\033[106m"
#define BG_B_WHITE "\033[107m"
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define ITALIC "\033[3m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"
#define FAST_BLINK "\033[6m"
#define REVERSED "\033[7m"
#define HIDDEN "\033[8m"
#define STRIKETHROUGH "\033[9m"

#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <thread>

using namespace std;

string custom_Colour(int r, int g, int b)
{
    return "\033[38;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m";
}
string custom_Background(int r, int g, int b)
{
    return "\033[48;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m";
}

class pre_requisites
{
    void clear_Screen()
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coordScreen = {0, 0};
        DWORD charsWritten;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD consoleSize;

        GetConsoleScreenBufferInfo(hConsole, &csbi);
        consoleSize = csbi.dwSize.X * csbi.dwSize.Y;

        FillConsoleOutputCharacter(hConsole, ' ', consoleSize, coordScreen, &charsWritten);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, coordScreen, &charsWritten);
        SetConsoleCursorPosition(hConsole, coordScreen);
    }
    void emoji_Support()
    {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof(cfi);
        GetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
        cfi.dwFontSize.Y = 24;
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_NORMAL;
        wcscpy_s(cfi.FaceName, L"Consolas");
        SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
    }

public:
    pre_requisites()
    {
        system("");      // For proper working of ANSI codes (Text Style and Colours)
        emoji_Support(); // For making Emojis work in CMD
        srand(time(0));  // Seed for Random Island Generation
    }
};
class MouseInputHandler
{
private:
    HANDLE hStdin;
    bool listening = false;

    std::function<void(int, int)> onLeftClick;
    std::function<void(int, int)> onRightClick;
    std::function<void(int, int)> onMove;
    std::function<void(int, int)> onDrag;

    bool dragging = false;

public:
    MouseInputHandler()
    {
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

    void startListening()
    {
        INPUT_RECORD inputRecord;
        DWORD events;

        while (true)
        {
            ReadConsoleInput(hStdin, &inputRecord, 1, &events);

            if (inputRecord.EventType == MOUSE_EVENT)
            {
                auto &mouse = inputRecord.Event.MouseEvent;
                int x = mouse.dwMousePosition.X;
                int y = mouse.dwMousePosition.Y;

                if (mouse.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
                {
                    if (mouse.dwEventFlags == 0)
                    {
                        if (onLeftClick)
                            onLeftClick(x, y);
                        dragging = true;
                    }
                    else if (mouse.dwEventFlags == MOUSE_MOVED && dragging)
                    {
                        if (onDrag)
                            onDrag(x, y);
                    }
                }
                else
                {
                    dragging = false;
                }

                if (mouse.dwEventFlags == MOUSE_MOVED && !dragging)
                {
                    if (onMove)
                        onMove(x, y);
                }

                if (mouse.dwButtonState & RIGHTMOST_BUTTON_PRESSED && mouse.dwEventFlags == 0)
                {
                    if (onRightClick)
                        onRightClick(x, y);
                }
            }
        }
    }
};

struct tile
{
    int x, y;
    int xChar, yChar;
    string texture;
    string type;
    string colour;
};
const int map_width = 42;
const int map_height = 16;

void generate_Map(tile map[map_height][map_width])
{
    int center_x = (map_width / 2);
    int center_y = map_height / 2;
    int radius = 16;
    float stretch_x = 1;
    float stretch_y = 2.5;

    for (int i = 0; i < map_height; i++)
    {
        for (int j = 0; j < map_width; j++)
        {
            float distance = sqrt(pow((j - center_x) * stretch_x, 2) + pow((i - center_y) * stretch_y, 2));
            bool isWater = (distance + (rand() % 3 - 1)) > radius;

            if (isWater)
            {
                string bg = custom_Background(117, 226, 254);
                string fg = custom_Colour(93, 181, 203);
                string texture = (rand() % 100 < 30) ? "~ " : "  ";

                map[i][j].colour = bg + fg;
                map[i][j].texture = texture;
                map[i][j].type = "Water";
            }
            else
            {
                string bg = custom_Background(131, 198, 105);
                map[i][j].colour = bg;
                map[i][j].texture = "  ";
                map[i][j].type = "Land";
            }
        }
    }
}

void display_Map(tile map[map_height][map_width], tile previous_map[map_height][map_width], int tileWidth = 2)
{
    string left_Distance = "            ";

    // Determine if we should force a full redraw (when same array passed twice)
    bool fullRedraw = (map == previous_map);

    // Print top border
    cout << "\n"
         << left_Distance << BOLD << custom_Background(32, 32, 32) << "   ";
    for (int i = 1; i < map_width - 14; i++)
    {
        cout << setw(tileWidth) << left << i;
    }
    cout << "  " << RESET << "\n";

    for (int i = 0; i < map_height; i++)
    {
        for (int j = 0; j < map_width; j++)
        {
            // Handle border characters always
            if (j == 0 || j == map_width - 1)
            {
                if (j == 0)
                {
                    cout << left_Distance;
                }
                cout << custom_Background(32, 32, 32) << BOLD;
                char alpha = 'A' + (i % 26);
                cout << " " << alpha << " " << RESET;
                continue;
            }

            bool changed = fullRedraw ||
                           map[i][j].colour != previous_map[i][j].colour ||
                           map[i][j].texture != previous_map[i][j].texture ||
                           map[i][j].type != previous_map[i][j].type;

            if (changed)
            {
                // Calculate the position based on terminal window size and tile width
                int console_row = i + 2; // 1 for top margin, 1 for header
                int console_col = (j - 1) * tileWidth + (int)left_Distance.size() + 3;

                // Adjust cursor position to be at the correct tile placement
                cout << "\033[" << console_row << ";" << console_col << "H";

                // Print the tile's texture and color
                if (map[i][j].type == "Water")
                {
                    cout << DIM << map[i][j].colour << map[i][j].texture << RESET;
                }
                else
                {
                    cout << map[i][j].colour << map[i][j].texture << RESET;
                }

                // Update previous map only when it's not a full redraw
                if (!fullRedraw)
                {
                    previous_map[i][j] = map[i][j];
                }
            }
        }
    }

    // Move cursor to bottom to avoid overwriting
    cout << "\033[" << (map_height + 3) << ";1H";

    // Print bottom border
    cout << left_Distance << BOLD << custom_Background(32, 32, 32) << "   ";
    for (int i = 1; i < map_width - 14; i++)
    {
        cout << setw(tileWidth) << left << i;
    }
    cout << "  " << RESET << "\n";
}






void copy_Map(tile arr1[map_height][map_width], tile arr2[map_height][map_width])
{
    for (int i = 0; i < map_height; i++)
    {
        for (int j = 0; j < map_width; j++)
        {
            arr1[i][j] = arr2[i][j];
        }
    }
}


int main()
{
    pre_requisites();
    MouseInputHandler mouse;

    tile map[map_height][map_width];
    tile previous_map[map_height][map_width];

    generate_Map(map);
    copy_Map(previous_map, map);
    display_Map(map, map);
    display_Map(map, previous_map);

    // mouse.startListening(); // Blocking call
    return 0;
}