#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>

extern int rows;
extern int columns;
extern int total_mines;

void Execute(int r, int c, int type);

static std::vector<std::vector<char>> seen;

void InitGame() {
  seen.assign(rows, std::vector<char>(columns, '?'));
  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

void ReadMap() {
  seen.assign(rows, std::vector<char>(columns, '?'));
  for (int r = 0; r < rows; ++r) {
    std::string line;
    std::cin >> line;
    for (int c = 0; c < columns; ++c) {
      seen[r][c] = line[c];
    }
  }
}

static bool in_bounds_c(int r, int c) { return r >= 0 && r < rows && c >= 0 && c < columns; }

void Decide() {
  // Auto-explore when marked neighbors equal number
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      char ch = seen[r][c];
      if (ch >= '0' && ch <= '8') {
        int k = ch - '0';
        int marked = 0;
        for (int dr = -1; dr <= 1; ++dr) {
          for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr, nc = c + dc;
            if (!in_bounds_c(nr, nc)) continue;
            if (seen[nr][nc] == '@') ++marked;
          }
        }
        if (marked == k) { Execute(r, c, 2); return; }
      }
    }
  }
  // Mark obvious mines: (number - marked) == unknown
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      char ch = seen[r][c];
      if (ch >= '1' && ch <= '8') {
        int k = ch - '0';
        int marked = 0, unknown = 0, un_r = -1, un_c = -1;
        for (int dr = -1; dr <= 1; ++dr) {
          for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr, nc = c + dc;
            if (!in_bounds_c(nr, nc)) continue;
            if (seen[nr][nc] == '@') ++marked;
            else if (seen[nr][nc] == '?') { ++unknown; un_r = nr; un_c = nc; }
          }
        }
        if (unknown > 0 && (k - marked) == unknown) { Execute(un_r, un_c, 1); return; }
      }
    }
  }
  // Visit any unknown
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (seen[r][c] == '?') { Execute(r, c, 0); return; }
    }
  }
  Execute(0, 0, 0);
}

#endif
