#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Input {
protected:
    template <typename T>
    T getValidInput(T minValue, T maxValue, const string& prompt) {
        T inputValue;
        while (true) {
            cout << prompt << " (" << minValue << " - " << maxValue << "): ";
            cin >> inputValue;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid input type. Please enter a valid number.\n";
            } else if (inputValue < minValue || inputValue > maxValue) {
                cout << "Out of range! Please enter a number between " << minValue << " and " << maxValue << ".\n";
            } else {
                cin.ignore(1000, '\n');
                return inputValue;
            }
        }
    }

    template <typename T>
    T displayMenu(int minOption, int maxOption, const string& prompt, string options[]) {
        for (int i = 0; i < maxOption; i++) {
            cout<< options[i] << "\n";
        }
        return getValidInput<int>(minOption, maxOption, prompt);
    }

    int extract_menu(const string& start, const string& end, string menu[]) {
        ifstream file("menu.txt");
        if (!file) {
            cerr << "Error opening the file." << endl;
            return 0;
        }

        string line;
        bool inSection = false;
        int j = 0;

        while (getline(file, line)) {
            if (line == start) {
                inSection = true;
                continue; // skip title line
            }
            if (inSection) {
                if (line == end  || line.empty()) {
                    break;
                }
                if (!line.empty() && isdigit(line[0])) {
                    menu[j++] = line;
                }
            }
        }
        return j; // number of items filled
    }

};

class Menu : public Input {
private:
    string menu[30];

public:
    void displaySubMenu(const string& start, const string& end) {
        int count = extract_menu(start, end, menu);
        if (count == 0) {
            cout << start << "\n";
            return;
        }
        int choice = displayMenu<int>(1, count, "Select an option", menu);
        system("cls");
    }

    void showBankAccountMenu() {
        bool condition = true;
        while (condition) {
            int mainCount = extract_menu("Main Menu:", "Bank Account:", menu);
            if (mainCount == 0) {
                cout << "Main Menu not found.\n";
                return;
            }

            int choice = displayMenu<int>(1, mainCount, "Main Menu", menu);
            system("cls");

            switch (choice) {
                case 1:  displaySubMenu("Bank Account:", "Choose a Profession to Earn Money:"); break;
                case 2:  displaySubMenu("Choose a Profession to Earn Money:", "Shop Essentials (Clothes, Groceries, Oil, etc.):"); break;
                case 3:  displaySubMenu("Shop Essentials (Clothes, Groceries, Oil, etc.):", "Play Games to Have Fun and Earn:"); break;
                case 4:  displaySubMenu("Play Games to Have Fun and Earn:", "Earn Through Competitions (Win to Earn Rewards):"); break;
                case 5:  displaySubMenu("Earn Through Competitions (Win to Earn Rewards):", "Earn Money by Helping Others:"); break;
                case 6:  displaySubMenu("Earn Money by Helping Others:", "Chat with Others:"); break;
                case 7:  displaySubMenu("Chat with Others:", "Construct Eco-Friendly Buildings:"); break;
                case 8:  displaySubMenu("Construct Eco-Friendly Buildings:", "Manage Resources (Water, Energy, etc.):"); break;
                case 9:  displaySubMenu("Manage Resources (Water, Energy, etc.):", "Simulate Traffic and Transportation:"); break;
                case 10: displaySubMenu("Simulate Traffic and Transportation:", "Environmental Policies and Regulations:"); break;
                case 11: displaySubMenu("Environmental Policies and Regulations:", "Simulate Climate Change Effects:"); break;
                case 12: displaySubMenu("Simulate Climate Change Effects:", "Generate EcoCity Reports:"); break;
                case 13: displaySubMenu("Generate EcoCity Reports:", ""); break;
                case 14:
                    cout << "Exiting program. Goodbye!\n";
                    condition = false;
                    break;
                default:
                    cout << "Invalid option.\n";
                    break;
            }
        }
    }
};
int main() {
    Menu menu;
    menu.showBankAccountMenu();
    return 0;
}

