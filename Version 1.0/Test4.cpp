#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <iomanip>
#include <cmath>
using namespace std;

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

int main()
{
    emoji_Support();
    cout << "| Type         | Emojis                                 || -- -- -- -- -- -- --| -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -|| Residential | 🏠 🏡 🛖 🏘️ 🏚️ || Commercial | 🏬 🏪 🛍️ 🏦 || Industrial | 🏭 ⚙️ 🛢️ 🧱 || Offices | 🏢 🏣 🏤 🏛️ || Entertainment | 🎡 🎢 🎠 🎭 🎬 🕹️ || Farm | 🌾 🌽 🐄 🐓 🚜 || Power Plants | 🔋 🔌 ⚡ ☀️ 🌬️ 🌀 || Public | 🏥 🏫 🚒 🏛️ 🏦 🏪 🕍 | ";
    
    return 0;
}