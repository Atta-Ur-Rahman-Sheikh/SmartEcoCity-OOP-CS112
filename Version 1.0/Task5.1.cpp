#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h> // For _kbhit() and _getch()

using namespace std;

const int WIDTH = 20;
const int HEIGHT = 20;
const char SNAKE_HEAD = '@';
const char SNAKE_BODY = 'o';
const char FOOD = '*';
const char EMPTY = ' ';

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;

bool gameOver;
int x, y, foodX, foodY, score;
vector<pair<int, int>> snake;

void setup() {
    gameOver = false;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    snake.clear();
    snake.push_back({x, y});
    score = 0;
    foodX = rand() % WIDTH;
    foodY = rand() % HEIGHT;
}

void draw() {
    system("cls"); // Clear the console

    for (int i = 0; i < WIDTH + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (j == 0)
                cout << "#";

            bool printed = false;
            if (i == y && j == x) {
                cout << SNAKE_HEAD;
                printed = true;
            } else if (i == foodY && j == foodX) {
                cout << FOOD;
                printed = true;
            } else {
                for (size_t k = 1; k < snake.size(); k++) {
                    if (snake[k].first == j && snake[k].second == i) {
                        cout << SNAKE_BODY;
                        printed = true;
                        break;
                    }
                }
            }

            if (!printed)
                cout << EMPTY;

            if (j == WIDTH - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < WIDTH + 2; i++)
        cout << "#";
    cout << endl;

    cout << "Score: " << score << endl;
}

void input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'a':
                if (dir != RIGHT) dir = LEFT;
                break;
            case 'd':
                if (dir != LEFT) dir = RIGHT;
                break;
            case 'w':
                if (dir != DOWN) dir = UP;
                break;
            case 's':
                if (dir != UP) dir = DOWN;
                break;
            case 'x':
                gameOver = true;
                break;
        }
    }
}

void logic() {
    int prevX = x;
    int prevY = y;

    switch (dir) {
        case LEFT:  x--; break;
        case RIGHT: x++; break;
        case UP:    y--; break;
        case DOWN:  y++; break;
        default: break;
    }

    // Check for collision with walls
    if (x >= WIDTH || x < 0 || y >= HEIGHT || y < 0)
        gameOver = true;

    // Move the body
    for (int i = snake.size() - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    snake[0] = {x, y};

    // Check collision with itself
    for (size_t i = 1; i < snake.size(); i++)
        if (snake[i].first == x && snake[i].second == y)
            gameOver = true;

    // Eat food
    if (x == foodX && y == foodY) {
        score += 10;
        snake.push_back({0, 0}); // Temporary, will be adjusted in next frame
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    setup();
    while (!gameOver) {
        draw();
        input();
        logic();
        _sleep(100); // Pause to slow game loop
    }
    cout << "Game Over!" << endl;
    return 0;
}
