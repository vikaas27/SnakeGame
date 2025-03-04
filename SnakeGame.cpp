#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
//snake game 
using namespace std;

const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;
const int INITIAL_SNAKE_LENGTH = 3;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct Node {
    int x, y;
    Node* next;
    Node(int x, int y) : x(x), y(y), next(nullptr) {}
};

class Snake {
private:
    Node* head;
    Direction dir;

public:
    Snake() {
        initSnake();
    }

    ~Snake() {
        freeSnake();
    }

    void initSnake() {
        int startX = GRID_WIDTH / 2;
        int startY = GRID_HEIGHT / 2;
    
        head = new Node(startX, startY);
        dir = STOP;
    
        Node* temp = head;
        for (int i = 1; i < INITIAL_SNAKE_LENGTH; i++) {
            Node* newNode = new Node(startX + i, startY);
            newNode->next = temp;
            temp = newNode;
        }
        head = temp; 
        }
    
    void freeSnake() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void reset() {
        freeSnake();
        initSnake();
    }

    Node* getHead() { return head; }

    Direction getDirection() { return dir; }
    void setDirection(Direction newDir) {
        if (dir == STOP && newDir == LEFT) 
            return; // Ignore left move at the start
    
        if ((dir == LEFT && newDir == RIGHT) || (dir == RIGHT && newDir == LEFT) ||
            (dir == UP && newDir == DOWN) || (dir == DOWN && newDir == UP))
            return; // Prevent reversing direction
    
        dir = newDir;
    }
    

    
    

    void move(bool grow = false) {
        if (dir == STOP) return;

        int newX = head->x, newY = head->y;
        switch (dir) {
            case LEFT:  newX--; break;
            case RIGHT: newX++; break;
            case UP:    newY--; break;
            case DOWN:  newY++; break;
            default: break;
        }

        Node* newHead = new Node(newX, newY);
        newHead->next = head;
        head = newHead;

        if (!grow) {
            Node* temp = head;
            while (temp->next->next) {
                temp = temp->next;
            }
            delete temp->next;
            temp->next = nullptr;
        }
    }

    bool checkCollision() {
        for (Node* temp = head->next; temp != nullptr; temp = temp->next) {
            if (temp->x == head->x && temp->y == head->y)
                return true;
        }
        return false;
    }
};

class Game {
private:
    Snake snake;
    pair<int, int> food;
    bool gameOver;
    int score;

public:
    Game() : gameOver(false), score(0) {
        srand(static_cast<unsigned>(time(0)));
        spawnFood();
    }

    void spawnFood() {
        while (true) {
            food.first = 1 + rand() % (GRID_WIDTH - 2);
            food.second = 1 + rand() % (GRID_HEIGHT - 2);

            bool valid = true;
            for (Node* temp = snake.getHead(); temp != nullptr; temp = temp->next) {
                if (temp->x == food.first && temp->y == food.second) {
                    valid = false;
                    break;
                }
            }
            if (valid) break;
        }
    }

    void input() {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'a': case 'A': snake.setDirection(LEFT); break;
                case 'd': case 'D': snake.setDirection(RIGHT); break;
                case 'w': case 'W': snake.setDirection(UP); break;
                case 's': case 'S': snake.setDirection(DOWN); break;
                case 'q': case 'Q': gameOver = true; break;
            }
        }
    }
    void logic() {
        if (snake.getDirection() == STOP) return;
    
        int newX = snake.getHead()->x;
        int newY = snake.getHead()->y;
    
        switch (snake.getDirection()) {
            case LEFT:  newX--; break;
            case RIGHT: newX++; break;
            case UP:    newY--; break;
            case DOWN:  newY++; break;
            default: break;
        }
    
        
        if (newX == 0 || newX == GRID_WIDTH - 1 || newY == 0 || newY == GRID_HEIGHT - 1) {
            gameOver = true;
            return;
        }
    
        
        if (snake.getHead()->next != nullptr) {
            for (Node* temp = snake.getHead()->next; temp != nullptr; temp = temp->next) {
                if (temp->x == newX && temp->y == newY) {
                    gameOver = true;
                    return;
                }
            }
        }
    
        bool grow = (newX == food.first && newY == food.second);
        snake.move(grow);
    
        if (grow) {
            score++;
            spawnFood();
        }
    }
    

    void draw() {
        COORD cursorPos = { 0, 0 };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);

        string grid(GRID_WIDTH * GRID_HEIGHT, ' ');

        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (y == 0 || y == GRID_HEIGHT - 1 || x == 0 || x == GRID_WIDTH - 1) {
                    grid[y * GRID_WIDTH + x] = '#';
                }
            }
        }

        grid[food.second * GRID_WIDTH + food.first] = 'F';

        for (Node* temp = snake.getHead(); temp != nullptr; temp = temp->next) {
            if (temp->x >= 0 && temp->x < GRID_WIDTH &&
                temp->y >= 0 && temp->y < GRID_HEIGHT) {
                grid[temp->y * GRID_WIDTH + temp->x] = 'O';
            }
        }

        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                cout << grid[y * GRID_WIDTH + x];
            }
            cout << endl;
        }

        cout << "Score: " << score << endl;
    }

    void run() {
        while (!gameOver) {
            input();
            logic();
            draw();
            Sleep(100);
        }

        cout << "Game Over! Final Score: " << score << endl;
        cout << "Press R to Restart or Q to Quit." << endl;
        
        char choice;
        while (true) {
            choice = _getch();
            if (choice == 'r' || choice == 'R') {
                resetGame();
                run();
                break;
            } else if (choice == 'q' || choice == 'Q') {
                break;
            }
        }
    }

    void resetGame() {
        snake.reset();
        score = 0;
        gameOver = false;
        spawnFood();
    }
};

    int main() {
        cout << "Welcome to the Snake Game! Press any key to start..." << endl;
        while (!_kbhit());
        

        _getch();
        system("cls");
        Game game;
        game.run();
        return 0;
    }
    

