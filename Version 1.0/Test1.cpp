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
#include <algorithm>

using namespace std;

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define REVERSE "\033[7m"

// ANSI custom colour
string custom_Colour(int r, int g, int b) { return "\033[38;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m"; }
string custom_Background(int r, int g, int b) { return "\033[48;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m"; }

// Map dimensions
static const int map_width = 42, map_height = 16, tileWidth = 2;
int leftOffset = 12;
struct tile
{
    string texture, colour, type;
};

// UI layout
int menuRow = map_height + 4, menuCol = leftOffset;
int statsRow = map_height + 4, statsCol = map_width * tileWidth + 4;
int tooltipRow = map_height + 8, tooltipCol = 0; // tooltip position will be set at runtime
int messageRow = tooltipRow - 1;

void clear_Screen()
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD c = {0, 0};
    DWORD w;
    CONSOLE_SCREEN_BUFFER_INFO b;
    GetConsoleScreenBufferInfo(h, &b);
    DWORD sz = b.dwSize.X * b.dwSize.Y;
    FillConsoleOutputCharacter(h, ' ', sz, c, &w);
    FillConsoleOutputAttribute(h, b.wAttributes, sz, c, &w);
    SetConsoleCursorPosition(h, c);
}
void emoji_Support()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX f;
    f.cbSize = sizeof(f);
    GetCurrentConsoleFontEx(h, FALSE, &f);
    f.dwFontSize.Y = 24;
    f.FontFamily = FF_DONTCARE;
    f.FontWeight = FW_NORMAL;
    wcscpy_s(f.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(h, FALSE, &f);
}

void generate_Map(tile m[map_height][map_width])
{
    int cx = map_width / 2, cy = map_height / 2, r = 16;
    float sx = 1, sy = 2.5;
    for (int i = 0; i < map_height; i++)
        for (int j = 0; j < map_width; j++)
        {
            float d = sqrt(pow((j - cx) * sx, 2) + pow((i - cy) * sy, 2));
            bool w = (d + (rand() % 3 - 1) > r);
            if (w)
            {
                m[i][j].colour = custom_Background(117, 226, 254) + custom_Colour(93, 181, 203);
                m[i][j].texture = (rand() % 100 < 30 ? "~ " : "  ");
                m[i][j].type = "Water";
            }
            else
            {
                m[i][j].colour = custom_Background(131, 198, 105) + custom_Colour(80, 130, 60);
                m[i][j].texture = (rand() % 100 < 20 ? "^^" : "  ");
                m[i][j].type = "Land";
            }
        }
}
void display_Map(tile m[map_height][map_width])
{
    cout << "\033[1;1H" << string(leftOffset, ' ') << BOLD << custom_Background(32, 32, 32) << "   ";
    for (int c = 0; c < map_width; c++)
        cout << setw(tileWidth) << left << c + 1;
    cout << RESET << "\n";
    for (int i = 0; i < map_height; i++)
    {
        cout << string(leftOffset, ' ') << BOLD << custom_Background(32, 32, 32) << " " << char('A' + i) << " " << RESET;
        for (int j = 0; j < map_width; j++)
            cout << m[i][j].colour << m[i][j].texture << RESET;
        cout << BOLD << custom_Background(32, 32, 32) << " " << char('A' + i) << " " << RESET << "\n";
    }
    cout << string(leftOffset, ' ') << BOLD << custom_Background(32, 32, 32) << "   ";
    for (int c = 0; c < map_width; c++)
        cout << setw(tileWidth) << left << c + 1;
    cout << RESET << "\n";
}

// Menu definitions
struct Menu
{
    string name;
    vector<string> items;
    vector<function<void()>> act;
};
vector<Menu> menus;
int currentTab = 0, currentOpt = 0;
vector<int> tabPos;
void rebuild_Tab()
{
    tabPos.clear();
    int x = menuCol;
    for (auto &M : menus)
    {
        tabPos.push_back(x);
        x += M.name.size() + 3;
    }
}
void clear_MenuArea()
{
    int h = 1;
    for (int r = menuRow; r < menuRow + h; r++)
    {
        cout << "\033[" << r << ";" << menuCol << "H" << string(tabPos.back() + menus.back().name.size() + 2 - menuCol, ' ');
    }
}
void draw_Menu()
{
    clear_MenuArea();
    for (int t = 0; t < menus.size(); t++)
    {
        cout << "\033[" << menuRow << ";" << tabPos[t] << "H" << (t == currentTab ? REVERSE : "[" + menus[t].name + "]");
    }
    for (int i = 0; i < menus[currentTab].items.size(); i++)
    {
        cout << "\033[" << (menuRow + 1 + i) << ";" << tabPos[currentTab] << "H" << (i == currentOpt ? REVERSE : "") << menus[currentTab].items[i] << RESET;
    }
}
void draw_Stats() { cout << "\033[" << statsRow << ";" << statsCol << "H" << BOLD << "---Stats---" << RESET; }
void draw_Tooltip(const string &m)
{
    if (tooltipCol == 0)
        tooltipCol = leftOffset + (map_width * tileWidth) / 2 - m.size() / 2;
    cout << "\033[" << tooltipRow << ";" << tooltipCol << "H" << BOLD << m << RESET;
}
void draw_Message(const string &m) { cout << "\033[" << messageRow << ";" << menuCol << "H" << string(80, ' ') << "\033[" << messageRow << ";" << menuCol << "H" << m; }
void input()
{
    while (true)
    {
        int ch = _getch();
        if (ch == 224)
        {
            int d = _getch();
            if (d == 75)
                currentTab = (currentTab - 1 + menus.size()) % menus.size(), currentOpt = 0;
            if (d == 77)
                currentTab = (currentTab + 1) % menus.size(), currentOpt = 0;
            if (d == 80)
                currentOpt = (currentOpt + 1) % menus[currentTab].items.size();
            if (d == 72)
                currentOpt = (currentOpt - 1 + menus[currentTab].items.size()) % menus[currentTab].items.size();
        }
        else if (ch == 13)
        {
            menus[currentTab].act[currentOpt]();
        }
        else if (ch == 27)
            break;
        draw_Menu();
        draw_Stats();
        draw_Tooltip("←/→tab ↑/↓opt Enter act Esc exit");
    }
}
int main()
{
    Sleep(2000);
    srand(time(NULL));
    system("");
    emoji_Support();
    tile m[map_height][map_width];
    generate_Map(m);
    clear_Screen();
    display_Map(m);
    
    
    menus = {{"Build", {"Road", "House"}, {[]
                                           { draw_Message("Build Road"); }, []
                                           { draw_Message("Build House"); }}},
             {"View", {"Zoom", "Reset"}, {[]
                                          { draw_Message("Zoom"); }, []
                                          { draw_Message("Reset"); }}}};
    rebuild_Tab();
    draw_Menu();
    draw_Stats();
    draw_Tooltip("←/→tab ↑/↓opt Enter act Esc exit");
    input();
    return 0;
}
