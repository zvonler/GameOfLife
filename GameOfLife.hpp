#ifndef GAME_OF_LIFE_HPP
#define GAME_OF_LIFE_HPP

template <int ROWS, int COLS>
class GameOfLife
{
    bool _cells[ROWS * COLS];
    byte _accumulator[ROWS * COLS];

  public:
    GameOfLife()
    {
      clear();
    }

    constexpr int rows() {
      return ROWS;
    }
    constexpr int cols() {
      return COLS;
    }

    void clear()
    {
      for (auto& cell : _cells)
        cell = false;
    }

    void randomize()
    {
      for (auto& cell : _cells)
        cell = random(100) < 30;
    }

    bool isAlive(int row, int col)
    {
      return _cells[row * COLS + col];
    }

    void setAlive(int row, int col, bool alive)
    {
      _cells[row * COLS + col] = alive;
    }

    void update()
    {
      accumulateNeighbors();
      updateCells();
    }

  private:
    void accumulateNeighbors()
    {
      memset(_accumulator, 0, ROWS * COLS);

      for (int row = 0; row < ROWS; ++row)
      {
        for (int col = 0; col < COLS; ++col)
        {
          if (!_cells[row * COLS + col]) continue;

          if (row - 1 >= 0)
          {
            // Add to the row below
            accumulateColumnNeighbors(row - 1, col);
          }
          else
          {
            // Add to the top row
            accumulateColumnNeighbors(ROWS - 1, col);
          }

          if (row + 1 < ROWS)
          {
            // Add to the row above
            accumulateColumnNeighbors(row + 1, col);
          }
          else
          {
            // Add to the bottom row
            accumulateColumnNeighbors(0, col);
          }

          // Add to the current row
          if (col - 1 >= 0) {
            _accumulator[row * COLS + (col - 1)]++;
          } else {
            _accumulator[row * COLS + (COLS - 1)]++;
          }
          if (col + 1 < COLS) {
            _accumulator[row * COLS + (col + 1)]++;
          } else {
            _accumulator[row * COLS + 0]++;
          }
        }
      }
    }

    void accumulateColumnNeighbors(int row, int col)
    {
      if (col - 1 >= 0) {
        // Add to column on left
        _accumulator[row * COLS + (col - 1)]++;
      } else {
        // Add to rightmost column
        _accumulator[row * COLS + (COLS - 1)]++;
      }
      if (col + 1 < COLS) {
        // Add to column on right
        _accumulator[row * COLS + (col + 1)]++;
      } else {
        // Add to first column
        _accumulator[row * COLS + 0]++;
      }
      // Add to this column
      _accumulator[row * COLS + col]++;
    }

    void updateCells()
    {
      for (int row = 0; row < ROWS; ++row)
      {
        for (int col = 0; col < COLS; ++col)
        {
          int index = row * COLS + col;
          if (_cells[index])
          {
            _cells[index] = _accumulator[index] == 2 || _accumulator[index] == 3;
          }
          else
          {
            _cells[index] = _accumulator[index] == 3;
          }
        }
      }
    }
};

#endif
