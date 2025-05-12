#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <ctime>
#include <string>

using namespace std;

bool gameOver;
const int width = 80;
const int height = 30;
int x, y, fruit_X, fruit_Y, score, level, speed;
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

// Set text color in console
void SetColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
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
    level = 1;
    speed = 100;
    n_tail = 0;
}

void Draw()
{
    MoveCursorToTop();

    // Top border
    SetColor(1); // Blue
    for (int i = 0; i < width + 2; i++) cout << "#";
    SetColor(7); // Reset
    cout << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0) { SetColor(1); cout << "#"; SetColor(7); }

            if (i == y && j == x)
            {
                SetColor(10); cout << "O"; SetColor(7); // Snake head
            }
            else if (i == fruit_Y && j == fruit_X)
            {
                SetColor(12); cout << "F"; SetColor(7); // Fruit
            }
            else
            {
                bool print = false;
                for (int k = 0; k < n_tail; k++)
                {
                    if (tail_X[k] == j && tail_Y[k] == i)
                    {
                        SetColor(14); cout << "o"; SetColor(7); // Snake tail
                        print = true;
                        break;
                    }
                }
                if (!print) cout << " ";
            }

            if (j == width - 1) { SetColor(1); cout << "#"; SetColor(7); }
        }
        cout << endl;
    }

    // Bottom border
    SetColor(1);
    for (int i = 0; i < width + 2; i++) cout << "#";
    SetColor(7);
    cout << endl;

    cout << "Score: " << score << "   Level: " << level << endl;
    cout << "Press 'X' to exit.\n";
}

void Input()
{
    if (_kbhit())
    {
        int ch = _getch();
        if (ch == 224) // Arrow keys
        {
            ch = _getch(); // Get actual key code
            switch (ch)
            {
            case 75: if (dir != RIGHT) dir = LEFT; break;  // Left Arrow
            case 77: if (dir != LEFT)  dir = RIGHT; break; // Right Arrow
            case 72: if (dir != DOWN)  dir = UP; break;    // Up Arrow
            case 80: if (dir != UP)    dir = DOWN; break;  // Down Arrow
            }
        }
        else
        {
            switch (ch)
            {
            case 'a': if (dir != RIGHT) dir = LEFT; break;
            case 'd': if (dir != LEFT)  dir = RIGHT; break;
            case 'w': if (dir != DOWN)  dir = UP; break;
            case 's': if (dir != UP)    dir = DOWN; break;
            case 'x': gameOver = true; break;
            }
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
        n_tail++;

        Beep(750, 100); // Fruit collected sound

        // Avoid spawning fruit on tail
        do {
            fruit_X = rand() % width;
            fruit_Y = rand() % height;
        } while ([&]() {
            for (int i = 0; i < n_tail; i++)
                if (tail_X[i] == fruit_X && tail_Y[i] == fruit_Y)
                    return true;
            return false;
            }());

        // Increase level every 5 points
        if (score % 5 == 0 && level < 10)
        {
            level++;
            speed = max(30, speed - 10); // Faster snake
        }
    }
}

int main()
{
    Setup();

    // Hide blinking cursor
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);

    while (!gameOver)
    {
        Draw();
        Input();
        Logic();
        Sleep(speed);
    }

    // Clear the screen completely
    system("cls");

    // Draw the border again
    SetColor(1);
    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++)
    {
        cout << "#";
        for (int j = 0; j < width; j++) cout << " ";
        cout << "#" << endl;
    }
    for (int i = 0; i < width + 2; i++) cout << "#";
    SetColor(7);

    // Position cursor to center vertically
    COORD coord;
    coord.Y = height / 2 - 1;

    // Center "GAME OVER"
    string msg1 = "  *** GAME OVER ***  ";
    coord.X = (width - msg1.length()) / 2 + 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    SetColor(12);
    cout << msg1;

    // Center Score & Level
    string msg2 = "Final Score: " + to_string(score) + "  |  Level: " + to_string(level);
    coord.Y += 2;
    coord.X = (width - msg2.length()) / 2 + 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    SetColor(14);
    cout << msg2;
    SetColor(7);

    // Beep
    Beep(300, 500);

    // Pause at bottom
    coord.Y = height + 2;
    coord.X = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    system("pause");

    return 0;
}
