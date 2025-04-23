#include <iostream>
#include <string>
using namespace std;

class cityObjects{
    int coordinates;
    int objectCount;
};
class Building : public cityObjects{
    string type;
    int floors= 1;
};

class Residential : public Building{
    int capacity;
    int energyRequirment;
};
