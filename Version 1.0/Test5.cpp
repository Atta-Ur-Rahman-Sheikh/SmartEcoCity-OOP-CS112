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
#include <memory>    // for unique_ptr
#include <thread>    // for std::thread

using namespace std;

void draw_Message(const std::string &msg);
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

// Enum for building types
enum class BuildingType
{
    None,
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
    Airport
};

// Stats structure
typedef struct
{
    int cost;
    int maintenance;
    int popEffect;
    int happiness;
    int pollution;
    int green;
    int ecoPoints;
    int energy;
} Stats;

// Base Building class
class Building
{
public:
    BuildingType type;
    Stats stats;
    string icon;
    bool removable;
    string color;

    Building(BuildingType t, const Stats &s, string ic, string col = WHITE, bool rem = true)
        : type(t), stats(s), icon(ic), color(col), removable(rem) {}

    virtual ~Building() {}

    string getDisplayIcon() const
    {
        return color + icon + RESET;
    }
};

// Concrete buildings
class Road : public Building
{
public:
    Road() : Building(BuildingType::Road, {10, 1, 0, 0, 1, 0, 0, 0}, "==", B_BLACK) {}
};

class Residential : public Building
{
public:
    Residential() : Building(BuildingType::Residential, {100, 5, 4, 2, 1, 0, 0, 0}, "🏠", B_GREEN) {}
};

class Commercial : public Building
{
public:
    Commercial() : Building(BuildingType::Commercial, {150, 10, 2, 3, 2, 0, 0, 0}, "🏢", B_BLUE) {}
};

class Industrial : public Building
{
public:
    Industrial() : Building(BuildingType::Industrial, {200, 15, 1, -2, 5, 0, 0, 0}, "🏭", B_RED) {}
};

class Park : public Building
{
public:
    Park() : Building(BuildingType::Park, {50, 2, 0, 5, -1, 5, 1, 0}, "🌳", B_GREEN) {}
};

class Tree : public Building
{
public:
    Tree() : Building(BuildingType::Tree, {10, 0, 0, 1, -1, 3, 1, 0}, "🌲", B_GREEN) {}
};

class Hospital : public Building
{
public:
    Hospital() : Building(BuildingType::Hospital, {300, 20, 0, 4, 0, 0, 2, 0}, "🏥", B_WHITE) {}
};

class School : public Building
{
public:
    School() : Building(BuildingType::School, {250, 15, 0, 3, 0, 0, 2, 0}, "🏫", B_YELLOW) {}
};

class SolarPlant : public Building
{
public:
    SolarPlant() : Building(BuildingType::SolarPlant, {400, 5, 0, 0, 0, 0, 3, 5}, "🔆", B_YELLOW) {}
};

class WindTurbine : public Building
{
public:
    WindTurbine() : Building(BuildingType::WindTurbine, {500, 5, 0, 0, 0, 0, 3, 8}, "🌀", B_CYAN) {}
};

class Airport : public Building
{
public:
    Airport() : Building(BuildingType::Airport, {1000, 50, 10, 5, 10, 0, 0, -10}, "✈️", B_WHITE, false) {}
};

// Tile structure for map
struct Tile
{
    string texture;
    string colour;
    string type;
    bool isLand;
    unique_ptr<Building> building;
    bool highlight;

    Tile() : isLand(false), highlight(false) {}
};

// Time Manager for months
class TimeManager
{
public:
    int monthLengthSec;
    int currentMonth;
    int currentYear;
    time_t lastTick;
    function<void()> onMonthChange;

    TimeManager(int secs = 10) : monthLengthSec(secs), currentMonth(1), currentYear(2025)
    {
        lastTick = time(nullptr);
    }

    void update()
    {
        time_t now = time(nullptr);
        if (difftime(now, lastTick) >= monthLengthSec)
        {
            lastTick = now;
            currentMonth++;
            if (currentMonth > 12)
            {
                currentMonth = 1;
                currentYear++;
            }
            if (onMonthChange)
                onMonthChange();
        }
    }

    string getDate() const
    {
        string months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        return months[currentMonth - 1] + " " + to_string(currentYear);
    }
};

// UI layout variables
int menuRow = map_height + 4;
int menuCol = leftOffset + 1;
int statsRow = map_height + 4;
int statsCol = map_width * tileWidth + 4;
int maxDropdownItems = 10; // computed after menus init
int tooltipRow = 2;        // set after computing maxDropdownItems
int messageRow = 2;        // row to display action messages

// City class to manage game state
class City
{
public:
    // Game state
    Tile map[map_height][map_width];
    int money;
    int population;
    int happiness;
    int ecoPoints;
    int pollution;
    int greenLevel;
    int energy;
    int maintenance;
    TimeManager timer;
    BuildingType currentTool;
    int totalBuildings;

    // Selected tile for building
    int selectedRow;
    int selectedCol;
    bool hasSelection;

    // Stats display state
    int statsDisplayMode;

    City(int monthSec = 10) : money(10000),
                              population(100),
                              happiness(75),
                              ecoPoints(0),
                              pollution(0),
                              greenLevel(0),
                              energy(0),
                              maintenance(0),
                              timer(monthSec),
                              currentTool(BuildingType::None),
                              totalBuildings(0),
                              selectedRow(-1),
                              selectedCol(-1),
                              hasSelection(false),
                              statsDisplayMode(1)
    {
        generate_Map();
        timer.onMonthChange = [this]()
        { this->monthlyUpdate(); };

        // Place central airport
        int cx = map_height / 2;
        int cy = map_width / 2;
        map[cx][cy].building = make_unique<Airport>();
        totalBuildings++;
    }

    void generate_Map()
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
                    map[i][j].isLand = false;
                }
                else
                {
                    map[i][j].colour = custom_Background(131, 198, 105) + custom_Colour(80, 130, 60);
                    map[i][j].texture = (rand() % 100 < 20 ? "^^" : "  ");
                    map[i][j].type = "Land";
                    map[i][j].isLand = true;
                }
                map[i][j].highlight = false;
            }
        }
    }

    void display_Map()
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
            {
                if (map[i][j].building)
                {
                    cout << map[i][j].building->getDisplayIcon();
                }
                else
                {
                    if (map[i][j].highlight)
                    {
                        cout << REVERSE << map[i][j].colour << map[i][j].texture << RESET;
                    }
                    else
                    {
                        cout << map[i][j].colour << map[i][j].texture << RESET;
                    }
                }
            }

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

    void monthlyUpdate()
    {
        // Calculate income and expenses
        int income = population * 5; // $5 per citizen
        int expenses = maintenance;
        money += (income - expenses);

        // Update population based on happiness and residential buildings
        int popGrowth = 0;
        if (happiness > 50)
        {
            popGrowth = population * 0.02; // 2% growth if happy
        }
        else
        {
            popGrowth = population * 0.01; // 1% growth otherwise
        }
        population += popGrowth;

        // Update happiness based on various factors
        int happinessChange = 0;
        happinessChange += (greenLevel - pollution) / 10;

        // More parks/trees increase happiness
        if (greenLevel > 20)
            happinessChange += 1;

        // High pollution decreases happiness
        if (pollution > 30)
            happinessChange -= 2;

        happiness = max(0, min(100, happiness + happinessChange));

        // Natural reduction in ecoPoints and pollution
        ecoPoints = max(0, ecoPoints - 1);
        pollution = max(0, pollution - 1);

        draw_Stats(); // Update stats display
    }

    bool canBuildAt(int row, int col, BuildingType type)
    {
        // Check bounds
        if (row < 0 || row >= map_height || col < 0 || col >= map_width)
            return false;

        // Can't build on water (except specific buildings)
        if (!map[row][col].isLand && type != BuildingType::None)
            return false;

        // Can't build where there's already a building
        if (map[row][col].building)
            return false;

        // Building-specific restrictions could be added here

        return true;
    }

    bool placeBuilding(int row, int col, BuildingType type)
    {
        if (!canBuildAt(row, col, type))
            return false;

        // Check if enough money
        int cost = 0;
        switch (type)
        {
        case BuildingType::Road:
            map[row][col].building = make_unique<Road>();
            cost = 10;
            break;
        case BuildingType::Residential:
            map[row][col].building = make_unique<Residential>();
            cost = 100;
            break;
        case BuildingType::Commercial:
            map[row][col].building = make_unique<Commercial>();
            cost = 150;
            break;
        case BuildingType::Industrial:
            map[row][col].building = make_unique<Industrial>();
            cost = 200;
            break;
        case BuildingType::Hospital:
            map[row][col].building = make_unique<Hospital>();
            cost = 300;
            break;
        case BuildingType::School:
            map[row][col].building = make_unique<School>();
            cost = 250;
            break;
        case BuildingType::Park:
            map[row][col].building = make_unique<Park>();
            cost = 50;
            break;
        case BuildingType::Tree:
            map[row][col].building = make_unique<Tree>();
            cost = 10;
            break;
        case BuildingType::SolarPlant:
            map[row][col].building = make_unique<SolarPlant>();
            cost = 400;
            break;
        case BuildingType::WindTurbine:
            map[row][col].building = make_unique<WindTurbine>();
            cost = 500;
            break;
        default:
            return false;
        }

        if (money < cost)
        {
            // Not enough money, remove the building
            map[row][col].building.reset();
            draw_Message("Not enough money to build!");
            return false;
        }

        // Apply building costs and effects
        money -= cost;
        maintenance += map[row][col].building->stats.maintenance;
        population += map[row][col].building->stats.popEffect;
        happiness = max(0, min(100, happiness + map[row][col].building->stats.happiness));
        pollution += map[row][col].building->stats.pollution;
        greenLevel += map[row][col].building->stats.green;
        ecoPoints += map[row][col].building->stats.ecoPoints;
        energy += map[row][col].building->stats.energy;
        totalBuildings++;

        draw_Stats();
        return true;
    }

    bool removeBuilding(int row, int col)
    {
        if (row < 0 || row >= map_height || col < 0 || col >= map_width)
            return false;

        if (!map[row][col].building || !map[row][col].building->removable)
            return false;

        // Refund 50% of cost
        int refund = map[row][col].building->stats.cost / 2;

        // Remove building's effects
        maintenance -= map[row][col].building->stats.maintenance;
        population = max(0, population - map[row][col].building->stats.popEffect);
        happiness = max(0, min(100, happiness - map[row][col].building->stats.happiness));
        pollution = max(0, pollution - map[row][col].building->stats.pollution);
        greenLevel = max(0, greenLevel - map[row][col].building->stats.green);
        energy = max(0, energy - map[row][col].building->stats.energy);

        map[row][col].building.reset();
        money += refund;
        totalBuildings--;

        draw_Stats();
        return true;
    }

    void clearHighlights()
    {
        for (int i = 0; i < map_height; i++)
        {
            for (int j = 0; j < map_width; j++)
            {
                map[i][j].highlight = false;
            }
        }
    }

    void setSelection(int row, int col)
    {
        clearHighlights();

        if (row >= 0 && row < map_height && col >= 0 && col < map_width)
        {
            map[row][col].highlight = true;
            selectedRow = row;
            selectedCol = col;
            hasSelection = true;

            // Show info about the selected tile
            if (map[row][col].building)
            {
                string buildingName;
                switch (map[row][col].building->type)
                {
                case BuildingType::Road:
                    buildingName = "Road";
                    break;
                case BuildingType::Residential:
                    buildingName = "House";
                    break;
                case BuildingType::Commercial:
                    buildingName = "Office";
                    break;
                case BuildingType::Industrial:
                    buildingName = "Factory";
                    break;
                case BuildingType::Hospital:
                    buildingName = "Hospital";
                    break;
                case BuildingType::School:
                    buildingName = "School";
                    break;
                case BuildingType::Park:
                    buildingName = "Park";
                    break;
                case BuildingType::Tree:
                    buildingName = "Tree";
                    break;
                case BuildingType::SolarPlant:
                    buildingName = "Solar Plant";
                    break;
                case BuildingType::WindTurbine:
                    buildingName = "Wind Turbine";
                    break;
                case BuildingType::Airport:
                    buildingName = "Airport";
                    break;
                default:
                    buildingName = "Unknown";
                    break;
                }
                draw_Message("Selected: " + buildingName + " at " + char('A' + row) + to_string(col + 1));
            }
            else
            {
                string terrainType = map[row][col].isLand ? "Land" : "Water";
                draw_Message("Selected: " + terrainType + " at " + char('A' + row) + to_string(col + 1));
            }
        }
        else
        {
            hasSelection = false;
        }
    }

    void draw_Stats()
    {
        // Clear stats area
        for (int i = 0; i < 15; i++)
        {
            cout << "\033[" << statsRow + i << ";" << statsCol << "H" << string(30, ' ');
        }

        cout << "\033[" << statsRow << ";" << statsCol << "H" << BOLD << "--- City Stats ---" << RESET;
        cout << "\033[" << statsRow + 1 << ";" << statsCol << "H" << "Date: " << timer.getDate();

        // Display different stats based on mode
        if (statsDisplayMode == 1)
        {
            // Economy & General Stats
            cout << "\033[" << statsRow + 2 << ";" << statsCol << "H" << "Money: $" << money;
            cout << "\033[" << statsRow + 3 << ";" << statsCol << "H" << "Population: " << population;
            cout << "\033[" << statsRow + 4 << ";" << statsCol << "H" << "Happiness: " << happiness << "%";
            cout << "\033[" << statsRow + 5 << ";" << statsCol << "H" << "Buildings: " << totalBuildings;
            cout << "\033[" << statsRow + 6 << ";" << statsCol << "H" << "Monthly Income: $" << (population * 5);
            cout << "\033[" << statsRow + 7 << ";" << statsCol << "H" << "Maintenance: $" << maintenance;
            cout << "\033[" << statsRow + 8 << ";" << statsCol << "H" << "Net Income: $" << (population * 5 - maintenance);
        }
        else if (statsDisplayMode == 2)
        {
            // Environment Stats
            cout << "\033[" << statsRow + 2 << ";" << statsCol << "H" << "Eco Points: " << ecoPoints;
            cout << "\033[" << statsRow + 3 << ";" << statsCol << "H" << "Pollution: " << pollution;
            cout << "\033[" << statsRow + 4 << ";" << statsCol << "H" << "Green Level: " << greenLevel;
            cout << "\033[" << statsRow + 5 << ";" << statsCol << "H" << "Energy: " << energy;
            cout << "\033[" << statsRow + 6 << ";" << statsCol << "H" << "Air Quality: " << max(0, 100 - pollution) << "%";
            cout << "\033[" << statsRow + 7 << ";" << statsCol << "H" << "Sustainability: " << (ecoPoints * 5) << "%";
        }
        else if (statsDisplayMode == 3)
        {
            // Population Stats
            cout << "\033[" << statsRow + 2 << ";" << statsCol << "H" << "Population: " << population;
            cout << "\033[" << statsRow + 3 << ";" << statsCol << "H" << "Happiness: " << happiness << "%";
            cout << "\033[" << statsRow + 4 << ";" << statsCol << "H" << "Growth Rate: " << (happiness > 50 ? "2%" : "1%");
            cout << "\033[" << statsRow + 5 << ";" << statsCol << "H" << "Monthly Growth: " << (happiness > 50 ? population * 0.02 : population * 0.01);
            cout << "\033[" << statsRow + 6 << ";" << statsCol << "H" << "Tax Income: $" << (population * 5);
        }

        // Show current tool (in all modes)
        string toolName;
        switch (currentTool)
        {
        case BuildingType::None:
            toolName = "None";
            break;
        case BuildingType::Road:
            toolName = "Road";
            break;
        case BuildingType::Residential:
            toolName = "House";
            break;
        case BuildingType::Commercial:
            toolName = "Office";
            break;
        case BuildingType::Industrial:
            toolName = "Factory";
            break;
        case BuildingType::Hospital:
            toolName = "Hospital";
            break;
        case BuildingType::School:
            toolName = "School";
            break;
        case BuildingType::Park:
            toolName = "Park";
            break;
        case BuildingType::Tree:
            toolName = "Tree";
            break;
        case BuildingType::SolarPlant:
            toolName = "Solar Plant";
            break;
        case BuildingType::WindTurbine:
            toolName = "Wind Turbine";
            break;
        default:
            toolName = "None";
            break;
        }
        cout << "\033[" << statsRow + 11 << ";" << statsCol << "H" << "Current Tool: " << toolName;
        cout << "\033[" << statsRow + 13 << ";" << statsCol << "H" << "[1-3] Change Stats View";
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

// Global city reference for menu actions
City *cityPtr = nullptr;

// Map coordinates from console position
bool mapCoordinatesFromConsole(int consoleX, int consoleY, int &mapRow, int &mapCol)
{
    // Check if within the map area
    if (consoleY < 1 || consoleY > map_height ||
        consoleX < leftOffset + 3 || consoleX >= leftOffset + 3 + map_width * tileWidth)
        return false;

    mapRow = consoleY - 1;
    mapCol = (consoleX - leftOffset - 3) / tileWidth;

    if (mapRow >= 0 && mapRow < map_height && mapCol >= 0 && mapCol < map_width)
        return true;
    return false;
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

int main()
{
    cin.get(); 
    // --- 1) Console setup --------------------------------
    emoji_Support();           // UTF-8 + emoji support (Test5.cpp)  
    clear_Screen();            // blank out console buffer  

    // --- 2) Create city and global pointer -------------
    City city;                 // your City class instance  
    cityPtr = &city;           // used by menu lambdas and actions  

    // --- 3) Menu setup (customizable!) -----------------
    menus = {
        { "Build", 
          { "Road","House","Office","Factory","Hospital","School","Park","Tree","Solar","Wind" },
          {
            [](){ cityPtr->currentTool = BuildingType::Road; },
            [](){ cityPtr->currentTool = BuildingType::Residential; },
            [](){ cityPtr->currentTool = BuildingType::Commercial; },
            [](){ cityPtr->currentTool = BuildingType::Industrial; },
            [](){ cityPtr->currentTool = BuildingType::Hospital; },
            [](){ cityPtr->currentTool = BuildingType::School; },
            [](){ cityPtr->currentTool = BuildingType::Park; },
            [](){ cityPtr->currentTool = BuildingType::Tree; },
            [](){ cityPtr->currentTool = BuildingType::SolarPlant; },
            [](){ cityPtr->currentTool = BuildingType::WindTurbine; }
          }
        },
        { "Stats",
          { "General","Environment","Population" },
          {
            [](){ cityPtr->statsDisplayMode = 1; },
            [](){ cityPtr->statsDisplayMode = 2; },
            [](){ cityPtr->statsDisplayMode = 3; }
          }
        }
    };
    rebuild_TabPositions();     // calculate tab X‑positions  
    compute_UI_positions();     // compute tooltip/message rows  

    // --- 4) Initial draw --------------------------------
    draw_Menu();               // top menu + dropdown (Test5.cpp)  
    city.display_Map();        // map grid with any buildings  
    city.draw_Stats();         // stats panel  

    draw_Message("Welcome! Click to select/place.");  

    // --- 5) Mouse callbacks --------------------------------
    MouseInputHandler mouse;    // handles console mouse events  

    mouse.setLeftClickCallback([](int x,int y){
        int r,c;
        if(mapCoordinatesFromConsole(x,y,r,c)){
            cityPtr->setSelection(r,c);               // highlight & message  
            if(cityPtr->currentTool!=BuildingType::None){
                cityPtr->placeBuilding(r,c,cityPtr->currentTool);
                cityPtr->draw_Stats();               // update stats after build
            }
            cityPtr->display_Map();                  // refresh map view
        }
    });

    mouse.setMoveCallback([](int x,int y){
        int r,c;
        if(mapCoordinatesFromConsole(x,y,r,c)){
            cityPtr->setSelection(r,c);               // hover highlight
        }
    });

    // --- 6) Enter event loop ------------------------------
    mouse.startListening();      // blocks, processing mouse events  

    return 0;
}



