// snake_terminal.cpp
// Compile: g++ -std=c++17 snake_terminal.cpp -o snake
// Run: ./snake

#include <bits/stdc++.h>
#include <unistd.h>     // usleep
#include <termios.h>    // terminal control
#include <fcntl.h>      // fcntl
#include <sys/ioctl.h>  // ioctl for window size
#include <fstream>

using namespace std;

struct Terminal {
    termios oldt;
    Terminal() {
        tcgetattr(STDIN_FILENO, &oldt);
        termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // raw-ish mode, no echo
        newt.c_cc[VMIN] = 0;
        newt.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        // set non-blocking
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        // hide cursor
        cout << "\033[?25l";
    }
    ~Terminal() {
        // restore
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        // reset non-blocking flags (optional)
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        flags &= ~O_NONBLOCK;
        fcntl(STDIN_FILENO, F_SETFL, flags);
        // show cursor
        cout << "\033[?25h";
    }
};

enum class Dir { UP, DOWN, LEFT, RIGHT, NONE };

struct Point {
    int r, c;
    bool operator==(const Point &o) const { return r==o.r && c==o.c; }
};

class Snake {
public:
    deque<Point> body; // front = head
    Dir dir;

    Snake() { reset(); }

    void reset(int startR = 5, int startC = 5) {
        body.clear();
        body.push_back({startR, startC});
        body.push_back({startR, startC - 1});
        body.push_back({startR, startC - 2});
        dir = Dir::RIGHT;
    }

    Point head() const { return body.front(); }

    void grow(const Point &newHead) { body.push_front(newHead); /* don't pop tail */ }

    void move(const Point &newHead) {
        body.push_front(newHead);
        body.pop_back();
    }

    bool occupies(const Point &p) const {
        for (const auto &s: body) if (s == p) return true;
        return false;
    }

    bool collides_with_self() const {
        Point h = head();
        for (size_t i = 1; i < body.size(); ++i)
            if (body[i] == h) return true;
        return false;
    }
};

class Food {
public:
    Point p;
    Food() : p({-1,-1}) {}
};

class Game {
private:
    int rows, cols;       // playable grid size (without border)
    Snake snake;
    Food food;
    bool gameOver;
    int score;
    int highScore;
    string highScoreFile = "highscore.txt";
    Terminal term; // RAII: sets raw mode and hides cursor

public:
    Game(int R = 20, int C = 40) {
        rows = R;
        cols = C;
        loadHighScore();
    }

    void init_new_game() {
        // start roughly centered
        int sr = rows / 2;
        int sc = cols / 2;
        snake.reset(sr, sc);
        score = 0;
        place_food();
        gameOver = false;
    }

    void run_loop() {
        init_new_game();
        const useconds_t FRAME_US = 110000; // 110ms per frame ~ ~9 FPS; change for speed
        render_instructions();
        usleep(800000); // small pause to let user read
        while (true) {
            auto start = chrono::steady_clock::now();
            handle_input();
            if (!gameOver) update();
            render();
            if (gameOver) {
                handle_game_over();
                char ch;
                while (true) {
                    if (read_char(ch)) {
                        if (ch=='r' || ch=='R') { init_new_game(); break; }
                        if (ch=='q' || ch=='Q') { cleanup_and_exit(); return; }
                    }
                    usleep(50000);
                }
            }
            auto elapsed = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start).count();
            if (elapsed < FRAME_US) usleep(FRAME_US - elapsed);
        }
    }

private:
    void loadHighScore() {
        highScore = 0;
        ifstream in(highScoreFile);
        if (in.is_open()) {
            in >> highScore;
            in.close();
        }
    }
    void saveHighScore() {
        if (score > highScore) {
            ofstream out(highScoreFile);
            if (out.is_open()) {
                out << score;
                out.close();
                highScore = score;
            }
        }
    }

    bool read_char(char &outc) {
        char buf;
        ssize_t n = read(STDIN_FILENO, &buf, 1);
        if (n <= 0) return false;
        outc = buf;
        return true;
    }

    // parse inputs; supports WASD and arrow keys (escape sequences)
    void handle_input() {
        char ch;
        while (read_char(ch)) {
            if (ch == '\033') { // maybe arrow
                // try to read two more bytes for arrow keys
                char b1=0,b2=0;
                usleep(1000);
                read(STDIN_FILENO, &b1, 1);
                usleep(1000);
                read(STDIN_FILENO, &b2, 1);
                if (b1 == '[') {
                    if (b2 == 'A') set_dir(Dir::UP);
                    else if (b2 == 'B') set_dir(Dir::DOWN);
                    else if (b2 == 'C') set_dir(Dir::RIGHT);
                    else if (b2 == 'D') set_dir(Dir::LEFT);
                }
            } else {
                switch (ch) {
                    case 'w': case 'W': set_dir(Dir::UP); break;
                    case 's': case 'S': set_dir(Dir::DOWN); break;
                    case 'a': case 'A': set_dir(Dir::LEFT); break;
                    case 'd': case 'D': set_dir(Dir::RIGHT); break;
                    case 'q': case 'Q': cleanup_and_exit(); exit(0); break;
                    default: break;
                }
            }
        }
    }

    void set_dir(Dir d) {
        // disallow reverse direction
        if (d == Dir::UP && snake.dir == Dir::DOWN) return;
        if (d == Dir::DOWN && snake.dir == Dir::UP) return;
        if (d == Dir::LEFT && snake.dir == Dir::RIGHT) return;
        if (d == Dir::RIGHT && snake.dir == Dir::LEFT) return;
        snake.dir = d;
    }

    void update() {
        Point head = snake.head();
        Point next = head;
        switch (snake.dir) {
            case Dir::UP:    next.r -= 1; break;
            case Dir::DOWN:  next.r += 1; break;
            case Dir::LEFT:  next.c -= 1; break;
            case Dir::RIGHT: next.c += 1; break;
            default: break;
        }

        // check boundary collision
        if (next.r < 0 || next.r >= rows || next.c < 0 || next.c >= cols) {
            gameOver = true;
            saveHighScore();
            return;
        }

        // check self-collision (when move into body). If moving into current tail (allowed if not growing),
        // we will treat it as non-collision because tail will move out. Simpler: check excluding tail:
        bool intoBody = false;
        for (size_t i = 0; i < snake.body.size()-1; ++i) {
            if (snake.body[i] == next) { intoBody = true; break; }
        }
        if (intoBody) {
            gameOver = true;
            saveHighScore();
            return;
        }

        // eat?
        if (next == food.p) {
            snake.grow(next);
            score += 10;
            place_food();
        } else {
            snake.move(next);
        }

        // additional guard
        if (snake.collides_with_self()) {
            gameOver = true;
            saveHighScore();
        }
    }

    void place_food() {
        // random location not on snake
        static std::mt19937 rng((unsigned)time(nullptr) ^ (unsigned)clock());
        uniform_int_distribution<int> dr(0, rows-1);
        uniform_int_distribution<int> dc(0, cols-1);
        Point p;
        int tries = 0;
        do {
            p.r = dr(rng);
            p.c = dc(rng);
            tries++;
            // if snake fills most of board, avoid infinite loop: if tries huge, break
            if (tries > 10000) break;
        } while (snake.occupies(p));
        food.p = p;
    }

    void clear_screen() {
        cout << "\033[H\033[J"; // move home + clear
    }

    void render() {
        // Drawing border and grid:
        // Top border: +----+
        clear_screen();

        // top info
        cout << "SNAKE (WASD or arrows). Score: " << score << "  High: " << highScore << "   (Q to quit)\n";

        // top border
        cout << '+';
        for (int c = 0; c < cols; ++c) cout << '-';
        cout << "+\n";

        for (int r = 0; r < rows; ++r) {
            cout << '|';
            for (int c = 0; c < cols; ++c) {
                Point p{r,c};
                if (p == snake.head()) cout << 'O';
                else if (snake.occupies(p)) cout << 'o';
                else if (p == food.p) cout << '*';
                else cout << ' ';
            }
            cout << "|\n";
        }

        // bottom border
        cout << '+';
        for (int c = 0; c < cols; ++c) cout << '-';
        cout << "+\n";
        cout << flush;
    }

    void render_instructions() {
        clear_screen();
        cout << "=== Terminal Snake ===\n\n"
             << "Controls:\n"
             << "  - W/A/S/D or Arrow keys to move\n"
             << "  - Q to quit\n\n"
             << "Goal: Eat '*' to grow. Avoid borders and yourself.\n\n"
             << "Press any movement key to start. Game will start in 1 second...\n";
        cout << flush;
    }

    void handle_game_over() {
        saveHighScore();
        // show final screen
        cout << "\n\nGAME OVER! Final score: " << score << "\n";
        cout << "High score: " << highScore << "\n";
        cout << "Press 'r' to restart or 'q' to quit.\n";
        cout << flush;
    }

    void cleanup_and_exit() {
        // destructor of Terminal will restore settings
        clear_screen();
        cout << "Thanks for playing. Bye!\n";
        cout << flush;
    }
};

int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // optionally allow grid size from args
    int R = 20, C = 40;
    if (argc >= 3) {
        try {
            R = max(10, stoi(argv[1]));
            C = max(10, stoi(argv[2]));
        } catch(...) {}
    }

    Game g(R, C);
    g.run_loop();

    return 0;
}
