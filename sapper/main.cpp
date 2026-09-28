/******************
 *                        ИГРА "САПЁР" (MINESWEEPER) На C++
 *
 * ПОДРОБНЫЙ РАЗБОР ИСПОЛЬЗУЕМЫХ БИБЛИОТЕК И КОМАНД:
 *
 * 1. <iostream>
 *    - std::cout : Поток вывода. Используется для печати игрового поля, меню и сообщений.
 *    - std::cin  : Поток ввода. Считывает команды игрока (o/f) и координаты.
 *    - cin.clear() : Сбрасывает флаги ошибок ввода (например, если вместо числа ввели букву).
 *    - cin.ignore(10000, '\n') : Очищает буфер ввода до символа новой строки, предотвращая
 *                                зацикливание при неверном вводе.
 *
 * 2. <cstdlib>
 *    - std::rand()  : Генерирует псевдослучайное целое число. Нужен для случайного выбора
 *                     координат мин на поле.
 *    - std::srand() : Задает начальную точку ("зерно") для генератора rand(). Без него
 *                     вызовы rand() генерировали бы одну и ту же последовательность.
 *    - std::abs()   : Возвращает абсолютное значение (модуль) числа. Используется для
 *                     проверки distance <= 1 вокруг первого клика.
 *    - std::system(): Выполняет системную команду консоли. В нашем коде: "cls" для Windows
 *                     или "clear" для UNIX-систем (очистка экрана).
 *
 * 3. <ctime>
 *    - std::time(nullptr) : Возвращает текущее системное время в секундах.
 *                           Передается в srand(), чтобы при каждом запуске игры мины
 *                           располагались по-новому.
 *
 * 4. <cstring>
 *    - Подключает строковые функции и поддержку std::string(length, '-'), которая
 *      используется для динамического рисования рамок поля заданной ширины.
 ******************/

#include <iostream>  // Для std::cout, std::cin, std::cin.clear(), std::cin.ignore()
#include <cstdlib>   // Для std::rand(), std::srand(), std::system(), std::abs()
#include <ctime>     // Для std::time()
#include <cstring>   // Для std::string

using namespace std;

/**
 * КЛАСС: Cell
 * НАЗНАЧЕНИЕ: Хранит состояние и свойства одной конкретной клетки поля.
 */
class Cell {
private:
    int value;     // Значение клетки: -1 — мина, 0..8 — количество мин вокруг
    bool revealed; // true, если клетка уже открыта игроком
    bool flagged;  // true, если игрок поставил на клетку флажок 'F'

public:
    // Конструктор по умолчанию (инициализирует клетку как закрытую и пустую)
    Cell() : value(0), revealed(false), flagged(false) {}

    // Геттеры и сеттеры для управления состоянием ячейки
    int getValue() const { return value; }
    void setValue(int val) { value = val; }

    bool isRevealed() const { return revealed; }
    void setRevealed(bool state) { revealed = state; }

    bool isFlagged() const { return flagged; }
    void toggleFlag() { flagged = !flagged; } // Переключает флаг (поставить/снять)
    void setFlagged(bool state) { flagged = state; }

    bool isMine() const { return value == -1; } // Проверка, является ли клетка миной

    void reset() {
        value = 0;
        revealed = false;
        flagged = false;
    }
};

/**
 * КЛАСС: Minesweeper
 * НАЗНАЧЕНИЕ: Содержит всю логику игры, управление памятью и консольный интерфейс.
 */
class Minesweeper {
private:
    int width;       // Ширина игрового поля (колонки)
    int height;      // Высота игрового поля (строки)
    int totalMines;  // Общее количество мин
    Cell** board;    // Указатель на двумерный динамический массив объектов Cell
    bool gameOver;   // Флаг состояния игры (true = игра окончена)
    bool firstMove;  // Флаг первого хода (для гарантии безопасности первого клика)

    /**
     * Вспомогательный метод: проверяет, не выходят ли координаты за пределы поля.
     */
    bool isValid(int r, int c) const {
        return r >= 0 && r < height && c >= 0 && c < width;
    }
    /**
         * Вспомогательный метод: считает количество мин вокруг клетки (r, c) во всех 8 направлениях.
         */
    int countMinesAround(int r, int c) const {
        int count = 0;
        // Двойной цикл от -1 до 1 проходится по всем 8 соседним ячейкам
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                int nr = r + dr;
                int nc = c + dc;
                // Защита от выхода за границы массива с помощью isValid
                if (isValid(nr, nc) && board[nr][nc].isMine()) {
                    count++;
                }
            }
        }
        return count;
    }

    /**
     * Вспомогательный метод: считает общее количество флагов, установленных игроком.
     */
    int countFlags() const {
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

public:
    /**
     * КОНСТРУКТОР: Выделяет динамическую память под двумерный массив board[height][width].
     */
    Minesweeper(int h = 8, int w = 8, int mines = 10)
        : height(h), width(w), totalMines(mines), gameOver(false), firstMove(true) {

        // Выделяем массив указателей на строки
        board = new Cell * [height];
        for (int i = 0; i < height; ++i) {
            // Для каждой строки выделяем массив ячеек
            board[i] = new Cell[width];
        }
    }

    /**
     * ДЕСТРУКТОР: Корректно освобождает всю выделенную динамическую память,
     * исключая утечки памяти (memory leaks).
     */
    ~Minesweeper() {
        for (int i = 0; i < height; ++i) {
            delete[] board[i]; // Удаляем каждую строку
        }
        delete[] board; // Удаляем массив указателей
    }

    /**
     * ФУНКЦИЯ: generateMines
     * Генерирует мины ПОСЛЕ первого клика открытия ('o').
     * Гарантирует, что в районе 3x3 от клетки (firstR, firstC) мин не будет.
     */
    void generateMines(int firstR, int firstC) {
        // Инициализируем генератор случайных чисел текущим временем из <ctime>
        srand(static_cast<unsigned int>(time(nullptr)));

        int placed = 0;
        while (placed < totalMines) {
            // Выбираем случайные координаты с помощью rand() из <cstdlib>
            int r = rand() % height;
            int c = rand() % width;

            // Использование abs() из <cstdlib>: запрещаем ставить мину в радиусе 1 ячейки от первого хода
            if ((abs(r - firstR) <= 1 && abs(c - firstC) <= 1) || board[r][c].isMine()) {
                continue; // Пропускаем, если слишком близко к первому клику или здесь уже есть мина
            }

            board[r][c].setValue(-1); // Устанавливаем мину
            placed++;
        }

        // Подсчитываем и записываем цифры мин для всех остальных ячеек
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                if (!board[r][c].isMine()) {
                    board[r][c].setValue(countMinesAround(r, c));
                }
            }
        }
    }

    /**
     * ФУНКЦИЯ: draw
     * Отрисовывает поле в консоли. Использует двухуровневую шапку,
     * чтобы двузначные индексы (10, 11) выравнивались идеально ровно.
     */
    void draw() const {
        // Кроссплатформенная очистка консоли через условную компиляцию
#ifdef _WIN32
        system("cls");   // Для ОС Windows
#else
        system("clear"); // Для Linux и macOS
#endif

        int availableFlags = totalMines - countFlags();
        cout << "Flags left: " << availableFlags << "\n\n";

        // Верхний уровень шапки: выводит десятки (1 для столбцов 10, 11 и т.д.)
        cout << "    ";
        for (int c = 0; c < width; ++c) {
            if (c >= 10) cout << c / 10 << " ";
            else cout << "  ";
        }
        cout << "\n";
        // Нижний уровень шапки: выводит единицы (0 1 2 3 4 5 6 7 8 9 0 1...)
        cout << "    ";
        for (int c = 0; c < width; ++c) {
            cout << c % 10 << " ";
        }
        // Вывод границы с помощью std::string(length, '-') из <cstring>
        cout << "\n   +" << string(width * 2, '-') << "+\n";

        // Отрисовка строк игрового поля
        for (int r = 0; r < height; ++r) {
            // Форматирование бокового индекса (выравнивание пробелами)
            if (r < 10) cout << " " << r << " |";
            else cout << r << " |";

            for (int c = 0; c < width; ++c) {
                if (board[r][c].isFlagged()) {
                    cout << "F "; // Флаг
                }
                else if (!board[r][c].isRevealed()) {
                    cout << ". "; // Закрытая клетка
                }
                else if (board[r][c].isMine()) {
                    cout << "* "; // Мина (в случае проигрыша)
                }
                else {
                    cout << board[r][c].getValue() << " "; // Число открытых мин вокруг
                }
            }
            cout << "|\n";
        }
        cout << "   +" << string(width * 2, '-') << "+\n";
    }

    /**
     * ФУНКЦИЯ: openCell
     * Рекурсивный алгоритм (Flood Fill): автоматически раскрывает
     * прилегающие пустые области при клике на '0'.
     */
    void openCell(int r, int c) {
        // Базовый случай рекурсии: прерываем вызов, если координаты невалидны,
        // или клетка уже открыта, или на ней стоит флаг.
        if (!isValid(r, c) || board[r][c].isRevealed() || board[r][c].isFlagged()) return;

        board[r][c].setRevealed(true);

        // Если значение клетки равно 0, рекурсивно вызываем openCell для всех 8 соседей
        if (board[r][c].getValue() == 0) {
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr != 0 || dc != 0) {
                        openCell(r + dr, c + dc); // Рекурсивный вызов
                    }
                }
            }
        }
    }

    /**
     * ФУНКЦИЯ: checkWin
     * Проверяет, остались ли на поле неоткрытые безопасные ячейки.
     */
    bool checkWin() const {
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                // Если клетка не мина и до сих пор закрыта — победа еще не наступила
                if (!board[r][c].isMine() && !board[r][c].isRevealed()) {
                    return false;
                }
            }
        }
        return true; // Все безопасные клетки успешно открыты
    }

    /**
     * ФУНКЦИЯ: play
     * Основной цикл обработки ходов игрока.
     */
    void play() {
        while (!gameOver) {
            draw();

            char action;
            int r, c;

            cout << "Enter action (o - open, f - flag) and coordinates (row col): ";

            // Безопасный ввод из <iostream>:
            // Если игрок вводит неверный тип данных, cin переходит в состояние ошибки
            if (!(cin >> action >> r >> c) || !isValid(r, c)) {
                cout << "Invalid input! Press Enter to try again...";
                cin.clear();              // Сбрасываем флаг ошибки потока cin
                cin.ignore(10000, '\n');  // Игнорируем весь некорректный ввод в буфере
                continue;
            }

            // Обработка действия "Поставить / Снять флаг"
            if (action == 'f' || action == 'F') {
                if (!board[r][c].isRevealed()) {
                    // Проверка лимита флагов: нельзя поставить больше флагов, чем всего мин
                    if (!board[r][c].isFlagged() && countFlags() >= totalMines) {
                        cout << "Cannot place more flags! Limit reached.\nPress Enter...";
                        cin.ignore(10000, '\n');
                        cin.get();
                        continue;
                    }
                    board[r][c].toggleFlag();
                }
            }
            // Обработка действия "Открыть клетку"
            else if (action == 'o' || action == 'O') {
                // Если ячейка была под флагом, при прямом открытии снимаем флаг
                if (board[r][c].isFlagged()) {
                    board[r][c].setFlagged(false);
                }

                // Генерация мин происходит только при ПЕРВОМ открытии ячейки
                if (firstMove) {
                    generateMines(r, c);
                    firstMove = false;
                }

                // Проверка на поражение
                if (board[r][c].isMine()) {
                    gameOver = true;
                    board[r][c].setRevealed(true);
                    draw();
                    cout << "\nBOOM! You hit a mine. Game Over!\n";
                }
                // Безопасный ход
                else {
                    openCell(r, c);
                    // Проверка на победу после открытия
                    if (checkWin()) {
                        gameOver = true;
                        draw();
                        cout << "\nCongratulations! You cleared the entire board!\n";
                    }
                }
            }
        }
    }
};

/**
 * ГЛАВНАЯ ФУНКЦИЯ: main
 * Точка входа в программу.
 */
int main() {
    // Очищаем экран перед меню выбора
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    int choice = 0;
    cout << "=============================\n";
    cout << "   WELCOME TO MINESWEEPER    \n";
    cout << "=============================\n";
    cout << "Select difficulty:\n";
    cout << "1. Easy   (8x8, 10 mines)\n";
    cout << "2. Medium (10x10, 15 mines)\n";
    cout << "3. Hard   (12x12, 25 mines)\n";
    cout << "Enter choice (1-3): ";

    // Защита ввода меню: повторяем запрос, пока не введут число от 1 до 3
    while (!(cin >> choice) || choice < 1 || choice > 3) {
        cout << "Invalid choice! Please enter 1, 2, or 3: ";
        cin.clear();              // Сброс флага ошибки cin
        cin.ignore(10000, '\n');  // Очистка буфера ввода
    }

    // Задание параметров поля в зависимости от выбора пользователя
    int h = 8, w = 8, mines = 10;
    if (choice == 2) {
        h = 10; w = 10; mines = 15;
    }
    else if (choice == 3) {
        h = 12; w = 12; mines = 25;
    }

    // Создание объекта игры и запуск основного цикла
    Minesweeper game(h, w, mines);
    game.play();

    return 0; // Успешное завершение программы
}