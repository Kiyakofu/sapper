#pragma once
class Cell {
private:
    int value;     
    bool revealed; 
    bool flagged;  

public:
    Cell();

    int getValue() const;

    void setValue(int val);

    bool isRevealed() const;

    void setRevealed(bool state);

    bool isFlagged() const;

    void toggleFlag();

    void setFlagged(bool state);

    bool isMine() const;

    void reset();
};

