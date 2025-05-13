#include <iostream>
#include <string>
using namespace std;

// ANSI escape code generators for foreground and background colors
string setForeground(int r, int g, int b) {
    return "\033[38;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m";
}

string setBackground(int r, int g, int b) {
    return "\033[48;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m";
}

// Function to print the welcome banner with spacing and theme
void printWelcomeBanner(int spacing = 0) {
    string fg = setForeground(255, 165, 90);    // Soft warm orange     // Deep slate blue
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
    cout << space << "Welcome to Green Haven!\n";
    cout << space << "You're the proud mayor of an island paradise...\n";
    cout << space << "Too bad it's one high tide away from becoming Atlantis 2.0!\n";
    cout << space << "Build wind turbines, plant forests, compost citizen complaints,\n";
    cout << space << "and fight climate chaos one eco-policy at a time.\n";
    cout << space << "Remember: floods don't take bribes. 🌊🏝️\n";
    cout << reset;
}

int main() {
    printWelcomeBanner(20); // Adjust spacing as desired
    return 0;
}