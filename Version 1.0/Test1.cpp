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
        if (map[row][col].building) {
            const Stats& bStats = map[row][col].building->stats;
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
        case BuildingType::Road: buildingName = "Road"; break;
        case BuildingType::Residential: buildingName = "House"; break;
        case BuildingType::Commercial: buildingName = "Office"; break;
        case BuildingType::Industrial: buildingName = "Factory"; break;
        case BuildingType::Hospital: buildingName = "Hospital"; break;
        case BuildingType::School: buildingName = "School"; break;
        case BuildingType::Park: buildingName = "Park"; break;
        case BuildingType::Tree: buildingName = "Tree"; break;
        case BuildingType::SolarPlant: buildingName = "Solar Plant"; break;
        case BuildingType::WindTurbine: buildingName = "Wind Turbine"; break;
        case BuildingType::Airport: buildingName = "Airport"; break;
        case BuildingType::WaterTreatment: buildingName = "Water Treatment"; break;
        case BuildingType::RecyclingCenter: buildingName = "Recycling Center"; break;
        case BuildingType::FireStation: buildingName = "Fire Station"; break;
        case BuildingType::PoliceStation: buildingName = "Police Station"; break;
        default: buildingName = "Building"; break;
        }

        // Get building's stats to revert changes
        const Stats& bStats = map[row][col].building->stats;
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
        if (happiness < 30) happinessColor = RED;
        else if (happiness < 70) happinessColor = YELLOW;
        else happinessColor = GREEN;
        
        cout << happinessColor << "[";
        int bars = happiness / 10;
        for (int i = 0; i < 10; i++) {
            if (i < bars) cout << "■";
            else cout << "□";
        }
        cout << "] " << happiness << "%" << RESET;
        
        row++;
        
        // Show different stats based on display mode
        if (statsDisplayMode == 1) { // Environmental stats
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
        else if (statsDisplayMode == 2) { // Social stats
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
        else if (statsDisplayMode == 3) { // Building stats
            cout << "\033[" << row++ << ";" << col << "H" 
                 << BOLD << theme_Color(2) << "-- Building Stats --" << RESET;
            
            cout << "\033[" << row++ << ";" << col << "H" 
                 << theme_Color(1) << "Total Buildings: " << RESET << totalBuildings;
            
            cout << "\033[" << row++ << ";" << col << "H" 
                 << theme_Color(1) << "Maintenance: " << RESET << "$" << maintenance;
            
            // Count each building type
            int residential = 0, commercial = 0, industrial = 0;
            for (int i = 0; i < map_height; i++) {
                for (int j = 0; j < map_width; j++) {
                    if (map[i][j].building) {
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
            if (selectedItem >= 0 && selectedItem < items.size()) {
                string desc = items[selectedItem].description;
                if (!desc.empty()) {
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
    buildMenu.addItem("Road", [](){ city->currentTool = BuildingType::Road; }, 
                    BuildingType::Road, "Build roads to connect buildings");
    buildMenu.addItem("Residential", [](){ city->currentTool = BuildingType::Residential; }, 
                    BuildingType::Residential, "Build housing for your citizens");
    buildMenu.addItem("Commercial", [](){ city->currentTool = BuildingType::Commercial; }, 
                    BuildingType::Commercial, "Build offices and shops");
    buildMenu.addItem("Industrial", [](){ city->currentTool = BuildingType::Industrial; }, 
                    BuildingType::Industrial, "Build factories for jobs");
    buildMenu.addItem("Hospital", [](){ city->currentTool = BuildingType::Hospital; }, 
                    BuildingType::Hospital, "Improve city health");
    buildMenu.addItem("School", [](){ city->currentTool = BuildingType::School; }, 
                    BuildingType::School, "Improve city education");
    buildMenu.addItem("Park", [](){ city->currentTool = BuildingType::Park; }, 
                    BuildingType::Park, "Increase happiness and greenery");
    buildMenu.addItem("Tree", [](){ city->currentTool = BuildingType::Tree; }, 
                    BuildingType::Tree, "Cheap way to improve environment");
    menus.push_back(buildMenu);

    // Eco menu
    Menu ecoMenu("Ecology");
    ecoMenu.addItem("Solar Plant", [](){ city->currentTool = BuildingType::SolarPlant; }, 
                  BuildingType::SolarPlant, "Clean energy source");
    ecoMenu.addItem("Wind Turbine", [](){ city->currentTool = BuildingType::WindTurbine; }, 
                  BuildingType::WindTurbine, "Efficient renewable energy");
    ecoMenu.addItem("Water Plant", [](){ city->currentTool = BuildingType::WaterTreatment; }, 
                  BuildingType::WaterTreatment, "Clean water and reduce pollution");
    ecoMenu.addItem("Recycling", [](){ city->currentTool = BuildingType::RecyclingCenter; }, 
                  BuildingType::RecyclingCenter, "Reduce waste and pollution");
    menus.push_back(ecoMenu);

    // Services menu
    Menu servicesMenu("Services");
    servicesMenu.addItem("Fire Station", [](){ city->currentTool = BuildingType::FireStation; }, 
                        BuildingType::FireStation, "Improve city safety");
    servicesMenu.addItem("Police", [](){ city->currentTool = BuildingType::PoliceStation; }, 
                        BuildingType::PoliceStation, "Reduce crime and improve safety");
    menus.push_back(servicesMenu);

    // Tools menu
    Menu toolsMenu("Tools");
    toolsMenu.addItem("Select", [](){ city->currentTool = BuildingType::None; },
                     BuildingType::None, "Select and view buildings");
    toolsMenu.addItem("Demolish", [](){ 
        if (city->hasSelection) {
            city->removeBuilding(city->selectedRow, city->selectedCol);
            city->hasSelection = false;
        } else {
            draw_Message("First select a building to demolish");
        }
    }, BuildingType::None, "Remove existing buildings");
    toolsMenu.addItem("Toggle Stats", [](){ 
        city->statsDisplayMode = (city->statsDisplayMode % 3) + 1; 
        city->draw_Stats();
    }, BuildingType::None, "Cycle through different stat views");
    menus.push_back(toolsMenu);

    // Options menu
    Menu optionsMenu("Options");
    optionsMenu.addItem("Save Game", [](){
        save_Config("ecocity.cfg");
        draw_Message("Game settings saved to ecocity.cfg");
    }, BuildingType::None, "Save game settings");
    optionsMenu.addItem("Load Game", [](){
        if (load_Config("ecocity.cfg")) {
            draw_Message("Game settings loaded from ecocity.cfg");
        } else {
            draw_Message("Could not load settings");
        }
    }, BuildingType::None, "Load saved game settings");
    optionsMenu.addItem("Toggle Sound", [](){
        config.enableSounds = !config.enableSounds;
        draw_Message(config.enableSounds ? "Sound enabled" : "Sound disabled");
    }, BuildingType::None, "Turn game sounds on/off");
    optionsMenu.addItem("Change Theme", [](){
        config.uiTheme = (config.uiTheme + 1) % 4;
        draw_Message("Theme changed to " + to_string(config.uiTheme));
    }, BuildingType::None, "Cycle through UI themes");
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
            if (bt != BuildingType::None) {
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
            if (menus[activeMenu].selectedItem != itemIndex) {
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
    
    statsRow = 2; // Start stats at top
    statsCol = leftOffset + (map_width * tileWidth) + 10; // Right of map
    
    maxDropdownItems = 10; // Max items to show in dropdown
    
    tooltipRow = menuRow + maxDropdownItems + 2; // Below dropdowns
    messageRow = tooltipRow + 2; // Below tooltip
    
    logRow = messageRow + 2; // Below message
    logCol = leftOffset;
}

// Save/Load configuration
void save_Config(const string &filename)
{
    ofstream file(filename);
    if (!file) {
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
    if (!file) {
        draw_Message("Error: Could not open file for loading");
        return false;
    }
    
    string line;
    
    if (getline(file, line)) config.monthLengthSeconds = stoi(line);
    if (getline(file, line)) config.startingMoney = stoi(line);
    if (getline(file, line)) config.startingPopulation = stoi(line);
    if (getline(file, line)) config.leftOffset = stoi(line);
    if (getline(file, line)) config.mapWidth = stoi(line);
    if (getline(file, line)) config.mapHeight = stoi(line);
    if (getline(file, line)) config.tileWidth = stoi(line);
    if (getline(file, line)) config.menuSpacing = stoi(line);
    if (getline(file, line)) config.enableSounds = stoi(line);
    if (getline(file, line)) config.uiTheme = stoi(line);
    if (getline(file, line)) config.fontHeight = stoi(line);
    if (getline(file, line)) config.fontName = line;
    
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
    
    try {
        col = stoi(input.substr(1)) - 1;
    }
    catch (...) {
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
        if (input == "quit" || input == "exit") {
            running = false;
        }
        else if (input == "help") {
            draw_Message("Commands: quit, help, build <type> <coords>, demolish <coords>, stats");
        }
        else if (input == "stats") {
            city->statsDisplayMode = (city->statsDisplayMode % 3) + 1;
            city->draw_Stats();
        }
        else if (input.substr(0, 5) == "build" && input.length() > 6) {
            // Parse build command: build <type> <coords>
            stringstream ss(input.substr(6));
            string type, coords;
            ss >> type >> coords;
            
            int row, col;
            if (parse_Coords(coords, row, col)) {
                BuildingType bt = BuildingType::None;
                
                if (type == "road") bt = BuildingType::Road;
                else if (type == "house" || type == "residential") bt = BuildingType::Residential;
                else if (type == "office" || type == "commercial") bt = BuildingType::Commercial;
                else if (type == "factory" || type == "industrial") bt = BuildingType::Industrial;
                else if (type == "hospital") bt = BuildingType::Hospital;
                else if (type == "school") bt = BuildingType::School;
                else if (type == "park") bt = BuildingType::Park;
                else if (type == "tree") bt = BuildingType::Tree;
                else if (type == "solar") bt = BuildingType::SolarPlant;
                else if (type == "wind") bt = BuildingType::WindTurbine;
                else if (type == "water") bt = BuildingType::WaterTreatment;
                else if (type == "recycle") bt = BuildingType::RecyclingCenter;
                else if (type == "fire") bt = BuildingType::FireStation;
                else if (type == "police") bt = BuildingType::PoliceStation;
                
                if (bt != BuildingType::None) {
                    if (city->placeBuilding(row, col, bt)) {
                        draw_Message("Building constructed at " + coords);
                    }
                } else {
                    draw_Message("Unknown building type: " + type);
                }
            } else {
                draw_Message("Invalid coordinates: " + coords);
            }
        }
        else if (input.substr(0, 8) == "demolish" && input.length() > 9) {
            // Parse demolish command: demolish <coords>
            string coords = input.substr(9);
            
            int row, col;
            if (parse_Coords(coords, row, col)) {
                if (city->removeBuilding(row, col)) {
                    draw_Message("Building demolished at " + coords);
                } else {
                    draw_Message("No building at " + coords);
                }
            } else {
                draw_Message("Invalid coordinates: " + coords);
            }
        }
        else {
            // Check if input is coordinates
            int row, col;
            if (parse_Coords(input, row, col)) {
                if (city->currentTool == BuildingType::None) {
                    // Select the tile
                    city->selectedRow = row;
                    city->selectedCol = col;
                    city->hasSelection = true;
                    
                    // Show info about the tile
                    if (city->map[row][col].building) {
                        string buildingName;
                        switch (city->map[row][col].building->type) {
                            case BuildingType::Road: buildingName = "Road"; break;
                            case BuildingType::Residential: buildingName = "House"; break;
                            case BuildingType::Commercial: buildingName = "Office"; break;
                            case BuildingType::Industrial: buildingName = "Factory"; break;
                            case BuildingType::Hospital: buildingName = "Hospital"; break;
                            case BuildingType::School: buildingName = "School"; break;
                            case BuildingType::Park: buildingName = "Park"; break;
                            case BuildingType::Tree: buildingName = "Tree"; break;
                            case BuildingType::SolarPlant: buildingName = "Solar Plant"; break;
                            case BuildingType::WindTurbine: buildingName = "Wind Turbine"; break;
                            case BuildingType::Airport: buildingName = "Airport"; break;
                            case BuildingType::WaterTreatment: buildingName = "Water Treatment"; break;
                            case BuildingType::RecyclingCenter: buildingName = "Recycling Center"; break;
                            case BuildingType::FireStation: buildingName = "Fire Station"; break;
                            case BuildingType::PoliceStation: buildingName = "Police Station"; break;
                            default: buildingName = "Unknown Building"; break;
                        }
                        draw_Message(buildingName + " at " + input);
                    } else {
                        draw_Message(city->map[row][col].type + " at " + input);
                    }
                } else {
                    // Build with current tool
                    if (city->placeBuil