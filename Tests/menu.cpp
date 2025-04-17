#include <iostream>
#include <windows.h>
#include <conio.h>
#include <thread>
#include <chrono>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace std::chrono;

// === Console Utilities ===
enum Color {
    BLACK = 0, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE, GRAY = 8, BRIGHT_WHITE = 15
};

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void setCursorPosition(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clearScreen() {
    system("cls");
}

// === UI Components ===
void typewriter(string text, int speed = 40) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(milliseconds(speed));
    }
    cout << endl;
}

void loadingBar(int width = 30, int delay = 25) {
    cout << "Loading: [";
    for (int i = 0; i < width; i++) {
        cout << "#";
        this_thread::sleep_for(milliseconds(delay));
    }
    cout << "] Done!" << endl;
}

void popupMessage(const string& msg) {
    setColor(BRIGHT_WHITE);
    cout << "\n[!] ";
    setColor(YELLOW);
    cout << msg << endl;
    setColor(WHITE);
    cout << "Press any key to continue...";
    _getch();
}

void showCityStats() {
    srand(time(0));
    clearScreen();
    setColor(CYAN);
    cout << "=== City Live Stats ===\n\n";
    setColor(GREEN);
    cout << "  Temperature: " << (rand() % 15 + 20) << " °C" << endl;
    cout << " Water Level: " << (rand() % 100) << " %" << endl;
    cout << " Electricity Usage: " << (rand() % 500 + 1000) << " kWh" << endl;
    cout << " Traffic Congestion: " << (rand() % 100) << " %" << endl;
    cout << " Air Quality Index: " << (rand() % 50 + 50) << endl;

    popupMessage("These values are simulated for demo purposes.");
}

// === File Operations ===
void saveCityState() {
    ofstream file("city_save.txt");
    file << "Energy=1200\nWater=82\nPollution=34\nPopulation=1500\n";
    file.close();
    popupMessage("City state saved successfully!");
}

void loadCityState() {
    ifstream file("city_save.txt");
    if (!file) {
        popupMessage("No saved data found.");
        return;
    }
    string line;
    clearScreen();
    setColor(CYAN);
    cout << "=== Loaded City Data ===\n";
    setColor(WHITE);
    while (getline(file, line)) {
        cout << "• " << line << endl;
    }
    file.close();
    popupMessage("Data loaded from 'city_save.txt'");
}

// === Menu System ===
void mainMenu();

void settingsMenu() {
    vector<string> options = {
        "Enable Notifications",
        "Toggle Dark Mode",
        "Back to Main Menu"
    };
    int selected = 0;

    while (true) {
        clearScreen();
        setColor(BRIGHT_WHITE);
        cout << "=== Settings ===\n\n";
        for (int i = 0; i < options.size(); i++) {
            if (i == selected) {
                setColor(GREEN);
                cout << " > " << options[i] << endl;
            } else {
                setColor(WHITE);
                cout << "   " << options[i] << endl;
            }
        }

        char key = _getch();
        if (key == -32 || key == 224) {
            key = _getch();
            if (key == 72 && selected > 0) selected--;
            else if (key == 80 && selected < options.size() - 1) selected++;
        } else if (key == '\r') {
            if (selected == 2) return;
            popupMessage("This feature is under development.");
        }
    }
}

void mainMenu() {
    vector<string> options = {
        "Start Simulation",
        "Live City Stats",
        "Save City State",
        "Load City State",
        "Settings",
        "Exit"
    };
    int selected = 0;

    while (true) {
        clearScreen();
        setColor(BRIGHT_WHITE);
        cout << "=== SMART ECO CITY PLATFORM ===\n\n";
        for (int i = 0; i < options.size(); i++) {
            if (i == selected) {
                setColor(GREEN);
                cout << " > " << options[i] << endl;
            } else {
                setColor(WHITE);
                cout << "   " << options[i] << endl;
            }
        }

        char key = _getch();
        if (key == -32 || key == 224) {
            key = _getch();
            if (key == 72 && selected > 0) selected--;
            else if (key == 80 && selected < options.size() - 1) selected++;
        } else if (key == '\r') {
            switch (selected) {
                case 0: typewriter("Starting simulation..."); loadingBar(); break;
                case 1: showCityStats(); break;
                case 2: saveCityState(); break;
                case 3: loadCityState(); break;
                case 4: settingsMenu(); break;
                case 5: typewriter("Thank you for visiting Smart Eco City!"); return;
            }
        }
    }
}

// === Main ===
int main() {
    clearScreen();
    setColor(YELLOW);
    typewriter("Welcome to Smart Eco City CLI Platform", 50);
    this_thread::sleep_for(milliseconds(300));
    loadingBar();
    mainMenu();
    return 0;
}






//Uncomment for tab-like menu example.

// #include <iostream>
// #include <conio.h>
// #include <windows.h>
// #include <vector>
// #include <string>

// using namespace std;

// void setCursorPosition(int x, int y) {
//     COORD coord = {(SHORT)x, (SHORT)y};
//     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
// }

// void setColor(int color) {
//     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
// }

// void drawTabs(const vector<string>& tabs, int activeIndex) {
//     setCursorPosition(0, 0);
//     for (int i = 0; i < tabs.size(); i++) {
//         if (i == activeIndex) {
//             setColor(11);  // Bright cyan
//             cout << "[ " << tabs[i] << " ] ";
//         } else {
//             setColor(7);   // Default
//             cout << "  " << tabs[i] << "   ";
//         }
//     }
//     setColor(7);
//     cout << "\n\n";
// }

// void displayTabContent(int index) {
//     setCursorPosition(0, 3);
//     system("cls");
//     drawTabs({ "Dashboard", "Simulation", "Settings", "Reports" }, index);

//     switch (index) {
//         case 0: cout << "Dashboard: Overview of your Smart Eco City.\n"; break;
//         case 1: cout << "Simulation: Live sensors and city dynamics.\n"; break;
//         case 2: cout << "Settings: Customize your experience.\n"; break;
//         case 3: cout << "Reports: View detailed city analytics.\n"; break;
//     }
// }

// void tabMenu() {
//     vector<string> tabs = { "Dashboard", "Simulation", "Settings", "Reports" };
//     int currentTab = 0;

//     displayTabContent(currentTab);

//     while (true) {
//         char ch = _getch();
//         if (ch == -32 || ch == 224) {
//             ch = _getch();  // Arrow key
//             if (ch == 75 && currentTab > 0) currentTab--;     
//             else if (ch == 77 && currentTab < tabs.size() - 1) currentTab++; 
//             displayTabContent(currentTab);
//         } else if (ch == 27) {
//             break;  // ESC to exit
//         }
//     }
// }

// int main() {
//     system("cls");
//     tabMenu();
//     return 0;
// }
