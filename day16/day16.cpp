#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

int64_t parse_number(std::string_view s) {
  int32_t length = (int32_t)s.length();
  int64_t value = 0;
  for (int32_t idx = 0; idx < length; ++idx) {
    value += (s[idx] - '0') * (int64_t)std::pow(10, length - idx - 1);
  }
  return value;
}
std::vector<int64_t> parse_string_to_vec_numbers(char *line, size_t length,
                                                 char delimiter = ' ') {
  std::vector<int64_t> numbers;
  enum class Sign {
    Pos,
    Neg,
  };
  std::vector<std::pair<Sign, std::string_view>> num_strings;
  std::optional<int32_t> digit_start;
  for (size_t num_parser_idx = 0; num_parser_idx < length; ++num_parser_idx) {
    if (((line[num_parser_idx] >= '0' && line[num_parser_idx] <= '9') ||
         line[num_parser_idx] == '-') &&
        !digit_start.has_value()) {
      digit_start = num_parser_idx;
    }
    if ((line[num_parser_idx] == delimiter || num_parser_idx == length - 1) &&
        digit_start.has_value()) {
      if (num_parser_idx == length - 1)
        num_parser_idx = length;
      bool has_neg = line[*digit_start] == '-';
      int32_t actual_digit_start = has_neg ? *digit_start + 1 : *digit_start;
      num_strings.push_back(
          {has_neg ? Sign::Neg : Sign::Pos,
           std::string_view(line + actual_digit_start,
                            num_parser_idx - actual_digit_start)});
      digit_start.reset();
    }
  }
  for (auto [sign, seed_str] : num_strings) {
    auto num = parse_number(seed_str);
    switch (sign) {
    case Sign::Pos:
      numbers.push_back(num);
      break;
    case Sign::Neg:
      numbers.push_back(-num);
      break;
    }
  }
  return numbers;
}
template <typename T>
  requires std::is_integral_v<T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  for (const T &n : v) {
    os << n << " ";
  }
  os << std::endl;
  return os;
}
struct Coord {
  size_t x;
  size_t y;
};
std::ostream &operator<<(std::ostream &os, const Coord &c) {
  os << std::format("x: {}, y: {}\n", c.x, c.y);
  return os;
}
bool operator==(const Coord &a, const Coord &b) {
  return a.x == b.x && a.y == b.y;
}
struct Range {
  size_t start;
  size_t length;
};

enum class Direction {
  Top,
  Bottom,
  Left,
  Right,
};
struct Mirror {
  Direction d;
  Coord c;
};
bool operator==(const Mirror &one, const Mirror &two) {
  return one.d == two.d && one.c == two.c;
}

void go_left(const Mirror &t, const std::vector<std::string> &contraption,
             const std::vector<Mirror> &already_processed,
             std::stack<Mirror> &mirrors, std::vector<Coord> &traveled) {
  size_t col_going_left = t.c.x;
  if (col_going_left == 0) {
    return;
  }
  --col_going_left;
  while (true) {
    traveled.push_back(Coord{.x = col_going_left, .y = t.c.y});
    if (contraption[t.c.y][col_going_left] != '.') {
      auto find_result =
          std::find(already_processed.begin(), already_processed.end(),
                    Mirror{.d = Direction::Right,
                           .c = Coord{.x = col_going_left, .y = t.c.y}});
      if (find_result == already_processed.end()) {
        mirrors.push(Mirror{.d = Direction::Right,
                            .c = Coord{.x = col_going_left, .y = t.c.y}});
      }
      break;
    }
    if (col_going_left == 0) {
      break;
    }
    col_going_left -= 1;
  }
}

void go_right(const Mirror &t, const std::vector<std::string> &contraption,
              const std::vector<Mirror> &already_processed,
              std::stack<Mirror> &mirrors, std::vector<Coord> &traveled) {
  size_t col_going_right = t.c.x;
  col_going_right += 1;
  while (col_going_right < contraption[0].length()) {
    traveled.push_back(Coord{.x = col_going_right, .y = t.c.y});
    if (contraption[t.c.y][col_going_right] != '.') {
      auto find_result =
          std::find(already_processed.begin(), already_processed.end(),
                    Mirror{.d = Direction::Left,
                           .c = Coord{.x = col_going_right, .y = t.c.y}});
      if (find_result == already_processed.end()) {
        mirrors.push(Mirror{.d = Direction::Left,
                            .c = Coord{.x = col_going_right, .y = t.c.y}});
      }
      break;
    }
    col_going_right += 1;
  }
}
void go_up(const Mirror &t, const std::vector<std::string> &contraption,
           const std::vector<Mirror> &already_processed,
           std::stack<Mirror> &mirrors, std::vector<Coord> &traveled) {
  size_t row_going_up = t.c.y;
  if (row_going_up == 0) {
    return;
  }
  row_going_up -= 1;
  while (true) {
    traveled.push_back(Coord{.x = t.c.x, .y = row_going_up});
    if (contraption[row_going_up][t.c.x] != '.') {
      auto find_result =
          std::find(already_processed.begin(), already_processed.end(),
                    Mirror{.d = Direction::Bottom,
                           .c = Coord{.x = t.c.x, .y = row_going_up}});
      if (find_result == already_processed.end()) {
        mirrors.push(Mirror{.d = Direction::Bottom,
                            .c = Coord{.x = t.c.x, .y = row_going_up}});
      }
      break;
    }
    if (row_going_up == 0) {
      break;
    }
    row_going_up -= 1;
  }
}
void go_down(const Mirror &t, const std::vector<std::string> &contraption,
             const std::vector<Mirror> &already_processed,
             std::stack<Mirror> &mirrors, std::vector<Coord> &traveled) {
  size_t row_going_down = t.c.y;
  row_going_down += 1;
  while (row_going_down < contraption.size()) {
    traveled.push_back(Coord{.x = t.c.x, .y = row_going_down});
    if (contraption[row_going_down][t.c.x] != '.') {
      auto find_result =
          std::find(already_processed.begin(), already_processed.end(),
                    Mirror{.d = Direction::Top,
                           .c = Coord{.x = t.c.x, .y = row_going_down}});
      if (find_result == already_processed.end()) {
        mirrors.push(Mirror{.d = Direction::Top,
                            .c = Coord{.x = t.c.x, .y = row_going_down}});
      }
      break;
    }
    row_going_down += 1;
  }
}

size_t run_beam(const std::vector<std::string> &contraption, Coord initial_tile,
                Direction start) {
  std::vector<Mirror> already_processed;
  std::stack<Mirror> mirrors;
  std::vector<Coord> traveled;
  switch (start) {
  case Direction::Top: {
    size_t row = initial_tile.y;
    while (true) {
      traveled.push_back(Coord{.x = initial_tile.x, .y = row});
      if (contraption[row][initial_tile.x] != '.') {
        mirrors.push(Mirror{.d = Direction::Bottom,
                            .c = Coord{.x = initial_tile.x, .y = row}});
        break;
      }
      if (row == 0) {
        break;
      }
      row -= 1;
    }
  } break;
  case Direction::Bottom: {
    for (size_t row = initial_tile.y; row < contraption.size(); ++row) {
      traveled.push_back(Coord{.x = initial_tile.x, .y = row});
      if (contraption[row][initial_tile.x] != '.') {
        mirrors.push(Mirror{.d = Direction::Top,
                            .c = Coord{.x = initial_tile.x, .y = row}});
        break;
      }
    }
  } break;
  case Direction::Left: {
    size_t col = initial_tile.x;
    while (true) {
      traveled.push_back(Coord{.x = col, .y = initial_tile.y});
      if (contraption[initial_tile.y][col] != '.') {
        mirrors.push(Mirror{.d = Direction::Right,
                            .c = Coord{.x = col, .y = initial_tile.y}});
        break;
      }
      if (col == 0) {
        break;
      }
      col -= 1;
    }
  } break;
  case Direction::Right: {
    for (size_t col = initial_tile.x; col < contraption[0].length(); ++col) {
      traveled.push_back(Coord{.x = col, .y = initial_tile.y});
      if (contraption[initial_tile.y][col] != '.') {
        mirrors.push(Mirror{.d = Direction::Left,
                            .c = Coord{.x = col, .y = initial_tile.y}});
        break;
      }
    }
  } break;
  }
  while (!mirrors.empty()) {
    Mirror t = mirrors.top();
    already_processed.push_back(t);
    mirrors.pop();
    char c = contraption[t.c.y][t.c.x];
    assert(c != '.');
    Direction coming_from = t.d;
    switch (c) {
    case '-': {
      switch (coming_from) {
      case Direction::Top:
      case Direction::Bottom: {
        go_left(t, contraption, already_processed, mirrors, traveled);
        go_right(t, contraption, already_processed, mirrors, traveled);
      } break;
      case Direction::Left: {
        go_right(t, contraption, already_processed, mirrors, traveled);
      } break;
      case Direction::Right: {
        go_left(t, contraption, already_processed, mirrors, traveled);
      } break;
      }
    } break;
    case '|': {
      switch (coming_from) {
      case Direction::Top: {
        go_down(t, contraption, already_processed, mirrors, traveled);
      } break;
      case Direction::Bottom: {
        go_up(t, contraption, already_processed, mirrors, traveled);
      } break;
      case Direction::Left:
      case Direction::Right: {
        go_up(t, contraption, already_processed, mirrors, traveled);
        go_down(t, contraption, already_processed, mirrors, traveled);
      } break;
      }
    } break;
    case '/': {
      switch (coming_from) {
      case Direction::Top: {
        go_left(t, contraption, already_processed, mirrors, traveled);
      } break;
      case Direction::Bottom: {
        go_right(t, contraption, already_processed, mirrors, traveled);
      } break;
      case Direction::Left: {
        go_up(t, contraption, already_processed, mirrors, traveled);
      } break;
      case Direction::Right: {
        go_down(t, contraption, already_processed, mirrors, traveled);
      } break;
      }
    } break;
    case '\\': {
      switch (coming_from) {
      case Direction::Top: {
        go_right(t, contraption, already_processed, mirrors, traveled);
      } break;
      case Direction::Bottom: {
        go_left(t, contraption, already_processed, mirrors, traveled);
      } break;
      case Direction::Left: {
        go_down(t, contraption, already_processed, mirrors, traveled);
      } break;
      case Direction::Right: {
        go_up(t, contraption, already_processed, mirrors, traveled);
      } break;
      }
    } break;
    }
  }
  for (size_t idx = 0; idx < traveled.size();) {
    auto find_res =
        std::find(traveled.begin() + idx + 1, traveled.end(), traveled[idx]);
    if (find_res != traveled.end()) {
      traveled.erase(traveled.begin() + idx);
      continue;
    }
    ++idx;
  }
  return traveled.size();
}

int32_t main(int32_t argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "called the binary wrong\n";
    return 1;
  }
  std::ifstream input(argv[1]);
  if (!input.is_open()) {
    std::cerr << argv[1] << " file cannot be opened\n";
    return 1;
  }
  const size_t LINE_CAPACITY = 1 << 16;
  char line[LINE_CAPACITY];
  std::vector<std::string> contraption;
  while (input.getline(line, LINE_CAPACITY)) {
    size_t line_length = input.gcount() - 1;
    contraption.push_back(std::string(line, line_length));
  }
  std::cout << std::format(
      "PART1: {}\n",
      run_beam(contraption, Coord{.x = 0, .y = 0}, Direction::Right));
  size_t part2 = 0;
  for (size_t col = 0; col < contraption[0].length(); ++col) {
    part2 = std::max(part2, run_beam(contraption, Coord{.x = col, .y = 0},
                                     Direction::Bottom));
  }
  for (size_t row = 0; row < contraption.size(); ++row) {
    part2 = std::max(part2, run_beam(contraption, Coord{.x = 0, .y = row},
                                     Direction::Right));
  }
  for (size_t row = 0; row < contraption.size(); ++row) {
    part2 =
        std::max(part2, run_beam(contraption,
                                 Coord{.x = contraption[0].length() - 1, .y = row},
                                 Direction::Left));
  }
  for (size_t col = 0; col < contraption[0].length(); ++col) {
    part2 = std::max(part2, run_beam(contraption, Coord{.x = col, .y = contraption.size() - 1},
                                     Direction::Top));
  }
  std::cout << std::format("PART2: {}\n", part2);
  return 0;
}
