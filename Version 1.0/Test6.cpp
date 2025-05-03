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
#include <fstream>   // for file operations
#include <sstream>   // for string stream

using namespace std;

// Forward declarations
void draw_Message(const std::string &msg);
void save_Config(const string &filename);
bool load_Config(const string &filename);

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

// Initialize config with globals for now
int leftOffset = config.leftOffset;
int map_width = config.mapWidth;
int map_height = config.mapHeight;
int tileWidth = config.tileWidth;

// Prerequisite functions
void emoji_Support(bool reconfigure = false)
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
                             {200, 15, 0, 2, 0, 0, 0, 0, 5, 0, 0},
                             "🚒", B_RED, true,
                             "Improves city safety") {}
};

class PoliceStation : public Building
{
public:
    PoliceStation() : Building(BuildingType::PoliceStation,
                               {200, 15, 0, 2, 0, 0, 0, 0, 5, 0, 0},
                               "🚓", B_BLUE, true,
                               "Improves city safety and reduces crime") {}
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

// UI layout variables - will be calculated based on map size
int menuRow;          // Row for menu tabs
int menuCol;          // Column for menu start
int statsRow;         // Row for stats panel start
int statsCol;         // Column for stats panel
int maxDropdownItems; // Max dropdown items to display
int tooltipRow;       // Row for tooltip display
int messageRow;       // Row for action messages
int logRow;           // Row for game log
int logCol;           // Column for game log

// Menu system forward declaration
struct Menu;
void draw_Menu();
void setup_Menus();
void rebuild_TabPositions();
void compute_UI_positions();

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
    int safety;
    int education;
    int health;
    TimeManager timer;
    BuildingType currentTool;
    int totalBuildings;
    vector<string> gameLog;

    // Selected tile for building
    int selectedRow;
    int selectedCol;
    bool hasSelection;

    // Stats display mode
    int statsDisplayMode;

    City(int monthSec = 10) : money(config.startingMoney),
                              population(config.startingPopulation),
                              happiness(75),
                              ecoPoints(0),
                              pollution(0),
                              greenLevel(0),
                              energy(0),
                              maintenance(0),
                              safety(0),
                              education(0),
                              health(0),
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

        // Add initial log entry
        addToLog("Welcome to Eco-City Builder! City founded in " + timer.getDate());
    }

    void generate_Map()
    {
        int cx = map_width / 2, cy = map_height / 2, radius = map_width / 3;
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
                // Clear any existing buildings
                map[i][j].building.reset();
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

    void addToLog(const string &message)
    {
        // Add timestamp
        time_t now = time(nullptr);
        tm *timeinfo = localtime(&now);
        char buffer[9];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);

        string logEntry = string(buffer) + " - " + message;
        gameLog.push_back(logEntry);

        // Keep log at a reasonable size
        if (gameLog.size() > 100)
        {
            gameLog.erase(gameLog.begin());
        }

        // Draw the log
        draw_Log();
    }

    void draw_Log()
    {
        int logLimit = 5; // Show last 5 log entries
        int startIdx = max(0, (int)gameLog.size() - logLimit);

        // Clear log area
        for (int i = 0; i < logLimit; i++)
        {
            cout << "\033[" << (logRow + i) << ";" << logCol << "H" << string(60, ' ');
        }

        // Display header
        cout << "\033[" << logRow - 1 << ";" << logCol << "H" << BOLD << "--- Event Log ---" << RESET;

        // Display log entries
        for (int i = 0; i < min(logLimit, (int)gameLog.size()); i++)
        {
            int idx = startIdx + i;
            if (idx < gameLog.size())
            {
                cout << "\033[" << (logRow + i) << ";" << logCol << "H" << gameLog[idx];
            }
        }
    }

    void monthlyUpdate()
    {
        // Calculate income and expenses
        int income = population * 5; // $5 per citizen
        int expenses = maintenance;
        money += (income - expenses);

        // Add to log
        addToLog("Monthly update: $" + to_string(income) + " income, $" + to_string(expenses) + " expenses");

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

        if (popGrowth > 0)
        {
            addToLog("Population grew by " + to_string(popGrowth) + " citizens");
        }

        // Update happiness based on various factors
        int happinessChange = 0;
        happinessChange += (greenLevel - pollution) / 10;
        happinessChange += (safety / 10);
        happinessChange += (health / 20);
        happinessChange += (education / 20);

        // More parks/trees increase happiness
        if (greenLevel > 20)
            happinessChange += 1;

        // High pollution decreases happiness
        if (pollution > 30)
            happinessChange -= 2;

        // Low energy decreases happiness
        if (energy < population / 100)
            happinessChange -= 1;

        int oldHappiness = happiness;
        happiness = max(0, min(100, happiness + happinessChange));

        if (happiness != oldHappiness)
        {
            if (happiness > oldHappiness)
            {
                addToLog("Citizens happiness increased to " + to_string(happiness) + "%");
            }
            else
            {
                addToLog("Citizens happiness decreased to " + to_string(happiness) + "%");
            }
        }

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
        string buildingName;

        switch (type)
        {
        case BuildingType::Road:
            map[row][col].building = make_unique<Road>();
            cost = 10;
            buildingName = "Road";
            break;
        case BuildingType::Residential:
            map[row][col].building = make_unique<Residential>();
            cost = 100;
            buildingName = "House";
            break;
        case BuildingType::Commercial:
            map[row][col].building = make_unique<Commercial>();
            cost = 150;
            buildingName = "Office";
            break;
        case BuildingType::Industrial:
            map[row][col].building = make_unique<Industrial>();
            cost = 200;
            buildingName = "Factory";
            break;
        case BuildingType::Hospital:
            map[row][col].building = make_unique<Hospital>();
            cost = 300;
            buildingName = "Hospital";
            break;
        case BuildingType::School:
            map[row][col].building = make_unique<School>();
            cost = 250;
            buildingName = "School";
            break;
        case BuildingType::Park:
            map[row][col].building = make_unique<Park>();
            cost = 50;
            buildingName = "Park";
            break;
        case BuildingType::Tree:
            map[row][col].building = make_unique<Tree>();
            cost = 10;
            buildingName = "Tree";
            break;
        case BuildingType::SolarPlant:
            map[row][col].building = make_unique<SolarPlant>();
            cost = 400;
            buildingName = "Solar Plant";
            break;
        case BuildingType::WindTurbine:
            map[row][col].building = make_unique<WindTurbine>();
            cost = 500;
            buildingName = "Wind Turbine";
            break;
        case BuildingType::WaterTreatment:
            map[row][col].building = make_unique<WaterTreatment>();
            cost = 350;
            buildingName = "Water Treatment";
            break;
        case BuildingType::RecyclingCenter:
            map[row][col].building = make_unique<RecyclingCenter>();
            cost = 300;
            buildingName = "Recycling Center";
            break;
        case BuildingType::FireStation:
            map[row][col].building = make_unique<FireStation>();
            cost = 200;
            buildingName = "Fire Station";
            break;
        case BuildingType::PoliceStation:
            map[row][col].building = make_unique<PoliceStation>();
            cost = 200;
            buildingName = "Police Station";
            break;
        case BuildingType::Airport:
            map[row][col].building = make_unique<Airport>();
            cost = 1000;
            buildingName = "Airport";
            break;
        case BuildingType::None:
            // Clear the tile
            map[row][col].building.reset();
            return true;
        default:
            return false;
        }

        // Check if player has enough money
        if (money < cost)
        {
            play_Sound(3); // Error sound
            draw_Message("Not enough money. Need $" + to_string(cost));
            return false;
        }

        // Apply the building's effects
        money -= cost;
        totalBuildings++;

        // Update city stats
        if (map[row][col].building)
        {
            const Stats &bStats = map[row][col].building->stats;
            maintenance += bStats.maintenance;
            pollution += bStats.pollution;
            greenLevel += bStats.green;
            ecoPoints += bStats.ecoPoints;
            energy += bStats.energy;
            safety += bStats.safety;
            education += bStats.education;
            health += bStats.health;
        }

        // Log the construction
        addToLog("Built " + buildingName + " at " + char('A' + row) + to_string(col + 1));
        play_Sound(1); // Build sound

        draw_Stats(); // Update stats display
        return true;
    }

    bool removeBuilding(int row, int col)
    {
        // Check if there's a building
        if (!map[row][col].building)
            return false;

        // Check if building is removable
        if (!map[row][col].building->removable)
        {
            play_Sound(3); // Error sound
            draw_Message("This building cannot be removed.");
            return false;
        }

        // Get building name before removing
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
            buildingName = "Building";
            break;
        }

        // Get building's stats to revert changes
        const Stats &bStats = map[row][col].building->stats;
        maintenance -= bStats.maintenance;
        pollution -= bStats.pollution;
        greenLevel -= bStats.green;
        ecoPoints -= bStats.ecoPoints;
        energy -= bStats.energy;
        safety -= bStats.safety;
        education -= bStats.education;
        health -= bStats.health;

        // Remove the building
        map[row][col].building.reset();
        totalBuildings--;

        // Log the demolition
        addToLog("Demolished " + buildingName + " at " + char('A' + row) + to_string(col + 1));
        play_Sound(2); // Demolish sound

        draw_Stats(); // Update stats display
        return true;
    }

    void draw_Stats()
    {
        int row = statsRow;
        int col = statsCol;

        // Clear stats area first
        for (int i = 0; i < 15; i++)
        {
            cout << "\033[" << (row + i) << ";" << col << "H" << string(30, ' ');
        }

        // Title
        cout << "\033[" << row++ << ";" << col << "H"
             << BOLD << theme_Color(2) << "--- CITY STATS ---" << RESET;

        // Date
        cout << "\033[" << row++ << ";" << col << "H"
             << theme_Color(1) << "Date: " << RESET << timer.getDate();

        // Money
        cout << "\033[" << row++ << ";" << col << "H"
             << theme_Color(1) << "Money: " << RESET << "$" << money;

        // Population
        cout << "\033[" << row++ << ";" << col << "H"
             << theme_Color(1) << "Population: " << RESET << population;

        // Happiness
        cout << "\033[" << row++ << ";" << col << "H"
             << theme_Color(1) << "Happiness: " << RESET;

        // Happiness bar
        string happinessColor;
        if (happiness < 30)
            happinessColor = RED;
        else if (happiness < 70)
            happinessColor = YELLOW;
        else
            happinessColor = GREEN;

        cout << happinessColor << "[";
        int bars = happiness / 10;
        for (int i = 0; i < 10; i++)
        {
            if (i < bars)
                cout << "■";
            else
                cout << "□";
        }
        cout << "] " << happiness << "%" << RESET;

        row++;

        // Show different stats based on display mode
        if (statsDisplayMode == 1)
        { // Environmental stats
            cout << "\033[" << row++ << ";" << col << "H"
                 << BOLD << theme_Color(2) << "-- Environmental --" << RESET;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Eco Points: " << RESET << ecoPoints;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Pollution: " << RESET << pollution;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Green Level: " << RESET << greenLevel;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Energy: " << RESET << energy;
        }
        else if (statsDisplayMode == 2)
        { // Social stats
            cout << "\033[" << row++ << ";" << col << "H"
                 << BOLD << theme_Color(2) << "-- Social Stats --" << RESET;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Safety: " << RESET << safety;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Education: " << RESET << education;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Health: " << RESET << health;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Crime Rate: " << RESET
                 << max(0, min(100, 100 - safety)) << "%";
        }
        else if (statsDisplayMode == 3)
        { // Building stats
            cout << "\033[" << row++ << ";" << col << "H"
                 << BOLD << theme_Color(2) << "-- Building Stats --" << RESET;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Total Buildings: " << RESET << totalBuildings;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Maintenance: " << RESET << "$" << maintenance;

            // Count each building type
            int residential = 0, commercial = 0, industrial = 0;
            for (int i = 0; i < map_height; i++)
            {
                for (int j = 0; j < map_width; j++)
                {
                    if (map[i][j].building)
                    {
                        if (map[i][j].building->type == BuildingType::Residential)
                            residential++;
                        else if (map[i][j].building->type == BuildingType::Commercial)
                            commercial++;
                        else if (map[i][j].building->type == BuildingType::Industrial)
                            industrial++;
                    }
                }
            }

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Residential: " << RESET << residential;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Commercial: " << RESET << commercial;

            cout << "\033[" << row++ << ";" << col << "H"
                 << theme_Color(1) << "Industrial: " << RESET << industrial;
        }

        // Display cycle button
        cout << "\033[" << (row + 1) << ";" << col << "H"
             << "[S] " << theme_Color(2) << "Cycle Stats" << RESET;
    }
};

// Global city instance
City *city;

// Draw message at bottom of screen
void draw_Message(const std::string &msg)
{
    cout << "\033[" << messageRow << ";1H" << string(80, ' '); // Clear line
    cout << "\033[" << messageRow << ";1H" << BOLD << msg << RESET;
}

// Menu system
struct MenuItem
{
    string label;
    function<void()> action;
    BuildingType buildingType;
    string description;

    MenuItem(string l, function<void()> a, BuildingType bt = BuildingType::None, string desc = "")
        : label(l), action(a), buildingType(bt), description(desc) {}
};

struct Menu
{
    string name;
    vector<MenuItem> items;
    int tabPos;       // Position of this tab
    int tabWidth;     // Width of tab
    bool visible;     // Is menu expanded
    int selectedItem; // Currently selected item

    Menu(string n) : name(n), tabPos(0), tabWidth(name.length() + 2), visible(false), selectedItem(-1) {}

    void addItem(string label, function<void()> action, BuildingType bt = BuildingType::None, string desc = "")
    {
        items.emplace_back(label, action, bt, desc);
    }

    void toggleVisibility()
    {
        visible = !visible;
        if (visible)
        {
            selectedItem = -1; // Reset selection
        }
    }

    void draw()
    {
        // Draw tab
        cout << "\033[" << menuRow << ";" << tabPos << "H";
        if (visible)
        {
            cout << BOLD << BG_WHITE << BLACK << " " << name << " " << RESET;
        }
        else
        {
            cout << BOLD << " " << name << " " << RESET;
        }

        // Draw dropdown if visible
        if (visible)
        {
            int numToShow = min(maxDropdownItems, (int)items.size());
            for (int i = 0; i < numToShow; i++)
            {
                cout << "\033[" << (menuRow + i + 1) << ";" << tabPos << "H";

                if (i == selectedItem)
                {
                    cout << BOLD << REVERSE << " " << items[i].label << string(15 - items[i].label.length(), ' ') << " " << RESET;
                }
                else
                {
                    cout << " " << items[i].label << string(15 - items[i].label.length(), ' ') << " ";
                }
            }

            // Show description of selected item
            if (selectedItem >= 0 && selectedItem < items.size())
            {
                string desc = items[selectedItem].description;
                if (!desc.empty())
                {
                    cout << "\033[" << tooltipRow << ";1H" << string(80, ' '); // Clear line
                    cout << "\033[" << tooltipRow << ";1H" << ITALIC << desc << RESET;
                }
            }
        }
    }
};

// Menu instances
vector<Menu> menus;
int activeMenu = -1;

// Create menu system
void setup_Menus()
{
    // Build menu
    Menu buildMenu("Build");
    buildMenu.addItem("Road", []()
                      { city->currentTool = BuildingType::Road; }, BuildingType::Road, "Build roads to connect buildings");
    buildMenu.addItem("Residential", []()
                      { city->currentTool = BuildingType::Residential; }, BuildingType::Residential, "Build housing for your citizens");
    buildMenu.addItem("Commercial", []()
                      { city->currentTool = BuildingType::Commercial; }, BuildingType::Commercial, "Build offices and shops");
    buildMenu.addItem("Industrial", []()
                      { city->currentTool = BuildingType::Industrial; }, BuildingType::Industrial, "Build factories for jobs");
    buildMenu.addItem("Hospital", []()
                      { city->currentTool = BuildingType::Hospital; }, BuildingType::Hospital, "Improve city health");
    buildMenu.addItem("School", []()
                      { city->currentTool = BuildingType::School; }, BuildingType::School, "Improve city education");
    buildMenu.addItem("Park", []()
                      { city->currentTool = BuildingType::Park; }, BuildingType::Park, "Increase happiness and greenery");
    buildMenu.addItem("Tree", []()
                      { city->currentTool = BuildingType::Tree; }, BuildingType::Tree, "Cheap way to improve environment");
    menus.push_back(buildMenu);

    // Eco menu
    Menu ecoMenu("Ecology");
    ecoMenu.addItem("Solar Plant", []()
                    { city->currentTool = BuildingType::SolarPlant; }, BuildingType::SolarPlant, "Clean energy source");
    ecoMenu.addItem("Wind Turbine", []()
                    { city->currentTool = BuildingType::WindTurbine; }, BuildingType::WindTurbine, "Efficient renewable energy");
    ecoMenu.addItem("Water Plant", []()
                    { city->currentTool = BuildingType::WaterTreatment; }, BuildingType::WaterTreatment, "Clean water and reduce pollution");
    ecoMenu.addItem("Recycling", []()
                    { city->currentTool = BuildingType::RecyclingCenter; }, BuildingType::RecyclingCenter, "Reduce waste and pollution");
    menus.push_back(ecoMenu);

    // Services menu
    Menu servicesMenu("Services");
    servicesMenu.addItem("Fire Station", []()
                         { city->currentTool = BuildingType::FireStation; }, BuildingType::FireStation, "Improve city safety");
    servicesMenu.addItem("Police", []()
                         { city->currentTool = BuildingType::PoliceStation; }, BuildingType::PoliceStation, "Reduce crime and improve safety");
    menus.push_back(servicesMenu);

    // Tools menu
    Menu toolsMenu("Tools");
    toolsMenu.addItem("Select", []()
                      { city->currentTool = BuildingType::None; }, BuildingType::None, "Select and view buildings");
    toolsMenu.addItem("Demolish", []()
                      { 
        if (city->hasSelection) {
            city->removeBuilding(city->selectedRow, city->selectedCol);
            city->hasSelection = false;
        } else {
            draw_Message("First select a building to demolish");
        } }, BuildingType::None, "Remove existing buildings");
    toolsMenu.addItem("Toggle Stats", []()
                      { 
        city->statsDisplayMode = (city->statsDisplayMode % 3) + 1; 
        city->draw_Stats(); }, BuildingType::None, "Cycle through different stat views");
    menus.push_back(toolsMenu);

    // Options menu
    Menu optionsMenu("Options");
    optionsMenu.addItem("Save Game", []()
                        {
        save_Config("ecocity.cfg");
        draw_Message("Game settings saved to ecocity.cfg"); }, BuildingType::None, "Save game settings");
    optionsMenu.addItem("Load Game", []()
                        {
        if (load_Config("ecocity.cfg")) {
            draw_Message("Game settings loaded from ecocity.cfg");
        } else {
            draw_Message("Could not load settings");
        } }, BuildingType::None, "Load saved game settings");
    optionsMenu.addItem("Toggle Sound", []()
                        {
        config.enableSounds = !config.enableSounds;
        draw_Message(config.enableSounds ? "Sound enabled" : "Sound disabled"); }, BuildingType::None, "Turn game sounds on/off");
    optionsMenu.addItem("Change Theme", []()
                        {
        config.uiTheme = (config.uiTheme + 1) % 4;
        draw_Message("Theme changed to " + to_string(config.uiTheme)); }, BuildingType::None, "Cycle through UI themes");
    menus.push_back(optionsMenu);

    // Calculate tab positions
    rebuild_TabPositions();
}

// Calculate tab positions
void rebuild_TabPositions()
{
    int pos = 1;
    for (auto &menu : menus)
    {
        menu.tabPos = pos;
        menu.tabWidth = menu.name.length() + 2;
        pos += menu.tabWidth + config.menuSpacing;
    }
}

// Draw all menus
void draw_Menu()
{
    // Clear menu area
    for (int i = menuRow; i < menuRow + maxDropdownItems + 2; i++)
    {
        cout << "\033[" << i << ";1H" << string(80, ' ');
    }

    // Draw all menus
    for (auto &menu : menus)
    {
        menu.draw();
    }
}

// Process menu clicks
bool handle_Menu_Click(int x, int y)
{
    // Check if click is on menu row (tabs)
    if (y == menuRow)
    {
        for (int i = 0; i < menus.size(); i++)
        {
            if (x >= menus[i].tabPos && x < menus[i].tabPos + menus[i].tabWidth)
            {
                // Toggle this menu
                if (activeMenu == i)
                {
                    menus[i].toggleVisibility();
                    if (!menus[i].visible)
                        activeMenu = -1;
                }
                else
                {
                    // Close previous menu
                    if (activeMenu >= 0)
                        menus[activeMenu].visible = false;

                    // Open this menu
                    menus[i].visible = true;
                    activeMenu = i;
                }

                play_Sound(0); // Click sound
                draw_Menu();
                return true;
            }
        }
    }
    // Check if click is on dropdown item
    else if (activeMenu >= 0 && menus[activeMenu].visible)
    {
        int itemIndex = y - menuRow - 1;
        if (itemIndex >= 0 && itemIndex < menus[activeMenu].items.size() &&
            x >= menus[activeMenu].tabPos && x < menus[activeMenu].tabPos + 17) // 17 = width of dropdown item
        {
            // Execute the action
            menus[activeMenu].items[itemIndex].action();

            // Update the current tool info
            BuildingType bt = menus[activeMenu].items[itemIndex].buildingType;
            if (bt != BuildingType::None)
            {
                draw_Message("Selected tool: " + menus[activeMenu].items[itemIndex].label);
            }

            // Close the menu
            menus[activeMenu].visible = false;
            activeMenu = -1;
            draw_Menu();
            return true;
        }
    }

    return false;
}

// Handle menu hover
void handle_Menu_Hover(int x, int y)
{
    if (activeMenu >= 0 && menus[activeMenu].visible)
    {
        int itemIndex = y - menuRow - 1;
        if (itemIndex >= 0 && itemIndex < menus[activeMenu].items.size() &&
            x >= menus[activeMenu].tabPos && x < menus[activeMenu].tabPos + 17)
        {
            if (menus[activeMenu].selectedItem != itemIndex)
            {
                menus[activeMenu].selectedItem = itemIndex;
                menus[activeMenu].draw();
            }
        }
    }
}

// Layout calculation
void compute_UI_positions()
{
    // Calculate UI positions based on map size
    menuRow = 1 + map_height + 3; // 3 rows after map (top border, map rows, bottom border)
    menuCol = leftOffset;

    statsRow = 2;                                         // Start stats at top
    statsCol = leftOffset + (map_width * tileWidth) + 10; // Right of map

    maxDropdownItems = 10; // Max items to show in dropdown

    tooltipRow = menuRow + maxDropdownItems + 2; // Below dropdowns
    messageRow = tooltipRow + 2;                 // Below tooltip

    logRow = messageRow + 2; // Below message
    logCol = leftOffset;
}

// Save/Load configuration
void save_Config(const string &filename)
{
    ofstream file(filename);
    if (!file)
    {
        draw_Message("Error: Could not open file for saving");
        return;
    }

    file << config.monthLengthSeconds << endl;
    file << config.startingMoney << endl;
    file << config.startingPopulation << endl;
    file << config.leftOffset << endl;
    file << config.mapWidth << endl;
    file << config.mapHeight << endl;
    file << config.tileWidth << endl;
    file << config.menuSpacing << endl;
    file << config.enableSounds << endl;
    file << config.uiTheme << endl;
    file << config.fontHeight << endl;
    file << config.fontName << endl;

    file.close();
}

bool load_Config(const string &filename)
{
    ifstream file(filename);
    if (!file)
    {
        draw_Message("Error: Could not open file for loading");
        return false;
    }

    string line;

    if (getline(file, line))
        config.monthLengthSeconds = stoi(line);
    if (getline(file, line))
        config.startingMoney = stoi(line);
    if (getline(file, line))
        config.startingPopulation = stoi(line);
    if (getline(file, line))
        config.leftOffset = stoi(line);
    if (getline(file, line))
        config.mapWidth = stoi(line);
    if (getline(file, line))
        config.mapHeight = stoi(line);
    if (getline(file, line))
        config.tileWidth = stoi(line);
    if (getline(file, line))
        config.menuSpacing = stoi(line);
    if (getline(file, line))
        config.enableSounds = stoi(line);
    if (getline(file, line))
        config.uiTheme = stoi(line);
    if (getline(file, line))
        config.fontHeight = stoi(line);
    if (getline(file, line))
        config.fontName = line;

    // Update globals
    leftOffset = config.leftOffset;
    map_width = config.mapWidth;
    map_height = config.mapHeight;
    tileWidth = config.tileWidth;

    // Reconfigure UI
    compute_UI_positions();
    rebuild_TabPositions();
    emoji_Support(true);

    file.close();
    return true;
}

// Parse coordinates like "A1" into row, col
bool parse_Coords(const string &input, int &row, int &col)
{
    if (input.length() < 2)
        return false;

    char rowChar = toupper(input[0]);
    if (rowChar < 'A' || rowChar >= ('A' + map_height))
        return false;

    row = rowChar - 'A';

    try
    {
        col = stoi(input.substr(1)) - 1;
    }
    catch (...)
    {
        return false;
    }

    if (col < 0 || col >= map_width)
        return false;

    return true;
}

// Main game loop
void game_Loop()
{
    bool running = true;
    string input;

    while (running)
    {
        // Clear screen
        clear_Screen();

        // Update game state
        city->timer.update();

        // Display UI
        city->display_Map();
        city->draw_Stats();
        draw_Menu();
        city->draw_Log();

        // Show cursor position prompt
        cout << "\033[" << (menuRow + maxDropdownItems + 5) << ";1H";
        cout << "Enter coordinates (like A1) or command, 'help' for help, 'quit' to exit: ";

        // Get input
        getline(cin, input);

        // Process commands
        if (input == "quit" || input == "exit")
        {
            running = false;
        }
        else if (input == "help")
        {
            draw_Message("Commands: quit, help, build <type> <coords>, demolish <coords>, stats");
        }
        else if (input == "stats")
        {
            city->statsDisplayMode = (city->statsDisplayMode % 3) + 1;
            city->draw_Stats();
        }
        else if (input.substr(0, 5) == "build" && input.length() > 6)
        {
            // Parse build command: build <type> <coords>
            stringstream ss(input.substr(6));
            string type, coords;
            ss >> type >> coords;

            int row, col;
            if (parse_Coords(coords, row, col))
            {
                BuildingType bt = BuildingType::None;

                if (type == "road")
                    bt = BuildingType::Road;
                else if (type == "house" || type == "residential")
                    bt = BuildingType::Residential;
                else if (type == "office" || type == "commercial")
                    bt = BuildingType::Commercial;
                else if (type == "factory" || type == "industrial")
                    bt = BuildingType::Industrial;
                else if (type == "hospital")
                    bt = BuildingType::Hospital;
                else if (type == "school")
                    bt = BuildingType::School;
                else if (type == "park")
                    bt = BuildingType::Park;
                else if (type == "tree")
                    bt = BuildingType::Tree;
                else if (type == "solar")
                    bt = BuildingType::SolarPlant;
                else if (type == "wind")
                    bt = BuildingType::WindTurbine;
                else if (type == "water")
                    bt = BuildingType::WaterTreatment;
                else if (type == "recycle")
                    bt = BuildingType::RecyclingCenter;
                else if (type == "fire")
                    bt = BuildingType::FireStation;
                else if (type == "police")
                    bt = BuildingType::PoliceStation;

                if (bt != BuildingType::None)
                {
                    if (city->placeBuilding(row, col, bt))
                    {
                        draw_Message("Building constructed at " + coords);
                    }
                }
                else
                {
                    draw_Message("Unknown building type: " + type);
                }
            }
            else
            {
                draw_Message("Invalid coordinates: " + coords);
            }
        }
        else if (input.substr(0, 8) == "demolish" && input.length() > 9)
        {
            // Parse demolish command: demolish <coords>
            string coords = input.substr(9);

            int row, col;
            if (parse_Coords(coords, row, col))
            {
                if (city->removeBuilding(row, col))
                {
                    draw_Message("Building demolished at " + coords);
                }
                else
                {
                    draw_Message("No building at " + coords);
                }
            }
            else
            {
                draw_Message("Invalid coordinates: " + coords);
            }
        }
        else
        {
            // Check if input is coordinates
            int row, col;
            if (parse_Coords(input, row, col))
            {
                if (city->currentTool == BuildingType::None)
                {
                    // Select the tile
                    city->selectedRow = row;
                    city->selectedCol = col;
                    city->hasSelection = true;

                    // Show info about the tile
                    if (city->map[row][col].building)
                    {
                        string buildingName;
                        switch (city->map[row][col].building->type)
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
                        draw_Message(buildingName + " at " + input);
                    }
                    else
                    {
                        draw_Message(city->map[row][col].type + " at " + input);
                    }
                }
                else
                {
                    // Build with current tool
                    // Build with current tool
                    if (city->placeBuilding(row, col, city->currentTool))
                    {
                        draw_Message("Building constructed at " + input);
                    }
                }
            }
            else
            {
                draw_Message("Invalid input: " + input);
            }
        }
    }
}

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
                if (city->map[row][col].building)
                {
                    string buildingName;
                    switch (city->map[row][col].building->type)
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
                    draw_Message(city->map[row][col].type + " at " + char('A' + row) + to_string(col + 1));
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

            if (city->map[row][col].building)
            {
                string buildingName;
                switch (city->map[row][col].building->type)
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
                cout << " - " << city->map[row][col].type << RESET;
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
    // (you can leave this empty if you don’t need special teardown)
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