#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <utility>

using namespace std;

// Map dimensions
enum { MAP_WIDTH = 42, MAP_HEIGHT = 16 };

// Enum for building types
enum class BuildingType {
    None = 0,
    Road,
    Residential,
    Commercial,
    Industrial,
    Hospital,
    School,
    Park,
    Tree,
    SolarPlant,
    WindTurbine,
    Airport,
    RemoveMode
};

// Stats structure
struct Stats { int cost, maintenance, popEffect, happiness, pollution, green, ecoPoints, energy; };

// Base Building class
class Building {
public:
    BuildingType type;
    Stats stats;
    wchar_t icon;
    bool removable;
    Building(BuildingType t, const Stats& s, wchar_t ic, bool rem = true)
        : type(t), stats(s), icon(ic), removable(rem) {}
    virtual ~Building() {}
};

// Factory for creating buildings
unique_ptr<Building> makeBuilding(BuildingType t) {
    switch (t) {
        case BuildingType::Road:        return make_unique<Building>(t, Stats{10,1,0,0,1,0,0,0}, L'═');
        case BuildingType::Residential: return make_unique<Building>(t, Stats{100,5,4,2,1,0,0,0}, L'🏠');
        case BuildingType::Commercial:  return make_unique<Building>(t, Stats{150,10,2,3,2,0,0,0}, L'🏢');
        case BuildingType::Industrial:  return make_unique<Building>(t, Stats{200,15,1,-2,5,0,0,0}, L'🏭');
        case BuildingType::Hospital:    return make_unique<Building>(t, Stats{300,20,0,4,0,0,2,0}, L'🏥');
        case BuildingType::School:      return make_unique<Building>(t, Stats{250,15,0,3,0,0,2,0}, L'🏫');
        case BuildingType::Park:        return make_unique<Building>(t, Stats{50,2,0,5,-1,5,1,0}, L'🌳');
        case BuildingType::Tree:        return make_unique<Building>(t, Stats{10,0,0,1,-1,3,1,0}, L'🌲');
        case BuildingType::SolarPlant:  return make_unique<Building>(t, Stats{400,5,0,0,0,0,3,5}, L'🔆');
        case BuildingType::WindTurbine: return make_unique<Building>(t, Stats{500,5,0,0,0,0,3,8}, L'🌬️');
        case BuildingType::Airport:     return make_unique<Building>(t, Stats{0,0,0,0,0,0,0,0}, L'✈️', false);
        default:                        return nullptr;
    }
}

// Tile struct and grid
typedef struct { bool isLand; unique_ptr<Building> bld; } Tile;
static Tile grid[MAP_HEIGHT][MAP_WIDTH];

// Check if placement is allowed
bool canPlace(int r, int c, BuildingType mode) {
    if (r < 0 || r >= MAP_HEIGHT || c < 0 || c >= MAP_WIDTH) return false;
    if (!grid[r][c].isLand) return false;
    if (grid[r][c].bld) return false;
    if (mode == BuildingType::None || mode == BuildingType::RemoveMode) return false;
    if (mode != BuildingType::Road && mode != BuildingType::Tree) {
        bool adjRoad = false;
        int dr[4] = { -1, 1, 0, 0 }, dc[4] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i], nc = c + dc[i];
            if (nr >= 0 && nr < MAP_HEIGHT && nc >= 0 && nc < MAP_WIDTH && grid[nr][nc].bld && grid[nr][nc].bld->type == BuildingType::Road) {
                adjRoad = true;
                break;
            }
        }
        if (!adjRoad) return false;
    }
    return true;
}

// Global stats
typedef struct { int cash = 10000, population = 100; } CityStats;
static CityStats stats;

// Time management
static int month = 1, year = 2025;
static const int monthSecDefault = 5;
static int monthSec = monthSecDefault;
static time_t lastTick;
void tickMonth() {
    time_t now = time(nullptr);
    if (difftime(now, lastTick) >= monthSec) {
        lastTick = now;
        if (++month > 12) { month = 1; year++; }
        // TODO: update cash, population, eco points, etc.
    }
}

// Cursor and mode
typedef pair<int,int> Pos;
static Pos cursor = { MAP_HEIGHT/2, MAP_WIDTH/2 };
static BuildingType currentMode = BuildingType::Road;

// Console color helper
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
void setColor(int fg, int bg) {
    SetConsoleTextAttribute(hConsole, fg | (bg << 4));
}

// Draw grid and UI
void draw() {
    system("cls");
    for (int i = 0; i < MAP_HEIGHT; ++i) {
        for (int j = 0; j < MAP_WIDTH; ++j) {
            bool isCursor = (i == cursor.first && j == cursor.second);
            if (isCursor) {
                bool ok = canPlace(i, j, currentMode);
                setColor(0, ok ? 2 : 4);
            } else {
                setColor(7, 0);
            }
            if (grid[i][j].bld) {
                wcout << grid[i][j].bld->icon;
            } else {
                wcout << (grid[i][j].isLand ? L'.' : L'~');
            }
            setColor(7, 0);
        }
        cout << "\n";
    }
    cout << "Mode: ";
    switch (currentMode) {
        case BuildingType::Road: cout << "Road"; break;
        case BuildingType::Residential: cout << "Residential"; break;
        case BuildingType::Commercial: cout << "Commercial"; break;
        case BuildingType::Industrial: cout << "Industrial"; break;
        case BuildingType::Hospital: cout << "Hospital"; break;
        case BuildingType::School: cout << "School"; break;
        case BuildingType::Park: cout << "Park"; break;
        case BuildingType::Tree: cout << "Tree"; break;
        case BuildingType::SolarPlant: cout << "Solar"; break;
        case BuildingType::WindTurbine: cout << "Wind"; break;
        case BuildingType::RemoveMode: cout << "Remove"; break;
        default: cout << "None";
    }
    cout << "    Month: " << month << "/" << year;
    cout << "    Cash: " << stats.cash << "  Pop: " << stats.population << "\n";
    cout << "Keys: Arrows=Move  Space=Action  R=RemoveMode  1-0=Select  Q=Quit\n";
}

// Handle keyboard input
void handleInput() {
    if (!_kbhit()) return;
    int ch = _getch();
    if (ch == 224) { // arrow keys
        int dir = _getch();
        switch (dir) {
            case 72: cursor.first = max(0, cursor.first - 1); break;
            case 80: cursor.first = min(MAP_HEIGHT - 1, cursor.first + 1); break;
            case 75: cursor.second = max(0, cursor.second - 1); break;
            case 77: cursor.second = min(MAP_WIDTH - 1, cursor.second + 1); break;
        }
    } else {
        switch (ch) {
            case '1': currentMode = BuildingType::Road; break;
            case '2': currentMode = BuildingType::Residential; break;
            case '3': currentMode = BuildingType::Commercial; break;
            case '4': currentMode = BuildingType::Industrial; break;
            case '5': currentMode = BuildingType::Hospital; break;
            case '6': currentMode = BuildingType::School; break;
            case '7': currentMode = BuildingType::Park; break;
            case '8': currentMode = BuildingType::Tree; break;
            case '9': currentMode = BuildingType::SolarPlant; break;
            case '0': currentMode = BuildingType::WindTurbine; break;
            case 'r': case 'R': currentMode = BuildingType::RemoveMode; break;
            case ' ': // action: place or remove
                if (currentMode == BuildingType::RemoveMode) {
                    if (grid[cursor.first][cursor.second].bld && grid[cursor.first][cursor.second].bld->removable) {
                        grid[cursor.first][cursor.second].bld.reset();
                    }
                } else {
                    if (canPlace(cursor.first, cursor.second, currentMode)) {
                        grid[cursor.first][cursor.second].bld = makeBuilding(currentMode);
                    }
                }
                break;
            case 'q': case 'Q': exit(0);
            default: break;
        }
    }
}

int main() {
    // Initialize grid
    for (int i = 0; i < MAP_HEIGHT; ++i)
        for (int j = 0; j < MAP_WIDTH; ++j)
            grid[i][j] = { true, nullptr };
    // Place central airport
    grid[MAP_HEIGHT/2][MAP_WIDTH/2].bld = makeBuilding(BuildingType::Airport);
    lastTick = time(nullptr);

    // Main loop
    while (true) {
        tickMonth();
        handleInput();
        draw();
        Sleep(50);
    }
    return 0;
}
