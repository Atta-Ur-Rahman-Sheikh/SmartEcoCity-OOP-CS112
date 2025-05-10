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

using namespace std;

void draw_Message(const std::string &msg);
void save_Config(const string& filename = "eco_city_config.txt");
bool load_Config(const string& filename = "eco_city_config.txt");

// Prerequisite functions
void emoji_Support(const string& font = "Consolas", int fontSize = 24)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    GetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
    cfi.dwFontSize.Y = fontSize;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, wstring(font.begin(), font.end()).c_str());
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

// Global configuration - customizable settings
struct GameConfig {
    int map_width = 42;
    int map_height = 16;
    int tileWidth = 2;
    int leftOffset = 11;
    int menuRow = 0;  // Will be calculated based on map_height
    int menuCol = 0;  // Will be calculated based on leftOffset
    int statsRow = 0; // Will be calculated based on map_height
    int statsCol = 0; // Will be calculated based on map dimensions
    int tooltipRow = 0;
    int messageRow = 0;
    int startingMoney = 10000;
    int startingPopulation = 100;
    int monthLengthSeconds = 10;
    string font = "Consolas";
    int fontSize = 24;
    bool showTooltips = true;
    bool showHelp = true;
    bool showGrid = true;
    bool showInfo = true;
} config;

// Map dimensions - these will be set from config in initialize_Config
int map_width = 42;
int map_height = 16;
int tileWidth = 2;
int leftOffset = 11;

// UI layout variables - these will be set from config
int menuRow;
int menuCol;
int statsRow; 
int statsCol;
int maxDropdownItems = 6; // Max items to show in a dropdown - shorter for better UI
int tooltipRow;        
int messageRow;        

// Initialize the configuration with calculated values
void initialize_Config() {
    // First set the map dimensions from config
    map_width = config.map_width;
    map_height = config.map_height;
    tileWidth = config.tileWidth;
    leftOffset = config.leftOffset;
    
    // Then calculate the dependent values - ensure proper positioning
    config.menuRow = map_height + 2;  // Moved up by 2 rows
    config.menuCol = leftOffset - 3;
    config.statsRow = map_height + 2;  // Moved up to align with menu
    config.statsCol = (map_width * tileWidth + leftOffset) / 2 - 15;  // Center the stats
    
    // Set global UI variables from config
    menuRow = config.menuRow;
    menuCol = config.menuCol;
    statsRow = config.statsRow;
    statsCol = config.statsCol;
    
    // Calculate tooltip and message rows
    tooltipRow = menuRow + maxDropdownItems + 2;
    messageRow = menuRow + 1; // Place message row just below menu tabs
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
struct Stats { 
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
};

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

// City class to manage game state
class City
{
public:
    // Game state
    Tile **map; // Dynamic map array instead of fixed size
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
                              statsDisplayMode(1),
                              map(nullptr)
    {
        // Allocate the map based on current dimensions
        allocate_Map();
        
        generate_Map();
        timer.onMonthChange = [this]()
        { this->monthlyUpdate(); };

        // Place central airport
        int cx = map_height / 2;
        int cy = map_width / 2;
        map[cx][cy].building = make_unique<Airport>();
        totalBuildings++;
    }
    
    ~City() {
        // Free the map memory
        deallocate_Map();
    }
    
    // Allocate map with current dimensions
    void allocate_Map() {
        map = new Tile*[map_height];
        for (int i = 0; i < map_height; i++) {
            map[i] = new Tile[map_width];
        }
    }
    
    // Free map memory
    void deallocate_Map() {
        if (map) {
            for (int i = 0; i < map_height; i++) {
                delete[] map[i];
            }
            delete[] map;
            map = nullptr;
        }
    }
    
    // Resize the map if dimensions change
    void resize_Map(int newHeight, int newWidth) {
        // Free old map
        deallocate_Map();
        
        // Update dimensions
        map_height = newHeight;
        map_width = newWidth;
        
        // Allocate new map
        allocate_Map();
        
        // Regenerate the map
        generate_Map();
    }

    void generate_Map()
    {
        // Ensure map is allocated
        if (!map) {
            allocate_Map();
        }
        
        int cx = map_width / 2, cy = map_height / 2;
        float radius = min(map_width, map_height) * 0.8; // Scale radius based on map size
        float sx = 1, sy = 2.5;
        
        // Initialize random seed
        srand(static_cast<unsigned>(time(nullptr)));
        
        for (int i = 0; i < map_height; i++)
        {
            for (int j = 0; j < map_width; j++)
            {
                float d = sqrt(pow((j - cx) * sx, 2) + pow((i - cy) * sy, 2));
                bool isWater = (d + (rand() % 3 - 1)) > radius;
                
                // Clear any existing building first
                map[i][j].building.reset();
                
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
        
        // Reset total buildings counter
        totalBuildings = 0;
    }

    void display_Map()
    {
        // Ensure the map exists
        if (!map) return;
        
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
        cout << "   " << RESET << flush;

        // Rows
        for (int i = 0; i < map_height; i++)
        {
            cout << "\033[" << (i+2) << ";1H" << string(leftOffset, ' ') << BOLD << custom_Background(32, 32, 32)
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
                 << " " << char('A' + i) << " " << RESET;
        }

        // Bottom border
        cout << "\033[" << (map_height+2) << ";1H" << string(leftOffset, ' ') << BOLD << custom_Background(32, 32, 32) << "   ";
        for (int c = 0, k = 1; c < map_width; c++, ++k)
        {
            cout << setw(tileWidth) << left << k;
            if (k > 8)
            {
                k -= 9;
            }
        }
        cout << "   " << RESET << flush;
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
        if (!map) return; // Safety check
        
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
        // Clear stats area with proper background
        int statsWidth = 30;
        for (int i = 0; i < 10; i++)  // Reduced from 15 to 10 rows
        {
            cout << "\033[" << statsRow + i << ";" << statsCol << "H" 
                 << BG_B_BLACK << string(statsWidth, ' ') << RESET << flush;
        }

        // Draw stats header with styling
        cout << "\033[" << statsRow << ";" << statsCol << "H" 
             << BG_B_BLACK << BOLD << WHITE << "--- City Stats ---" << RESET << flush;
        cout << "\033[" << statsRow + 1 << ";" << statsCol << "H" 
             << BG_B_BLACK << WHITE << "Date: " << timer.getDate() << RESET << flush;

        // Display different stats based on mode with enhanced styling
        if (statsDisplayMode == 1)
        {
            // Economy & General Stats - More compact layout
            cout << "\033[" << statsRow + 2 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Money: " << GREEN << "$" << money << RESET << flush;
            cout << "\033[" << statsRow + 3 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Pop: " << CYAN << population << RESET << flush;
            cout << "\033[" << statsRow + 4 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Happy: " << YELLOW << happiness << "%" << RESET << flush;
            cout << "\033[" << statsRow + 5 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Income: " << GREEN << "$" << (population * 5) << RESET << flush;
            cout << "\033[" << statsRow + 6 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Maint: " << RED << "$" << maintenance << RESET << flush;
        }
        else if (statsDisplayMode == 2)
        {
            // Environment Stats - More compact layout
            cout << "\033[" << statsRow + 2 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Eco: " << GREEN << ecoPoints << RESET << flush;
            cout << "\033[" << statsRow + 3 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Poll: " << RED << pollution << RESET << flush;
            cout << "\033[" << statsRow + 4 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Green: " << GREEN << greenLevel << RESET << flush;
            cout << "\033[" << statsRow + 5 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Energy: " << CYAN << energy << RESET << flush;
            cout << "\033[" << statsRow + 6 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Air: " << GREEN << max(0, 100 - pollution) << "%" << RESET << flush;
        }
        else if (statsDisplayMode == 3)
        {
            // Population Stats - More compact layout
            cout << "\033[" << statsRow + 2 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Pop: " << CYAN << population << RESET << flush;
            cout << "\033[" << statsRow + 3 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Happy: " << YELLOW << happiness << "%" << RESET << flush;
            cout << "\033[" << statsRow + 4 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Growth: " << GREEN << (happiness > 50 ? "2%" : "1%") << RESET << flush;
            cout << "\033[" << statsRow + 5 << ";" << statsCol << "H" 
                 << BG_B_BLACK << WHITE << "Tax: " << GREEN << "$" << (population * 5) << RESET << flush;
        }

        // Show current tool (in all modes)
        string toolName;
        string toolColor;
        
        switch (currentTool)
        {
        case BuildingType::None:
            toolName = "None";
            toolColor = WHITE;
            break;
        case BuildingType::Road:
            toolName = "Road";
            toolColor = YELLOW;
            break;
        case BuildingType::Residential:
            toolName = "House";
            toolColor = GREEN;
            break;
        case BuildingType::Commercial:
            toolName = "Office";
            toolColor = BLUE;
            break;
        case BuildingType::Industrial:
            toolName = "Factory";
            toolColor = RED;
            break;
        case BuildingType::Hospital:
            toolName = "Hospital";
            toolColor = WHITE;
            break;
        case BuildingType::School:
            toolName = "School";
            toolColor = YELLOW;
            break;
        case BuildingType::Park:
            toolName = "Park";
            toolColor = GREEN;
            break;
        case BuildingType::Tree:
            toolName = "Tree";
            toolColor = GREEN;
            break;
        case BuildingType::SolarPlant:
            toolName = "Solar Plant";
            toolColor = YELLOW;
            break;
        case BuildingType::WindTurbine:
            toolName = "Wind Turbine";
            toolColor = CYAN;
            break;
        default:
            toolName = "None";
            toolColor = WHITE;
            break;
        }
        
        cout << "\033[" << statsRow + 8 << ";" << statsCol << "H" 
             << BG_B_BLACK << BOLD << WHITE << "--- Controls ---" << RESET << flush;
        cout << "\033[" << statsRow + 9 << ";" << statsCol << "H" 
             << BG_B_BLACK << WHITE << "Tool: " << toolColor << toolName << RESET << flush;
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

// Menu system with improved structure
struct Menu
{
    string name;
    vector<string> items;
    vector<function<void()>> actions;
    bool isOpen = false; // Track whether this menu is open
};
vector<Menu> menus;
int currentTab = 0, currentOpt = 0;
vector<int> tabPositions;
bool menuActive = false; // Track if any menu is currently visible

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
    maxDropdownItems = max(6, maxDropdownItems); // Ensure reasonable size
    for (auto &m : menus)
        maxDropdownItems = min(maxDropdownItems, int(m.items.size())); // Use smaller dropdown height
    
    tooltipRow = menuRow + maxDropdownItems + 2;
    messageRow = menuRow + maxDropdownItems + 4;
}

// Clear ONLY the menu dropdown area
void clear_DropdownArea()
{
    int height = maxDropdownItems + 1;
    int width = 0;
    
    // Calculate maximum width needed for any dropdown
    for (auto &m : menus) {
        for (auto &item : m.items) {
            width = max(width, (int)item.size());
        }
    }
    width += 4; // Add padding
    
    // Only clear the dropdown area, not the menu tabs
    for (int r = menuRow + 1; r < menuRow + height + 1; r++)
    {
        cout << "\033[" << r << ";" << menuCol << "H" << string(width, ' ');
    }
}

// Clear menu tabs and dropdown
void clear_MenuArea()
{
    int height = maxDropdownItems + 1;
    int width = tabPositions.back() + int(menus.back().name.size()) + 5;
    
    // Clear menu tabs
    cout << "\033[" << menuRow << ";" << menuCol << "H" << string(width, ' ');
    
    // Clear dropdown area
    clear_DropdownArea();
}

bool isMouseOverMenu(int x, int y) {
    // Check if over menu tabs
    if (y == menuRow) {
        return (x >= menuCol && x < tabPositions.back() + int(menus.back().name.size()) + 3);
    }
    
    // Check if over dropdown
    if (currentTab >= 0 && currentTab < (int)menus.size() && menus[currentTab].isOpen) {
        int menuWidth = 0;
        for (auto &item : menus[currentTab].items) {
            menuWidth = max(menuWidth, (int)item.size());
        }
        menuWidth += 4; // Add padding
        
        return (y >= menuRow + 1 && 
                y < menuRow + 1 + min(maxDropdownItems, (int)menus[currentTab].items.size()) && 
                x >= tabPositions[currentTab] && 
                x < tabPositions[currentTab] + menuWidth);
    }
    
    return false;
}

void draw_Tooltip(const string &msg)
{
    if (!config.showTooltips) return;
    
    int row, col;
    row = tooltipRow;
    col = (map_width * tileWidth + leftOffset) / 2 - msg.size() / 2;
    col = max(10, col); // Ensure visible on screen
    
    // Clear previous tooltip
    cout << "\033[" << row << ";" << 5 << "H" << string(map_width * tileWidth + 10, ' ') << flush;
    // Draw new tooltip with a nice background
    cout << "\033[" << row << ";" << col << "H" << BOLD << BG_B_BLACK << WHITE << msg << RESET << flush;
}

void draw_Message(const string &msg)
{
    // Clear previous message
    cout << "\033[" << messageRow << ";" << menuCol << "H" << string(80, ' ') << flush;
    // Draw new message with color
    cout << "\033[" << messageRow << ";" << menuCol << "H" << BG_B_BLACK << B_CYAN << msg << RESET << flush;
}

void draw_Menu()
{
    // First ensure buffering is off
    cout << flush;
    
    // Calculate total menu width
    int totalMenuWidth = 0;
    for (const auto& menu : menus) {
        totalMenuWidth += menu.name.length() + 3; // +3 for brackets and spacing
    }
    
    // Center the menu
    int menuStartCol = (map_width * tileWidth + leftOffset - totalMenuWidth) / 2;
    menuCol = menuStartCol;
    
    // Rebuild tab positions with new centered layout
    tabPositions.clear();
    int x = menuCol;
    for (const auto& menu : menus) {
        tabPositions.push_back(x);
        x += menu.name.length() + 3;
    }
    
    // Clear menu area
    clear_MenuArea();
    
    // Draw menu tabs with visible styling
    for (int t = 0; t < int(menus.size()); t++)
    {
        cout << "\033[" << menuRow << ";" << tabPositions[t] << "H";
        if (t == currentTab && menus[t].isOpen)
            cout << BOLD << BG_BLUE << WHITE << "[" << menus[t].name << "]" << RESET << flush;
        else if (t == currentTab)
            cout << BOLD << BG_B_BLACK << WHITE << "[" << menus[t].name << "]" << RESET << flush;
        else
            cout << BG_B_BLACK << WHITE << "[" << menus[t].name << "]" << RESET << flush;
    }
    
    // Show dropdown for open menu with enhanced visibility
    if (currentTab >= 0 && currentTab < int(menus.size()) && menus[currentTab].isOpen) {
        int itemsToShow = min(maxDropdownItems, (int)menus[currentTab].items.size());
        
        // Draw dropdown background
        int dropdownWidth = 0;
        for (auto &item : menus[currentTab].items) {
            dropdownWidth = max(dropdownWidth, (int)item.size());
        }
        dropdownWidth += 4; // Add padding
        
        for (int i = 0; i < itemsToShow; i++) {
            cout << "\033[" << (menuRow + 1 + i) << ";" << tabPositions[currentTab] << "H" 
                 << BG_B_BLACK << string(dropdownWidth, ' ') << RESET << flush;
        }
        
        // Draw menu items
        for (int i = 0; i < itemsToShow; i++)
        {
            cout << "\033[" << (menuRow + 1 + i) << ";" << tabPositions[currentTab] << "H";
            if (i == currentOpt)
                cout << REVERSE << BOLD << BG_BLUE << WHITE << " " << menus[currentTab].items[i] << " " << RESET << flush;
            else
                cout << BG_B_BLACK << WHITE << " " << menus[currentTab].items[i] << " " << RESET << flush;
        }
    }
}

// Toggle menu dropdown
void toggleMenu(int tab) {
    // Close all other menus
    for (int i = 0; i < (int)menus.size(); i++) {
        if (i != tab) menus[i].isOpen = false;
    }
    
    // Toggle current menu
    menus[tab].isOpen = !menus[tab].isOpen;
    
    // Update menu active state
    menuActive = menus[tab].isOpen;
    
    // Reset current option when opening
    if (menus[tab].isOpen) {
        currentTab = tab;
        currentOpt = 0;
    }
    
    // Redraw menus
    draw_Menu();
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

// Functions to save and load configuration to a file
void save_Config(const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        file << config.map_width << endl;
        file << config.map_height << endl;
        file << config.tileWidth << endl;
        file << config.leftOffset << endl;
        file << config.startingMoney << endl;
        file << config.startingPopulation << endl;
        file << config.monthLengthSeconds << endl;
        file << config.fontSize << endl;
        file << config.showTooltips << endl;
        file << config.showHelp << endl;
        file << config.font << endl;
        file.close();
        draw_Message("Configuration saved to " + filename);
    } else {
        draw_Message("Could not save configuration");
    }
}

bool load_Config(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        file >> config.map_width;
        file >> config.map_height;
        file >> config.tileWidth;
        file >> config.leftOffset;
        file >> config.startingMoney;
        file >> config.startingPopulation;
        file >> config.monthLengthSeconds;
        file >> config.fontSize;
        file >> config.showTooltips;
        file >> config.showHelp;
        
        // Read font name (might contain spaces)
        file.ignore(); // Skip newline
        getline(file, config.font);
        
        file.close();
        initialize_Config(); // Recalculate dependent values
        draw_Message("Configuration loaded from " + filename);
        return true;
    } else {
        draw_Message("Could not load configuration");
        return false;
    }
}

// Draw the help overlay
void draw_Help() {
    if (!config.showHelp) return;
    
    // Position and calculate space
    int startRow = map_height + 7;
    int startCol = 5;
    int width = 80;
    
    // Clear area
    for (int r = startRow; r < startRow + 7; r++) {
        cout << "\033[" << r << ";" << startCol << "H" << string(width, ' ') << flush;
    }
    
    // Draw background
    for (int r = startRow; r < startRow + 7; r++) {
        cout << "\033[" << r << ";" << startCol << "H" << BG_B_BLACK << string(width, ' ') << RESET << flush;
    }
    
    // Draw help text with some styling
    cout << "\033[" << startRow << ";" << startCol << "H" << BG_B_BLACK << BOLD << UNDERLINE << WHITE << " Keyboard Controls:" << RESET << flush;
    cout << "\033[" << (startRow + 1) << ";" << startCol << "H" << BG_B_BLACK << WHITE << " - Arrow Keys: Navigate menus and map" << RESET << flush;
    cout << "\033[" << (startRow + 2) << ";" << startCol << "H" << BG_B_BLACK << WHITE << " - Enter: Select menu option" << RESET << flush;
    cout << "\033[" << (startRow + 3) << ";" << startCol << "H" << BG_B_BLACK << WHITE << " - ESC: Close menu or exit game" << RESET << flush;
    cout << "\033[" << (startRow + 4) << ";" << startCol << "H" << BG_B_BLACK << WHITE << " - Tab: Cycle through menu tabs" << RESET << flush;
    cout << "\033[" << (startRow + 5) << ";" << startCol << "H" << BG_B_BLACK << WHITE << " - 1-9: Quick access to menu tabs" << RESET << flush;
    cout << "\033[" << (startRow + 6) << ";" << startCol << "H" << BG_B_BLACK << WHITE << " - H: Toggle this help display" << RESET << flush;
}

// Screenshot function
void take_Screenshot() {
    // Use Windows API to capture the console
    HWND hwnd = GetConsoleWindow();
    if (hwnd == NULL) {
        draw_Message("Failed to get console window");
        return;
    }
    
    // Get window dimensions
    RECT rect;
    GetClientRect(hwnd, &rect);
    
    // Construct a unique filename with timestamp
    time_t now = time(nullptr);
    string filename = "eco_city_" + to_string(now) + ".bmp";
    
    draw_Message("Screenshot saved as " + filename + " (demo only)");
}

// Save game state to file
void save_Game(const string& filename = "eco_city_save.dat") {
    // In a real implementation, we'd save the city state
    // For this demo, we just pretend
    draw_Message("Game saved to " + filename + " (demo only)");
}

// Load game state from file
bool load_Game(const string& filename = "eco_city_save.dat") {
    // In a real implementation, we'd load the city state
    // For this demo, we just pretend
    draw_Message("Game loaded from " + filename + " (demo only)");
    return true;
}

// Add a function to toggle the help display
void toggle_Help() {
    config.showHelp = !config.showHelp;
    if (config.showHelp) {
        draw_Help();
        draw_Message("Help display enabled");
    } else {
        // Clear the help area
        int startRow = map_height + 7;
        int startCol = 5;
        int width = 80;
        for (int r = startRow; r < startRow + 7; r++) {
            cout << "\033[" << r << ";" << startCol << "H" << string(width, ' ');
        }
        draw_Message("Help display disabled");
    }
}


int main()
{
    emoji_Support();

    // --- 1) Console and setup options ---------------------------
    bool runGame = true;
    
    // Initialize random seed
    srand(static_cast<unsigned>(time(nullptr)));
    
    // Load default configuration
    initialize_Config();
    
    cout << BOLD << CYAN << "\n\n  ╔═══════════════════════════════════════════╗" << endl;
    cout << "  ║   ECO-CITY BUILDER v1.0                   ║" << endl;
    cout << "  ╚═══════════════════════════════════════════╝" << RESET << endl;
    cout << "\n  " << ITALIC << "Build a sustainable city and manage resources" << RESET << endl;
    
    // Optional: Add a settings menu before starting
    cout << "\n  Do you want to customize game settings? (y/n): ";
    char choice;
    cin >> choice;
    cin.ignore(); // Clear newline
    
    if (choice == 'y' || choice == 'Y') {
        int oldWidth = map_width;
        int oldHeight = map_height;
        
        cout << "\n  Map width (20-60): ";
        cin >> config.map_width;
        config.map_width = max(20, min(60, config.map_width));
        
        cout << "  Map height (10-20): ";
        cin >> config.map_height;
        config.map_height = max(10, min(20, config.map_height));
        
        cout << "  Starting money (1000-20000): ";
        cin >> config.startingMoney;
        config.startingMoney = max(1000, min(20000, config.startingMoney));
        
        cout << "  Starting population (50-500): ";
        cin >> config.startingPopulation;
        config.startingPopulation = max(50, min(500, config.startingPopulation));
        
        cout << "  Month length in seconds (5-30): ";
        cin >> config.monthLengthSeconds;
        config.monthLengthSeconds = max(5, min(30, config.monthLengthSeconds));
        
        cin.ignore(); // Clear newline
        
        // Recalculate positions based on new config values
        initialize_Config();
    }
    
    cout << "\n  Press Enter to start the game...";
    cin.get();
    
    // --- 2) Console setup --------------------------------
    emoji_Support(config.font, config.fontSize);
    clear_Screen();
    
    // --- 3) Create city and global pointer --------------
    City city(config.monthLengthSeconds);
    city.money = config.startingMoney;
    city.population = config.startingPopulation;
    cityPtr = &city;
    
    // --- 4) Menu setup -----------------
    menus = {
        { "Build", 
          { "House", "Apartment", "Office", "Shop", "Factory", "Warehouse" },
          {
            [](){ cityPtr->currentTool = BuildingType::Residential; draw_Message("Selected House tool - costs $100"); },
            [](){ cityPtr->currentTool = BuildingType::Residential; draw_Message("Selected Apartment tool - costs $200"); },
            [](){ cityPtr->currentTool = BuildingType::Commercial; draw_Message("Selected Office tool - costs $150"); },
            [](){ cityPtr->currentTool = BuildingType::Commercial; draw_Message("Selected Shop tool - costs $120"); },
            [](){ cityPtr->currentTool = BuildingType::Industrial; draw_Message("Selected Factory tool - costs $200"); },
            [](){ cityPtr->currentTool = BuildingType::Industrial; draw_Message("Selected Warehouse tool - costs $180"); }
          }
        },
        { "Services", 
          { "Hospital", "School", "Park", "Fire Station", "Police Station" },
          {
            [](){ cityPtr->currentTool = BuildingType::Hospital; draw_Message("Selected Hospital tool - costs $300"); },
            [](){ cityPtr->currentTool = BuildingType::School; draw_Message("Selected School tool - costs $250"); },
            [](){ cityPtr->currentTool = BuildingType::Park; draw_Message("Selected Park tool - costs $50"); },
            [](){ cityPtr->currentTool = BuildingType::FireStation; draw_Message("Selected Fire Station tool - costs $200"); },
            [](){ cityPtr->currentTool = BuildingType::PoliceStation; draw_Message("Selected Police Station tool - costs $200"); }
          }
        },
        { "Eco", 
          { "Solar Plant", "Wind Turbine", "Tree", "Recycling Center", "Water Treatment" },
          {
            [](){ cityPtr->currentTool = BuildingType::SolarPlant; draw_Message("Selected Solar Plant tool - costs $400"); },
            [](){ cityPtr->currentTool = BuildingType::WindTurbine; draw_Message("Selected Wind Turbine tool - costs $500"); },
            [](){ cityPtr->currentTool = BuildingType::Tree; draw_Message("Selected Tree tool - costs $10"); },
            [](){ cityPtr->currentTool = BuildingType::RecyclingCenter; draw_Message("Selected Recycling Center tool - costs $250"); },
            [](){ cityPtr->currentTool = BuildingType::WaterTreatment; draw_Message("Selected Water Treatment tool - costs $300"); }
          }
        },
        { "Tools", 
          { "Select", "Demolish", "Toggle Stats", "Toggle Grid", "Toggle Info" },
          {
            [](){ cityPtr->currentTool = BuildingType::None; draw_Message("Selected Select tool"); },
            [](){ 
                if (cityPtr->hasSelection) {
                    cityPtr->removeBuilding(cityPtr->selectedRow, cityPtr->selectedCol);
                    cityPtr->hasSelection = false;
                } else {
                    draw_Message("First select a building to demolish");
                }
            },
            [](){ 
                cityPtr->statsDisplayMode = (cityPtr->statsDisplayMode % 3) + 1; 
                cityPtr->draw_Stats();
                draw_Message("Changed stats view to " + to_string(cityPtr->statsDisplayMode));
            },
            [](){ 
                config.showGrid = !config.showGrid;
                cityPtr->display_Map();
                draw_Message(config.showGrid ? "Grid enabled" : "Grid disabled");
            },
            [](){ 
                config.showInfo = !config.showInfo;
                cityPtr->display_Map();
                draw_Message(config.showInfo ? "Info panel enabled" : "Info panel disabled");
            }
          }
        },
        { "Game",
          { "Save", "Load", "New Game", "Settings", "Exit" },
          {
            [](){ save_Game(); },
            [](){ load_Game(); },
            [](){ 
                cityPtr->generate_Map();
                cityPtr->money = config.startingMoney;
                cityPtr->population = config.startingPopulation;
                cityPtr->happiness = 75;
                cityPtr->ecoPoints = 0;
                cityPtr->pollution = 0;
                cityPtr->greenLevel = 0;
                cityPtr->energy = 0;
                cityPtr->maintenance = 0;
                cityPtr->currentTool = BuildingType::None;
                cityPtr->totalBuildings = 0;
                
                // Place central airport
                int cx = map_height / 2;
                int cy = map_width / 2;
                cityPtr->map[cx][cy].building = make_unique<Airport>();
                cityPtr->totalBuildings++;
                
                cityPtr->display_Map();
                cityPtr->draw_Stats();
                draw_Message("New game started!");
            },
            [](){ 
                // Show settings dialog and save config
                draw_Message("Settings updated and saved!");
                save_Config();
            },
            [](){ exit(0); }
          }
        }
    };
    
    // Calculate tab positions and UI layout
    rebuild_TabPositions();
    
    // --- 5) Initial draw --------------------------------
    // Clear screen first to ensure clean slate
    clear_Screen();
    
    // Draw the map and game UI
    city.display_Map();
    city.draw_Stats();
    
    // Force-open the first menu to make it initially visible
    currentTab = 0;
    menus[currentTab].isOpen = true;
    menuActive = true;
    draw_Menu();
    
    // Show helpful messages
    draw_Message("Welcome to Eco-City Builder! Click on menus or map to get started.");
    draw_Tooltip("Use arrow keys or mouse to navigate. ESC to exit. M to toggle menu.");
    
    // Show initial help display
    if (config.showHelp) {
        draw_Help();
    }
    
    // --- 6) Set up mouse callbacks --------------------------------
    MouseInputHandler mouse;
    int lastHighlightRow = -1, lastHighlightCol = -1;
    
    // Left click handler
    mouse.setLeftClickCallback([](int x, int y){
        // Handle menu tab clicks
        if (y == menuRow) {
            for (int i = 0; i < int(menus.size()); i++) {
                if (x >= tabPositions[i] && x < tabPositions[i] + int(menus[i].name.size()) + 2) {
                    toggleMenu(i);
                    return;
                }
            }
        } 
        // Handle dropdown menu item clicks
        else if (currentTab >= 0 && menus[currentTab].isOpen && 
                 y >= menuRow + 1 && y < menuRow + 1 + min(maxDropdownItems, (int)menus[currentTab].items.size()) &&
                 x >= tabPositions[currentTab] && x < tabPositions[currentTab] + 20) {
            
            int opt = y - (menuRow + 1);
            if (opt >= 0 && opt < int(menus[currentTab].items.size())) {
                currentOpt = opt;
                // Execute the selected action
                menus[currentTab].actions[opt]();
                // Close the menu after selection
                menus[currentTab].isOpen = false;
                menuActive = false;
                draw_Menu();
                return;
            }
        }
        // If clicked outside menus, close any open menu
        else if (menuActive) {
            for (auto &m : menus) {
                m.isOpen = false;
            }
            menuActive = false;
            draw_Menu();
        }
        
        // Handle map clicks if not interacting with menu
        int r, c;
        if (!menuActive && mapCoordinatesFromConsole(x, y, r, c)) {
            cityPtr->setSelection(r, c);  // Highlight & message
            if (cityPtr->currentTool != BuildingType::None) {
                if (cityPtr->placeBuilding(r, c, cityPtr->currentTool)) {
                    draw_Message("Building placed successfully!");
                }
                cityPtr->display_Map();    // Refresh map view
            }
        }
    });
    
    // Right click handler
    mouse.setRightClickCallback([](int x, int y){
        // Don't process right-clicks if menu is active
        if (menuActive) return;
        
        int r, c;
        if (mapCoordinatesFromConsole(x, y, r, c)) {
            if (cityPtr->removeBuilding(r, c)) {
                draw_Message("Building demolished!");
                cityPtr->display_Map();    // Refresh map view
            } else {
                draw_Message("Can't remove that!");
            }
        }
    });

    // Mouse move handler with anti-flicker logic and menu awareness
    mouse.setMoveCallback([&lastHighlightRow, &lastHighlightCol](int x, int y){
        // Check if we're over the menu, and do nothing if so
        if (isMouseOverMenu(x, y)) {
            return;
        }
        
        int r, c;
        if (mapCoordinatesFromConsole(x, y, r, c)) {
            // Only update if position changed
            if (r != lastHighlightRow || c != lastHighlightCol) {
                lastHighlightRow = r;
                lastHighlightCol = c;
                
                cityPtr->clearHighlights();
                cityPtr->map[r][c].highlight = true;
                cityPtr->display_Map();
                
                // Show building costs/info in tooltip based on current tool
                string tooltip = "";
                switch (cityPtr->currentTool) {
                    case BuildingType::Road:
                        tooltip = "Road: $10 - Connects buildings";
                        break;
                    case BuildingType::Residential:
                        tooltip = "House: $100 - Increases population";
                        break;
                    case BuildingType::Commercial:
                        tooltip = "Office: $150 - Creates jobs and income";
                        break;
                    case BuildingType::Industrial:
                        tooltip = "Factory: $200 - More jobs but creates pollution";
                        break;
                    case BuildingType::Hospital:
                        tooltip = "Hospital: $300 - Improves happiness";
                        break;
                    case BuildingType::School:
                        tooltip = "School: $250 - Improves happiness and eco points";
                        break;
                    case BuildingType::Park:
                        tooltip = "Park: $50 - Improves happiness and reduces pollution";
                        break;
                    case BuildingType::Tree:
                        tooltip = "Tree: $10 - Natural beauty, reduces pollution";
                        break;
                    case BuildingType::SolarPlant:
                        tooltip = "Solar Plant: $400 - Clean energy source";
                        break;
                    case BuildingType::WindTurbine:
                        tooltip = "Wind Turbine: $500 - Efficient clean energy";
                        break;
                    default:
                        if (cityPtr->map[r][c].building) {
                            BuildingType type = cityPtr->map[r][c].building->type;
                            tooltip = "Building at " + string(1, 'A' + r) + to_string(c + 1);
                            tooltip += " - Right-click to demolish";
                        } else {
                            tooltip = "Empty " + cityPtr->map[r][c].type + " at " + string(1, 'A' + r) + to_string(c + 1);
                        }
                        break;
                }
                draw_Tooltip(tooltip);
            }
        }
    });
    
    // --- 7) Game update thread ----------------------------
    std::thread updateThread([&city, &runGame]() {
        while (runGame) {
            city.timer.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });
    updateThread.detach();  // Let it run independently
    
    // --- 8) Keyboard handling thread --------------
    std::thread keyboardThread([&runGame]() {
        while (runGame) {
            if (_kbhit()) {
                int key = _getch();
                
                if (key == 27) { // ESC key
                    // If a menu is open, close it
                    if (menuActive) {
                        for (auto &m : menus) {
                            m.isOpen = false;
                        }
                        menuActive = false;
                        draw_Menu();
                    } else {
                        // If no menu is open, exit the game
                        runGame = false;
                        exit(0);
                    }
                }
                else if (key == 13) { // Enter key
                    // If a menu is active, execute the selected action
                    if (menuActive && currentTab >= 0 && currentTab < int(menus.size()) &&
                        currentOpt >= 0 && currentOpt < int(menus[currentTab].actions.size())) {
                        
                        menus[currentTab].actions[currentOpt]();
                        
                        // Close the menu after selection
                        menus[currentTab].isOpen = false;
                        menuActive = false;
                        draw_Menu();
                    }
                    // If no menu is active, open the current tab
                    else if (!menuActive && currentTab >= 0 && currentTab < int(menus.size())) {
                        toggleMenu(currentTab);
                    }
                }
                else if (key == 9) { // Tab key
                    // Move to next menu tab
                    currentTab = (currentTab + 1) % menus.size();
                    currentOpt = 0;
                    if (menuActive) {
                        // If menu is open, switch to the new tab
                        for (auto &m : menus) {
                            m.isOpen = false;
                        }
                        menus[currentTab].isOpen = true;
                    }
                    draw_Menu();
                }
                else if (key >= '1' && key <= '5') { // Number keys 1-5 for quick menu access
                    int tabIndex = key - '1';
                    if (tabIndex < int(menus.size())) {
                        toggleMenu(tabIndex);
                    }
                }
                else if (key == 224) { // Special keys prefix
                    key = _getch();
                    switch (key) {
                        case 72: // Up arrow
                            if (menuActive && currentOpt > 0) {
                                currentOpt--;
                                draw_Menu();
                            }
                            break;
                        case 80: // Down arrow
                            if (menuActive && currentOpt < int(menus[currentTab].items.size()) - 1) {
                                currentOpt++;
                                draw_Menu();
                            }
                            break;
                        case 75: // Left arrow
                            if (currentTab > 0) {
                                currentTab--;
                                currentOpt = 0;
                                if (menuActive) {
                                    // If menu is open, switch to the new tab
                                    for (auto &m : menus) {
                                        m.isOpen = false;
                                    }
                                    menus[currentTab].isOpen = true;
                                }
                                draw_Menu();
                            }
                            break;
                        case 77: // Right arrow
                            if (currentTab < int(menus.size()) - 1) {
                                currentTab++;
                                currentOpt = 0;
                                if (menuActive) {
                                    // If menu is open, switch to the new tab
                                    for (auto &m : menus) {
                                        m.isOpen = false;
                                    }
                                    menus[currentTab].isOpen = true;
                                }
                                draw_Menu();
                            }
                            break;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });
    keyboardThread.detach();  // Let it run independently
    
    // --- 9) Enter main event loop --------------------------
    mouse.startListening();  // Blocks, processing mouse events
    
    return 0;
}