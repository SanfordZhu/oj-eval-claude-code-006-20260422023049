#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <queue>

int rows;
int columns;
int total_mines;
int game_state;  // 0 ongoing, 1 win, -1 lose

static std::vector<std::vector<bool>> is_mine;
static std::vector<std::vector<int>> adj;
static std::vector<std::vector<bool>> visited;
static std::vector<std::vector<bool>> marked_mine;   // user-marked mines (only valid if the cell is actually a mine)
static std::vector<std::vector<bool>> wrong_mark;    // user marked a non-mine → immediate lose, render as 'X'
static int visited_safe_count;

static bool in_bounds(int r, int c) { return r >= 0 && r < rows && c >= 0 && c < columns; }

static void compute_adj() {
  adj.assign(rows, std::vector<int>(columns, 0));
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (is_mine[r][c]) continue;
      int cnt = 0;
      for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
          if (dr == 0 && dc == 0) continue;
          int nr = r + dr, nc = c + dc;
          if (in_bounds(nr, nc) && is_mine[nr][nc]) ++cnt;
        }
      }
      adj[r][c] = cnt;
    }
  }
}

static void try_win() {
  if (visited_safe_count == rows * columns - total_mines) {
    game_state = 1;
  }
}

static void flood_zero(int sr, int sc) {
  std::queue<std::pair<int, int>> q;
  q.emplace(sr, sc);
  while (!q.empty()) {
    auto [r, c] = q.front();
    q.pop();
    for (int dr = -1; dr <= 1; ++dr) {
      for (int dc = -1; dc <= 1; ++dc) {
        if (dr == 0 && dc == 0) continue;
        int nr = r + dr, nc = c + dc;
        if (!in_bounds(nr, nc)) continue;
        if (visited[nr][nc]) continue;
        if (is_mine[nr][nc]) continue;
        visited[nr][nc] = true;
        ++visited_safe_count;
        if (adj[nr][nc] == 0) q.emplace(nr, nc);
      }
    }
  }
}

void InitMap() {
  std::cin >> rows >> columns;
  is_mine.assign(rows, std::vector<bool>(columns, false));
  visited.assign(rows, std::vector<bool>(columns, false));
  marked_mine.assign(rows, std::vector<bool>(columns, false));
  wrong_mark.assign(rows, std::vector<bool>(columns, false));
  total_mines = 0;
  visited_safe_count = 0;
  game_state = 0;
  for (int r = 0; r < rows; ++r) {
    std::string line;
    std::cin >> line;
    for (int c = 0; c < columns; ++c) {
      if (line[c] == 'X') {
        is_mine[r][c] = true;
        ++total_mines;
      }
    }
  }
  compute_adj();
}

void VisitBlock(int r, int c) {
  if (!in_bounds(r, c) || game_state != 0) return;
  if (visited[r][c] || marked_mine[r][c] || wrong_mark[r][c]) { game_state = 0; return; }
  if (is_mine[r][c]) {
    visited[r][c] = true;  // show as 'X' in PrintMap
    game_state = -1;
    return;
  }
  visited[r][c] = true;
  ++visited_safe_count;
  if (adj[r][c] == 0) flood_zero(r, c);
  try_win();
}

void MarkMine(int r, int c) {
  if (!in_bounds(r, c) || game_state != 0) return;
  if (visited[r][c] || marked_mine[r][c] || wrong_mark[r][c]) { game_state = 0; return; }
  if (is_mine[r][c]) {
    marked_mine[r][c] = true;
    game_state = 0;
    try_win();
  } else {
    wrong_mark[r][c] = true;  // render as 'X'
    game_state = -1;
  }
}

void AutoExplore(int r, int c) {
  if (!in_bounds(r, c) || game_state != 0) return;
  if (!visited[r][c] || is_mine[r][c]) { game_state = 0; return; }
  int need = adj[r][c];
  int marked = 0;
  for (int dr = -1; dr <= 1; ++dr) {
    for (int dc = -1; dc <= 1; ++dc) {
      if (dr == 0 && dc == 0) continue;
      int nr = r + dr, nc = c + dc;
      if (!in_bounds(nr, nc)) continue;
      if (marked_mine[nr][nc]) ++marked;
    }
  }
  if (marked != need) { game_state = 0; return; }
  for (int dr = -1; dr <= 1; ++dr) {
    for (int dc = -1; dc <= 1; ++dc) {
      if (dr == 0 && dc == 0) continue;
      int nr = r + dr, nc = c + dc;
      if (!in_bounds(nr, nc)) continue;
      if (visited[nr][nc]) continue;
      if (is_mine[nr][nc]) continue;
      visited[nr][nc] = true;
      ++visited_safe_count;
      if (adj[nr][nc] == 0) flood_zero(nr, nc);
    }
  }
  try_win();
}

void ExitGame() {
  if (game_state == 1) {
    std::cout << "YOU WIN!" << std::endl;
    std::cout << visited_safe_count << " " << total_mines << std::endl;
  } else if (game_state == -1) {
    int correct_marked = 0;
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < columns; ++c) {
        if (is_mine[r][c] && marked_mine[r][c]) ++correct_marked;
      }
    }
    std::cout << "GAME OVER!" << std::endl;
    std::cout << visited_safe_count << " " << correct_marked << std::endl;
  }
  exit(0);
}

void PrintMap() {
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      char ch = '?';
      if (game_state == 1) {
        ch = is_mine[r][c] ? '@' : ('0' + adj[r][c]);
      } else {
        if (wrong_mark[r][c]) {
          ch = 'X';
        } else if (marked_mine[r][c]) {
          ch = '@';
        } else if (visited[r][c]) {
          ch = is_mine[r][c] ? 'X' : ('0' + adj[r][c]);
        } else {
          ch = '?';
        }
      }
      std::cout << ch;
    }
    std::cout << std::endl;
  }
}

#endif
