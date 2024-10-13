#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <signal.h>

using namespace std;

const int ROWS = 5;
const int COLS = 5;

void printGrids(char playerGrid[][COLS])
{
  for (int i = 0; i < ROWS; ++i)
  {
    for (int j = 0; j < COLS; ++j)
    {
      cout << playerGrid[i][j] << " ";
    }
    cout << endl;
  }
}

bool checkEmpty(char playerGrid[][COLS], int shipNum, int row, int col, int pos)
{
  if (pos == 0)
  {
    if (col + shipNum <= COLS)
    {
      for (int i = col; i < col + shipNum; i++)
      {
        if (playerGrid[row][i] != '-')
          return false;
      }
      for (int i = col; i < col + shipNum; i++)
      {
        char ch = shipNum + '0';
        playerGrid[row][i] = ch;
      }
      return true;
    }
  }
  else if (pos == 1)
  {
    if (row + shipNum <= ROWS)
    {
      for (int i = row; i < row + shipNum; ++i)
      {
        if (playerGrid[i][col] != '-')
          return false;
      }
      for (int i = row; i < row + shipNum; ++i)
      {
        char ch = shipNum + '0';
        playerGrid[i][col] = ch;
      }
      return true;
    }
  }
  return false;
}

int getValidInput(string prompt, int limit)
{
  int input;
  cout << prompt;
  cin >> input;
  while (input < 0 || input >= limit)
  {
    cout << "Enter a valid value (0 - " << limit - 1 << "): ";
    cin >> input;
  }
  return input;
}

int checkHit(char playerGrid[][COLS], int r, int c)
{
  if (playerGrid[r][c] == '2')
  {
    playerGrid[r][c] = '-';
    return 2;
  }
  else if (playerGrid[r][c] == '3')
  {
    playerGrid[r][c] = '-';

    return 3;
  }
  else if (playerGrid[r][c] == '4')
  {
    playerGrid[r][c] = '-';

    return 4;
  }
  else
  {

    return 0;
  }
}

int main()
{
  int fd1[2], fd2[2];

  int pipe1 = pipe(fd1);
  int pipe2 = pipe(fd2);

  if (pipe1 < 0 || pipe2 < 0)
  {
    cerr << "Error creating pipes" << endl;
    exit(EXIT_FAILURE);
  }

  pid_t p = fork();

  if (p < 0)
  {
    cout << "Error creating Pipes" << endl;
    exit(EXIT_FAILURE);
  }

  if (p == 0)
  {
    close(fd1[0]);
    close(fd2[1]);
    char playerGrid[ROWS][COLS];
    int row = -1, col = -1, pos = -1;

    for (int i = 0; i < ROWS; ++i)
    {
      for (int j = 0; j < COLS; ++j)
      {

        playerGrid[i][j] = '-';
      }
    }

    for (int i = 2; i < 5; ++i)
    {
      cout << "Player " << 1 << " Enter the ROW for ship " << i - 1 << " of size " << i << " ";
      cin >> row;
      while (row < 0 || row > 5)
      {
        cout << "Enter positive value " << endl;
        cin >> row;
      }
      cout << "Enter the COL for ship " << i - 1 << " of size " << i << " ";
      cin >> col;
      while (col < 0 || col > 5)
      {
        cout << "Enter positive value " << endl;
        cin >> col;
      }
      cout << "Enter  the direction of ship " << i - 1 << " (0 for horizontal, 1 for vertical): ";
      cin >> pos;
      while (pos < 0 || pos > 1)
      {
        cout << "Enter positive value  " << endl;
        cin >> pos;
      }

      while (!checkEmpty(playerGrid, i, row, col, pos))
      {
        cout << "Ship already exists in that position. Please enter new position." << endl;
        cout << "----------------------------------" << endl;
        cout << "Enter the ROW for ship " << i - 1 << " of size " << i << " ";
        cin >> row;
        while (row < 0 || row > 5)
        {
          cout << "Enter positive value" << endl;
          cin >> row;
        }
        cout << "Enter the COL for ship " << i - 1 << " of size " << i;
        cin >> col;
        while (col < 0 || col > 5)
        {
          cout << "Enter positive value " << endl;
          cin >> col;
        }
        cout << "Enter the direction of ship " << i - 1 << " (0 for horizontal, 1 for vertical): ";
        cin >> pos;
        while (pos < 0 || pos > 1)
        {
          cout << "Enter positive value " << endl;
          cin >> pos;
        }
      }
      printGrids(playerGrid);
    }

    int placement = 1;

    write(fd1[1], &placement, sizeof(placement));
    read(fd2[0], &placement, sizeof(placement));

    int ship2 = 2, ship3 = 3, ship4 = 4;
    char h;
    int hit;
    int shipNumHit = 0;

    while (true)
    {
      cout << "----------------------------------" << endl;
      cout << "Player 1 turn " << endl;
      cout << "----------------------------------" << endl;

      cout << "Player 1 : guess the position, enter ROW " << endl;
      row = getValidInput("", ROWS);
      cout << "Player 1 : guess the position, enter COL " << endl;
      col = getValidInput("", COLS);

      write(fd1[1], &row, sizeof(row));
      write(fd1[1], &col, sizeof(col));

      read(fd2[0], &h, sizeof(h));
      read(fd2[0], &shipNumHit, sizeof(shipNumHit));

      if (h == 'H')
      {
        cout << "Ship hit of size " << shipNumHit << " " << endl;
      }
      else
      {
        cout << "Ship not hit " << endl;
      }

      h = '2';
      write(fd1[1], &h, sizeof(h));

      read(fd2[0], &row, sizeof(row));
      read(fd2[0], &col, sizeof(col));

      hit = checkHit(playerGrid, row, col);

      if (hit == 2)
      {
        shipNumHit = 2;
        ship2--;
        h = 'H';
      }
      else if (hit == 3)
      {
        shipNumHit = 3;
        ship3--;
        h = 'H';
      }
      else if (hit == 4)
      {
        shipNumHit = 4;
        ship4--;
        h = 'H';
      }
      else
      {
        shipNumHit = 2;
        h = 'N';
      }

      if (ship2 == 0 && ship3 == 0 && ship4 == 0)
      {
        cout << "Player 2 wins, all ships destroyed" << endl;
        kill(0, SIGTERM);
      }

      write(fd1[1], &h, sizeof(h));
      write(fd1[1], &shipNumHit, sizeof(shipNumHit));

      read(fd2[0], &h, sizeof(h));
    }
  }
  else
  {
    close(fd1[1]);
    close(fd2[0]);
    char playerGrid[ROWS][COLS];
    int row = -1, col = -1, pos = -1;
    int placement;

    read(fd1[0], &placement, sizeof(placement));

    for (int i = 0; i < ROWS; ++i)
    {
      for (int j = 0; j < COLS; ++j)
      {

        playerGrid[i][j] = '-';
      }
    }

    for (int i = 2; i < 5; ++i)
    {
      cout << "----------------------------------" << endl;
      cout << "Player " << 2 << " Enter the ROW for ship " << i - 1 << " of size " << i << " ";
      cin >> row;
      while (row < 0 || row > 5)
      {
        cout << "Enter positive value" << endl;
        cin >> row;
      }
      cout << "Enter the COL for ship " << i - 1 << " of size " << i << " ";
      cin >> col;
      while (col < 0 || col > 5)
      {
        cout << "Enter positive value" << endl;
        cin >> col;
      }
      cout << "Enter  the direction of ship " << i - 1 << " (0 for horizontal, 1 for vertical): ";
      cin >> pos;
      while (pos < 0 || pos > 1)
      {
        cout << "Enter positive value  " << endl;
        cin >> pos;
      }

      while (!checkEmpty(playerGrid, i, row, col, pos))
      {
        cout << "Ship already exists in that position. Please enter new position." << endl;
        cout << "----------------------------------" << endl;
        cout << "Enter the ROW for ship " << i - 1 << " of size " << i << " ";
        cin >> row;
        while (row < 0 || row > 5)
        {
          cout << "Enter positive value " << endl;
          cin >> row;
        }
        cout << "Enter the COL for ship " << i - 1 << " of size " << i;
        cin >> col;
        while (col < 0 || col > 5)
        {
          cout << "Enter positive value" << endl;
          cin >> col;
        }
        cout << "Enter the direction of ship " << i - 1 << " (0 for horizontal, 1 for vertical): ";
        cin >> pos;
        while (pos < 0 || pos > 1)
        {
          cout << "Enter positive value " << endl;
          cin >> pos;
        }
      }
      printGrids(playerGrid);
    }

    write(fd2[1], &placement, sizeof(placement));

    int ship2 = 2, ship3 = 3, ship4 = 4;
    char h;
    int hit;
    int shipNumHit = 0;

    while (true)
    {

      read(fd1[0], &row, sizeof(row));
      read(fd1[0], &col, sizeof(col));

      hit = checkHit(playerGrid, row, col);

      if (hit == 2)
      {
        ship2--;
        shipNumHit = 2;
        h = 'H';
      }
      else if (hit == 3)
      {
        ship3--;
        shipNumHit = 3;

        h = 'H';
      }
      else if (hit == 4)
      {
        ship4--;
        shipNumHit = 4;

        h = 'H';
      }
      else
      {
        h = 'N';
        shipNumHit = 0;
      }

      if (ship2 == 0 && ship3 == 0 && ship4 == 0)
      {
        cout << "Player 1 wins, all ships destroyed" << endl;
        kill(0, SIGTERM);
      }

      write(fd2[1], &h, sizeof(h));
      write(fd2[1], &shipNumHit, sizeof(shipNumHit));

      read(fd1[0], &h, sizeof(h));

      cout << "-------------------------------" << endl;
      cout << "Player 2 turn" << endl;
      cout << "----------------------------------" << endl;

      cout << "Player 2 : guess the position, enter ROW " << endl;
      row = getValidInput("", ROWS);
      cout << "Player 2 : guess the position, enter COL " << endl;
      col = getValidInput("", COLS);

      write(fd2[1], &row, sizeof(row));
      write(fd2[1], &col, sizeof(col));

      read(fd1[0], &h, sizeof(h));
      read(fd1[0], &shipNumHit, sizeof(shipNumHit));

      if (h == 'H')
      {
        cout << "Ship hit of size " << shipNumHit << " " << endl;
      }
      else
      {
        cout << "Ship not hit " << endl;
      }
      h = '1';
      write(fd2[1], &h, sizeof(h));
    }
  }
}
