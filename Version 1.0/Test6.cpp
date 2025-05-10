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
#include <thread>    // for std::thread
#include <fstream>   // for file operations
#include <sstream>   // for string stream
#include <memory>    // for std::unique_ptr

using namespace std;

// Forward class declarations
class City;
class Building;
struct Tile;
struct Menu;

// Forward function declarations
void draw_Message(const std::string &msg);
void save_Config(const string &filename);
bool load_Config(const string &filename);
void compute_UI_positions();
void setup_Menus();
void rebuild_TabPositions();
void draw_Menu();
bool handle_Menu_Click(int x, int y);
void handle_Menu_Hover(int x, int y);
void game_Loop();
void emoji_Support(bool reconfigure = false);
void handle_Mouse_Input(int x, int y, bool isClick);
void play_Sound(int soundType);
void clear_Screen();
void hide_Cursor();
void show_Cursor();
void setup_Terminal();
void reset_Terminal();
void init_Game();
void cleanup_Game();

// Global Configuration Structure
struct GameConfig
{
    int monthLengthSeconds = 10;  // Month length in seconds
    int startingMoney = 10000;    // Starting money
    int startingPopulation = 100; // Starting population
    int leftOffset = 11;          // Left offset for map display
    int mapWidth = 42;            // Map width
    int mapHeight = 16;           // Map height
    int tileWidth = 2;            // Width of each tile
    int menuSpacing = 3;          // Space between menu items
    bool enableSounds = true;     // Enable sound effects
    int uiTheme = 0;              // 0=Default, 1=Dark, 2=Light, 3=High Contrast
    int fontHeight = 24;          // Console font height
    string fontName = "Consolas"; // Console font name
} config;

// Global variables
int statsRow = 2;      // Row position for stats display
int statsCol = 60;     // Column position for stats display
int logRow = 20;       // Row position for log display
int logCol = 11;       // Column position for log display
int tooltipRow = 24;   // Row position for tooltip display
int messageRow = 25;   // Row for action messages
int menuRow = 1;       // Row for menu tabs
int menuCol = 10;      // Column for menu start
int maxDropdownItems = 10; // Max dropdown items to display

// These variables will be initialized from config in init_Game()
int leftOffset;
int map_width;
int map_height;
int tileWidth;

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
    Airport,
    WaterTreatment,
    RecyclingCenter,
    FireStation,
    PoliceStation
};

// Menu structures
struct MenuItem {
    int x, y;           // Position
    int width;          // Display width
    string label;       // Display label
    string tooltip;     // Tooltip text
    BuildingType toolType; // Associated building tool
    function<void()> action; // Custom action callback

    MenuItem(int _x, int _y, string _label, string _tooltip, BuildingType _type = BuildingType::None, function<void()> _action = nullptr)
        : x(_x), y(_y), width(_label.length()), label(_label), tooltip(_tooltip), toolType(_type), action(_action) {}
};

struct Menu {
    string name;
    vector<MenuItem> items;

    Menu(string _name) : name(_name) {}
};

// Global menus container
vector<Menu> menus;

// Global city instance
City *city; // Declaration of the global city pointer

// Prerequisite functions
void emoji_Support(bool reconfigure)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    GetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

    // Only set height if reconfiguring or first setup
    if (reconfigure || cfi.dwFontSize.Y != config.fontHeight)
    {
        cfi.dwFontSize.Y = config.fontHeight;
    }

    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    // Convert the string to wstring for wcscpy_s
    std::wstring wFontName(config.fontName.begin(), config.fontName.end());
    wcscpy_s(cfi.FaceName, wFontName.c_str());

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

// Hide console cursor
void hide_Cursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// Show console cursor
void show_Cursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// ANSI custom colour
string custom_Colour(int r, int g, int b) { return "\033[38;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m"; }
string custom_Background(int r, int g, int b) { return "\033[48;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m"; }

// Apply theme colors based on current UI theme
string theme_Color(int colorType)
{
    // Color types: 0=primary, 1=secondary, 2=highlight, 3=background, 4=text
    switch (config.uiTheme)
    {
    case 1: // Dark theme
        switch (colorType)
        {
        case 0:
            return custom_Colour(75, 75, 75);
        case 1:
            return custom_Colour(150, 150, 150);
        case 2:
            return custom_Colour(50, 120, 200);
        case 3:
            return custom_Background(20, 20, 20);
        case 4:
            return custom_Colour(200, 200, 200);
        default:
            return WHITE;
        }
    case 2: // Light theme
        switch (colorType)
        {
        case 0:
            return custom_Colour(50, 50, 50);
        case 1:
            return custom_Colour(100, 100, 100);
        case 2:
            return custom_Colour(0, 100, 200);
        case 3:
            return custom_Background(240, 240, 240);
        case 4:
            return custom_Colour(0, 0, 0);
        default:
            return BLACK;
        }
    case 3: // High contrast
        switch (colorType)
        {
        case 0:
            return YELLOW;
        case 1:
            return WHITE;
        case 2:
            return CYAN;
        case 3:
            return BG_BLACK;
        case 4:
            return WHITE;
        default:
            return WHITE;
        }
    default: // Default theme
        switch (colorType)
        {
        case 0:
            return CYAN;
        case 1:
            return WHITE;
        case 2:
            return YELLOW;
        case 3:
            return ""; // Default background
        case 4:
            return WHITE;
        default:
            return WHITE;
        }
    }
}

// Sound effects function
void play_Sound(int soundType)
{
    if (!config.enableSounds)
        return;

    // Sound types: 0=click, 1=build, 2=demolish, 3=error, 4=success
    switch (soundType)
    {
    case 0:
        Beep(800, 100);
        break; // Click
    case 1:
        Beep(500, 100);
        Beep(600, 100);
        break; // Build
    case 2:
        Beep(400, 100);
        Beep(300, 100);
        break; // Demolish
    case 3:
        Beep(200, 300);
        break; // Error
    case 4:
        Beep(700, 100);
        Beep(900, 200);
        break; // Success
    }
}

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
    int safety;
    int education;
    int health;
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
    string description;

    Building(BuildingType t, const Stats &s, string ic, string col = WHITE, bool rem = true, string desc = "")
        : type(t), stats(s), icon(ic), color(col), removable(rem), description(desc) {}

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
    Road() : Building(BuildingType::Road,
                      {10, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
                      "==", B_BLACK, true,
                      "Connects buildings and allows traffic flow") {}
};

class Residential : public Building
{
public:
    Residential() : Building(BuildingType::Residential,
                             {100, 5, 4, 2, 1, 0, 0, 0, 0, 0, 0},
                             "🏠", B_GREEN, true,
                             "Houses for citizens to live in") {}
};

class Commercial : public Building
{
public:
    Commercial() : Building(BuildingType::Commercial,
                            {150, 10, 2, 3, 2, 0, 0, 0, 0, 0, 0},
                            "🏢", B_BLUE, true,
                            "Offices and shops that provide jobs and services") {}
};

class Industrial : public Building
{
public:
    Industrial() : Building(BuildingType::Industrial,
                            {200, 15, 1, -2, 5, 0, 0, 0, 0, 0, 0},
                            "🏭", B_RED, true,
                            "Factories that provide jobs but create pollution") {}
};

class Park : public Building
{
public:
    Park() : Building(BuildingType::Park,
                      {50, 2, 0, 5, -1, 5, 1, 0, 0, 0, 1},
                      "🌳", B_GREEN, true,
                      "Improves happiness and reduces pollution") {}
};

class Tree : public Building
{
public:
    Tree() : Building(BuildingType::Tree,
                      {10, 0, 0, 1, -1, 3, 1, 0, 0, 0, 0},
                      "🌲", B_GREEN, true,
                      "Natural beauty, low cost eco-improvement") {}
};

class Hospital : public Building
{
public:
    Hospital() : Building(BuildingType::Hospital,
                          {300, 20, 0, 4, 0, 0, 2, 0, 0, 0, 10},
                          "🏥", B_WHITE, true,
                          "Improves health and happiness") {}
};

class School : public Building
{
public:
    School() : Building(BuildingType::School,
                        {250, 15, 0, 3, 0, 0, 2, 0, 0, 10, 0},
                        "🏫", B_YELLOW, true,
                        "Improves education and happiness") {}
};

class SolarPlant : public Building
{
public:
    SolarPlant() : Building(BuildingType::SolarPlant,
                            {400, 5, 0, 0, 0, 0, 3, 5, 0, 0, 0},
                            "🔆", B_YELLOW, true,
                            "Clean renewable energy source") {}
};

class WindTurbine : public Building
{
public:
    WindTurbine() : Building(BuildingType::WindTurbine,
                             {500, 5, 0, 0, 0, 0, 3, 8, 0, 0, 0},
                             "🌀", B_CYAN, true,
                             "Efficient clean energy") {}
};

class Airport : public Building
{
public:
    Airport() : Building(BuildingType::Airport,
                         {1000, 50, 10, 5, 10, 0, 0, -10, 0, 0, 0},
                         "✈️", B_WHITE, false,
                         "Major transportation hub") {}
};

class WaterTreatment : public Building
{
public:
    WaterTreatment() : Building(BuildingType::WaterTreatment,
                                {350, 15, 0, 2, -3, 2, 2, -2, 0, 0, 2},
                                "💧", B_BLUE, true,
                                "Treats wastewater and reduces pollution") {}
};

class RecyclingCenter : public Building
{
public:
    RecyclingCenter() : Building(BuildingType::RecyclingCenter,
                                 {300, 10, 0, 1, -4, 3, 3, 0, 0, 0, 0},
                                 "♻️", B_GREEN, true,
                                 "Reduces waste and increases eco points") {}
};

class FireStation : public Building
{
public:
    FireStation() : Building(BuildingType::FireStation,
                             {200, 10, 0, 2, 0, 0, 0, 0, 5, 0, 0},
                             "🚒", B_RED, true,
                             "Improves safety and emergency response") {}
};

class PoliceStation : public Building
{
public:
    PoliceStation() : Building(BuildingType::PoliceStation,
                               {200, 10, 0, 1, 0, 0, 0, 0, 8, 0, 0},
                               "👮", B_BLUE, true,
                               "Improves safety and reduces crime") {}
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

    Tile() : isLand(false), highlight(false), building(nullptr) {}
    ~Tile() { building.reset(); }
    
    // Delete copy operations
    Tile(const Tile&) = delete;
    Tile& operator=(const Tile&) = delete;
    
    // Add move operations
    Tile(Tile&& other) noexcept 
        : texture(std::move(other.texture))
        , colour(std::move(other.colour))
        , type(std::move(other.type))
        , isLand(other.isLand)
        , building(std::move(other.building))
        , highlight(other.highlight) 
    {
        other.building = nullptr;
    }
        
    Tile& operator=(Tile&& other) noexcept {
        if (this != &other) {
            building.reset();
            texture = std::move(other.texture);
            colour = std::move(other.colour);
            type = std::move(other.type);
            isLand = other.isLand;
            building = std::move(other.building);
            highlight = other.highlight;
            other.building = nullptr;
        }
        return *this;
    }

    void display() const
    {
        if (building)
        {
            cout << building->getDisplayIcon();
        }
        else
        {
            if (highlight)
            {
                cout << REVERSE << colour << texture << RESET;
            }
            else
            {
                cout << colour << texture << RESET;
            }
        }
    }
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

// City class to manage game state
class City
{
protected:
    vector<vector<unique_ptr<Tile>>> map;
    unique_ptr<TimeManager> timer;
    vector<string> gameLog;
    int maxLogEntries = 50;

    void initializeMap() {
        map.resize(map_height);
        for (int i = 0; i < map_height; i++) {
            map[i].resize(map_width);
            for (int j = 0; j < map_width; j++) {
                map[i][j] = make_unique<Tile>();
            }
        }
    }

    bool isValidPosition(int row, int col) const {
        return row >= 0 && row < map_height && col >= 0 && col < map_width;
    }

    bool canPlaceBuilding(int row, int col, BuildingType type) const {
        if (!isValidPosition(row, col) || map[row][col]->building) {
            return false;
        }

        // Check if tile is land
        if (!map[row][col]->isLand) {
            return false;
        }

        // Special placement rules
        switch (type) {
            case BuildingType::Road:
                return true;
            case BuildingType::Tree:
                return true;
            default:
                // Other buildings need to be adjacent to a road
                return hasAdjacentRoad(row, col);
        }
    }

    bool hasAdjacentRoad(int row, int col) const {
        const int dr[] = {-1, 1, 0, 0};
        const int dc[] = {0, 0, -1, 1};
        
        for (int i = 0; i < 4; i++) {
            int nr = row + dr[i];
            int nc = col + dc[i];
            if (isValidPosition(nr, nc) && 
                map[nr][nc]->building && 
                map[nr][nc]->building->type == BuildingType::Road) {
                return true;
            }
        }
        return false;
    }

    void updateCityStats(Building* building, bool adding) {
        int factor = adding ? 1 : -1;
        maintenance += building->stats.maintenance * factor;
        pollution += building->stats.pollution * factor;
        greenLevel += building->stats.green * factor;
        ecoPoints += building->stats.ecoPoints * factor;
        energy += building->stats.energy * factor;
        safety += building->stats.safety * factor;
        education += building->stats.education * factor;
        health += building->stats.health * factor;
    }

    string getCoordinateString(int row, int col) const {
        return string(1, 'A' + row) + to_string(col + 1);
    }

public:
    // Game state
    int money;
    int population;
    int happiness;
    int ecoPoints;
    int pollution;
    int greenLevel;
    int energy;
    int maintenance;
    int safety;
    int education;
    int health;
    BuildingType currentTool;
    int totalBuildings;

    // Selected tile for building
    int selectedRow;
    int selectedCol;
    bool hasSelection;

    // Stats display mode
    int statsDisplayMode;

    // Helper functions
    int calculatePopulation() const {
        int basePop = 100;
        int popFromBuildings = 0;
        
        // Count population from residential buildings
        for (int i = 0; i < map_height; i++) {
            for (int j = 0; j < map_width; j++) {
                if (map[i][j]->building && map[i][j]->building->type == BuildingType::Residential) {
                    popFromBuildings += 50; // Each house adds 50 people
                }
            }
        }
        
        // Apply happiness modifier
        float happinessModifier = 1.0f + (happiness - 50) / 100.0f;
        return basePop + (int)(popFromBuildings * happinessModifier);
    }

    int calculateHappiness() const {
        int baseHappiness = 50;
        
        // Factors affecting happiness
        int greenFactor = greenLevel / 10;
        int pollutionFactor = -pollution / 5;
        int safetyFactor = safety / 10;
        int healthFactor = health / 10;
        int educationFactor = education / 10;
        
        // Calculate total happiness
        int totalHappiness = baseHappiness + greenFactor + pollutionFactor + 
                           safetyFactor + healthFactor + educationFactor;
        
        // Ensure happiness stays within bounds
        return max(0, min(100, totalHappiness));
    }

    int calculateEcoPoints() const {
        int points = 0;
        
        // Base points from green buildings
        points += greenLevel;
        
        // Points from eco-friendly buildings
        for (int i = 0; i < map_height; i++) {
            for (int j = 0; j < map_width; j++) {
                if (map[i][j]->building) {
                    points += map[i][j]->building->stats.ecoPoints;
                }
            }
        }
        
        // Penalty for pollution
        points -= pollution / 2;
        
        return max(0, points);
    }

    int calculateIncome() const {
        int baseIncome = population * 5; // $5 per citizen
        int commercialIncome = 0;
        
        // Income from commercial buildings
        for (int i = 0; i < map_height; i++) {
            for (int j = 0; j < map_width; j++) {
                if (map[i][j]->building && map[i][j]->building->type == BuildingType::Commercial) {
                    commercialIncome += 100; // Each commercial building adds $100
                }
            }
        }
        
        return baseIncome + commercialIncome;
    }

    int calculateExpenses() const {
        return maintenance; // Currently just maintenance costs
    }

    // Static helper functions
    static int getBuildingCost(BuildingType type) {
        switch (type) {
            case BuildingType::Road: return 10;
            case BuildingType::Residential: return 100;
            case BuildingType::Commercial: return 150;
            case BuildingType::Industrial: return 200;
            case BuildingType::Hospital: return 300;
            case BuildingType::School: return 250;
            case BuildingType::Park: return 50;
            case BuildingType::Tree: return 10;
            case BuildingType::SolarPlant: return 400;
            case BuildingType::WindTurbine: return 500;
            case BuildingType::WaterTreatment: return 350;
            case BuildingType::RecyclingCenter: return 300;
            case BuildingType::FireStation: return 200;
            case BuildingType::PoliceStation: return 200;
            case BuildingType::Airport: return 1000;
            default: return 0;
        }
    }

    static string getBuildingName(BuildingType type) {
        switch (type) {
            case BuildingType::Road: return "Road";
            case BuildingType::Residential: return "House";
            case BuildingType::Commercial: return "Office";
            case BuildingType::Industrial: return "Factory";
            case BuildingType::Hospital: return "Hospital";
            case BuildingType::School: return "School";
            case BuildingType::Park: return "Park";
            case BuildingType::Tree: return "Tree";
            case BuildingType::SolarPlant: return "Solar Plant";
            case BuildingType::WindTurbine: return "Wind Turbine";
            case BuildingType::WaterTreatment: return "Water Treatment";
            case BuildingType::RecyclingCenter: return "Recycling Center";
            case BuildingType::FireStation: return "Fire Station";
            case BuildingType::PoliceStation: return "Police Station";
            case BuildingType::Airport: return "Airport";
            default: return "Unknown Building";
        }
    }

    static unique_ptr<Building> createBuilding(BuildingType type) {
        switch (type) {
            case BuildingType::Road: return make_unique<Road>();
            case BuildingType::Residential: return make_unique<Residential>();
            case BuildingType::Commercial: return make_unique<Commercial>();
            case BuildingType::Industrial: return make_unique<Industrial>();
            case BuildingType::Hospital: return make_unique<Hospital>();
            case BuildingType::School: return make_unique<School>();
            case BuildingType::Park: return make_unique<Park>();
            case BuildingType::Tree: return make_unique<Tree>();
            case BuildingType::SolarPlant: return make_unique<SolarPlant>();
            case BuildingType::WindTurbine: return make_unique<WindTurbine>();
            case BuildingType::WaterTreatment: return make_unique<WaterTreatment>();
            case BuildingType::RecyclingCenter: return make_unique<RecyclingCenter>();
            case BuildingType::FireStation: return make_unique<FireStation>();
            case BuildingType::PoliceStation: return make_unique<PoliceStation>();
            case BuildingType::Airport: return make_unique<Airport>();
            default: return nullptr;
        }
    }

    City(int monthSec = 10) : 
        money(10000),
        population(100),
        happiness(75),
        ecoPoints(0),
        pollution(0),
        greenLevel(0),
        energy(0),
        maintenance(0),
        safety(0),
        education(0),
        health(0),
        currentTool(BuildingType::None),
        totalBuildings(0),
        selectedRow(-1),
        selectedCol(-1),
        hasSelection(false),
        statsDisplayMode(1)
    {
        timer = make_unique<TimeManager>(monthSec);
        initializeMap();
        generate_Map();
        
        timer->onMonthChange = [this]() { this->monthlyUpdate(); };

        // Place central airport
        int cx = map_height / 2;
        int cy = map_width / 2;
        map[cx][cy]->building = make_unique<Airport>();
        totalBuildings++;

        // Add initial log entry
        addToLog("Welcome to Eco-City Builder! City founded in " + timer->getDate());
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
                    map[i][j]->colour = custom_Background(117, 226, 254) + custom_Colour(93, 181, 203);
                    map[i][j]->texture = (rand() % 100 < 30 ? "~ " : "  ");
                    map[i][j]->type = "Water";
                    map[i][j]->isLand = false;
                }
                else
                {
                    map[i][j]->colour = custom_Background(131, 198, 105) + custom_Colour(80, 130, 60);
                    map[i][j]->texture = (rand() % 100 < 20 ? "^^" : "  ");
                    map[i][j]->type = "Land";
                    map[i][j]->isLand = true;
                }
                map[i][j]->highlight = false;
            }
        }
    }

    void display_Map()
    {
        for (int i = 0; i < map_height; i++) {
            for (int j = 0; j < map_width; j++) {
                cout << "\033[" << (i + 2) << ";" << (j * tileWidth + leftOffset) << "H";
                if (i == selectedRow && j == selectedCol) {
                    cout << REVERSE;
                }
                map[i][j]->display();
                cout << RESET;
            }
        }
    }

    void addToLog(const string& message) {
        gameLog.push_back(message);
        if (gameLog.size() > maxLogEntries) {
            gameLog.erase(gameLog.begin());
        }
    }

    void monthlyUpdate() {
        // Update city stats based on buildings and current state
        population = calculatePopulation();
        happiness = calculateHappiness();
        ecoPoints = calculateEcoPoints();
        
        // Add monthly income/expenses
        int income = calculateIncome();
        int expenses = calculateExpenses();
        money += income - expenses;
        
        // Log monthly update
        addToLog(timer->getDate() + ": Population: " + to_string(population) + 
                ", Money: $" + to_string(money) + 
                ", Happiness: " + to_string(happiness) + "%");
    }

    bool placeBuilding(int row, int col, BuildingType type) {
        if (!isValidPosition(row, col) || !canPlaceBuilding(row, col, type)) {
            return false;
        }

        int cost = getBuildingCost(type);
        if (money < cost) {
            addToLog("Not enough money to build " + getBuildingName(type));
            return false;
        }

        // Create and place the building
        unique_ptr<Building> building = createBuilding(type);
        if (!building) {
            return false;
        }

        // Update city stats
        money -= cost;
        totalBuildings++;
        updateCityStats(building.get(), true);

        // Place the building
        map[row][col]->building = move(building);
        addToLog("Built " + getBuildingName(type) + " at " + getCoordinateString(row, col));
        
        return true;
    }

    bool removeBuilding(int row, int col) {
        if (!isValidPosition(row, col) || !map[row][col]->building) {
            return false;
        }

        if (!map[row][col]->building->removable) {
            addToLog("This building cannot be removed");
            return false;
        }

        // Update city stats
        updateCityStats(map[row][col]->building.get(), false);
        totalBuildings--;

        // Remove the building
        string buildingName = getBuildingName(map[row][col]->building->type);
        map[row][col]->building.reset();
        addToLog("Removed " + buildingName + " at " + getCoordinateString(row, col));
        
        return true;
    }

    void draw_Stats() {
        // Clear stats area
        cout << "\033[" << statsRow << ";" << statsCol << "H";
        cout << string(40, ' ');

        // Display stats based on current mode
        switch (statsDisplayMode) {
            case 1: // General stats
                cout << "\033[" << statsRow << ";" << statsCol << "H";
                cout << BOLD << "City Stats" << RESET << "\n";
                cout << "Money: $" << money << "\n";
                cout << "Population: " << population << "\n";
                cout << "Happiness: " << happiness << "%\n";
                cout << "Buildings: " << totalBuildings;
                break;
            case 2: // Environment stats
                cout << "\033[" << statsRow << ";" << statsCol << "H";
                cout << BOLD << "Environment" << RESET << "\n";
                cout << "Eco Points: " << ecoPoints << "\n";
                cout << "Pollution: " << pollution << "\n";
                cout << "Green Level: " << greenLevel << "\n";
                cout << "Energy: " << energy;
                break;
            case 3: // Services stats
                cout << "\033[" << statsRow << ";" << statsCol << "H";
                cout << BOLD << "Services" << RESET << "\n";
                cout << "Safety: " << safety << "\n";
                cout << "Education: " << education << "\n";
                cout << "Health: " << health << "\n";
                cout << "Maintenance: " << maintenance;
                break;
        }
    }

    void draw_Log() {
        // Clear log area
        cout << "\033[" << logRow << ";" << logCol << "H";
        cout << string(60, ' ');

        // Display recent log entries
        cout << "\033[" << logRow << ";" << logCol << "H";
        cout << BOLD << "Recent Events:" << RESET << "\n";
        for (size_t i = max(0, static_cast<int>(gameLog.size()) - 5); i < gameLog.size(); i++) {
            cout << gameLog[i] << "\n";
        }
    }
};

// Mouse input handler
void handle_Mouse_Input(int x, int y, bool isClick)
{
    // Adjust for UI offsets
    int mapX = x - leftOffset;
    int mapY = y - 2; // Account for top border

    // Check if click is within map boundaries
    if (mapX >= 0 && mapX < map_width * tileWidth && mapY >= 0 && mapY < map_height)
    {
        int col = mapX / tileWidth;
        int row = mapY;

        if (isClick)
        {
            // Handle click on map
            if (city->currentTool == BuildingType::None)
            {
                // Select mode - just select the tile
                city->selectedRow = row;
                city->selectedCol = col;
                city->hasSelection = true;

                // Show info about the tile
                if (city->map[row][col]->building)
                {
                    string buildingName;
                    switch (city->map[row][col]->building->type)
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
                    case BuildingType::WaterTreatment:
                        buildingName = "Water Treatment";
                        break;
                    case BuildingType::RecyclingCenter:
                        buildingName = "Recycling Center";
                        break;
                    case BuildingType::FireStation:
                        buildingName = "Fire Station";
                        break;
                    case BuildingType::PoliceStation:
                        buildingName = "Police Station";
                        break;
                    default:
                        buildingName = "Unknown Building";
                        break;
                    }
                    draw_Message(buildingName + " at " + char('A' + row) + to_string(col + 1));
                }
                else
                {
                    draw_Message(city->map[row][col]->type + " at " + char('A' + row) + to_string(col + 1));
                }
            }
            else
            {
                // Build with current tool
                city->placeBuilding(row, col, city->currentTool);
            }

            // Redraw the map after interaction
            city->display_Map();
        }
        else
        {
            // Handle hover over map - show tile info in tooltip
            cout << "\033[" << tooltipRow << ";1H" << string(80, ' '); // Clear line
            cout << "\033[" << tooltipRow << ";1H" << ITALIC
                 << "Position: " << char('A' + row) << (col + 1);

            if (city->map[row][col]->building)
            {
                string buildingName;
                switch (city->map[row][col]->building->type)
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
                case BuildingType::WaterTreatment:
                    buildingName = "Water Treatment";
                    break;
                case BuildingType::RecyclingCenter:
                    buildingName = "Recycling Center";
                    break;
                case BuildingType::FireStation:
                    buildingName = "Fire Station";
                    break;
                case BuildingType::PoliceStation:
                    buildingName = "Police Station";
                    break;
                default:
                    buildingName = "Unknown Building";
                    break;
                }
                cout << " - " << buildingName << RESET;
            }
            else
            {
                cout << " - " << city->map[row][col]->type << RESET;
            }
        }
    }
    // Check if interaction is with menu system
    else if (y >= menuRow && y <= menuRow + maxDropdownItems + 1)
    {
        if (isClick)
        {
            handle_Menu_Click(x, y);
        }
        else
        {
            handle_Menu_Hover(x, y);
        }
    }
    // Check if interaction is with stats cycle button
    else if (y == statsRow + 14 && x >= statsCol && x < statsCol + 15)
    {
        if (isClick)
        {
            city->statsDisplayMode = (city->statsDisplayMode % 3) + 1;
            city->draw_Stats();
            play_Sound(0); // Click sound
        }
    }
}

// Handle clicks on menu items
bool handle_Menu_Click(int x, int y)
{
    // Implementation of menu click handler
    // Check each menu item to see if it was clicked
    for (Menu& menu : menus)
    {
        for (MenuItem& item : menu.items)
        {
            if (x >= item.x && x < item.x + item.width &&
                y >= item.y && y < item.y + 1)
            {
                // Handle the action for this menu item
                if (item.action)
                {
                    item.action();
                    return true;
                }
                else if (item.toolType != BuildingType::None)
                {
                    // Set the current tool
                    city->currentTool = item.toolType;
                    draw_Message("Selected " + item.label);
                    play_Sound(0); // Click sound
                    return true;
                }
            }
        }
    }
    return false;
}

// Handle hovering over menu items
void handle_Menu_Hover(int x, int y)
{
    // Implementation of menu hover handler
    // Check each menu item to see if mouse is hovering over it
    for (Menu& menu : menus)
    {
        for (MenuItem& item : menu.items)
        {
            if (x >= item.x && x < item.x + item.width &&
                y >= item.y && y < item.y + 1)
            {
                // Display tooltip for this menu item
                cout << "\033[" << tooltipRow << ";1H" << string(80, ' '); // Clear line
                cout << "\033[" << tooltipRow << ";1H" << ITALIC << item.tooltip << RESET;
                return;
            }
        }
    }
}

// Game loop function
void game_Loop()
{
    bool running = true;
    int mouseX = 0, mouseY = 0;
    bool mouseClicked = false;

    // Main game loop
    while (running)
    {
        // Process input
        if (_kbhit())
        {
            int key = _getch();
            if (key == 27) // ESC key
            {
                running = false;
            }
            else if (key == 's' || key == 'S')
            {
                // Save game
                save_Config("eco_city_save.txt");
                draw_Message("Game saved!");
            }
            else if (key == 'l' || key == 'L')
            {
                // Load game
                if (load_Config("eco_city_save.txt"))
                {
                    draw_Message("Game loaded!");
                }
                else
                {
                    draw_Message("Could not load save file!");
                }
            }
            // Add other key handlers here
        }

        // Check for mouse input
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD ir[128];
        DWORD events;
        
        if (PeekConsoleInput(hInput, ir, 128, &events) && events > 0)
        {
            ReadConsoleInput(hInput, ir, events, &events);
            
            for (DWORD i = 0; i < events; i++)
            {
                if (ir[i].EventType == MOUSE_EVENT)
                {
                    MOUSE_EVENT_RECORD mer = ir[i].Event.MouseEvent;
                    
                    // Update mouse position
                    mouseX = mer.dwMousePosition.X;
                    mouseY = mer.dwMousePosition.Y;
                    
                    // Check for mouse clicks
                    if (mer.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
                    {
                        mouseClicked = true;
                    }
                    else
                    {
                        mouseClicked = false;
                    }
                    
                    // Handle mouse input
                    handle_Mouse_Input(mouseX, mouseY, mouseClicked);
                }
            }
        }

        // Update game state
        city->timer->update();

        // Redraw
        city->display_Map();
        draw_Menu();
        
        // Limit frame rate
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

void setup_Terminal()
{
    // enable ANSI processing on Windows console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

void reset_Terminal()
{
    // restore default console mode if you changed it
    // (you can leave this empty if you don't need special teardown)
}

// Initialize the game
void init_Game()
{
    // Set up terminal and UI
    setup_Terminal();
    emoji_Support(true);

    // Initialize configuration
    config.monthLengthSeconds = 30;
    config.startingMoney = 10000;
    config.startingPopulation = 100;
    config.leftOffset = 5;
    config.mapWidth = 15;
    config.mapHeight = 10;
    config.tileWidth = 3;
    config.menuSpacing = 2;
    config.enableSounds = true;
    config.uiTheme = 0;
    config.fontHeight = 16;
    config.fontName = "Consolas";

    // Try to load config from file
    if (!load_Config("ecocity.cfg"))
    {
        // Use defaults and create file
        save_Config("ecocity.cfg");
    }

    // Update globals from config
    leftOffset = config.leftOffset;
    map_width = config.mapWidth;
    map_height = config.mapHeight;
    tileWidth = config.tileWidth;

    // Calculate UI positions
    compute_UI_positions();

    // Create city
    city = new City();

    // Setup menus
    setup_Menus();

    // Show welcome message
    draw_Message("Welcome to EcoCity! Use menus above or type commands below.");
}

// Clean up the game
void cleanup_Game()
{
    delete city;
    reset_Terminal();
}

// Message display function
void draw_Message(const string &msg)
{
    cout << "\033[" << messageRow << ";1H" << string(80, ' '); // Clear line
    cout << "\033[" << messageRow << ";1H" << BOLD << msg << RESET;
}

// Main entry point
int main()
{
    // Initialize
    init_Game();

    game_Loop();

    cleanup_Game();

    cout << "Thanks for playing EcoCity!" << endl;
    return 0;
}