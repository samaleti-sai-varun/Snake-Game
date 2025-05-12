#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <ctime>

using namespace std;

bool gameOver;
const int width = 20;
const int height = 20;
int x, y, fruit_X, fruit_Y, score;
int tail_X[100], tail_Y[100];
int n_tail;

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

// Move cursor to top-left corner
void MoveCursorToTop()
{
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Setup()
{
    srand(time(0));
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruit_X = rand() % width;
    fruit_Y = rand() % height;
    score = 0;
    n_tail = 0;
}

void Draw()
{
    MoveCursorToTop();

    // Top border
    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0) cout << "#";

            if (i == y && j == x)
                cout << "O"; // Head
            else if (i == fruit_Y && j == fruit_X)
                cout << "F"; // Fruit
            else
            {
                bool print = false;
                for (int k = 0; k < n_tail; k++)
                {
                    if (tail_X[k] == j && tail_Y[k] == i)
                    {
                        cout << "o";
                        print = true;
                        break;
                    }
                }
                if (!print) cout << " ";
            }

            if (j == width - 1) cout << "#";
        }
        cout << endl;
    }

    // Bottom border
    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    cout << "Score: " << score << endl;
}

void Input()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'a': dir = LEFT; break;
        case 'd': dir = RIGHT; break;
        case 'w': dir = UP; break;
        case 's': dir = DOWN; break;
        case 'x': gameOver = true; break;
        default: break;
        }
    }
}

void Logic()
{
    int prev_X = tail_X[0];
    int prev_Y = tail_Y[0];
    int prev2_X, prev2_Y;

    tail_X[0] = x;
    tail_Y[0] = y;

    for (int i = 1; i < n_tail; i++)
    {
        prev2_X = tail_X[i];
        prev2_Y = tail_Y[i];
        tail_X[i] = prev_X;
        tail_Y[i] = prev_Y;
        prev_X = prev2_X;
        prev_Y = prev2_Y;
    }

    switch (dir)
    {
    case LEFT: x--; break;
    case RIGHT: x++; break;
    case UP: y--; break;
    case DOWN: y++; break;
    default: break;
    }

    // Wall collision
    if (x >= width || x < 0 || y >= height || y < 0)
        gameOver = true;

    // Self collision
    for (int i = 0; i < n_tail; i++)
        if (tail_X[i] == x && tail_Y[i] == y)
            gameOver = true;

    // Fruit collision
    if (x == fruit_X && y == fruit_Y)
    {
        score++;
        fruit_X = rand() % width;
        fruit_Y = rand() % height;
        n_tail++;
    }
}

int main()
{
    Setup();

    // Hide blinking cursor
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false; // set cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);

    while (!gameOver)
    {
        Draw();
        Input();
        Logic();
        Sleep(100);
    }

    return 0;
}
