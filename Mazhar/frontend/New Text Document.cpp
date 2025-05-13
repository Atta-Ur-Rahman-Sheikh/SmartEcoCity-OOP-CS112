#include <iostream>
#include <string>
using namespace std;


class User{
    private:
    string password;
    int balance = 1000;
    public:
        string username;
        string cityName;
        bool isNew = true;
        int level =0;
        User (){};
        User(string username, string password){
            this->username = username;
            this->password = password;
        }

        void setCityName(string name){
            cityName = name;
        }

        void updateBalance(int cost){
            this->balance -= cost;
        }
};

class City{
    private:
        User owner;
        int size = 11;
        int population = 100;
        int energyDemand = 100;
        int energySupply =  200;
        int pollutionScale =  0 ;
        int greenScale = 0;
        float aqi = 0;
    public:
        string name;
        string *grid = new string[size * size];

        City(User user, string cityName) : owner (user){
            name = cityName;
            for(int i =0 ; i<size;i++){
                int count =1;
                for(int j=0;j<size;j++){
                    if(i==0 && j==0){
                        *(grid + i * size + j) = " O";
                    }
                    else if (i ==0){
                            *(grid + i * size + j) = std::string("a") + to_string(i+count);
                            count++;
                    }
                    else if (j == 0 ){
                            *(grid + i * size + j) = std::string("b") + to_string(i+count -1);
                    }
                    else if (i!=0 && j!=0){
                        *(grid + i * size + j) = " E ";
                    }
                }
            }
            owner.setCityName(name);
        }

        void displayGrid(){
                for(int i =0 ; i<11;i++){
                    for(int j=0;j<11;j++){
                        cout<<"| " <<  this->grid[i * 11 + j] <<" |";
                    }
                    cout<<endl;
            }
        }
        
        int getSize(){
            return size;
        }

        void displayDetails(){
            cout<<"City Name: "<< this->owner.cityName << endl
                << "City Owner: " << this->owner.username << endl
                << "Population: " << population << endl
                <<"Green Scale: " << greenScale <<endl;
        }

        void upgradeCost(int cost){
            this->owner.updateBalance(cost);
        }

        void setOwner(User &user){
            cout<<"hello" << user.username;
        }

        void updateGrid(int x, int y){
            *(grid + x * size + y) = " R ";
            this->displayGrid();
        }

        void increasePopulation(int x){
            this->population += x;
            this->energyDemand += 20;
            displayDetails();
        }

        void updateCity(int x, int y, int populationIncrease, int cost){
            updateGrid(x, y);
            this->increasePopulation(populationIncrease);
        }
        
        string getCoordinateItem(int x, int y){
            string item  = *(grid + x * size + y);
            return item;
        }

    };

class cityObjects{
    private:
        int objectCount = 0;
        struct coordinates{
            int x;
            int y;
        };
        coordinates c1;
    public:
        cityObjects(City &city, int x, int y, int populationIncrease,  int cost){
            cout<<"City Name: "<<endl;
            c1.x = x;
            c1.y = y;
            objectCount++;
            city.updateCity(x, y, populationIncrease, cost);
        }
};

class Buildings : public cityObjects{
    private:
    public:
    int floors = 1;
        Buildings(City &city, int x, int y, int populationIncrease, int cost): cityObjects(city, x, y,populationIncrease, cost){}
        string type;
        virtual void setType(string type_ = "General"){
            type = type_;
        }

        virtual void upgradeBuilding(City &city, int cost){
            this->floors+=1;
            city.upgradeCost(cost);
        }

};

class Residential : public Buildings{
    private:
        int capacity = 100;
    public:
        Residential(City &city, int x, int y, int populationIncrease, int cost) : Buildings (city, x, y, populationIncrease, cost){}

        void setType(string type_) override{
            type = type_;
        }

        void upgradeBuilding(City &city, int cost) override{
            floors +=1;
            city.upgradeCost(cost);

        }
};

int getXCoordinate(int , string);
int getYCoordinate(int , string);
void newBuilding(City &);
void newResidentialBuilding(City &, string, string);
void upgradeBuilding(City &);

int main(){
    string userName, password;
    cout<<"Welcome!"<<endl;
    // cout<<"Enter User Name: ";
    // getline(cin, userName);
    // cout<<"Enter password: ";
    // cin>>password;
    User u1("Talal", "Talal123");
    // User u1(userName, password);
    City c1(u1, "Talal's City");
    c1.displayGrid();

    int option;
    cout<<"Select an option: "<<endl
        <<"1. Construct New Building"<<endl
        <<"2. Upgrade Existing Building"<<endl
        <<"Enter Option (1-2): ";
    cin>>option;
    switch (option)
    {
    case 1:
        newBuilding(c1);
        break;
    case 2:
        upgradeBuilding(c1);
    default:
        break;
    }
}

int getXCoordinate(int size, string x){
    for (int i =0 ; i<size; i++){
        int count =1;
        for(int j=0; j<size; j++){
            if (((std::string("a") + to_string(i + count) + std::string(" ")) == x) || 
                ((std::string("a") + to_string(i + count)) == x)){
                    return j;
            }
            count++;
        }
    }
}

int getYCoordinate(int size, string y){
    for (int i =0 ; i<size; i++){
        int count =1;
        for(int j=0; j<size; j++){
            if (((std::string("b") + to_string(i + count -1) + std::string(" ")) == y) || 
                ((std::string("b") + to_string(i + count -1)) == y)){
                    return j;
            }
            count++;
        }
    }
    return -1;
}

void newResidentialBuilding(City &city, string x , string y){
    int size = city.getSize();
    int cost =  100;
    int populationIncrease = 100;
    int X = getXCoordinate(size, x);
    int Y = getYCoordinate(size, y);

    Residential r1(city, X, Y ,  populationIncrease, cost);
    r1.setType("Residential");
}

void newBuilding(City &city){
    int option;
    string x, y;
    cout<<"\nSpecify Building Type: "<<endl
        <<"1. Residential Building"<<endl
        <<"Enter Option (1-2): ";
    cin>>option;
    cout<<"\nSpecify X-Coordinate(a1-a10): ";
    cin>>x;
    cout<<"Specify Y-Coordinate(b1-b10): ";
    cin>>y;

    switch (option)
    {
    case 1:
        newResidentialBuilding(city, x, y);
        break;
    
    default:
        break;
    }
};

void upgradeBuilding(City &city){
    int size = city.getSize();
    string x, y, item;
    cout<<"Mention Coordinates of the building to upgrade: ";
    cout<<"\nSpecify X-Coordinate(a1-a10): ";
    cin>>x;
    cout<<"Specify Y-Coordinate(b1-b10): ";
    cin>>y;
    int X = getXCoordinate(size, x);
    int Y = getYCoordinate(size, y);
    item = city.getCoordinateItem(X, Y);
    // if (item!= " E "){
    //     cout<<""
    // }
    cout<<item;
    
}