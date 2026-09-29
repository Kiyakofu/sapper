#include "Minesweeper.h"

bool Minesweeper::isValid(int r, int c) const {
    return r >= 0 && r < height && c >= 0 && c < width;
}

int Minesweeper::countMinesAround(int r, int c) const {
    int count = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            int nr = r + dr;
            int nc = c + dc;
            if (isValid(nr, nc) && board[nr][nc].isMine()) {
                count++;
            }
        }
    }
    return count;
}

int Minesweeper::countFlags() const {
    int flags = 0;
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            if (board[r][c].isFlagged()) {
                flags++;
            }
        }
    }
    return flags;
}

Minesweeper::Minesweeper(int h, int w, int mines)
    : height(h), width(w), totalMines(mines), gameOver(false), firstMove(true) {

    board = new Cell * [height];
    for (int i = 0; i < height; ++i) {
        board[i] = new Cell[width];
    }
}

Minesweeper::~Minesweeper() {
    for (int i = 0; i < height; ++i) {
        delete[] board[i];
    }
    delete[] board;
}

void Minesweeper::generateMines(int firstR, int firstC) {
    srand(static_cast<unsigned int>(time(nullptr)));

    int placed = 0;
    while (placed < totalMines) {
        int r = rand() % height;
        int c = rand() % width;

        if ((abs(r - firstR) <= 1 && abs(c - firstC) <= 1) || board[r][c].isMine()) {
            continue;
        }

        board[r][c].setValue(-1);
        placed++;
    }

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            if (!board[r][c].isMine()) {
                board[r][c].setValue(countMinesAround(r, c));
            }
        }
    }
}

void Minesweeper::draw() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    int availableFlags = totalMines - countFlags();
    cout << "Flags left: " << availableFlags << "\n\n";

    cout << "    ";
    for (int c = 0; c < width; ++c) {
        if (c >= 10) cout << c / 10 << " ";
        else cout << "  ";
    }
    cout << "\n";
    cout << "    ";
    for (int c = 0; c < width; ++c) {
        cout << c % 10 << " ";
    }
    cout << "\n   +" << string(width * 2, '-') << "+\n";

    for (int r = 0; r < height; ++r) {
        if (r < 10) cout << " " << r << " |";
        else cout << r << " |";

        for (int c = 0; c < width; ++c) {
            if (board[r][c].isFlagged()) {
                cout << "F ";
            }
            else if (!board[r][c].isRevealed()) {
                cout << ". ";
            }
            else if (board[r][c].isMine()) {
                cout << "* ";
            }
            else {
                cout << board[r][c].getValue() << " ";
            }
        }
        cout << "|\n";
    }
    cout << "   +" << string(width * 2, '-') << "+\n";
}

void Minesweeper::openCell(int r, int c) {
    if (!isValid(r, c) || board[r][c].isRevealed() || board[r][c].isFlagged()) return;

    board[r][c].setRevealed(true);

    if (board[r][c].getValue() == 0) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr != 0 || dc != 0) {
                    openCell(r + dr, c + dc);
                }
            }
        }
    }
}

bool Minesweeper::checkWin() const {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            if (!board[r][c].isMine() && !board[r][c].isRevealed()) {
                return false;
            }
        }
    }
    return true;
}

void Minesweeper::play() {
    while (!gameOver) {
        draw();

        char action;
        int r, c;

        cout << "Enter action (o - open, f - flag) and coordinates (row col), example: o 2 3: ";

        if (!(cin >> action >> r >> c) || !isValid(r, c)) {
            cout << "Invalid input. Press Enter to try again...";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (action == 'f' || action == 'F') {
            if (!board[r][c].isRevealed()) {
                if (!board[r][c].isFlagged() && countFlags() >= totalMines) {
                    cout << "Cannot place more flags. Limit reached.\nPress Enter...";
                    cin.ignore(10000, '\n');
                    cin.get();
                    continue;
                }
                board[r][c].toggleFlag();
            }
        }
        else if (action == 'o' || action == 'O') {
            if (board[r][c].isFlagged()) {
                board[r][c].setFlagged(false);
            }
            if (firstMove) {
                generateMines(r, c);
                firstMove = false;
            }
            if (board[r][c].isMine()) {
                gameOver = true;
                board[r][c].setRevealed(true);
                draw();
                cout << "\nYou hit a mine. Game Over\n";
            }
            else {
                openCell(r, c);
                if (checkWin()) {
                    gameOver = true;
                    draw();
                    cout << "\nYou cleared the entire board\n";
                }
            }
        }
    }
}
