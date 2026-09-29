#include "Cell.h"

Cell::Cell() : value(0), revealed(false), flagged(false) {}

int Cell::getValue() const { return value; }

void Cell::setValue(int val) { value = val; }

bool Cell::isRevealed() const { return revealed; }

void Cell::setRevealed(bool state) { revealed = state; }

bool Cell::isFlagged() const { return flagged; }

void Cell::toggleFlag() { flagged = !flagged; }

void Cell::setFlagged(bool state) { flagged = state; }

bool Cell::isMine() const { return value == -1; }

void Cell::reset() {
    value = 0;
    revealed = false;
    flagged = false;
}
