#include <chrono>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace chrono;

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

const char WALL = '#';
const char PATH = '.';
const char START = 'S';
const char END = 'E';
const char PLAYER = 'P';

typedef pair<int, int> Position;

void print_maze(const vector<string> &maze, const Position &player_pos) {
    for (int i = 0; i < maze.size(); ++i) {
        for (int j = 0; j < maze[i].length(); ++j) {
            if (player_pos.first == i && player_pos.second == j) {
                cout << PLAYER;
            } else {
                cout << maze[i][j];
            }
        }
        cout << endl;
    }
}

bool is_valid_move(const vector<string> &maze, int x, int y) {
    if (x >= 0 && x < maze.size() && y >= 0 && y < maze[x].length()) {
        return maze[x][y] == PATH || maze[x][y] == END;
    }
    return false;
}

#ifdef _WIN32
void clear_screen() {
    system("cls");
}

int getch() {
    return _getch();
}

void sleep_ms(int milliseconds) {
    Sleep(milliseconds);
}
#else
void clear_screen() {
    system("clear");
}

int getch() {
    int ch;
    struct termios old_t, new_t;
    tcgetattr(STDIN_FILENO, &old_t);
    new_t = old_t;
    new_t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_t);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
    return ch;
}

void sleep_ms(int milliseconds) {
    usleep(milliseconds * 1000);
}
#endif

bool play_game(vector<string> &maze, Position &start_pos, Position &end_pos,
               const duration<int> &time_limit) {
    Position player_pos = start_pos;
    auto start_time = steady_clock::now();

    while (true) {
        clear_screen();
        print_maze(maze, player_pos);

        auto elapsed_time =
            duration_cast<seconds>(steady_clock::now() - start_time);
        cout << "Time remaining: " << (time_limit - elapsed_time).count()
             << " seconds" << endl;

        if (elapsed_time >= time_limit) {
            return false;
        }

        char move = getch();
        int new_x = player_pos.first;
        int new_y = player_pos.second;

        switch (tolower(move)) {
        case 'w':
            new_x--;
            break;
        case 's':
            new_x++;
            break;
        case 'a':
            new_y--;
            break;
        case 'd':
            new_y++;
            break;
        default:
            cout << "Invalid Input." << endl;
            sleep_ms(500); // Sleep for 500 milliseconds
            continue;
        }

        if (is_valid_move(maze, new_x, new_y)) {
            player_pos = make_pair(new_x, new_y);
        } else if (maze[new_x][new_y] == WALL) {
            player_pos = start_pos;
        }
        if (player_pos == end_pos) {
            return true;
        }
    }
}

int main() {
    vector<string> maze = {
        "####################", "#..................#", "#.######.########.##",
        "#S#...##...........#", "###.#.##.####.##.#.#", "#.#.#..E#.###.##.#.#",
        "#.#.####.#.##.##...#", "#...#.##.#....##.#.#", "###.#.##.#####.#..##",
        "#...#.....#.....#..#", "#.###.###.#.#.#.##.#", "#.....###...#......#",
        "####################"};

    Position start_pos, end_pos;

    for (int i = 0; i < maze.size(); ++i) {
        for (int j = 0; j < maze[i].length(); ++j) {
            if (maze[i][j] == START) {
                start_pos = make_pair(i, j);
            } else if (maze[i][j] == END) {
                end_pos = make_pair(i, j);
            }
        }
    }

    system("clear");

    cout << "'.' represents an open path" << endl;
    cout << "'#' represents a wall or obstacle" << endl;
    cout << "'S' represents the start position" << endl;
    cout << "'E' represents the end position" << endl;
    cout << "'P' represents the player" << endl;
    cout << "Escape the maze using W/A/S/D" << endl;
    cout << "If you hit the wall, you return back to the start!" << endl;
    cout << "You have 1 minute to complete the maze. Good luck!" << endl;

    // Wait for the user to press Enter
    cout << "Press Enter to start the game...";
    cin.ignore(); // Ignore any input until Enter is pressed

    duration<int> time_limit(60);
    bool success = play_game(maze, start_pos, end_pos, time_limit);
    system("clear");

    if (success) {
        cout << "Congratulations! You have completed the maze." << endl;
    } else {
        cout << "Time's up! You did not complete the maze within the time limit."
             << endl;
    }

    return 0;
}