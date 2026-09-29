#include <iostream>   
#include "Cell.h"
#include "Minesweeper.h"
using namespace std;


int main() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    int choice = 0;
    cout << "\n";
    cout << "   WELCOME TO MINESWEEPER    \n";
    cout << "\n";
    cout << "Select difficulty:\n";
    cout << "1. Easy   (8x8, 10 mines)\n";
    cout << "2. Medium (10x10, 15 mines)\n";
    cout << "3. Hard   (12x12, 25 mines)\n";
    cout << "Enter choice (1-3): ";

    while (!(cin >> choice) || choice < 1 || choice > 3) {
        cout << "Invalid choice. Please enter 1, 2, or 3: ";
        cin.clear();              
        cin.ignore(10000, '\n');  
    }

    int h = 8, w = 8, mines = 10;
    if (choice == 2) {
        h = 10; w = 10; mines = 15;
    }
    else if (choice == 3) {
        h = 12; w = 12; mines = 25;
    }

    Minesweeper game(h, w, mines);
    game.play();
}