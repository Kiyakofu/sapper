#include <iostream>  
#include <cstdlib>   
#include <ctime>     
#include <cstring>   
#include "Cell.h"
using namespace std;

class Minesweeper {
private:
    int width;       
    int height;      
    int totalMines;  
    Cell** board;    
    bool gameOver;   
    bool firstMove;  

    bool isValid(int r, int c) const;
  
    int countMinesAround(int r, int c) const;

    int countFlags() const;

public:
    Minesweeper(int h = 8, int w = 8, int mines = 10);

    ~Minesweeper();

    void generateMines(int firstR, int firstC);

    
    void draw() const;

    void openCell(int r, int c);

    bool checkWin() const;

    void play();
};