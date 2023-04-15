#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

// Additional functions
// ...

#ifdef _WIN32
void clear_screen() { system("cls"); }
#else
void clear_screen() { system("clear"); }

#endif

char getch() {
  char buf = 0;
#ifdef _WIN32
  buf = _getch();
#else
  termios old = {0};
  if (tcgetattr(0, &old) < 0)
    perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0)
    perror("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror("tcsetattr ~ICANON");
#endif
  return buf;
}

// ...

const int BOARD_SIZE = 5;
const int NUM_GHOSTS = 5;
const int NUM_BULLETS = 23; // total number of bullets the player has

// 2D array to represent the game board
char board[BOARD_SIZE][BOARD_SIZE];

// vector to store the location of each ghost on the board
vector<pair<int, int>> ghosts;

int playerRow = 0;
int playerCol = 0;
int bulletsLeft = NUM_BULLETS; // number of bullets left

void initBoard() {
  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int col = 0; col < BOARD_SIZE; col++) {
      board[row][col] = '.';
    }
  }
}

void drawBoard(bool showGhosts = false) {
  cout << "   ";
  for (int col = 0; col < BOARD_SIZE; col++) {
    cout << col << " ";
  }
  cout << endl;

  for (int row = 0; row < BOARD_SIZE; row++) {
    cout << row << "  ";
    for (int col = 0; col < BOARD_SIZE; col++) {
      if (row == playerRow && col == playerCol) {
        cout << "P ";
      } else if (board[row][col] == 'X' || board[row][col] == '*' ||
                 (showGhosts && board[row][col] == 'G')) {
        cout << board[row][col] << " ";
      } else {
        cout << ". ";
      }
    }
    cout << endl;
  }
}

void placeGhosts() {
  int numPlaced = 0;

  while (numPlaced < NUM_GHOSTS) {
    int row = rand() % BOARD_SIZE;
    int col = rand() % BOARD_SIZE;

    if (board[row][col] != '.') {
      continue;
    }

    board[row][col] = 'G';
    ghosts.push_back(make_pair(row, col));

    numPlaced++;
  }
}

bool playerMove(char move) {
  if (move == 'w' && playerRow > 0) {
    playerRow--;
  } else if (move == 's' && playerRow < BOARD_SIZE - 1) {
    playerRow++;
  } else if (move == 'a' && playerCol > 0) {
    playerCol--;
  } else if (move == 'd' && playerCol < BOARD_SIZE - 1) {
    playerCol++;
  } else if (move == 'f') {
    if (board[playerRow][playerCol] == 'G') {
      board[playerRow][playerCol] = '*';
      cout << "You caught a ghost!" << endl;
      return true;
    } else {
      board[playerRow][playerCol] = 'X';
      cout << "You missed." << endl;
      return false;
    }
  }

  return false;
}

int main() {
  srand(time(NULL)); // seed the random number generator

  initBoard();
  placeGhosts();
  int numCaught = 0;
  int numBullets = NUM_BULLETS;

  while (numCaught < NUM_GHOSTS && numBullets > 0) {

    clear_screen();
    cout << "Welcome to the game! You have " << numBullets
         << " bullets to catch all the ghosts." << endl;
    drawBoard();
    cout << "You have " << numBullets
         << " bullets left. Move using 'w' (up), 'a' (left), 's' (down), 'd' "
            "(right), or 'f' (catch): "
         << endl;
    char move;
    move = getch();
    cout << move << endl; // Echo the pressed key
    if (move == 'f') {
      if (numBullets > 0) {
        numBullets--;
        if (playerMove(move)) {
          numCaught++;
          cout << endl;
          if (numCaught == NUM_GHOSTS) {
            cout << "Congratulations, you caught all the ghosts!" << endl;
            break;
          } else {
            cout << "You still need to catch " << NUM_GHOSTS - numCaught
                 << " more." << endl;
          }
        }
      } else {
        cout << "You don't have any bullets left. ";
      }
    } else {
      playerMove(move);
    }
  }

  if (numBullets == 0) {
    cout << "You're out of bullets! You didn't catch all the ghosts." << endl;
  }

  // print final board and game result
  drawBoard(true);

  if (numCaught == NUM_GHOSTS) {
    cout << "You won the game!" << endl;
  } else {
    cout << "You lost the game." << endl;
  }

  return 0;
}