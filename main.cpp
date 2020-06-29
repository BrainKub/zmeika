#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <list>
#include <ncurses.h>

enum Move {
    UP, DOWN, LEFT, RIGHT
};

struct Point {
    int x, y;
    Move m;

    Point() : x(0), y(0), m(RIGHT) {}

    Point(int x_, int y_, Move m_) : x(x_), y(y_), m(m_) {}
};

int Field_x0 = 0, Field_y0 = 2, Field_x1 = 30, Field_y1 = 15;
int Food_x, Food_y, score = 0;
std::list<Point> snake;

void moveSnake(Move m) {
    int x = snake.front().x;
    int y = snake.front().y;
    switch (m) {
        case UP:
            snake.push_front(Point(x, y - 1, m));
            snake.pop_back();
            break;
        case DOWN:
            snake.push_front(Point(x, y + 1, m));
            snake.pop_back();
            break;
        case LEFT:
            snake.push_front(Point(x - 1, y, m));
            snake.pop_back();
            break;
        case RIGHT:
            snake.push_front(Point(x + 1, y, m));
            snake.pop_back();
            break;
    }
}

void addPoint() {
    int x = snake.back().x;
    int y = snake.back().y;
    Move m = snake.back().m;
    switch (m) {
        case UP:
            snake.push_back(Point(x, y + 2, m));
            break;
        case DOWN:
            snake.push_back(Point(x, y - 2, m));
            break;
        case LEFT:
            snake.push_back(Point(x + 2, y, m));
            break;
        case RIGHT:
            snake.push_back(Point(x - 2, y, m));
            break;
    }
}

bool checkSnakeCoord() {
    int x = snake.front().x;
    int y = snake.front().y;
    std::list<Point>::iterator it = ++snake.begin();
    std::list<Point>::iterator end = snake.end();
    for (; it != end; ++it) {
        if (x == it->x && y == it->y) {
            return true;
        }
    }
    return false;
}

bool checkFieldCoord() {
    return snake.front().x == Field_x0 || snake.front().x == Field_x1
           || snake.front().y == Field_y0 || snake.front().y == Field_y1;
}

bool checkFoodCoord() {
    int x = Food_x;
    int y = Food_y;
    for (std::list<Point>::iterator it = snake.begin(); it != snake.end(); ++it) {
        if (x == it->x || y == it->y) {
            return false;
        }
    }
    return true;
}

void newFood() {
    bool good;
    do {
        Food_x = Field_x0 + 1 + rand() % (Field_x1 - (Field_x0 + 1));
        Food_y = Field_y0 + 1 + rand() % (Field_y1 - (Field_y0 + 1));
        good = checkFoodCoord();
    } while (!good);
}

void printSnake() {
    std::list<Point>::iterator it = snake.begin();
    std::list<Point>::iterator end = snake.end();
    for (; it != end; ++it) {
		mvaddch(it->y, it->x, 'o');
    }
    mvaddch(snake.back().y, snake.back().x, '*');
    mvaddch(snake.front().y, snake.front().x, '0');
}

void printRect(int x0, int y0, int x1, int y1) {
    int x = x0;
    int y = y0;
    for (; x <= x1; x++) {
        mvaddch(y, x, '-');
    }
    x--;
    y++;
    for (; y < y1; y++) {
        mvaddch(y, x, '|');
    }
    for (; x >= x0; x--) {
        mvaddch(y, x, '=');
    }
    x++;
    y--;
    for (; y > y0; y--) {
        mvaddch(y, x, '|');
    }
}

void render() {
    clear();
    printRect(Field_x0, Field_y0, Field_x1, Field_y1);
    printRect(0, 0, Field_x1, Field_y0);
    mvprintw(1, 1, "Score %d", score);
    printSnake();
    mvaddch(Food_y, Food_x, '+');
    mvaddch(0, 0, '-');
    refresh();
}

void initGame() {
    int xC = (Field_x0 + 1 + (Field_x1 - (Field_x0 + 1))) / 2;
    int yC = (Field_y0 + 1 + (Field_y1 - (Field_y0 + 1))) / 2;
    int r_pos = rand() % 4;
    xC += r_pos;
    yC += r_pos;
    Move m = (Move)(rand() % 4);
    switch (m) {
         case UP:
                snake.push_back(Point(xC, yC - 1, m));
                snake.push_back(Point(xC, yC, m));
                snake.push_back(Point(xC, yC + 1, m));
                break;
         case DOWN:
                snake.push_back(Point(xC, yC + 1, m));
                snake.push_back(Point(xC, yC, m));
                snake.push_back(Point(xC, yC - 1, m));
                break;
         case LEFT:
                snake.push_back(Point(xC - 1, yC, m));
                snake.push_back(Point(xC, yC, m));
                snake.push_back(Point(xC + 1, yC, m));
                break;
         case RIGHT:
                snake.push_back(Point(xC + 1, yC, m));
                snake.push_back(Point(xC, yC, m));
                snake.push_back(Point(xC - 1, yC, m));
                break;
    }
    newFood();
}

int main() {
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    nodelay(stdscr, TRUE);
    srand(time(0));
    initGame();
    Move m = snake.front().m;
    int wait = 200;
    int r_sk = rand() % 100;
    wait += r_sk;
    bool running = true;
    do {
        int key = getch();
        switch (key) {
            case 'q':
            case 'Q':
                running = false;
                break;
            case KEY_UP:
                m = UP;
                break;
            case KEY_DOWN:
                m = DOWN;
                break;
            case KEY_LEFT:
                m = LEFT;
                break;
            case KEY_RIGHT:
                m = RIGHT;
                break;
            default:
                break;
        }
        moveSnake(m);
        if (snake.front().x == Food_x && snake.front().y == Food_y) {
            addPoint();
            score++;
            newFood();
        }
        render();
        if (checkFieldCoord() || checkSnakeCoord()) {
            int xC = (Field_x0 + 1 + (Field_x1 - (Field_x0 + 1))) / 2 - 4;
            running = false;
        }
        flushinp();
        usleep(wait * 1000);
    } while (running);
	clear();
	endwin();
	return 0;
}
