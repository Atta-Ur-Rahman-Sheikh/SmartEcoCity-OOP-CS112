#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <cmath>
#include <ctime>
#include <windows.h>
#include <conio.h>
#include <iomanip>

using namespace std;

// Forward declarations
class Building;
class TimeManager;
class City;

// Map dimensions
typedef int TileID;
static const int MAP_WIDTH = 42;
static const int MAP_HEIGHT = 16;

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
    wchar_t icon;
    bool removable;

    Building(BuildingType t, const Stats &s, wchar_t ic, bool rem = true)
        : type(t), stats(s), icon(ic), removable(rem) {}

    virtual ~Building() {}
};

// Concrete buildings
class Road : public Building
{
public:
    Road() : Building(BuildingType::Road, {10, 1, 0, 0, 1, 0, 0, 0}, L'═') {}
};

class Residential : public Building
{
public:
    Residential() : Building(BuildingType::Residential, {100, 5, 4, 2, 1, 0, 0, 0}, L'🏠') {}
};

class Commercial : public Building
{
public:
    Commercial() : Building(BuildingType::Commercial, {150, 10, 2, 3, 2, 0, 0, 0}, L'🏢') {}
};

class Industrial : public Building
{
public:
    Industrial() : Building(BuildingType::Industrial, {200, 15, 1, -2, 5, 0, 0, 0}, L'🏭') {}
};

class Park : public Building
{
public:
    Park() : Building(BuildingType::Park, {50, 2, 0, 5, -1, 5, 1, 0}, L'🌳') {}
};

class Tree : public Building
{
public:
    Tree() : Building(BuildingType::Tree, {10, 0, 0, 1, -1, 3, 1, 0}, L'🌲') {}
};

class Hospital : public Building
{
public:
    Hospital() : Building(BuildingType::Hospital, {300, 20, 0, 4, 0, 0, 2, 0}, L'🏥') {}
};

class School : public Building
{
public:
    School() : Building(BuildingType::School, {250, 15, 0, 3, 0, 0, 2, 0}, L'🏫') {}
};

class SolarPlant : public Building
{
public:
    SolarPlant() : Building(BuildingType::SolarPlant, {400, 5, 0, 0, 0, 0, 3, 5}, L'🔆') {}
};

class WindTurbine : public Building
{
public:
    WindTurbine() : Building(BuildingType::WindTurbine, {500, 5, 0, 0, 0, 0, 3, 8}, L'🌬️') {}
};

class Airport : public Building
{
public:
    Airport() : Building(BuildingType::Airport, {0, 0, 0, 0, 0, 0, 0, 0}, L'✈️', false) {}
};

// Tile struct to hold building and terrain
struct Tile
{
    bool isLand;
    unique_ptr<Building> building;
    bool highlight;
};

// Game Map
typedef Tile Map[MAP_HEIGHT][MAP_WIDTH];

// Utility functions to check placement
bool canPlace(const Map &map, int r, int c, BuildingType type);

// Time Manager for months
class TimeManager
{
public:
    int monthLengthSec;
    int currentMonth;
    int currentYear;
    time_t lastTick;
    function<void()> onMonthChange;

    TimeManager(int secs) : monthLengthSec(secs), currentMonth(1), currentYear(2025)
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
};

// City stats structure
struct CityStats
{
    int cash = 10000;
    int population = 100;
    int ecoPoints = 0;
    int pollution = 0;
    int greenLevel = 0;
    int energy = 0;
};

// Main Game class
class City
{
public:
    Map grid;
    CityStats stats;
    TimeManager timer;

    City(int monthSec = 5)
        : timer(monthSec)
    {
        // Initialize map
        for (int i = 0; i < MAP_HEIGHT; i++)
            for (int j = 0; j < MAP_WIDTH; j++)
            {
                grid[i][j].isLand = true; // placeholder: call existing generate_Map
                grid[i][j].highlight = false;
            }
        // Place central airport
        int cx = MAP_HEIGHT / 2, cy = MAP_WIDTH / 2;
        grid[cx][cy].building = make_unique<Airport>();
        timer.onMonthChange = [&]()
        { this->tickMonth(); };
    }
    void tickMonth()
    {
        // Update economy, population, eco points, pollution...
    }
    void placeBuilding(int row, int col, BuildingType type)
    {
        if (canPlace(grid, row, col, type))
        {
            switch (type)
            {
            case BuildingType::Road:
                grid[row][col].building = make_unique<Road>();
                break;
            case BuildingType::Residential:
                grid[row][col].building = make_unique<Residential>();
                break;
                // ... other cases
            }
        }
    }
    void removeBuilding(int row, int col)
    {
        if (grid[row][col].building && grid[row][col].building->removable)
            grid[row][col].building.reset();
    }
};

// Entry point
int main()
{
    City city(10); // each month = 10 seconds
    // Setup UI, mouse handlers, menu actions: call city.placeBuilding, city.removeBuilding, etc.
    return 0;
}
