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
#define REVERSE "\033[7m"
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
#include <vector>
#include <functional>
#include <algorithm> // for std::max

using namespace std;

// Prerequisite functions
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

// ANSI custom colour
string custom_Colour(int r, int g, int b) { return "\033[38;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m"; }
string custom_Background(int r, int g, int b) { return "\033[48;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m"; }

// Map dimensions
static const int map_width = 42;
static const int map_height = 16;
static const int tileWidth = 2;
int leftOffset = 11;
struct tile
{
    string texture, colour, type;
};

// UI layout variables
int menuRow = map_height + 4;
int menuCol = leftOffset + 1;
int statsRow = map_height + 4;
int statsCol = map_width * tileWidth + 4;
int maxDropdownItems = 10; // computed after menus init
int tooltipRow = 2;       // set after computing maxDropdownItems
int messageRow = 2;        // row to display action messages

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

void generate_Map(tile map[map_height][map_width])
{
    int cx = map_width / 2, cy = map_height / 2, radius = 16;
    float sx = 1, sy = 2.5;
    for (int i = 0; i < map_height; i++)
    {
        for (int j = 0; j < map_width; j++)
        {
            float d = sqrt(pow((j - cx) * sx, 2) + pow((i - cy) * sy, 2));
            bool isWater = (d + (rand() % 3 - 1)) > radius;
            if (isWater)
            {
                map[i][j].colour = custom_Background(117, 226, 254) + custom_Colour(93, 181, 203);
                map[i][j].texture = (rand() % 100 < 30 ? "~ " : "  ");
                map[i][j].type = "Water";
            }
            else
            {
                map[i][j].colour = custom_Background(131, 198, 105) + custom_Colour(80, 130, 60);
                map[i][j].texture = (rand() % 100 < 20 ? "^^" : "  ");
                map[i][j].type = "Land";
            }
        }
    }
}

void display_Map(tile map[map_height][map_width])
{
    // Top border
    cout << "\033[1;1H" << string(leftOffset, ' ') << BOLD << custom_Background(32, 32, 32) << "   ";
    for (int c = 0, k = 1; c < map_width; c++, ++k)
    {
        cout << setw(tileWidth) << left << k;
        if (k > 8)
        {
            k -= 9;
        }
    }
    cout << "   " << RESET << "\n";
    // Rows
    for (int i = 0; i < map_height; i++)
    {
        cout << string(leftOffset, ' ') << BOLD << custom_Background(32, 32, 32)
             << " " << char('A' + i) << " " << RESET;
        for (int j = 0; j < map_width; j++)
            cout << map[i][j].colour << map[i][j].texture << RESET;
        cout << BOLD << custom_Background(32, 32, 32)
             << " " << char('A' + i) << " " << RESET << "\n";
    }
    // Bottom border
    cout << string(leftOffset, ' ') << BOLD << custom_Background(32, 32, 32) << "   ";
    for (int c = 0, k = 1; c < map_width; c++, ++k)
    {
        cout << setw(tileWidth) << left << k;
        if (k > 8)
        {
            k -= 9;
        }
    }
    cout << "   " << RESET << "\n";
}

// Menu system
struct Menu
{
    string name;
    vector<string> items;
    vector<function<void()>> actions;
};
vector<Menu> menus;
int currentTab = 0, currentOpt = 0;
vector<int> tabPositions;
void rebuild_TabPositions()
{
    tabPositions.clear();
    int x = menuCol;
    for (auto &m : menus)
    {
        tabPositions.push_back(x);
        x += int(m.name.size()) + 3;
    }
}
void compute_UI_positions()
{
    maxDropdownItems = 0;
    for (auto &m : menus)
        maxDropdownItems = max(maxDropdownItems, int(m.items.size()));
    tooltipRow = menuRow + 1 + maxDropdownItems + 1;
    messageRow = tooltipRow - 1;
}
void clear_MenuArea()
{
    int height = 1 + maxDropdownItems;
    int width = tabPositions.back() + int(menus.back().name.size()) + 2 - menuCol;
    for (int r = menuRow; r < menuRow + height; r++)
    {
        cout << "\033[" << r << ";" << menuCol << "H" << string(width, ' ');
    }
}
void draw_Menu()
{
    clear_MenuArea();
    for (int t = 0; t < int(menus.size()); t++)
    {
        cout << "\033[" << menuRow << ";" << tabPositions[t] << "H";
        if (t == currentTab)
            cout << REVERSE << "[" << menus[t].name << "]" << RESET;
        else
            cout << "[" << menus[t].name << "]";
    }
    // always show dropdown for currentTab
    for (int i = 0; i < int(menus[currentTab].items.size()); i++)
    {
        cout << "\033[" << (menuRow + 1 + i) << ";" << tabPositions[currentTab] << "H";
        if (i == currentOpt)
            cout << REVERSE << menus[currentTab].items[i] << RESET;
        else
            cout << menus[currentTab].items[i];
    }
}
void draw_Stats()
{
    cout << "\033[" << statsRow << ";" << statsCol << "H" << BOLD << "--- City Stats ---" << RESET;
    cout << "\033[" << statsRow + 1 << ";" << statsCol << "H" << "Population: 1234";
    cout << "\033[" << statsRow + 2 << ";" << statsCol << "H" << "Funds: $5000";
    cout << "\033[" << statsRow + 3 << ";" << statsCol << "H" << "Happiness: 75%";
}
void draw_Tooltip(const string &msg)
{
    // At bottom center of the screen, customizable with variables row and col
    int row, col;
    row = tooltipRow + 7;
    col = (map_width * tileWidth + leftOffset) / 2 - msg.size() / 2;
    col += 12;
    cout << "\033[" << row << ";" << col << "H" << string(80, ' ');
    cout << "\033[" << row << ";" << col << "H" << msg;

}
void draw_Message(const string &msg)
{
    cout << "\033[" << messageRow << ";" << menuCol << "H" << string(80, ' ');
    cout << "\033[" << messageRow << ";" << menuCol << "H" << msg;
}
void handle_Input()
{
    while (true)
    {
        int ch = _getch();
        if (ch == 224)
        {
            int dir = _getch();
            if (dir == 75)
            {
                currentTab = (currentTab - 1 + menus.size()) % menus.size();
                currentOpt = 0;
            }
            if (dir == 77)
            {
                currentTab = (currentTab + 1) % menus.size();
                currentOpt = 0;
            }
            if (dir == 80)
                currentOpt = (currentOpt + 1) % menus[currentTab].items.size();
            if (dir == 72)
                currentOpt = (currentOpt - 1 + menus[currentTab].items.size()) % menus[currentTab].items.size();
        }
        else if (ch == 13)
        {
            menus[currentTab].actions[currentOpt]();
        }
        else if (ch == 27)
        {
            break; // exit menu
        }
        draw_Menu();
        draw_Stats();
        draw_Tooltip("←/→ change tab, ↑/↓ select, Enter action, Esc exit");
    }
}

class City
{
public:
    static int money;
    static int buildings;
    static int population;
    static int happiness;
    static int carbon;
    static int pollution;
    static int complaints;
    static int greenCoverage;
    static int afforestation;
    static int disasterRisk;
    static int tourismRating;
    static int mayorLevel;
    static int ecoPoints;
    static int AIknowledge;
    static int year, month;

    void displayStats(int state = 1)
    {
        if (state == 1)
        {
            // Money, time, Eco Points, AI Knowledge,buildings,Mayor Level,
            
        }
        else if (state == 2)
        {
            // Carbon, Pollution, Green Coverage, Afforestation, Disaster Risk
        }
        else if (state == 3)
        {
            // Population, Happiness, Complaints, Tourism Rating,  
        }
    }
};

int main()
{
    // Prerequisites
    system(""); // For proper working of ANSI codes (Text Style and Colours)
    float sec = 2 * 1000;
    cout << "Wait " << sec / 1000 << " seconds for the map to load...\n";
    Sleep(sec);      // Time window for resizing CMD window
    emoji_Support(); // For making Emojis work in CMD
    MouseInputHandler mouse;
    srand(time(0)); // Seed for Random Island Generation

    tile map[map_height][map_width];
    generate_Map(map);
    clear_Screen();

    display_Map(map);
    City city;

    menus = {
        {"Build", {"Road", "House", "Park"}, {[]()
                                              { draw_Message("Building Road..."); }, []()
                                              { draw_Message("Building House..."); }, []()
                                              { draw_Message("Planting Park..."); }}},
        {"Manage", {"Taxes", "Population", "Resources"}, {[]()
                                                          { draw_Message("Adjusting Taxes..."); }, []()
                                                          { draw_Message("Analyzing Population..."); }, []()
                                                          { draw_Message("Checking Resources..."); }}},
        {"STATS", {"Economy", "Environment", "Population"}, {[](){ draw_Message("Zooming In..."); }, [](){ City city; city.displayStats(1); }, [](){ draw_Message("Zooming Out..."); }}},
        {"SETTINGS", {"Save", "Load", "Exit"}, {[]()
                                                { draw_Message("Saving Game..."); }, []()
                                                { draw_Message("Loading Game..."); }, []()
                                                { exit(0); }}}};

    rebuild_TabPositions();
    compute_UI_positions();

    draw_Menu();
    draw_Stats();
    draw_Tooltip("Tooltip: ←/→ change tab, ↑/↓ select, Enter action, Esc exit");
    handle_Input();
    return 0;
}