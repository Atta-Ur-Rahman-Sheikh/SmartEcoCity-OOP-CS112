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
#include <algorithm>
#include <fstream>   

using namespace std;



// NOTE:
// AI was used for code organization, generating comments and using libraries like <windows.h> and <conio.h> etc.
// AI was not used to generate any of the game logic, building types, upgrades, research projects, or any other game-related content.




// Utility to build ANSI color codes
string custom_Colour(int r, int g, int b) { return "\033[38;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m"; }
string custom_Background(int r, int g, int b) { return "\033[48;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m"; }

// CUSTOMIZATION VARIABLES
struct VisualSettings
{
    // Map appearance
    int mapWidth = 42;
    int mapHeight = 16;
    int tileWidth = 2;
    int leftOffset = 12;
    
    // UI positions
    int menuRow;         // Auto-calculated based on map height
    int menuCol;         // Auto-calculated based on left offset
    int statsRow;        // Auto-calculated based on map height
    int statsCol;        
    int messageRow;     
    int tabSpacing = 2;  // Spacing between tabs
    int statsWidth = 22; // Width of stats panel
    int maxRow = 33;     // Maximum row to prevent scrolling

    // Shared message bar for tooltips, building details, and warnings
    struct MessageBarSettings
    {
        int row;                    // Will be calculated dynamically
        int col;                    // Same as menu column
        int width = 120;            
        int height = 2;             // Height of the message bar
        bool showBorder = false;    
        string currentContent = ""; 
        string contentType = "";    
    } messageBar;

    // Menu layout customization
    struct MenuSettings
    {
        int dropdownSpacing = 2;          // Space from tab to dropdown
        int dropdownIndent = 0;           // Indent for dropdown items
        int maxDropdownItems = 6;         // Maximum items to show at once
        bool showBorders = true;          // Show borders around dropdowns
        string activeTabIndicator = "";   // Character to indicate active tab
        string selectedItemPrefix = "> "; // Prefix for selected items
    } menuLayout;

    // Tooltip customization
    struct TooltipSettings
    {
        int updateFrequency = 150;           
        string prefix = "Tip: ";            
        bool useBgColor = true;             
        int bgR = 0, bgG = 0, bgB = 255;     
        int fgR = 255, fgG = 255, fgB = 255; 
    } tooltips;

    // Stats panel customization
    struct StatsSettings
    {
        bool useBorders = true;  
        int headerSpacing = 0;   
        int sectionSpacing = 1; 
        bool boldHeaders = true; 
    } statsPanel;

    // Building cursor customization
    struct CursorSettings
    {
        string buildCursorSymbol = "[]";            
        string removeCursorSymbol = "[]";            
        bool useColors = true;                       
        int buildR = 0, buildG = 255, buildB = 0;    // Green for build
        int removeR = 255, removeG = 0, removeB = 0; // Red for remove
    } cursor;
    
    // Colors (RGB values)
    struct
    {
        struct
        {
            int r = 131, g = 198, b = 105;
        } landBackground;
        struct
        {
            int r = 80, g = 130, b = 60;
        } landForeground;
        struct
        {
            int r = 117, g = 226, b = 254;
        } waterBackground;
        struct
        {
            int r = 93, g = 181, b = 203;
        } waterForeground;
        struct
        {
            int r = 32, g = 32, b = 32;
        } borderBackground;
    } colors;
    
    // Initialize positions based on map dimensions
    void initPositions()
    {
        menuRow = mapHeight + 4;
        menuCol = leftOffset + 1;
        statsRow = menuRow; // One row below the menu plus spacing
        // messageRow will be calculated after computing maxDropdownItems
        // statsCol will be calculated later after tab positions are known
    }
} visual;

struct GameSettings
{
    // Building requirements
    bool requireRoadsForBuildings = true;
    bool allowRemovingAirport = false;
    bool allowFloodingOfBuildings = true;
    
    // Environmental settings
    int startingPollution = 15;       // Increased from 10 for more challenge
    int startingTemperature = 23;     // Decreased from 25 to give more time before climate issues
    int startingForestCoverage = 40;  // Decreased from 50 to encourage more forest planting
    int pollutionIncreasePerFactory = 4; // Increased from 3 to make industrial buildings more impactful
    int pollutionDecreasePerPark = 3;    // Increased from 2 to make forests more valuable
    
    // Time settings
    int startingYear = 2050;
    int cooldownTimeMs = 500;
    float randomEventChance = 0.25f; // Increased from 0.2 for more interesting gameplay
} gameSettings;

struct EconomySettings
{
    // Starting resources
    int startingFunds = 4000;      
    int startingPopulation = 1000; 
    int startingHappiness = 70;     
    
    // Income and expenses
    int baseYearlyRevenue = 600;   
    int baseYearlyExpenses = 400;   
    
    // Building impacts
    int populationPerHouse = 60;    
    int happinessPerPark = 6;     
    int happinessPenaltyPerFactory = 4; 
    
    // Building income/costs
    int factoryIncome = 250;        
    int shopIncome = 120;       
    int houseTax = 35;             
    int roadMaintenance = 6;        
    int buildingMaintenance = 12;   
    
    // Demolition
    float buildingRefundPercentage = 0.4f; // Decreased from 0.5f to make demolition decisions more meaningful
} economy;

// Map dimensions 
static const int map_width = 42;  
static const int map_height = 16; 
static const int tileWidth = 2;  
int leftOffset = 12;              

struct tile
{
    string texture, colour, type;
    string building; 
    bool hasBuilding; 
};

// Forward declarations
void emoji_Support();
void clear_Screen();
void draw_Message(const string &msg);
void display_Map(tile map[map_height][map_width]);
void draw_Stats();
void applyCustomSettings();
void draw_BuildingDetails();

bool hasUpgrade(const string& upgradeName);
bool hasCategoryUpgrade(const string& category, int level);
void purchaseUpgrade(int upgradeIndex);
void startResearch(int researchIndex);
void loadResearchQuestions(int researchIndex);
void handleResearchQuestion(int researchIndex);
void showSelectedItemDescription();

void display_EconomyStats();
void display_EnvironmentStats();
void display_PopulationStats();

void advanceYear(tile map[map_height][map_width]);
void advanceResearch(tile map[map_height][map_width]);

// UI layout variables
int menuRow = map_height + 4;
int menuCol = leftOffset;
int statsRow = map_height + 6;
int statsCol = tileWidth + 2;
int maxDropdownItems = 6; 

// Building system variables
int cursorRow = 0, cursorCol = 0;
string selectedBuilding = "";
bool buildMode = false;
bool removeMode = false;

struct BuildingType
{
    string name;
    string symbol;
    string color;
    int cost;
    string description;
};

vector<BuildingType> buildingTypes = {
    {"Road", "==", custom_Colour(80, 80, 80) + custom_Background(150, 150, 150), 12, "Connects buildings and increases city efficiency"},
    {"Residential", "🏠", custom_Colour(200, 100, 0) + custom_Background(131, 198, 105), 120, "Increases population with low carbon emissions"},
    {"Commercial", "🏪", custom_Colour(100, 100, 200) + custom_Background(131, 198, 105), 180, "Increases income and tourism with medium carbon emissions"},
    {"Industrial", "🏭", custom_Colour(100, 100, 100) + custom_Background(131, 198, 105), 350, "High income but decreases tourism and produces high pollution"},
    {"Power", "⚡", custom_Colour(255, 215, 0) + custom_Background(131, 198, 105), 250, "Provides power to the city"},
    {"Forest", "🌳", custom_Colour(0, 120, 0) + custom_Background(131, 198, 105), 60, "Reduces pollution and increases environment quality"},
    {"Education", "🏫", custom_Colour(220, 220, 220) + custom_Background(131, 198, 105), 300, "Increases happiness in surrounding area"},
    {"Hospital", "🏥", custom_Colour(255, 0, 0) + custom_Background(131, 198, 105), 350, "Provides healthcare and increases happiness in surrounding area"},
    {"Police", "🚨", custom_Colour(0, 0, 255) + custom_Background(131, 198, 105), 280, "Increases safety and happiness in surrounding area"},
    {"Entertainment", "🏰", custom_Colour(180, 50, 180) + custom_Background(131, 198, 105), 230, "Increases happiness and tourism in surrounding area"},
    {"Airport", "✈️", custom_Colour(50, 50, 200) + custom_Background(150, 150, 150), 1200, "Main transportation hub"}};

// Upgrade system structure
struct Upgrade {
    string name;
    string description;
    int cost;
    bool purchased;
    string category;
    int level;
    
    // Effects
    float incomeMultiplier;      // Multiplies income from related buildings
    float costReductionFactor;   // Reduces costs of related buildings
    float pollutionReductionFactor; // Reduces pollution
    float happinessBoostFactor;  // Boosts happiness from related buildings
    int populationBoost;         // Additional population per residential building
};

// List of available upgrades
vector<Upgrade> upgrades = {
    {"Residential Upgrade I", "Improves housing quality and capacity", 1200, false, "Residential", 1, 1.2, 0.0, 0.0, 1.2, 20},
    {"Residential Upgrade II", "Advanced housing with smart technology", 3000, false, "Residential", 2, 1.5, 0.0, 0.0, 1.5, 50},
    
    {"Commercial Upgrade I", "Enhanced shopping and business areas", 1500, false, "Commercial", 1, 1.3, 0.0, 0.0, 1.1, 0},
    {"Commercial Upgrade II", "Modern shopping malls and office complexes", 3500, false, "Commercial", 2, 1.6, 0.0, 0.0, 1.2, 0},
    
    {"Industrial Upgrade I", "More efficient factories with reduced pollution", 1800, false, "Industrial", 1, 1.25, 0.0, 0.25, 0.0, 0},
    {"Industrial Upgrade II", "Clean manufacturing technologies", 4000, false, "Industrial", 2, 1.6, 0.0, 0.5, 0.0, 0},
    
    {"Development Upgrade I", "Improved public facilities", 1200, false, "Development", 1, 0.0, 0.15, 0.0, 1.3, 0},
    {"Development Upgrade II", "Modern community centers and facilities", 3000, false, "Development", 2, 0.0, 0.25, 0.0, 1.7, 0},
    
    {"Power Upgrade I", "More efficient and cleaner power generation", 1500, false, "Power", 1, 0.0, 0.0, 0.2, 0.0, 0},
    {"Power Upgrade II", "Renewable energy technologies", 3500, false, "Power", 2, 0.0, 0.0, 0.4, 0.0, 0}
};

// Research system structure
struct Research {
    string name;
    string description;
    int cost;
    bool unlocked;
    int researchPoints;
    int requiredPoints;
    
    // New fields for question system
    string questionsFile;
    bool questionMode;
    int currentQuestionIndex;
    vector<string> questions;
    vector<string> correctAnswers;
    int questionsAskedThisSession; // Track questions asked in current session
    
    // Statistics for questions
    int correctAnswersCount;
    int totalAnswersCount;
    
    // Effects when completed
    vector<string> unlocksUpgrades;
    vector<string> unlocksTechnologies;
    float economyBoostFactor;
    float environmentBoostFactor;
    float happinessBoostFactor;
};

// List of research projects
vector<Research> researchProjects = {
    {"Smart City", "Develop smart infrastructure and digital technologies", 2500, false, 0, 120, 
     "SmartCity_Questions.txt", false, 0, {}, {}, 0,
     0, 0, // correctAnswersCount, totalAnswersCount
     {"Residential Upgrade II", "Commercial Upgrade II"}, {}, 1.3, 0.0, 1.15},
     
    {"Green City", "Research sustainable practices and green technologies", 2500, false, 0, 120, 
     "GreenCity_Questions.txt", false, 0, {}, {}, 0,
     0, 0, // correctAnswersCount, totalAnswersCount
     {"Industrial Upgrade II", "Power Upgrade II"}, {}, 0.0, 1.7, 1.25},
     
    {"Sustainable City", "Develop comprehensive sustainability solutions", 4000, false, 0, 180, 
     "SustainableCity_Questions.txt", false, 0, {}, {}, 0,
     0, 0, // correctAnswersCount, totalAnswersCount
     {"Development Upgrade II"}, {"Advanced Urban Planning", "Clean Transport"}, 1.4, 1.4, 1.4}
};

// Buildings that don't require road connections
vector<string> noRoadRequiredBuildings = {"Road", "Forest", "Airport"};

// Stats
int cityFunds = 5000;
int population = 1234;
int happiness = 75;

int currentYear = 2050;
int pollution = 10;       // 0-100 scale
int temperature = 25;     // in Celsius
int seaLevel = 0;         // 0-100 scale (relative rise)
int forestCoverage = 50;  // 0-100 scale
int yearlyRevenue = 500;  // base yearly income
int yearlyExpenses = 300; // base yearly maintenance
bool isCooldown = false;  // to prevent multiple year advances in succession

// Time progression effects
struct ClimateEvent
{
    string name;
    string description;
    int type; // Instead of function, use type identifier: 0=heatwave, 1=rain, 2=pollution, 3=economic
};

// Climate events array
vector<ClimateEvent> climateEvents = {
    {"Heat Wave", "A heat wave has struck your city! Happiness decreased.", 0},
    {"Heavy Rain", "Heavy rainfall has occurred! Parks grow faster but roads need maintenance.", 1},
    {"Pollution Crisis", "Pollution levels have reached a critical point!", 2},
    {"Economic Boom", "The economy is thriving! Extra revenue generated.", 3},
    {"Cold Snap", "A sudden cold snap has reduced power efficiency!", 2},  // Added new event
    {"Tourism Surge", "A tourism surge has boosted commercial income!", 3} // Added new event
};

// Structures for tracking historical data
struct EconomyHistory {
    int year;
    int funds;
    int yearlyIncome;
    int yearlyExpenses;
    int commercialCount;
    int industrialCount;
};

struct EnvironmentHistory {
    int year;
    int pollution;
    int temperature;
    int forestCoverage;
    int seaLevel;
};

struct PopulationHistory {
    int year;
    int population;
    int happiness;
    int residentialCount;
    int powerConsumption;
};

// Vectors to store historical data (max 10 years)
vector<EconomyHistory> economyHistory;
vector<EnvironmentHistory> environmentHistory;
vector<PopulationHistory> populationHistory;

// Apply climate event effects
void applyClimateEvent(int eventType, tile map[map_height][map_width])
{
    switch (eventType)
    {
        case 0: // Heat Wave
            happiness = max(0, happiness - 12);
            temperature += 3;
            draw_Message("Heat Wave: Temperature increased to " + to_string(temperature) + "°C");
            break;
        case 1: // Heavy Rain
            cityFunds -= 250;
            forestCoverage += 6;
            draw_Message("Heavy Rain: $250 spent on road maintenance");
            break;
        case 2: // Pollution Crisis
            happiness = max(0, happiness - 15);
            pollution += 15;
            draw_Message("Pollution Crisis: Pollution increased to " + to_string(pollution) + "%");
            break;
        case 3: // Economic Boom
            cityFunds += 1200;
            happiness += 8;
            draw_Message("Economic Boom: $1200 added to city funds");
            break;
        case 4: // Cold Snap (new)
            temperature -= 5;
            cityFunds -= 150;
            draw_Message("Cold Snap: Temperature dropped to " + to_string(temperature) + "°C and increased heating costs!");
            break;
        case 5: // Tourism Surge (new)
            int commercialCount = 0;
            for (int i = 0; i < map_height; i++) {
                for (int j = 0; j < map_width; j++) {
                    if (map[i][j].hasBuilding && map[i][j].building == "Commercial")
                        commercialCount++;
                }
            }
            int bonus = commercialCount * 75;
            cityFunds += bonus;
            happiness += 5;
            draw_Message("Tourism Surge: $" + to_string(bonus) + " additional income from " + to_string(commercialCount) + " commercial buildings!");
            break;
    }
}

// Tooltip system
vector<string> tooltips = {
    "Connect buildings with roads to maximize efficiency",
    "Balance residential, commercial, and industrial zones for optimal growth",
    "Forests reduce pollution and improve citizen happiness",
    "Place education buildings to boost research progress",
    "Power plants are essential but contribute to pollution",
    "Use R&D to unlock powerful Level II upgrades",
    "Negative funds will cause a happiness penalty - manage your budget!",
    "Answering research questions correctly speeds up progress",
    "Upgrade buildings to improve their effectiveness and reduce costs",
    "Monitor pollution levels to prevent environmental crises",
    "Rising sea levels can flood coastal areas - plan inland development",
    "A happy population grows faster and produces more income",
    "Entertainment buildings boost tourism and happiness",
    "Education buildings speed up research and improve quality of life",
    "Keep police stations distributed throughout your city for safety",
    "Hospitals improve overall health and contribute to happiness",
    "Press ESC to cancel building placement or exit menus",
    "Check the STATS tab to monitor your city's performance metrics",
    "Upgrades require cash investment but pay off in the long term",
    "Year advancement triggers random events - be prepared!"
};

int currentTooltip = 0;
bool showingTooltip = false;
int tooltipTimer = 0;

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
                map[i][j].colour = custom_Background(visual.colors.waterBackground.r, 
                                                    visual.colors.waterBackground.g, 
                                                    visual.colors.waterBackground.b) + 
                                  custom_Colour(visual.colors.waterForeground.r, 
                                                visual.colors.waterForeground.g, 
                                                visual.colors.waterForeground.b);
                map[i][j].texture = (rand() % 100 < 30 ? "~ " : "  ");
                map[i][j].type = "Water";
            }
            else
            {
                map[i][j].colour = custom_Background(visual.colors.landBackground.r, 
                                                    visual.colors.landBackground.g, 
                                                    visual.colors.landBackground.b) + 
                                  custom_Colour(visual.colors.landForeground.r, 
                                                visual.colors.landForeground.g, 
                                                visual.colors.landForeground.b);
                map[i][j].texture = (rand() % 100 < 20 ? "^^" : "  ");
                map[i][j].type = "Land";
            }
            map[i][j].hasBuilding = false;
            map[i][j].building = "";
        }
    }
    
    // Place airport at the center of the map
    int airportRow = map_height / 2;
    int airportCol = map_width / 2;
    
    // Ensure the center is land
    map[airportRow][airportCol].type = "Land";
    map[airportRow][airportCol].colour = custom_Background(visual.colors.landBackground.r, 
                                                          visual.colors.landBackground.g, 
                                                          visual.colors.landBackground.b) + 
                                        custom_Colour(visual.colors.landForeground.r, 
                                                     visual.colors.landForeground.g, 
                                                     visual.colors.landForeground.b);
    map[airportRow][airportCol].texture = "  ";
    
    map[airportRow][airportCol].hasBuilding = true;
    map[airportRow][airportCol].building = "Airport";
    
    for (int dr = -1; dr <= 1; dr++)
    {
        for (int dc = -1; dc <= 1; dc++)
        {
            if ((dr == 0 && dc == 0) || (abs(dr) == 1 && abs(dc) == 1))
                continue;
            
            int r = airportRow + dr;
            int c = airportCol + dc;
            
            if (r >= 0 && r < map_height && c >= 0 && c < map_width)
            {
                map[r][c].type = "Land";
                map[r][c].colour = custom_Background(visual.colors.landBackground.r, 
                                                    visual.colors.landBackground.g, 
                                                    visual.colors.landBackground.b) + 
                                  custom_Colour(visual.colors.landForeground.r, 
                                                visual.colors.landForeground.g, 
                                                visual.colors.landForeground.b);
                map[r][c].texture = "  ";
                
                // Place road
                map[r][c].hasBuilding = true;
                map[r][c].building = "Road";
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
        {
            if (buildMode && i == cursorRow && j == cursorCol)
            {
                // Show cursor in build mode with customized appearance
                if (visual.cursor.useColors)
                {
                    string buildColor = custom_Background(
                        visual.cursor.buildR,
                        visual.cursor.buildG,
                        visual.cursor.buildB);
                    cout << REVERSE << buildColor << visual.cursor.buildCursorSymbol << RESET;
                }
                else
                {
                    cout << REVERSE << map[i][j].colour << visual.cursor.buildCursorSymbol << RESET;
                }
            }
            else if (removeMode && i == cursorRow && j == cursorCol)
            {
                // Show cursor in remove mode with customized appearance
                if (visual.cursor.useColors)
                {
                    string removeColor = custom_Background(
                        visual.cursor.removeR,
                        visual.cursor.removeG,
                        visual.cursor.removeB);
                    cout << REVERSE << removeColor << visual.cursor.removeCursorSymbol << RESET;
                }
                else
                {
                    cout << REVERSE << B_RED << visual.cursor.removeCursorSymbol << RESET;
                }
            }
            else if (map[i][j].hasBuilding)
            {
                // Find the building type to display
                bool found = false;
                for (const auto &type : buildingTypes)
                {
                    if (map[i][j].building == type.name)
                    {
                        cout << type.color << type.symbol << RESET;
                        found = true;
                        break;
                    }
                }
                if (!found)
                    cout << map[i][j].colour << map[i][j].texture << RESET;
            }
            else
            {
            cout << map[i][j].colour << map[i][j].texture << RESET;
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

// Function to display content in the shared message bar
void display_MessageBar(const string &content, const string &contentType)
{
    // Stores what's being displayed and its type
    visual.messageBar.currentContent = content;
    visual.messageBar.contentType = contentType;

    int row = visual.messageBar.row;
    int col = visual.messageBar.col;
    int width = visual.messageBar.width;
    int maxContentWidth = width - 6; // Account for padding, prefix and other elements

    // Ensure we don't draw beyond max row
    if (row >= visual.maxRow)
        return;

    // Clear the entire message bar area with spaces
    // Use multiple lines to completely clear previous longer messages
    std::cout << "\033[" << row << ";" << col << "H" << string(width, ' ');
    if (row + 1 < visual.maxRow) {
        std::cout << "\033[" << (row+1) << ";" << col << "H" << string(width, ' ');
    }

    // Early return if nothing to display
    if (content.empty())
    {
        return;
    }

    // Apply styling based on content type - using text colors instead of backgrounds
    string style, prefix, suffix;
    if (contentType == "tooltip")
    {
        // Tooltip styling - blue text and bold with special prefix
        style = string(B_BLUE) + string(BOLD);
        prefix = "ℹ️ ";  // Info symbol
        suffix = "";
    }
    else if (contentType == "warning")
    {
        // Warning styling - red text and bold with warning prefix
        style = string(B_RED) + string(BOLD);
        prefix = "⚠️ ";  // Warning symbol
        suffix = " !";
    }
    else if (contentType == "building")
    {
        // Building details styling - cyan text and bold with building prefix
        style = string(B_CYAN) + string(BOLD);
        prefix = "🏢 ";  // Building symbol
        suffix = "";
    }
    else
    {
        // Default styling - white text
        style = string(B_WHITE);
        prefix = "";
        suffix = "";
    }

    // Intelligently process long content
    string displayContent = content;
    
    // With the larger width, we can be more generous with keeping content
    // but we still want to ensure we don't go over the maximum width
    if (displayContent.length() > maxContentWidth) {
        // Find pipe separators for smarter truncation
        vector<size_t> pipePositions;
        size_t pos = 0;
        while ((pos = displayContent.find(" | ", pos)) != string::npos) {
            pipePositions.push_back(pos);
            pos += 3; // Move past the found separator
        }
        
        // If we have pipe separators, try to preserve the most important parts
        if (!pipePositions.empty()) {
            // For long content with sections, try to keep essential parts from each section
            if (pipePositions.size() >= 2 && pipePositions.back() < displayContent.length() - 20) {
                // Keep first section, a key part of middle sections, and last section
                string firstPart = displayContent.substr(0, pipePositions[0]);
                string lastPart = displayContent.substr(pipePositions.back() + 3); // Skip the " | "
                
                // Calculate remaining space for middle parts
                int remainingSpace = maxContentWidth - firstPart.length() - lastPart.length() - 12; // 12 for separators and ellipses
                
                if (remainingSpace > 15) {  // Only add middle part if we have reasonable space
                    // Get a portion of the middle section
                    int middleStart = pipePositions[0] + 3;
                    int middleLength = pipePositions.back() - middleStart;
                    string middlePart = displayContent.substr(middleStart, min(remainingSpace, middleLength));
                    
                    // Format: FirstPart | Middle... | LastPart
                    displayContent = firstPart + " | " + middlePart + 
                                    (middleLength > remainingSpace ? "..." : "") +
                                    " | " + lastPart;
                } else {
                    // Not enough space for middle, just combine first and last
                    displayContent = firstPart + " | ... | " + lastPart;
                }
            } else {
                // Just keep first section plus part of the rest if possible
                string firstPart = displayContent.substr(0, pipePositions[0]);
                string restPart = displayContent.substr(pipePositions[0] + 3);
                
                int remainingSpace = maxContentWidth - firstPart.length() - 6; // 6 for separator and ellipsis
                if (remainingSpace > 10 && restPart.length() > 0) {
                    displayContent = firstPart + " | " + restPart.substr(0, remainingSpace) + 
                                    (restPart.length() > remainingSpace ? "..." : "");
                } else {
                    displayContent = firstPart + "...";
                }
            }
        } else {
            // No pipe separators, truncate with ellipsis
            displayContent = displayContent.substr(0, maxContentWidth - 3) + "...";
        }
    }
    
    // Add prefix and suffix to final display content
    displayContent = prefix + displayContent + suffix;

    // Display the content with padding and decorative elements
    std::cout << "\033[" << row << ";" << col << "H" << style << " " << displayContent << " " << RESET;
}

void draw_Message(const string &msg)
{
    // Use the shared message bar for warnings and messages
    display_MessageBar(msg, "warning");
}

// Tooltip functions
void draw_Tooltip()
{
    string tooltip = tooltips[currentTooltip];

    // Use the shared message bar to display tooltips
    display_MessageBar(tooltip, "tooltip");
}

// Update tooltip - call this periodically to change the displayed tip
void update_Tooltip()
{
    // Only update tooltip if we're showing tooltips and timer has expired
    if (showingTooltip)
    {
        tooltipTimer++;
        if (tooltipTimer >= visual.tooltips.updateFrequency)
        { 
            tooltipTimer = 0;
            
            // Select a new tooltip that's different from the current one
            int newTooltip;
            do {
                newTooltip = rand() % tooltips.size();
            } while (newTooltip == currentTooltip && tooltips.size() > 1);
            
            currentTooltip = newTooltip;
            draw_Tooltip();
        }
    }
}

// Function to enable tooltips when they should be shown
void enable_Tooltips()
{
    if (!showingTooltip)
    {
        showingTooltip = true;
        tooltipTimer = 0;
        
        // Select a random tooltip
        currentTooltip = rand() % tooltips.size();
        draw_Tooltip();
    }
}

// Function to disable tooltips when they shouldn't be shown
void disable_Tooltips()
{
    if (showingTooltip)
    {
        showingTooltip = false;

        // If the message bar is showing a tooltip, clear it
        if (visual.messageBar.contentType == "tooltip")
        {
            // Clear the message bar by displaying empty content
            display_MessageBar("", "");
        }
    }
}

void draw_Stats()
{
    // Use customized width
    const int statsWidth = visual.statsWidth;

    // Clear the entire stats area with a more reliable approach
    for (int i = 0; i < 15; i++)
    {
        int clearRow = statsRow + i;
        if (clearRow >= visual.maxRow)
            break;                                                                // Stop clearing if at max row
        cout << "\033[" << clearRow << ";" << statsCol << "H" << string(40, ' '); // Clear wider area in case previous was wider
    }

    // Determine if we're using borders
    bool useBorders = visual.statsPanel.useBorders;
    string horizontalLine = string(statsWidth - 2, '-');

    // Top border
    if (useBorders && statsRow < visual.maxRow)
    {
        cout << "\033[" << statsRow << ";" << statsCol << "H" << "+" << horizontalLine << "+";
    }

    // Compact Stats with full names
    string headerStyle = visual.statsPanel.boldHeaders ? BOLD : "";
    if (statsRow + 1 < visual.maxRow)
    {
        cout << "\033[" << (statsRow + 1) << ";" << statsCol << "H";
        if (useBorders)
            cout << "| ";
        cout << headerStyle << "City Stats" << RESET;

        // Draw the right border at the exact position
        if (useBorders)
            cout << "\033[" << (statsRow + 1) << ";" << (statsCol + statsWidth - 1) << "H" << "|";
    }

    int headerSpacing = visual.statsPanel.headerSpacing;
    int currentRow = statsRow + 1 + headerSpacing;

    // Draw stats only if within max row constraint
    auto drawStatsLine = [&](const string &label, const string &value)
    {
        currentRow++;
        if (currentRow >= visual.maxRow)
            return false;

        cout << "\033[" << currentRow << ";" << statsCol << "H";
        if (useBorders)
            cout << "| ";
        cout << label << value;

        if (useBorders)
        {
            // Calculate how many spaces needed for proper right border alignment
            int paddingRequired = statsWidth - 2 - label.length() - value.length();
            // Ensure padding is at least 1 to prevent misalignment
            paddingRequired = max(1, paddingRequired);

            // Move cursor to the exact position for the right border
            cout << "\033[" << currentRow << ";" << (statsCol + statsWidth - 1) << "H" << "|";
        }
        return true;
    };

    // Draw all stats lines using the helper function
    bool canContinue = true;
    canContinue = drawStatsLine("Year: ", to_string(currentYear));
    canContinue = canContinue && drawStatsLine("Population: ", to_string(population));
    canContinue = canContinue && drawStatsLine("Funds: $", to_string(cityFunds));
    canContinue = canContinue && drawStatsLine("Happiness: ", to_string(happiness) + "%");
    canContinue = canContinue && drawStatsLine("Temperature: ", to_string(temperature) + "°C");
    canContinue = canContinue && drawStatsLine("Pollution: ", to_string(pollution) + "%");

    // Bottom border for stats box
    if (canContinue)
    {
        currentRow++;
        if (useBorders && currentRow < visual.maxRow)
        {
            // Draw the bottom border at a fixed width for consistency
            cout << "\033[" << currentRow << ";" << statsCol << "H" << "+" << horizontalLine << "+";
        }
    }
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
        x += int(m.name.size()) + 3 + visual.tabSpacing;
    }

    // Calculate where stats should be positioned - right after the last tab
    // Add a smaller buffer (9 instead of 10) for spacing between menu and stats
    // and move it one space to the left
    statsCol = tabPositions.back() + menus.back().name.size() + 9;

    // Set message bar width to match menu width
    int menuWidth = statsCol - menuCol - 2; // Subtract 2 for spacing
    visual.messageBar.width = menuWidth;
}

void compute_UI_positions()
{
    // Determine maximum dropdown size
    maxDropdownItems = 0;
    for (auto &m : menus)
        maxDropdownItems = max(maxDropdownItems, int(m.items.size()));

    // Calculate positions for messages and tooltips
    // First calculate the ending row of the dropdown menu
    int dropdownLastRow = menuRow + visual.menuLayout.dropdownSpacing + maxDropdownItems;

    // If menu has borders, add one more row for bottom border
    if (visual.menuLayout.showBorders)
        dropdownLastRow += 1;

    // Position message bar right below the dropdown menu with a 1-row gap
    visual.messageBar.row = dropdownLastRow + 2;
    visual.messageBar.col = menuCol;

    // Calculate message bar width based on menu width
    // This will be updated after tabs are positioned

    // Make sure nothing goes beyond maxRow
    visual.messageBar.row = min(visual.messageBar.row, visual.maxRow - visual.messageBar.height);

    // Position message at same row as message bar for backward compatibility
    visual.messageRow = visual.messageBar.row;
}

void clear_MenuArea()
{
    // Calculate a generous area to clear
    int height = maxDropdownItems + 15; // Add extra rows for safety
    int width = 120;                   // Wide enough for all menu content

    // Clear the entire menu area including dropdown, respecting max row
    for (int r = menuRow; r < min(menuRow + height, visual.maxRow); r++)
    {
        cout << "\033[" << r << ";" << menuCol - 2 << "H" << string(width, ' ');
    }
}

void draw_Menu()
{
    // Clear the entire menu area
    clear_MenuArea();

    // Only draw if within max row constraint
    if (menuRow >= visual.maxRow)
        return;

    // Draw the tab headers
    for (int t = 0; t < int(menus.size()); t++)
    {
        cout << "\033[" << menuRow << ";" << tabPositions[t] << "H";
        if (t == currentTab)
        {
            cout << REVERSE << BOLD << "[" << menus[t].name << "]" << RESET;
            // Add indicator if configured
            if (!visual.menuLayout.activeTabIndicator.empty())
            {
                cout << " " << visual.menuLayout.activeTabIndicator;
            }
        }
        else
            cout << "[" << menus[t].name << "]";
    }

    // Draw a separator line below tabs if borders are enabled
    if (visual.menuLayout.showBorders && menuRow + 1 < visual.maxRow)
    {
        cout << "\033[" << (menuRow + 1) << ";" << menuCol << "H"
             << string(tabPositions.back() + menus.back().name.length() + 5 - menuCol - 2, '-');
    }

    // Calculate dropdown position based on customization
    int dropdownStartRow = menuRow + visual.menuLayout.dropdownSpacing;
    int dropdownIndent = visual.menuLayout.dropdownIndent;

    // Draw the dropdown items for the current tab, respecting max row
    int maxDropdownItems = min(int(menus[currentTab].items.size()),
                               visual.maxRow - dropdownStartRow);

    for (int i = 0; i < maxDropdownItems; i++)
    {
        // Position based on customized values
        int dropdownRow = dropdownStartRow + i;
        if (dropdownRow >= visual.maxRow)
            break;

        cout << "\033[" << dropdownRow << ";" << (menuCol + dropdownIndent) << "H";

        if (i == currentOpt)
        {
            // Selected item with customized prefix
            cout << REVERSE << BOLD << " " << visual.menuLayout.selectedItemPrefix
                 << menus[currentTab].items[i] << " " << RESET;
        }
        else
        {
            // Unselected item
            cout << " " << menus[currentTab].items[i] << " ";
        }
    }

    // Draw bottom border for dropdown if enabled
    int dropdownEndRow = dropdownStartRow + maxDropdownItems;
    if (visual.menuLayout.showBorders && !menus[currentTab].items.empty() &&
        dropdownEndRow < visual.maxRow)
    {
        int dropdownWidth = 0;
        // Find the longest item for proper width
        for (const auto &item : menus[currentTab].items)
        {
            dropdownWidth = max(dropdownWidth, int(item.length() + visual.menuLayout.selectedItemPrefix.length() + 4));
        }

        cout << "\033[" << dropdownEndRow << ";"
             << (menuCol + dropdownIndent - 1) << "H" << " " << string(62, '-');
    }
}

// Functions for the building system
void enterBuildMode(const string &buildingType)
{
    selectedBuilding = buildingType;
    buildMode = true;
    cursorRow = map_height / 2;
    cursorCol = map_width / 2;
    draw_Message("Build mode: Use arrow keys to position, Enter to build, Esc to cancel");
}

bool isAdjacentToRoad(tile map[map_height][map_width], int row, int col)
{
    // Check all 4 adjacent tiles (up, down, left, right)
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};
    
    for (int i = 0; i < 4; i++)
    {
        int newRow = row + dr[i];
        int newCol = col + dc[i];
        
        // Check if within bounds
        if (newRow >= 0 && newRow < map_height && newCol >= 0 && newCol < map_width)
        {
            // Check if there's a road or airport (which connects to roads)
            if (map[newRow][newCol].hasBuilding && 
                (map[newRow][newCol].building == "Road" || map[newRow][newCol].building == "Airport"))
            {
                return true;
            }
        }
    }
    return false;
}

bool canBuildHere(tile map[map_height][map_width], int row, int col, const string &buildingType)
{
    tile &currentTile = map[row][col];
    
    // Check if tile already has a building
    if (currentTile.hasBuilding)
        return false;
    
    // Check if tile is land (can't build on water except special buildings)
    if (currentTile.type != "Land" && buildingType != "Bridge")
        return false;
    
    // Check road connectivity requirements
    bool requiresRoad = true;
    for (const auto &noRoadBuilding : noRoadRequiredBuildings)
    {
        if (buildingType == noRoadBuilding)
        {
            requiresRoad = false;
            break;
        }
    }
    
    // Special case for roads: can only connect to existing roads or airports
    if (buildingType == "Road")
    {
        return isAdjacentToRoad(map, row, col);
    }
    
    // For other buildings, check if they need a road connection
    if (requiresRoad && !isAdjacentToRoad(map, row, col))
    {
        return false;
    }
    
    return true;
}

void placeBuildingOnMap(tile map[map_height][map_width], int row, int col, const string &buildingType)
{
    // Find the cost of the building
    int cost = 0;
    for (const auto &type : buildingTypes)
    {
        if (type.name == buildingType)
        {
            cost = type.cost;
            break;
        }
    }
    
    // Apply upgrade modifiers to building placement
    float incomeMultiplier = 1.0;
    float costReduction = 0.0;
    float pollutionReduction = 0.0;
    float happinessBoost = 1.0;
    int populationBoost = 0;
    
    // Calculate upgrade modifiers directly
    string category = "";
    if (buildingType == "Residential") category = "Residential";
    else if (buildingType == "Commercial") category = "Commercial";
    else if (buildingType == "Industrial") category = "Industrial";
    else if (buildingType == "Power") category = "Power";
    else if (buildingType == "Education" || buildingType == "Hospital" || 
             buildingType == "Police" || buildingType == "Entertainment") category = "Development";
    
    // Apply upgrades if category matches
    if (!category.empty()) {
        for (const auto& upgrade : upgrades) {
            if (upgrade.category == category && upgrade.purchased) {
                incomeMultiplier *= (upgrade.incomeMultiplier > 0) ? upgrade.incomeMultiplier : 1.0;
                costReduction += upgrade.costReductionFactor;
                pollutionReduction += upgrade.pollutionReductionFactor;
                happinessBoost *= (upgrade.happinessBoostFactor > 0) ? upgrade.happinessBoostFactor : 1.0;
                populationBoost += upgrade.populationBoost;
            }
        }
        
        // Apply research effects as well to further enhance upgrades
        for (const auto& research : researchProjects) {
            if (research.unlocked && research.researchPoints >= research.requiredPoints) {
                // Apply general boosts based on category
                if (category == "Residential" || category == "Commercial") {
                    incomeMultiplier *= (research.economyBoostFactor > 0) ? research.economyBoostFactor : 1.0;
                }
                
                if (category == "Industrial" || category == "Power") {
                    float additionalReduction = (1.0 - pollutionReduction) * 
                                              ((research.environmentBoostFactor > 0) ? 
                                              (research.environmentBoostFactor - 1.0) / research.environmentBoostFactor : 0.0);
                    pollutionReduction += additionalReduction;
                }
                
                happinessBoost *= (research.happinessBoostFactor > 0) ? research.happinessBoostFactor : 1.0;
            }
        }
        
        // Cap reductions at 0.9 (90%)
        costReduction = min(costReduction, 0.9f);
        pollutionReduction = min(pollutionReduction, 0.9f);
    }
    
    // Apply the cost reduction
    if (costReduction > 0) {
        int originalCost = cost;
        cost = int(cost * (1.0 - costReduction));
        
        // Show cost reduction message if significant
        if (originalCost - cost >= 10) {
            draw_Message("Upgrade applied: Cost reduced from $" + to_string(originalCost) + 
                       " to $" + to_string(cost) + " (-" + to_string(int(costReduction * 100)) + "%)");
            Sleep(1000); // Show message for a moment
        }
    }
    
    // Check if enough funds
    if (cityFunds < cost)
    {
        draw_Message("Not enough funds to build " + buildingType + "! Need $" + to_string(cost));
        return;
    }
    
    // Check if we can build here
    if (!canBuildHere(map, row, col, buildingType))
    {
        if (buildingType == "Road")
        {
            draw_Message("Roads must connect to existing roads or the airport!");
        }
        else if (find(noRoadRequiredBuildings.begin(), noRoadRequiredBuildings.end(), buildingType) == noRoadRequiredBuildings.end())
        {
            draw_Message(buildingType + " must be placed adjacent to a road!");
        }
        else
        {
            draw_Message("Cannot build " + buildingType + " here!");
        }
        return;
    }
    
    // Place the building
    map[row][col].hasBuilding = true;
    map[row][col].building = buildingType;
    cityFunds -= cost;
    
    // Effects of buildings with upgrade modifiers applied
    if (buildingType == "Residential")
    {
        int popIncrease = economy.populationPerHouse + populationBoost;
        population += popIncrease;
        
        if (populationBoost > 0) {
            draw_Message("Residential upgrade applied: +" + to_string(popIncrease) + " population");
            Sleep(800); // Show message for a moment
        }
    }
    else if (buildingType == "Forest")
    {
        int happinessIncrease = int(economy.happinessPerPark * happinessBoost);
        happiness = min(100, happiness + happinessIncrease);
        
        if (happinessBoost > 1.0) {
            draw_Message("Development upgrade applied: +" + to_string(happinessIncrease) + " happiness");
            Sleep(800); // Show message for a moment
        }
    }
    else if (buildingType == "Industrial")
    {
        int happinessDecrease = int(economy.happinessPenaltyPerFactory * (1.0 - pollutionReduction));
        happiness = max(0, happiness - happinessDecrease);
        
        if (pollutionReduction > 0) {
            draw_Message("Industrial upgrade applied: Reduced pollution impact by " + 
                       to_string(int(pollutionReduction * 100)) + "%");
            Sleep(800); // Show message for a moment
        }
    }
    else if (buildingType == "Commercial")
    {
        int bonusAmount = int(50 * incomeMultiplier);
        cityFunds += bonusAmount;
        
        if (incomeMultiplier > 1.0) {
            draw_Message("Commercial upgrade applied: Bonus increased to $" + to_string(bonusAmount));
            Sleep(800); // Show message for a moment
        }
    }
    else if (buildingType == "Education" || buildingType == "Hospital" ||
              buildingType == "Police" || buildingType == "Entertainment")
    {
        int happinessIncrease = int(3 * happinessBoost);
        happiness = min(100, happiness + happinessIncrease);
        
        if (happinessBoost > 1.0) {
            draw_Message("Development upgrade applied: +" + to_string(happinessIncrease) + " happiness");
            Sleep(800); // Show message for a moment
        }
    }
    
    draw_Message(buildingType + " built successfully!");
    draw_Stats();
}

void handleBuildingMode(tile map[map_height][map_width])
{
    int ch = _getch();
    if (ch == 224)
    {
        int dir = _getch();
        if (dir == 72 && cursorRow > 0)
            cursorRow--; // Up
        if (dir == 80 && cursorRow < map_height - 1)
            cursorRow++; // Down
        if (dir == 75 && cursorCol > 0)
            cursorCol--; // Left
        if (dir == 77 && cursorCol < map_width - 1)
            cursorCol++; // Right
    }
    else if (ch == 13)
    { // Enter
        placeBuildingOnMap(map, cursorRow, cursorCol, selectedBuilding);
    }
    else if (ch == 27)
    { // Escape
        buildMode = false;
        selectedBuilding = "";
        draw_Message("Build mode canceled");
    }
    display_Map(map);
    draw_Stats();

    // Draw building details below menu
    if (buildMode && !selectedBuilding.empty())
    {
        draw_BuildingDetails();
        disable_Tooltips();
    }
}

void enterRemoveMode()
{
    removeMode = true;
    buildMode = false;
    cursorRow = map_height / 2;
    cursorCol = map_width / 2;
    draw_Message("Remove mode: Use arrow keys to select, Enter to remove, Esc to cancel");
}

void handleRemoveMode(tile map[map_height][map_width])
{
    int ch = _getch();
    if (ch == 224)
    {
        int dir = _getch();
        if (dir == 72 && cursorRow > 0)
            cursorRow--; // Up
        if (dir == 80 && cursorRow < map_height - 1)
            cursorRow++; // Down
        if (dir == 75 && cursorCol > 0)
            cursorCol--; // Left
        if (dir == 77 && cursorCol < map_width - 1)
            cursorCol++; // Right
    }
    else if (ch == 13)
    { // Enter
        tile &t = map[cursorRow][cursorCol];
        if (t.hasBuilding && (t.building != "Airport" || gameSettings.allowRemovingAirport))
        {
            string removed = t.building;
            
            // Update stats based on what building is being removed
            if (removed == "Residential")
            {
                population = max(0, population - economy.populationPerHouse); // Reduce population
            }
            else if (removed == "Industrial")
            {
                pollution = max(0, pollution - gameSettings.pollutionIncreasePerFactory); // Reduce pollution
                happiness = min(100, happiness + economy.happinessPenaltyPerFactory);     // Increase happiness
            }
            else if (removed == "Forest")
            {
                happiness = max(0, happiness - economy.happinessPerPark); // Decrease happiness
            }
            else if (removed == "Education" || removed == "Hospital" ||
                     removed == "Police" || removed == "Entertainment")
            {
                happiness = max(0, happiness - 3); // All development buildings decrease happiness when removed
            }
            else if (removed == "Road")
            {
                // Check if removing this road would disconnect buildings
                if (gameSettings.requireRoadsForBuildings)
                {
                    bool hasAdjacentBuildings = false;
                    const int dr[] = {-1, 1, 0, 0};
                    const int dc[] = {0, 0, -1, 1};
                    
                    for (int i = 0; i < 4; i++)
                    {
                        int nr = cursorRow + dr[i];
                        int nc = cursorCol + dc[i];
                        
                        if (nr >= 0 && nr < map_height && nc >= 0 && nc < map_width)
                        {
                            if (map[nr][nc].hasBuilding && 
                                map[nr][nc].building != "Road" && 
                                map[nr][nc].building != "Airport" &&
                                map[nr][nc].building != "Park")
                            {
                                
                                // Check if this building has another road connection
                                bool hasOtherRoad = false;
                                for (int j = 0; j < 4; j++)
                                {
                                    int nnr = nr + dr[j];
                                    int nnc = nc + dc[j];
                                    
                                    if (nnr >= 0 && nnr < map_height && nnc >= 0 && nnc < map_width &&
                                        !(nnr == cursorRow && nnc == cursorCol) && // Not the road we're removing
                                        map[nnr][nnc].hasBuilding && 
                                        (map[nnr][nnc].building == "Road" || map[nnr][nnc].building == "Airport"))
                                    {
                                        hasOtherRoad = true;
                                        break;
                                    }
                                }
                                
                                if (!hasOtherRoad)
                                {
                                    hasAdjacentBuildings = true;
                                    break;
                                }
                            }
                        }
                    }
                    
                    if (hasAdjacentBuildings)
                    {
                        draw_Message("Cannot remove road - buildings would be disconnected!");
                        display_Map(map);
                        draw_Stats();
                        return;
                    }
                }
            }
            
            // Remove the building
            t.hasBuilding = false;
            t.building = "";
            draw_Message(removed + " removed successfully.");
            
            // Refund part of the cost when removing buildings
            for (const auto &type : buildingTypes)
            {
                if (type.name == removed)
                {
                    int refund = int(type.cost * economy.buildingRefundPercentage);
                    cityFunds += refund;
                    draw_Message(removed + " removed. Refunded $" + to_string(refund));
                    break;
                }
            }
        }
        else if (t.building == "Airport" && !gameSettings.allowRemovingAirport)
        {
            draw_Message("Cannot remove the Airport - it's the central hub!");
        }
        else
        {
            draw_Message("No building to remove here.");
        }
    }
    else if (ch == 27)
    { // Escape
        removeMode = false;
        draw_Message("Remove mode canceled");
    }
    display_Map(map);
    draw_Stats();
}

// Modify the menu actions for building
void handle_Input(tile map[map_height][map_width])
{
    while (true)
    {
        // Update tooltip display
        update_Tooltip();
        
        // Handle building and removal modes
        if (buildMode)
        {
            handleBuildingMode(map);
            continue;
        }
        if (removeMode)
        {
            handleRemoveMode(map);
            continue;
        }
        
        // Check if any research is in question mode
        bool inQuestionMode = false;
        for (int i = 0; i < researchProjects.size(); i++) {
            if (researchProjects[i].questionMode) {
                // Make sure UI is updated
                draw_Menu();
                draw_Stats();
                
                // Handle the question
                handleResearchQuestion(i);
                inQuestionMode = true;
                break;
            }
        }
        
        if (inQuestionMode) {
            // Stay in question mode until it's resolved
            continue;
        }
        
        // Handle keyboard input
        int ch = _getch();
        if (ch == 224)
        {
            int dir = _getch();
            if (dir == 75)
            {
                currentTab = (currentTab - 1 + menus.size()) % menus.size();
                currentOpt = 0;
                
                // Show description for the selected option
                showSelectedItemDescription();
            }
            if (dir == 77)
            {
                currentTab = (currentTab + 1) % menus.size();
                currentOpt = 0;
                
                // Show description for the selected option
                showSelectedItemDescription();
            }
            if (dir == 80)
            {
                currentOpt = (currentOpt + 1) % menus[currentTab].items.size();
                // Reset tooltip timer to allow more time to read menu item description
                tooltipTimer = 0;
                
                // Show description for the selected option
                showSelectedItemDescription();
            }
            if (dir == 72)
            {
                currentOpt = (currentOpt - 1 + menus[currentTab].items.size()) % menus[currentTab].items.size();
                // Reset tooltip timer to allow more time to read menu item description
                tooltipTimer = 0;
                
                // Show description for the selected option
                showSelectedItemDescription();
            }
        }
        else if (ch == 13)
        {
            // Execute the selected action
            // Check if it's the STATS tab
            if (menus[currentTab].name == "STATS")
            {
                // Disable tooltips when showing stats
                disable_Tooltips();
                
                // Execute the action and ensure it returns properly
                menus[currentTab].actions[currentOpt]();
                
                // Re-enable tooltips
                enable_Tooltips();
            }
            else
            {
                // Normal menu action execution
                menus[currentTab].actions[currentOpt]();
            }
        }
        else if (ch == 27)
        {
            // Disable tooltips when exiting menu
            disable_Tooltips();
            break; // exit menu
        }
        
        // Update UI elements
        draw_Menu();
        draw_Stats();
        
        // Draw building details below menu if in build mode
        if (buildMode && !selectedBuilding.empty())
        {
            draw_BuildingDetails();
            disable_Tooltips(); // Don't show tooltips when showing building details
        }
        else
        {
            // Enable tooltips when not showing building details
            enable_Tooltips();
        }
    }
}

// Function to display descriptions for selected menu items
void showSelectedItemDescription() {
    if (menus[currentTab].name == "UPGRADES" && currentOpt < menus[currentTab].items.size()) {
        // Get the selected upgrade item name
        string selectedItem = menus[currentTab].items[currentOpt];
        
        // Remove " (Completed)" if present
        size_t completedPos = selectedItem.find(" (Completed)");
        if (completedPos != string::npos) {
            selectedItem = selectedItem.substr(0, completedPos);
        }
        
        // Find the upgrade
        for (const auto& upgrade : upgrades) {
            if (upgrade.name == selectedItem) {
                // Display cost and description
                string status = upgrade.purchased ? "PURCHASED" : "COST: $" + to_string(upgrade.cost);
                string description = upgrade.description;
                
                // Display effects - use shorter format
                string effects = "";
                if (upgrade.incomeMultiplier > 1.0)
                    effects += "Inc+" + to_string(int((upgrade.incomeMultiplier - 1.0) * 100)) + "% ";
                if (upgrade.costReductionFactor > 0)
                    effects += "Cost-" + to_string(int(upgrade.costReductionFactor * 100)) + "% ";
                if (upgrade.pollutionReductionFactor > 0)
                    effects += "Poll-" + to_string(int(upgrade.pollutionReductionFactor * 100)) + "% ";
                if (upgrade.happinessBoostFactor > 1.0)
                    effects += "Hap+" + to_string(int((upgrade.happinessBoostFactor - 1.0) * 100)) + "% ";
                if (upgrade.populationBoost > 0)
                    effects += "Pop+" + to_string(upgrade.populationBoost);
                
                // Show info in message bar
                display_MessageBar(status + " | " + description + (effects.empty() ? "" : " | " + effects), "building");
                disable_Tooltips();
                return;
            }
        }
    } else if (menus[currentTab].name == "R&D" && currentOpt < menus[currentTab].items.size()) {
        // Get the selected research item name
        string selectedItem = menus[currentTab].items[currentOpt];
        
        // Find the research
        for (const auto& research : researchProjects) {
            if (research.name == selectedItem) {
                // Display status and description in a more compact form
                string status;
                if (!research.unlocked) {
                    status = "COST: $" + to_string(research.cost);
                } else if (research.researchPoints < research.requiredPoints) {
                    if (research.questionMode) {
                        status = "IN PROGRESS: " + to_string(research.researchPoints) + "/" + to_string(research.requiredPoints) + " (Q)";
                    } else {
                        status = "IN PROGRESS: " + to_string(research.researchPoints) + "/" + to_string(research.requiredPoints);
                    }
                } else {
                    status = "COMPLETED";
                }
                
                string description = research.description;
                
                // Display unlocks in more compact form
                string unlocks = "";
                if (!research.unlocksUpgrades.empty()) {
                    unlocks = "Unlocks: ";
                    for (const auto& upgradeName : research.unlocksUpgrades) {
                        // Extract just the category and level for brevity
                        size_t spacePos = upgradeName.find(" ");
                        if (spacePos != string::npos) {
                            unlocks += upgradeName.substr(0, spacePos) + " II, ";
                        } else {
                            unlocks += upgradeName + ", ";
                        }
                    }
                    // Remove trailing comma and space
                    if (unlocks.length() > 9) {
                        unlocks = unlocks.substr(0, unlocks.length() - 2);
                    }
                }
                
                // Add success rate if there have been any answers
                string statsInfo = "";
                if (research.totalAnswersCount > 0) {
                    int successRate = (research.correctAnswersCount * 100) / research.totalAnswersCount;
                    statsInfo = " | Success: " + to_string(successRate) + "% (" + 
                               to_string(research.correctAnswersCount) + "/" + 
                               to_string(research.totalAnswersCount) + ")";
                }
                
                // Add questions info if relevant
                string questionsInfo = "";
                if (research.unlocked && research.researchPoints < research.requiredPoints) {
                    questionsInfo = " | Q: " + to_string(research.questionsAskedThisSession) + "/2";
                }
                
                // Show info in message bar
                display_MessageBar(status + " | " + description + 
                                 (unlocks.empty() ? "" : " | " + unlocks) + 
                                 statsInfo + questionsInfo, "building");
                disable_Tooltips();
                return;
            }
        }
    }
    
    // For other menu items, show tooltips
    enable_Tooltips();
}

// Time progression function
void advanceYear(tile map[map_height][map_width])
{
    if (isCooldown)
    {
        draw_Message("Please wait a moment before advancing another year");
        return;
    }
    
    currentYear++;
    
    // Count buildings
    int residentialCount = 0;
    int commercialCount = 0;
    int industrialCount = 0;
    int forestCount = 0;
    int roadCount = 0;
    int educationCount = 0;
    int hospitalCount = 0;
    int policeCount = 0;
    int entertainmentCount = 0;
    int powerCount = 0;
    
    // Count buildings
    for (int i = 0; i < map_height; i++)
    {
        for (int j = 0; j < map_width; j++)
        {
            if (map[i][j].hasBuilding)
            {
                if (map[i][j].building == "Industrial")
                    industrialCount++;
                else if (map[i][j].building == "Forest")
                    forestCount++;
                else if (map[i][j].building == "Residential")
                    residentialCount++;
                else if (map[i][j].building == "Road")
                    roadCount++;
                else if (map[i][j].building == "Commercial")
                    commercialCount++;
                else if (map[i][j].building == "Education")
                    educationCount++;
                else if (map[i][j].building == "Hospital")
                    hospitalCount++;
                else if (map[i][j].building == "Police")
                    policeCount++;
                else if (map[i][j].building == "Entertainment")
                    entertainmentCount++;
                else if (map[i][j].building == "Power")
                    powerCount++;
            }
        }
    }
    
    // Calculate upgrade modifiers for each building type
    float residentialIncomeMultiplier = 1.0;
    float commercialIncomeMultiplier = 1.0;
    float industrialIncomeMultiplier = 1.0;
    float industrialPollutionReduction = 0.0;
    float residentialHappinessBoost = 1.0;
    int residentialPopulationBoost = 0;
    
    // Apply residential upgrades
    for (const auto& upgrade : upgrades) {
        if (upgrade.purchased) {
            if (upgrade.category == "Residential") {
                residentialIncomeMultiplier *= (upgrade.incomeMultiplier > 0) ? upgrade.incomeMultiplier : 1.0;
                residentialHappinessBoost *= (upgrade.happinessBoostFactor > 0) ? upgrade.happinessBoostFactor : 1.0;
                residentialPopulationBoost += upgrade.populationBoost;
            }
            else if (upgrade.category == "Commercial") {
                commercialIncomeMultiplier *= (upgrade.incomeMultiplier > 0) ? upgrade.incomeMultiplier : 1.0;
            }
            else if (upgrade.category == "Industrial") {
                industrialIncomeMultiplier *= (upgrade.incomeMultiplier > 0) ? upgrade.incomeMultiplier : 1.0;
                industrialPollutionReduction += upgrade.pollutionReductionFactor;
            }
        }
    }
    
    // Cap pollution reduction at 90%
    industrialPollutionReduction = min(industrialPollutionReduction, 0.9f);
    
    // Apply research effects to modifiers
    for (const auto& research : researchProjects) {
        if (research.unlocked && research.researchPoints >= research.requiredPoints) {
            if (research.economyBoostFactor > 0) {
                residentialIncomeMultiplier *= research.economyBoostFactor;
                commercialIncomeMultiplier *= research.economyBoostFactor;
            }
            if (research.environmentBoostFactor > 0) {
                float additionalReduction = (1.0 - industrialPollutionReduction) * 
                                           ((research.environmentBoostFactor - 1.0) / research.environmentBoostFactor);
                industrialPollutionReduction += additionalReduction;
            }
            if (research.happinessBoostFactor > 0) {
                residentialHappinessBoost *= research.happinessBoostFactor;
            }
        }
    }
    
    // Economic effects with upgrade modifiers
    int industrialIncome = int(industrialCount * economy.factoryIncome * industrialIncomeMultiplier);
    int commercialIncome = int(commercialCount * economy.shopIncome * commercialIncomeMultiplier);
    int residentialTax = int(residentialCount * economy.houseTax * residentialIncomeMultiplier);
    int roadMaintenance = roadCount * economy.roadMaintenance;
    int buildingMaintenance = (industrialCount + commercialCount + residentialCount +
                                educationCount + hospitalCount + policeCount +
                                entertainmentCount + powerCount) *
                               economy.buildingMaintenance;
    
    int totalIncome = industrialIncome + commercialIncome + residentialTax + yearlyRevenue;
    int totalExpenses = roadMaintenance + buildingMaintenance + yearlyExpenses;
    int netIncome = totalIncome - totalExpenses;
    
    cityFunds += netIncome;
    
    // Population growth with upgrade effects
    int populationGrowth = int(residentialCount * (economy.populationPerHouse + residentialPopulationBoost) / 5 * (happiness / 100.0));
    population += populationGrowth;
    
    // Environmental effects with pollution reduction from upgrades
    pollution += int(industrialCount * gameSettings.pollutionIncreasePerFactory * (1.0 - industrialPollutionReduction)) - 
                 forestCount * gameSettings.pollutionDecreasePerPark;
    pollution = max(0, min(100, pollution));
    
    temperature += (pollution > 50) ? 1 : 0;
    
    if (pollution > 70)
    {
        happiness = max(0, happiness - 5);
    }
    
    forestCoverage += forestCount - (industrialCount / 2);
    forestCoverage = max(0, min(100, forestCoverage));
    
    if (temperature > 30 && gameSettings.allowFloodingOfBuildings)
    {
        seaLevel += 2;
        seaLevel = min(100, seaLevel);
        
        // Flooding of coastal tiles if sea level rises too much
        if (seaLevel > 50)
        {
            for (int i = 0; i < map_height; i++)
            {
                for (int j = 0; j < map_width; j++)
                {
                    if (map[i][j].type == "Water")
                    {
                        // Check adjacent land tiles for flooding
                        const int dr[] = {-1, 1, 0, 0};
                        const int dc[] = {0, 0, -1, 1};
                        
                        for (int dir = 0; dir < 4; dir++)
                        {
                            int ni = i + dr[dir];
                            int nj = j + dc[dir];
                            
                            if (ni >= 0 && ni < map_height && nj >= 0 && nj < map_width && 
                                map[ni][nj].type == "Land" && rand() % 100 < 10)
                            {
                                // 10% chance to flood adjacent land tiles
                                if (map[ni][nj].hasBuilding)
                                {
                                    // Destroy building on flooding
                                    map[ni][nj].hasBuilding = false;
                                    map[ni][nj].building = "";
                                }
                                map[ni][nj].type = "Water";
                                map[ni][nj].colour = custom_Background(visual.colors.waterBackground.r, 
                                                                      visual.colors.waterBackground.g, 
                                                                      visual.colors.waterBackground.b) + 
                                                    custom_Colour(visual.colors.waterForeground.r, 
                                                                 visual.colors.waterForeground.g, 
                                                                 visual.colors.waterForeground.b);
                                map[ni][nj].texture = "~ ";
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Record historical data (keep max 10 records)
    // Economy history
    if (economyHistory.size() >= 10) {
        economyHistory.erase(economyHistory.begin());
    }
    economyHistory.push_back({
        currentYear,
        cityFunds,
        totalIncome,
        totalExpenses,
        commercialCount,
        industrialCount
    });
    
    // Environment history
    if (environmentHistory.size() >= 10) {
        environmentHistory.erase(environmentHistory.begin());
    }
    environmentHistory.push_back({
        currentYear,
        pollution,
        temperature,
        forestCoverage,
        seaLevel
    });
    
    // Population history
    if (populationHistory.size() >= 10) {
        populationHistory.erase(populationHistory.begin());
    }
    
    // Calculate power consumption based on buildings
    int powerConsumption = (residentialCount * 1) + (commercialCount * 2) + (industrialCount * 3);
    
    populationHistory.push_back({
        currentYear,
        population,
        happiness,
        residentialCount,
        powerConsumption
    });
    
    // Advance research progress
    advanceResearch(map);
    
    // Random climate events
    if (rand() % 100 < gameSettings.randomEventChance * 100 && !climateEvents.empty())
    {
        int eventIndex = rand() % climateEvents.size();
        draw_Message(climateEvents[eventIndex].description);
        applyClimateEvent(climateEvents[eventIndex].type, map);
    }
    
    // Financial crisis if funds are negative
    if (cityFunds < 0)
    {
        happiness = max(0, happiness - 20);
        draw_Message("FINANCIAL CRISIS! City is in debt. Happiness decreased significantly.");
    }
    
    // Success message
    string yearSummary = "Year " + to_string(currentYear) + ": Income $" + to_string(totalIncome) + 
                         ", Expenses $" + to_string(totalExpenses) + ", Net $" + to_string(netIncome);
    draw_Message(yearSummary);
    
    // Set a brief cooldown to prevent rapid clicking
    isCooldown = true;
    HANDLE hTimer = CreateThread(NULL, 0, [](LPVOID param) -> DWORD
                                  {
        Sleep(gameSettings.cooldownTimeMs);  // Customizable cooldown time
        isCooldown = false;
        return 0; }, NULL, 0, NULL);
    CloseHandle(hTimer);
    
    // Update display
    display_Map(map);
    draw_Stats();
}

// Update map dimensions in main() before using them
void applyCustomSettings()
{
    // Apply visual settings
    // We can't modify static const variables, so we'll use the original values
    // but initialize all dependent variables from our settings
    
    // Initialize UI positions based on map dimensions
    visual.initPositions();
    
    // Set UI variables from our customization struct
    menuRow = visual.menuRow;
    menuCol = visual.menuCol;
    statsRow = visual.statsRow;
    statsCol = visual.statsCol;
    leftOffset = visual.leftOffset;
    
    // Initialize game settings
    pollution = gameSettings.startingPollution;
    temperature = gameSettings.startingTemperature;
    forestCoverage = gameSettings.startingForestCoverage;
    currentYear = gameSettings.startingYear;
    
    // Initialize economy
    cityFunds = economy.startingFunds;
    population = economy.startingPopulation;
    happiness = economy.startingHappiness;
    yearlyRevenue = economy.baseYearlyRevenue;
    yearlyExpenses = economy.baseYearlyExpenses;
    
    // Initialize first history records to have data available immediately
    economyHistory.push_back({
        currentYear,
        cityFunds,
        yearlyRevenue,
        yearlyExpenses,
        0, // no commercial buildings yet
        0  // no industrial buildings yet
    });
    
    environmentHistory.push_back({
        currentYear,
        pollution,
        temperature,
        forestCoverage,
        seaLevel
    });
    
    populationHistory.push_back({
        currentYear,
        population,
        happiness,
        0, // no residential buildings yet
        0  // no power consumption yet
    });
}

// New function to draw building details below main menu
void draw_BuildingDetails()
{
    // Only show if in build mode with a selected building
    if (!buildMode || selectedBuilding.empty())
    {
        return;
    }

    // Find the building type
    const BuildingType *selectedType = nullptr;
    for (const auto &type : buildingTypes)
    {
        if (type.name == selectedBuilding)
        {
            selectedType = &type;
            break;
        }
    }

    if (!selectedType)
        return;

    // Format the building details as a concise message
    string details = "Building: " + selectedType->name +
                     " | Cost: $" + to_string(selectedType->cost) +
                     " | " + selectedType->description;

    // Display using the shared message bar
    display_MessageBar(details, "building");
}

// Function to check if a specific upgrade has been purchased
bool hasUpgrade(const string& upgradeName) {
    for (const auto& upgrade : upgrades) {
        if (upgrade.name == upgradeName && upgrade.purchased) {
            return true;
        }
    }
    return false;
}

// Function to check if a category has an upgrade of specific level
bool hasCategoryUpgrade(const string& category, int level) {
    for (const auto& upgrade : upgrades) {
        if (upgrade.category == category && upgrade.level == level && upgrade.purchased) {
            return true;
        }
    }
    return false;
}

// Function to get modifiers for a building type based on purchased upgrades
void getUpgradeModifiers(const string& buildingType, float& incomeMultiplier, float& costReduction, 
                         float& pollutionReduction, float& happinessBoost, int& populationBoost) {
    // Default values
    incomeMultiplier = 1.0;
    costReduction = 0.0;
    pollutionReduction = 0.0;
    happinessBoost = 1.0;
    populationBoost = 0;
    
    string category = "";
    
    // Determine category based on building type
    if (buildingType == "Residential") category = "Residential";
    else if (buildingType == "Commercial") category = "Commercial";
    else if (buildingType == "Industrial") category = "Industrial";
    else if (buildingType == "Power") category = "Power";
    else if (buildingType == "Education" || buildingType == "Hospital" || 
             buildingType == "Police" || buildingType == "Entertainment") category = "Development";
    
    // No upgrades for other building types
    if (category.empty()) return;
    
    // Apply modifiers from purchased upgrades - stack level 1 and 2 effects
    for (const auto& upgrade : upgrades) {
        if (upgrade.category == category && upgrade.purchased) {
            incomeMultiplier *= (upgrade.incomeMultiplier > 0) ? upgrade.incomeMultiplier : 1.0;
            costReduction += upgrade.costReductionFactor;
            pollutionReduction += upgrade.pollutionReductionFactor;
            happinessBoost *= (upgrade.happinessBoostFactor > 0) ? upgrade.happinessBoostFactor : 1.0;
            populationBoost += upgrade.populationBoost;
        }
    }
    
    // Apply research effects as well to further enhance upgrades
    for (const auto& research : researchProjects) {
        if (research.unlocked && research.researchPoints >= research.requiredPoints) {
            // Apply general boosts based on category
            if (category == "Residential" || category == "Commercial") {
                incomeMultiplier *= (research.economyBoostFactor > 0) ? research.economyBoostFactor : 1.0;
            }
            
            if (category == "Industrial" || category == "Power") {
                float additionalReduction = (1.0 - pollutionReduction) * 
                                          ((research.environmentBoostFactor > 0) ? 
                                          (research.environmentBoostFactor - 1.0) / research.environmentBoostFactor : 0.0);
                pollutionReduction += additionalReduction;
            }
            
            happinessBoost *= (research.happinessBoostFactor > 0) ? research.happinessBoostFactor : 1.0;
        }
    }
    
    // Cap reductions at 0.9 (90%)
    costReduction = min(costReduction, 0.9f);
    pollutionReduction = min(pollutionReduction, 0.9f);
}

// Function to update the menu items after an upgrade is purchased
void updateUpgradesMenu() {
    // Get a reference to the UPGRADES menu
    int upgradesMenuIndex = -1;
    for (int i = 0; i < menus.size(); i++) {
        if (menus[i].name == "UPGRADES") {
            upgradesMenuIndex = i;
            break;
        }
    }
    
    if (upgradesMenuIndex == -1) return; // Not found
    
    // Define menu item descriptions for each upgrade level
    vector<string> level1Names = {"Residential Upgrade I", "Commercial Upgrade I", "Industrial Upgrade I", 
                                 "Development Upgrade I", "Power Upgrade I"};
    vector<string> level2Names = {"Residential Upgrade II", "Commercial Upgrade II", "Industrial Upgrade II", 
                                 "Development Upgrade II", "Power Upgrade II"};
    
    // Find matching upgrade indices
    vector<int> level1Indices = {0, 2, 4, 6, 8};  // Indices in upgrades array for level 1 upgrades
    vector<int> level2Indices = {1, 3, 5, 7, 9};  // Indices in upgrades array for level 2 upgrades
    
    // Clear and rebuild menu actions
    menus[upgradesMenuIndex].items.clear();
    menus[upgradesMenuIndex].actions.clear();
    
    // Check each upgrade and update menu
    for (int i = 0; i < level1Names.size(); i++) {
        // Find if current level upgrade is purchased
        bool level1Purchased = false;
        for (const auto& upgrade : upgrades) {
            if (upgrade.name == level1Names[i] && upgrade.purchased) {
                level1Purchased = true;
                break;
            }
        }
        
        // If level 1 is purchased, check for level 2
        if (level1Purchased) {
            // Find if level 2 upgrade is available (research completed)
            bool level2Available = false;
            bool level2Purchased = false;
            
            for (const auto& upgrade : upgrades) {
                if (upgrade.name == level2Names[i]) {
                    // Check if this upgrade is unlocked by any completed research
                    for (const auto& research : researchProjects) {
                        if (research.unlocked && 
                            research.researchPoints >= research.requiredPoints &&
                            find(research.unlocksUpgrades.begin(), research.unlocksUpgrades.end(), upgrade.name) 
                            != research.unlocksUpgrades.end()) {
                            level2Available = true;
                        }
                    }
                    
                    // Check if already purchased
                    if (upgrade.purchased) {
                        level2Purchased = true;
                    }
                    
                    break;
                }
            }
            
            // Update menu item and action
            if (level2Available && !level2Purchased) {
                // Add level 2 upgrade to menu
                menus[upgradesMenuIndex].items.push_back(level2Names[i]);
                
                // Use captured index for level 2
                int capturedIndex = level2Indices[i];
                menus[upgradesMenuIndex].actions.push_back([capturedIndex]() { purchaseUpgrade(capturedIndex); });
            } else if (level2Purchased) {
                // Both levels purchased, show as completed
                menus[upgradesMenuIndex].items.push_back(level2Names[i] + " (Completed)");
                
                // Add an empty action that shows a message
                menus[upgradesMenuIndex].actions.push_back([]() { 
                    draw_Message("This upgrade has already been completed!"); 
                });
            } else {
                // Level 1 completed but level 2 not available yet
                menus[upgradesMenuIndex].items.push_back(level1Names[i] + " (Completed)");
                
                // Add an action that explains the research requirement
                menus[upgradesMenuIndex].actions.push_back([]() { 
                    draw_Message("This upgrade has been completed. Research needed for next level."); 
                });
            }
        } else {
            // Level 1 not purchased, add to menu
            menus[upgradesMenuIndex].items.push_back(level1Names[i]);
            
            // Use captured index for level 1
            int capturedIndex = level1Indices[i];
            menus[upgradesMenuIndex].actions.push_back([capturedIndex]() { purchaseUpgrade(capturedIndex); });
        }
    }
    
    // Ensure there's at least one option
    if (menus[upgradesMenuIndex].items.empty()) {
        menus[upgradesMenuIndex].items.push_back("All upgrades completed!");
        menus[upgradesMenuIndex].actions.push_back([]() {
            draw_Message("Congratulations! You've completed all available upgrades.");
        });
    }
    
    // Reset current option to 0 if it's out of bounds
    if (currentTab == upgradesMenuIndex && currentOpt >= menus[upgradesMenuIndex].items.size()) {
        currentOpt = 0;
    }
    
    // Rebuild tab positions after menu changes
    rebuild_TabPositions();
}

// Function to purchase an upgrade
void purchaseUpgrade(int upgradeIndex) {
    if (upgradeIndex < 0 || upgradeIndex >= upgrades.size()) {
        draw_Message("Invalid upgrade selection!");
        return;
    }
    
    Upgrade& selectedUpgrade = upgrades[upgradeIndex];
    
    // Check if already purchased
    if (selectedUpgrade.purchased) {
        draw_Message("You've already purchased " + selectedUpgrade.name);
        return;
    }
    
    // Check for dependencies - Level 2 upgrades require Level 1
    if (selectedUpgrade.level == 2) {
        // Find the level 1 upgrade of the same category
        bool hasLevel1 = false;
        for (const auto& upgrade : upgrades) {
            if (upgrade.category == selectedUpgrade.category &&
                upgrade.level == 1 &&
                upgrade.purchased) {
                hasLevel1 = true;
                break;
            }
        }
        
        if (!hasLevel1) {
            draw_Message("You need to purchase " + selectedUpgrade.category + " Upgrade I first!");
            return;
        }
        
        // Check if this upgrade requires research
        bool requiresResearch = false;
        bool researchCompleted = false;
        string requiredResearch = "";
        
        for (const auto& research : researchProjects) {
            // Check if this upgrade is mentioned in research unlocks
            bool upgradeInUnlocks = (find(research.unlocksUpgrades.begin(), 
                                          research.unlocksUpgrades.end(), 
                                          selectedUpgrade.name) != research.unlocksUpgrades.end());
            
            if (upgradeInUnlocks) {
                requiresResearch = true;
                requiredResearch = research.name;
                
                // Check if that research is completed
                if (research.unlocked && research.researchPoints >= research.requiredPoints) {
                    researchCompleted = true;
                    break;
                }
            }
        }
        
        if (requiresResearch && !researchCompleted) {
            draw_Message("This upgrade requires " + requiredResearch + " research to be completed!");
            return;
        }
    }
    
    // Check if player has enough funds
    if (cityFunds < selectedUpgrade.cost) {
        draw_Message("Not enough funds to purchase " + selectedUpgrade.name + "! Need $" + 
                   to_string(selectedUpgrade.cost) + " (Have $" + to_string(cityFunds) + ")");
        return;
    }
    
    // Purchase the upgrade
    cityFunds -= selectedUpgrade.cost;
    selectedUpgrade.purchased = true;
    
    // Display detailed information about the upgrade effects
    string upgradeEffects = "";
    
    if (selectedUpgrade.incomeMultiplier > 1.0)
        upgradeEffects += "Income +" + to_string(int((selectedUpgrade.incomeMultiplier - 1.0) * 100)) + "%, ";
        
    if (selectedUpgrade.costReductionFactor > 0)
        upgradeEffects += "Building cost -" + to_string(int(selectedUpgrade.costReductionFactor * 100)) + "%, ";
        
    if (selectedUpgrade.pollutionReductionFactor > 0)
        upgradeEffects += "Pollution -" + to_string(int(selectedUpgrade.pollutionReductionFactor * 100)) + "%, ";
        
    if (selectedUpgrade.happinessBoostFactor > 1.0)
        upgradeEffects += "Happiness +" + to_string(int((selectedUpgrade.happinessBoostFactor - 1.0) * 100)) + "%, ";
        
    if (selectedUpgrade.populationBoost > 0)
        upgradeEffects += "Population +" + to_string(selectedUpgrade.populationBoost) + " per house, ";
    
    // Remove trailing comma and space
    if (upgradeEffects.length() > 2) {
        upgradeEffects = upgradeEffects.substr(0, upgradeEffects.length() - 2);
    }
    
    // Apply immediate effects
    // Add an immediate happiness bonus for purchasing upgrades
    happiness = min(100, happiness + 5);
    
    draw_Message("Purchased " + selectedUpgrade.name + "! Effects: " + upgradeEffects);
    
    // Update menu to reflect the purchase
    updateUpgradesMenu();
    
    draw_Stats();
}

// Function to start a research project
void startResearch(int researchIndex) {
    if (researchIndex < 0 || researchIndex >= researchProjects.size()) {
        draw_Message("Invalid research selection!");
        return;
    }
    
    Research& selectedResearch = researchProjects[researchIndex];
    
    // Check if already unlocked
    if (selectedResearch.unlocked) {
        // If already unlocked but in question mode, continue with questions
        if (selectedResearch.questionMode) {
            handleResearchQuestion(researchIndex);
        } else {
            draw_Message(selectedResearch.name + " research is already in progress!");
        }
        return;
    }
    
    // Check if player has enough funds
    if (cityFunds < selectedResearch.cost) {
        draw_Message("Not enough funds to start " + selectedResearch.name + " research!");
        return;
    }
    
    // Start the research
    cityFunds -= selectedResearch.cost;
    selectedResearch.unlocked = true;
    selectedResearch.researchPoints = 0;
    
    draw_Message("Started " + selectedResearch.name + " research! " + selectedResearch.description);
    
    // Load questions from file and enter question mode
    loadResearchQuestions(researchIndex);
    
    // If questions were loaded successfully, handle the first question
    if (selectedResearch.questionMode && !selectedResearch.questions.empty()) {
        handleResearchQuestion(researchIndex);
    }
    
    // Update both stats and upgrade menu in case research affects available upgrades
    draw_Stats();
    updateUpgradesMenu();
}

// Function to advance research - called during year advancement
void advanceResearch(tile map[map_height][map_width]) {
    bool researchCompleted = false;
    
    for (int i = 0; i < researchProjects.size(); i++) {
        auto& research = researchProjects[i];
        if (research.unlocked && research.researchPoints < research.requiredPoints) {
            // If in question mode, don't advance automatically - questions drive progress
            if (research.questionMode) {
                continue;
            }
            
            // Progress depends on city size and development
            int progressPoints = 5 + (population / 1000);
            
            // Education buildings boost research
            int educationCount = 0;
            for (int r = 0; r < map_height; r++) {
                for (int c = 0; c < map_width; c++) {
                    if (map[r][c].hasBuilding && map[r][c].building == "Education") {
                        educationCount++;
                    }
                }
            }
            
            progressPoints += educationCount * 2;
            research.researchPoints += progressPoints;
            
            // Check if research completed
            if (research.researchPoints >= research.requiredPoints) {
                researchCompleted = true;
                draw_Message(research.name + " research complete! New upgrades unlocked!");
                
                // Collect upgrade names for a single message
                string unlockedUpgrades = "";
                for (const string& upgradeName : research.unlocksUpgrades) {
                    // Extract just the category for brevity
                    size_t spacePos = upgradeName.find(" ");
                    if (spacePos != string::npos) {
                        unlockedUpgrades += upgradeName.substr(0, spacePos) + " II, ";
                    } else {
                        unlockedUpgrades += upgradeName + ", ";
                    }
                    
                    // Make it available but don't announce each one separately
                    for (auto& upgrade : upgrades) {
                        if (upgrade.name == upgradeName) {
                            // Skip the individual messages
                        }
                    }
                }
                
                // Remove trailing comma and space
                if (unlockedUpgrades.length() > 2) {
                    unlockedUpgrades = unlockedUpgrades.substr(0, unlockedUpgrades.length() - 2);
                    // Show a single message with all unlocks
                    draw_Message("Unlocked: " + unlockedUpgrades);
                }
                
                // Apply immediate effects
                happiness = min(100, happiness + 10); // Research completion boosts happiness
            } else {
                // Randomly trigger a question if not in question mode
                // 25% chance each year to get a research question
                if (rand() % 100 < 25) {
                    // Load questions if not loaded
                    if (research.questions.empty()) {
                        loadResearchQuestions(i);
                    }
                    
                    // Only enter question mode if we have questions
                    if (!research.questions.empty()) {
                        research.questionMode = true;
                        research.questionsAskedThisSession = 0; // Reset counter
                        
                        // Reset index if needed
                        if (research.currentQuestionIndex >= research.questions.size()) {
                            research.currentQuestionIndex = 0;
                        }
                        
                        draw_Message("New insights for " + research.name + "! Answer questions to advance research.");
                    }
                }
            }
        }
    }
    
    // Update the upgrades menu if any research was completed
    if (researchCompleted) {
        updateUpgradesMenu();
    }
}

// Function to load research questions from file
void loadResearchQuestions(int researchIndex) {
    if (researchIndex < 0 || researchIndex >= researchProjects.size()) {
        draw_Message("Invalid research index!");
        return;
    }
    
    Research& research = researchProjects[researchIndex];
    if (research.questions.empty()) {
        // Only load if not already loaded
        ifstream file(research.questionsFile);
        if (!file.is_open()) {
            draw_Message("Error: Could not open " + research.questionsFile);
            return;
        }
        
        string line;
        while (getline(file, line)) {
            // Format: Question|Answer
            size_t separatorPos = line.find('|');
            if (separatorPos != string::npos) {
                string question = line.substr(0, separatorPos);
                string answer = line.substr(separatorPos + 1);
                research.questions.push_back(question);
                research.correctAnswers.push_back(answer);
            }
        }
        file.close();
        
        if (research.questions.empty()) {
            draw_Message("Warning: No questions found in " + research.questionsFile);
        }
    }
    
    // Reset to first question when starting
    research.currentQuestionIndex = 0;
    research.questionMode = true;
    research.questionsAskedThisSession = 0; // Reset questions asked counter
}

// Function to handle research questions
void handleResearchQuestion(int researchIndex) {
    if (researchIndex < 0 || researchIndex >= researchProjects.size()) {
        draw_Message("Invalid research index!");
        return;
    }
    
    Research& research = researchProjects[researchIndex];
    
    // Make sure questions are loaded
    if (research.questions.empty()) {
        loadResearchQuestions(researchIndex);
        if (research.questions.empty()) {
            // Failed to load questions
            research.questionMode = false;
            return;
        }
    }
    
    // Check if we've already asked 2 questions in this session
    if (research.questionsAskedThisSession >= 2) {
        research.questionMode = false;
        draw_Message("Questions complete for now. Research continues.");
        return;
    }
    
    if (research.currentQuestionIndex >= research.questions.size()) {
        // All questions answered, reset to first question
        research.currentQuestionIndex = 0;
    }
    
    // Display current question
    string currentQuestion = research.questions[research.currentQuestionIndex];
    draw_Message(currentQuestion + " (Y/N)");
    
    // Wait for yes/no answer
    bool answered = false;
    while (!answered) {
        int ch = _getch();
        if (ch == 'y' || ch == 'Y') {
            // Check if answer is correct
            bool isCorrect = (research.correctAnswers[research.currentQuestionIndex] == "Yes");
            
            // Track statistics
            research.totalAnswersCount++;
            if (isCorrect) {
                research.correctAnswersCount++;
                // Correct answer
                draw_Message("✓ SUCCESS! +10 research points");
                research.researchPoints += 10;  // Bonus points for correct answer
            } else {
                // Incorrect answer
                draw_Message("✗ FAIL! +2 research points");
                research.researchPoints += 2;   // Small progress for incorrect answer
            }
            research.currentQuestionIndex++;
            research.questionsAskedThisSession++;
            answered = true;
        } else if (ch == 'n' || ch == 'N') {
            // Check if answer is correct
            bool isCorrect = (research.correctAnswers[research.currentQuestionIndex] == "No");
            
            // Track statistics
            research.totalAnswersCount++;
            if (isCorrect) {
                research.correctAnswersCount++;
                // Correct answer
                draw_Message("✓ SUCCESS! +10 research points");
                research.researchPoints += 10;  // Bonus points for correct answer
            } else {
                // Incorrect answer
                draw_Message("✗ FAIL! +2 research points");
                research.researchPoints += 2;   // Small progress for incorrect answer
            }
            research.currentQuestionIndex++;
            research.questionsAskedThisSession++;
            answered = true;
        } else if (ch == 27) {  // ESC key
            // Exit question mode
            research.questionMode = false;
            draw_Message("Research questions paused.");
            answered = true;
        }
    }
    
    // Check if research is completed after this question
    if (research.researchPoints >= research.requiredPoints) {
        research.questionMode = false;
        draw_Message(research.name + " research complete! New upgrades unlocked!");
        
        // Collect upgrade names for a single message
        string unlockedUpgrades = "";
        for (const string& upgradeName : research.unlocksUpgrades) {
            // Extract just the category for brevity
            size_t spacePos = upgradeName.find(" ");
            if (spacePos != string::npos) {
                unlockedUpgrades += upgradeName.substr(0, spacePos) + " II, ";
            } else {
                unlockedUpgrades += upgradeName + ", ";
            }
        }
        
        // Remove trailing comma and space
        if (unlockedUpgrades.length() > 2) {
            unlockedUpgrades = unlockedUpgrades.substr(0, unlockedUpgrades.length() - 2);
            // Show a single message with all unlocks
            draw_Message("Unlocked: " + unlockedUpgrades);
        }
        
        // Apply immediate effects
        happiness = min(100, happiness + 10); // Research completion boosts happiness
        
        // Update the upgrades menu
        updateUpgradesMenu();
    } else if (research.questionsAskedThisSession >= 2) {
        // Two questions answered in this session, exit question mode
        research.questionMode = false;
        draw_Message("Questions complete for now. Research continues.");
    }
}

// Function to display Economy Statistics
void display_EconomyStats() {
    const int statsWidth = 70;
    
    // Clear any previous messages and menus
    clear_MenuArea();
    display_MessageBar("", "");
    
    // Calculate display row to ensure it's visible
    const int displayRow = menuRow + 3;
    
    // Display title
    std::cout << "\033[" << displayRow << ";" << menuCol << "H" << BOLD << B_YELLOW << "ECONOMY STATISTICS" << RESET;
    
    // Display record count
    std::cout << "\033[" << (displayRow + 1) << ";" << menuCol << "H" 
              << "Records: " << economyHistory.size();
    
    // Define column widths for consistent alignment
    const int yearWidth = 6;
    const int fundsWidth = 12;
    const int incomeWidth = 12;
    const int expensesWidth = 12;
    const int comWidth = 6;
    const int indWidth = 6;
    
    // Display header with aligned columns
    std::cout << "\033[" << (displayRow + 2) << ";" << menuCol << "H" 
              << BOLD << setw(yearWidth) << left << "Year" << RESET << " | " 
              << BOLD << setw(fundsWidth) << left << "Funds" << RESET << " | " 
              << BOLD << setw(incomeWidth) << left << "Income" << RESET << " | " 
              << BOLD << setw(expensesWidth) << left << "Expenses" << RESET << " | " 
              << BOLD << setw(comWidth) << left << "Com." << RESET << " | " 
              << BOLD << setw(indWidth) << left << "Ind." << RESET;
    
    // Display horizontal line
    std::cout << "\033[" << (displayRow + 3) << ";" << menuCol << "H" << string(statsWidth, '-');
    
    // Display historical data (most recent at the top)
    int row = displayRow + 4;
    if (economyHistory.empty()) {
        std::cout << "\033[" << row << ";" << menuCol << "H" << "No economic data available yet.";
        row++;
    } else {
        for (int i = economyHistory.size() - 1; i >= 0; i--) {
            // Format each data point
            string year = to_string(economyHistory[i].year);
            string funds = "$" + to_string(economyHistory[i].funds);
            string income = "$" + to_string(economyHistory[i].yearlyIncome);
            string expenses = "$" + to_string(economyHistory[i].yearlyExpenses);
            string commercial = to_string(economyHistory[i].commercialCount);
            string industrial = to_string(economyHistory[i].industrialCount);
            
            // Color code income/expenses based on positive/negative net
            string incomeColor = (economyHistory[i].yearlyIncome > economyHistory[i].yearlyExpenses) ? GREEN : "";
            string expenseColor = (economyHistory[i].yearlyExpenses > economyHistory[i].yearlyIncome) ? RED : "";
            
            // Display the row with aligned columns
            std::cout << "\033[" << row << ";" << menuCol << "H"
                      << setw(yearWidth) << left << year << " | "
                      << setw(fundsWidth) << left << funds << " | "
                      << incomeColor << setw(incomeWidth) << left << income << RESET << " | "
                      << expenseColor << setw(expensesWidth) << left << expenses << RESET << " | "
                      << setw(comWidth) << left << commercial << " | "
                      << setw(indWidth) << left << industrial;
            
            row++;
            
            // Limit display to 5 rows to prevent overflow
            if (row > displayRow + 9) break;
        }
    }
    
    // Display economic summary
    if (!economyHistory.empty()) {
        int latest = economyHistory.size() - 1;
        int growth = 0;
        if (economyHistory.size() > 1) {
            growth = economyHistory[latest].funds - economyHistory[latest-1].funds;
        }
        
        std::cout << "\033[" << (row + 1) << ";" << menuCol << "H" << BOLD << "SUMMARY:" << RESET;
        std::cout << "\033[" << (row + 2) << ";" << menuCol << "H" 
                  << setw(20) << left << "Current Funds:" << "$" << economyHistory[latest].funds;
        std::cout << "\033[" << (row + 3) << ";" << menuCol << "H" 
                  << setw(20) << left << "Last Year Growth:" << (growth >= 0 ? GREEN : RED) 
                  << (growth >= 0 ? "+" : "") << growth << RESET;
        std::cout << "\033[" << (row + 4) << ";" << menuCol << "H"
                  << setw(20) << left << "Com:Ind Ratio:" 
                  << economyHistory[latest].commercialCount << ":" 
                  << economyHistory[latest].industrialCount;
    }
    
    
    // Ensure output is flushed to display
    std::cout.flush();
    
    // Wait for key press
    _getch();
    
    // Redraw the main interface
    clear_MenuArea();
    draw_Menu();
    draw_Stats();
}

// Function to display Environment Statistics
void display_EnvironmentStats() {
    const int statsWidth = 70;
    
    // Clear any previous messages and menus
    clear_MenuArea();
    display_MessageBar("", "");
    
    // Calculate display row to ensure it's visible
    const int displayRow = menuRow + 3;
    
    // Display title
    std::cout << "\033[" << displayRow << ";" << menuCol << "H" << BOLD << B_GREEN << "ENVIRONMENT STATISTICS" << RESET;
    
    // Display record count
    std::cout << "\033[" << (displayRow + 1) << ";" << menuCol << "H" 
              << "Records: " << environmentHistory.size();
    
    // Define column widths for consistent alignment
    const int yearWidth = 6;
    const int pollutionWidth = 10;
    const int tempWidth = 8;
    const int forestWidth = 8;
    const int seaLevelWidth = 10;
    
    // Display header with aligned columns
    std::cout << "\033[" << (displayRow + 2) << ";" << menuCol << "H" 
              << BOLD << setw(yearWidth) << left << "Year" << RESET << " | " 
              << BOLD << setw(pollutionWidth) << left << "Pollution" << RESET << " | " 
              << BOLD << setw(tempWidth) << left << "Temp." << RESET << " | " 
              << BOLD << setw(forestWidth) << left << "Forest" << RESET << " | " 
              << BOLD << setw(seaLevelWidth) << left << "Sea Level" << RESET;
    
    // Display horizontal line
    std::cout << "\033[" << (displayRow + 3) << ";" << menuCol << "H" << string(statsWidth, '-');
    
    // Display historical data (most recent at the top)
    int row = displayRow + 4;
    if (environmentHistory.empty()) {
        std::cout << "\033[" << row << ";" << menuCol << "H" << "No environmental data available yet.";
        row++;
    } else {
        for (int i = environmentHistory.size() - 1; i >= 0; i--) {
            // Format each data point
            string year = to_string(environmentHistory[i].year);
            string pollutionStr = to_string(environmentHistory[i].pollution) + "%";
            string tempStr = to_string(environmentHistory[i].temperature) + "°C";
            string forestStr = to_string(environmentHistory[i].forestCoverage) + "%";
            string seaLevelStr = to_string(environmentHistory[i].seaLevel) + "%";
            
            // Color code based on severity
            string pollutionColor = (environmentHistory[i].pollution > 70) ? RED : 
                                   (environmentHistory[i].pollution > 40) ? YELLOW : GREEN;
            string tempColor = (environmentHistory[i].temperature > 30) ? RED : 
                              (environmentHistory[i].temperature > 25) ? YELLOW : GREEN;
            string forestColor = (environmentHistory[i].forestCoverage < 30) ? RED : 
                                (environmentHistory[i].forestCoverage < 50) ? YELLOW : GREEN;
            string seaLevelColor = (environmentHistory[i].seaLevel > 50) ? RED : 
                                  (environmentHistory[i].seaLevel > 30) ? YELLOW : GREEN;
            
            // Display the row with aligned columns
            std::cout << "\033[" << row << ";" << menuCol << "H"
                      << setw(yearWidth) << left << year << " | "
                      << pollutionColor << setw(pollutionWidth) << left << pollutionStr << RESET << " | "
                      << tempColor << setw(tempWidth) << left << tempStr << RESET << " | "
                      << forestColor << setw(forestWidth) << left << forestStr << RESET << " | "
                      << seaLevelColor << setw(seaLevelWidth) << left << seaLevelStr << RESET;
            
            row++;
            
            // Limit display to 5 rows to prevent overflow
            if (row > displayRow + 9) break;
        }
    }
    
    // Display environmental risk assessment
    if (!environmentHistory.empty()) {
        int latest = environmentHistory.size() - 1;
        
        // Calculate environmental risk score (0-100)
        int riskScore = 
            (environmentHistory[latest].pollution * 0.4) + 
            ((environmentHistory[latest].temperature - 20) * 5) + 
            ((100 - environmentHistory[latest].forestCoverage) * 0.3) +
            (environmentHistory[latest].seaLevel * 0.3);
        
        riskScore = max(0, min(100, riskScore));
        
        string riskCategory;
        string riskColor;
        
        if (riskScore < 30) {
            riskCategory = "Low Risk";
            riskColor = GREEN;
        } else if (riskScore < 60) {
            riskCategory = "Moderate Risk";
            riskColor = YELLOW;
        } else if (riskScore < 85) {
            riskCategory = "High Risk";
            riskColor = MAGENTA;
        } else {
            riskCategory = "Critical Risk";
            riskColor = RED;
        }
        
        std::cout << "\033[" << (row + 1) << ";" << menuCol << "H" << BOLD << "ENVIRONMENTAL ASSESSMENT:" << RESET;
        std::cout << "\033[" << (row + 2) << ";" << menuCol << "H" 
                  << setw(20) << left << "Risk Score:" << riskColor << riskScore << "/100" << RESET;
        std::cout << "\033[" << (row + 3) << ";" << menuCol << "H" 
                  << setw(20) << left << "Status:" << riskColor << riskCategory << RESET;
        
        // Add specific warnings
        if (environmentHistory[latest].pollution > 70) {
            std::cout << "\033[" << (row + 4) << ";" << menuCol << "H" 
                      << RED << "⚠ High pollution levels causing health issues" << RESET;
        }
        if (environmentHistory[latest].temperature > 30) {
            std::cout << "\033[" << (row + 5) << ";" << menuCol << "H" 
                      << RED << "⚠ Rising temperatures causing sea level increase" << RESET;
        }
        if (environmentHistory[latest].forestCoverage < 30) {
            std::cout << "\033[" << (row + 6) << ";" << menuCol << "H" 
                      << RED << "⚠ Low forest coverage reducing quality of life" << RESET;
        }
    }
    
    
    // Ensure output is flushed to display
    std::cout.flush();
    
    // Wait for key press
    _getch();
    
    // Redraw the main interface
    clear_MenuArea();
    draw_Menu();
    draw_Stats();
}

// Function to display Population Statistics
void display_PopulationStats() {
    const int statsWidth = 70;
    
    // Clear any previous messages and menus
    clear_MenuArea();
    display_MessageBar("", "");
    
    // Calculate display row to ensure it's visible
    const int displayRow = menuRow + 3;
    
    // Display title
    std::cout << "\033[" << displayRow << ";" << menuCol << "H" << BOLD << B_BLUE << "POPULATION STATISTICS" << RESET;
    
    // Display record count
    std::cout << "\033[" << (displayRow + 1) << ";" << menuCol << "H" 
              << "Records: " << populationHistory.size();
    
    // Define column widths for consistent alignment
    const int yearWidth = 6;
    const int popWidth = 12;
    const int happinessWidth = 10;
    const int housingWidth = 8;
    const int powerWidth = 10;
    
    // Display header with aligned columns
    std::cout << "\033[" << (displayRow + 2) << ";" << menuCol << "H" 
              << BOLD << setw(yearWidth) << left << "Year" << RESET << " | " 
              << BOLD << setw(popWidth) << left << "Population" << RESET << " | " 
              << BOLD << setw(happinessWidth) << left << "Happiness" << RESET << " | " 
              << BOLD << setw(housingWidth) << left << "Housing" << RESET << " | " 
              << BOLD << setw(powerWidth) << left << "Power Use" << RESET;
    
    // Display horizontal line
    std::cout << "\033[" << (displayRow + 3) << ";" << menuCol << "H" << string(statsWidth, '-');
    
    // Display historical data (most recent at the top)
    int row = displayRow + 4;
    if (populationHistory.empty()) {
        std::cout << "\033[" << row << ";" << menuCol << "H" << "No population data available yet.";
        row++;
    } else {
        for (int i = populationHistory.size() - 1; i >= 0; i--) {
            // Format each data point
            string year = to_string(populationHistory[i].year);
            string popStr = to_string(populationHistory[i].population);
            string happinessStr = to_string(populationHistory[i].happiness) + "%";
            string housingStr = to_string(populationHistory[i].residentialCount);
            string powerStr = to_string(populationHistory[i].powerConsumption) + " MW";
            
            // Color code based on metrics
            string happinessColor = (populationHistory[i].happiness > 75) ? GREEN : 
                                   (populationHistory[i].happiness > 50) ? YELLOW : RED;
            
            // Housing capacity (each house = 50 people)
            int housingCapacity = populationHistory[i].residentialCount * 50;
            string housingColor = (populationHistory[i].population > housingCapacity) ? RED : GREEN;
            
            // Display the row with aligned columns
            std::cout << "\033[" << row << ";" << menuCol << "H"
                      << setw(yearWidth) << left << year << " | "
                      << setw(popWidth) << left << popStr << " | "
                      << happinessColor << setw(happinessWidth) << left << happinessStr << RESET << " | "
                      << housingColor << setw(housingWidth) << left << housingStr << RESET << " | "
                      << setw(powerWidth) << left << powerStr;
            
            row++;
            
            // Limit display to 5 rows to prevent overflow
            if (row > displayRow + 9) break;
        }
    }
    
    // Display population trends and statistics
    if (!populationHistory.empty() && populationHistory.size() > 1) {
        int latest = populationHistory.size() - 1;
        int popGrowth = populationHistory[latest].population - populationHistory[latest-1].population;
        int popGrowthPercent = (populationHistory[latest-1].population > 0) ? 
                             (popGrowth * 100) / populationHistory[latest-1].population : 0;
        
        // Housing capacity
        int housingCapacity = populationHistory[latest].residentialCount * 50;
        int housingUtilization = (housingCapacity > 0) ? 
                               (populationHistory[latest].population * 100) / housingCapacity : 100;
        
        int powerPlants = 0;
        
        int estimatedPowerPlants = (populationHistory[latest].powerConsumption + 199) / 200;
        // Or use a fixed ratio based on population (1 plant per 1000 people)
        powerPlants = max(1, estimatedPowerPlants);
        
        int powerCapacity = powerPlants * 200;
        int powerUtilization = (powerCapacity > 0) ?
                             (populationHistory[latest].powerConsumption * 100) / powerCapacity : 100;
        
        std::cout << "\033[" << (row + 1) << ";" << menuCol << "H" << BOLD << "POPULATION TRENDS:" << RESET;
        std::cout << "\033[" << (row + 2) << ";" << menuCol << "H" 
                  << setw(20) << left << "Current Population:" << populationHistory[latest].population;
        std::cout << "\033[" << (row + 3) << ";" << menuCol << "H" 
                  << setw(20) << left << "Growth:" << (popGrowth >= 0 ? "+" : "") << popGrowth 
                  << " (" << (popGrowthPercent >= 0 ? "+" : "") << popGrowthPercent << "%)";
        
        // Housing assessment
        std::cout << "\033[" << (row + 4) << ";" << menuCol << "H"
                  << setw(20) << left << "Housing Capacity:" << housingCapacity 
                  << " - Utilization: " << (housingUtilization > 100 ? RED : GREEN) 
                  << housingUtilization << "%" << RESET;
        
        // Power assessment
        std::cout << "\033[" << (row + 5) << ";" << menuCol << "H"
                  << setw(20) << left << "Power Capacity:" << powerCapacity << " MW"
                  << " - Utilization: " << (powerUtilization > 90 ? RED : GREEN) 
                  << powerUtilization << "%" << RESET;
        
        // Happiness assessment
        std::cout << "\033[" << (row + 6) << ";" << menuCol << "H"
                  << setw(20) << left << "Happiness Level:"
                  << (populationHistory[latest].happiness > 75 ? GREEN : 
                     populationHistory[latest].happiness > 50 ? YELLOW : RED)
                  << populationHistory[latest].happiness << "%" << RESET;
    } else if (!populationHistory.empty()) {
        // Special case for just one record
        int latest = populationHistory.size() - 1;
        
        std::cout << "\033[" << (row + 1) << ";" << menuCol << "H" << BOLD << "POPULATION SUMMARY:" << RESET;
        std::cout << "\033[" << (row + 2) << ";" << menuCol << "H" 
                  << setw(20) << left << "Current Population:" << populationHistory[latest].population;
    }
    
    // Add instructions to return
    
    // Ensure output is flushed to display
    std::cout.flush();
    
    // Wait for key press
    _getch();
    
    // Redraw the main interface
    clear_MenuArea();
    draw_Menu();
    draw_Stats();
}

void printWelcomeBanner(int spacing = 0) {
    string fg = custom_Colour(255, 165, 90);    // Soft warm orange     // Deep slate blue
    string reset = "\033[0m";

    string space(spacing, ' ');

    // Apply background only to the ASCII part
    cout << fg;
    cout << space  << R"(                                                                                                             
                                                                  ,--,                                         
    ,----..                                                     ,--.'|                                         
   /   /   \                                                 ,--,  | :                                         
  |   :     :   __  ,-.                       ,---,       ,---.'|  : '                                  ,---,  
  .   |  ;. / ,' ,'/ /|                   ,-+-. /  |      |   | : _' |               .---.          ,-+-. /  | 
  .   ; /--`  '  | |' | ,---.     ,---.  ,--.'|'   |      :   : |.'  |  ,--.--.    /.  ./|  ,---.  ,--.'|'   | 
  ;   | ;  __ |  |   ,'/     \   /     \|   |  ,"' |      |   ' '  ; : /       \ .-' . ' | /     \|   |  ,"' | 
  |   : |.' .''  :  / /    /  | /    /  |   | /  | |      '   |  .'. |.--.  .-. /___/ \: |/    /  |   | /  | | 
  .   | '_.' :|  | ' .    ' / |.    ' / |   | |  | |      |   | :  | ' \__\/: . .   \  ' .    ' / |   | |  | | 
  '   ; : \  |;  : | '   ;   /|'   ;   /|   | |  |/       '   : |  : ; ," .--.; |\   \   '   ;   /|   | |  |/  
  '   | '/  .'|  , ; '   |  / |'   |  / |   | |--'        |   | '  ,/ /  /  ,.  | \   \  '   |  / |   | |--'   
  |   :    /   ---'  |   :    ||   :    |   |/            ;   : ;--' ;  :   .'   \ \   \ |   :    |   |/       
   \   \ .'           \   \  /  \   \  /'---'             |   ,/     |  ,     .-./  '---" \   \  /'---'        
    `---`              `----'    `----'                   '---'       `--`---'             `----'              
                                                                                                       
)" << reset << endl;

    // Footer text with no background color
    cout << fg;
    cout << space << "                  Welcome to Green Haven!\n";
    cout << space << "       You're the proud mayor of an island paradise...\n";
    cout << space << "  Too bad it's one high tide away from becoming Atlantis 2.0!\n";
    cout << space << "Build wind turbines, plant forests, compost citizen complaints,\n";
    cout << space << "       and fight climate chaos one eco-policy at a time.\n";
    cout << space << "            Remember: floods don't take bribes. \n\n";
    cout << BOLD << space << "                  PRESS ANY KEY TO START";
    cout << reset;
}

int main()
{
    system(""); // For proper working of ANSI codes (Text Style and Colours)

    
    ::emoji_Support(); // For making Emojis work in CMD
    srand(time(0));    // Seed for Random Island Generation
    
    printWelcomeBanner(25);

    cin.get();
    system("cls");
    ::emoji_Support();

    // Apply customization settings
    ::applyCustomSettings();

    tile map[map_height][map_width];
    ::generate_Map(map);
    ::clear_Screen();

    ::display_Map(map);

    menus = {
        {"BUILD", {"Road", "Residential Building", "Commercial Building", "Industrial Building", "Power Plant", "Remove Building"}, {[&map]()
                                                                                                                                      { enterBuildMode("Road"); }, [&map]()
                                                                                                                                      { enterBuildMode("Residential"); }, [&map]()
                                                                                                                                      { enterBuildMode("Commercial"); }, [&map]()
                                                                                                                                      { enterBuildMode("Industrial"); }, [&map]()
                                                                                                                                      { enterBuildMode("Power"); }, [&map]()
                                                                                                                                      { enterRemoveMode(); }}},
        {"DEVELOP", {"Plant Forest", "School", "Hospital", "Police Station", "Amusement Park", "Remove Building"}, {[&map]()
                                                                                                                     { enterBuildMode("Forest"); }, [&map]()
                                                                                                                     { enterBuildMode("Education"); }, [&map]()
                                                                                                                     { enterBuildMode("Hospital"); }, [&map]()
                                                                                                                     { enterBuildMode("Police"); }, [&map]()
                                                                                                                     { enterBuildMode("Entertainment"); }, [&map]()
                                                                                                                     { enterRemoveMode(); }}},
        {"MANAGE", {"Advance Year", "Citizen Demands", "Complaints"}, {[&map]()
                                                                        { advanceYear(map); }, []()
                                                                        { draw_Message("Reviewing citizen demands..."); }, []()
                                                                        { draw_Message("Checking citizen complaints..."); }}},
        {"UPGRADES", {"Residential Upgrade I", "Commercial Upgrade I", "Industrial Upgrade I", "Development Upgrade I", "Power Upgrade I"}, {
            []() { purchaseUpgrade(0); }, // Residential I
            []() { purchaseUpgrade(2); }, // Commercial I
            []() { purchaseUpgrade(4); }, // Industrial I
            []() { purchaseUpgrade(6); }, // Development I
            []() { purchaseUpgrade(8); }, // Power I
        }},
        {"R&D", {"Smart City", "Green City", "Sustainable City"}, {
            []() { startResearch(0); }, // Smart City
            []() { startResearch(1); }, // Green City
            []() { startResearch(2); }, // Sustainable City
        }},
        {"STATS", {"Economy", "Environment", "Population"}, {
            []() { display_EconomyStats(); },
            []() { display_EnvironmentStats(); },
            []() { display_PopulationStats(); }
        }}};

    updateUpgradesMenu();
    rebuild_TabPositions();
    compute_UI_positions();

    draw_Menu();
    draw_Stats();

    enable_Tooltips();
    handle_Input(map);

    return 0;
}