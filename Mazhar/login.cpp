#include <iostream>
#include <string>
using namespace std;

class Input {
protected:
    string fullName;
    int age;
    string phoneNumber;
    string gender;
    string password;
    string genderOptions[4] = {"Male", "Female", "Transgender", "Other"};
    int Choice;

public:
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
            cout << (i + 1) << ". " << options[i] << "\n";
        }
        return getValidInput<int>(minOption, maxOption, prompt);
    }

    void collectUserData() {
        cout << "Enter your full name: ";
        getline(cin, fullName);
        cout << "Enter strong password: ";
        getline(cin, password);
        age = getValidInput<int>(1, 150, "Enter your age");
        cout << "Enter your phone number (e.g. +923001234567): +92";
        getline(cin, phoneNumber);

        int genderChoice = displayMenu<int>(1, 4, "Select your gender", genderOptions);
        gender = genderOptions[genderChoice - 1];
    }

    void displayData() {
        cout << "\n--- User Information ---\n";
        cout << "Name: " << fullName << "\n";
        cout << "Age: " << age << "\n";
        cout << "Phone Number: +92" << phoneNumber << "\n";
        cout << "Gender: " << gender << "\n";
    }
};

class Bank : public Input {
protected:
    long deposit = 0;
    long withdraw = 0;
    long loan = 0;
    long balance = 0;
    float tax = 0.0;
};

class MainMenu : public Bank {
public:
    void executeOption(int choice) {
        switch (choice) {
            case 1:
                cout << "Opening Bank Account Menu...\n";
                // Call bank account related function here
                break;
            case 2:
                cout << "Loading Profession Selection...\n";
                // Call profession selection logic here
                break;
            case 3:
                cout << "Redirecting to Shopping Essentials...\n";
                // Show shopping interface
                break;
            case 4:
                cout << "Starting Fun Games to Earn...\n";
                // Load game-based earning module
                break;
            case 5:
                cout << "Entering Competition Zone...\n";
                // Competition-based earning logic
                break;
            case 6:
                cout << "Finding Opportunities to Help Others and Earn...\n";
                // Helping others module
                break;
            case 7:
                cout << "Exiting... Thank you for using the system.\n";
                exit(0);
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    void showMainMenu() {
        string mainMenuOptions[7] = {
            "Bank Account",
            "Choose a Profession to Earn Money",
            "Shop Essentials (Clothes, Groceries, Oil, etc.)",
            "Play Games to Have Fun and Earn",
            "Earn Through Competitions (Win to Earn Rewards)",
            "Earn Money by Helping Others",
            "Exit"
        };

        int userChoice = displayMenu<int>(1, 7, "Select an option:", mainMenuOptions);
        executeOption(userChoice);
    }
};

int main() {
    MainMenu app;
    app.collectUserData();
    app.displayData();
    while (true) {
        app.showMainMenu();
    }
    return 0;
}

