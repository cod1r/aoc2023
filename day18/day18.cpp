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
#include <queue>
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
std::vector<int64_t> parse_string_to_vec_numbers(char *line, int64_t length,
                                                 char delimiter = ' ') {
  std::vector<int64_t> numbers;
  enum class Sign {
    Pos,
    Neg,
  };
  std::vector<std::pair<Sign, std::string_view>> num_strings;
  std::optional<int32_t> digit_start;
  for (int64_t num_parser_idx = 0; num_parser_idx < length; ++num_parser_idx) {
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
  int64_t x;
  int64_t y;
  Coord &operator=(const Coord &c) {
    this->x = c.x;
    this->y = c.y;
    return *this;
  }
  Coord() = default;
  Coord(const Coord &c) { *this = c; }
  Coord(int64_t x, int64_t y) {
    this->x = x;
    this->y = y;
  }
};
std::ostream &operator<<(std::ostream &os, const Coord &c) {
  os << std::format("x: {}, y: {}", c.x, c.y);
  return os;
}
bool operator==(const Coord &a, const Coord &b) {
  return a.x == b.x && a.y == b.y;
}
struct Range {
  int64_t start;
  int64_t length;
  Range() = default;
  Range(int64_t s, int64_t l) {
    start = s;
    length = l;
  }
};
bool operator==(const Range &a, const Range &b) {
  return a.start == b.start && a.length == b.length;
}
struct Region {
  Coord top_left;
  Coord top_right;
  Coord bottom_left;
  Coord bottom_right;
  Region(Coord one, Coord two, Coord three, Coord four) {
    top_left = one;
    top_right = two;
    bottom_left = three;
    bottom_right = four;
  }
  int64_t get_area() const & {
    return (top_right.x - top_left.x + 1) * (bottom_left.y - top_left.y + 1);
  }
};
enum class Direction {
  U,
  D,
  L,
  R,
};
Direction d_from_char(char c) {
  switch (c) {
  case 'R':
    return Direction::R;
  case 'L':
    return Direction::L;
  case 'U':
    return Direction::U;
  }
  return Direction::D;
}
struct DigInstruction {
  Direction d;
  int64_t amt;
  std::string hex;
  DigInstruction(Direction dir, int64_t a, std::string hexx) {
    d = dir;
    amt = a;
    hex = hexx;
  }
};
bool dfs_is_inside(std::vector<std::string> &matrix, const int64_t max_row,
                   const int64_t min_row, const int64_t max_col,
                   const int64_t min_col, int64_t row, int64_t col) {
  if (row < min_row || row > max_row || col < min_col || col > max_col) {
    return false;
  }
  if (matrix[row - min_row][col - min_col] == '1') {
    return true;
  }
  matrix[row - min_row][col - min_col] = '1';
  bool down =
      dfs_is_inside(matrix, max_row, min_row, max_col, min_col, row + 1, col);
  bool right =
      dfs_is_inside(matrix, max_row, min_row, max_col, min_col, row, col + 1);
  bool up =
      dfs_is_inside(matrix, max_row, min_row, max_col, min_col, row - 1, col);
  bool left =
      dfs_is_inside(matrix, max_row, min_row, max_col, min_col, row, col - 1);
  return down && right && up && left;
}
void dfs_mark(std::vector<std::string> &matrix, int64_t row, int64_t col,
              const int64_t min_row, const int64_t min_col) {
  if (matrix[row - min_row][col - min_col] == '1') {
    return;
  }
  matrix[row - min_row][col - min_col] = '1';
  dfs_mark(matrix, row + 1, col, min_row, min_col);
  dfs_mark(matrix, row, col + 1, min_row, min_col);
  dfs_mark(matrix, row - 1, col, min_row, min_col);
  dfs_mark(matrix, row, col - 1, min_row, min_col);
}
int64_t get_area(std::vector<std::string> &matrix, int64_t max_row,
                 int64_t min_row, int64_t max_col, int64_t min_col) {
  for (int64_t row_check = min_row; row_check <= max_row; ++row_check) {
    for (int64_t col_check = min_col; col_check <= max_col; ++col_check) {
      if (matrix[row_check - min_row][col_check - min_col] == '1') {
        if (row_check + 1 < (int64_t)matrix.size() &&
            matrix[row_check - min_row][col_check - min_col] == ' ') {
          std::vector<std::string> matrix_copy = matrix;
          bool inside = dfs_is_inside(matrix_copy, max_row, min_row, max_col,
                                      min_col, row_check + 1, col_check);
          if (inside) {
            dfs_mark(matrix, row_check + 1, col_check, min_row, min_col);
            int64_t total_area = std::accumulate(
                matrix.begin(), matrix.end(), int64_t{0},
                [](int64_t acc, const std::string &row) {
                  return acc + std::accumulate(row.begin(), row.end(),
                                               int64_t{0},
                                               [](int64_t acc, const char &c) {
                                                 if (c == '1')
                                                   return acc + 1;
                                                 return acc;
                                               });
                });
            return total_area;
          }
        }
        if (row_check > min_row &&
            matrix[row_check - 1 - min_row][col_check - min_col] == ' ') {
          std::vector<std::string> matrix_copy = matrix;
          bool inside = dfs_is_inside(matrix_copy, max_row, min_row, max_col,
                                      min_col, row_check - 1, col_check);
          if (inside) {
            dfs_mark(matrix, row_check - 1, col_check, min_row, min_col);
            int64_t total_area = std::accumulate(
                matrix.begin(), matrix.end(), int64_t{0},
                [](int64_t acc, const std::string &row) {
                  return acc + std::accumulate(row.begin(), row.end(),
                                               int64_t{0},
                                               [](int64_t acc, const char &c) {
                                                 if (c == '1')
                                                   return acc + 1;
                                                 return acc;
                                               });
                });
            return total_area;
          }
        }
      }
    }
  }
  return 0;
}
enum class InsideSide {
  U,
  D,
  L,
  R,
};
std::ostream &operator<<(std::ostream &os, InsideSide is) {
  switch (is) {
  case InsideSide::U:
    os << "UP";
    break;
  case InsideSide::D:
    os << "DOWN";
    break;
  case InsideSide::L:
    os << "LEFT";
    break;
  case InsideSide::R:
    os << "RIGHT";
    break;
  }
  return os;
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
  const int64_t LINE_CAPACITY = 1 << 16;
  char line[LINE_CAPACITY];
  std::vector<DigInstruction> instructions;
  while (input.getline(line, LINE_CAPACITY)) {
    int64_t line_length = input.gcount() - 1;
    std::string line_str(line, line_length);
    auto idx_of_s = line_str.find(' ', 2);
    int64_t amt = parse_number(
        std::string_view(line_str.begin() + 2, line_str.begin() + idx_of_s));
    instructions.push_back(DigInstruction(
        d_from_char(line[0]), amt,
        std::string(line + idx_of_s + 1, line_length - (idx_of_s + 1))));
  }
  input.close();

  int64_t row_offset = 0;
  int64_t col_offset = 0;
  std::vector<Coord> coords;
  for (const DigInstruction &di : instructions) {
    switch (di.d) {
    case Direction::U: {
      int64_t row_end = row_offset - di.amt;
      while (row_offset > row_end) {
        coords.push_back(Coord(col_offset, row_offset));
        row_offset -= 1;
      }
    } break;
    case Direction::D: {
      int64_t row_end = row_offset + di.amt;
      while (row_offset < row_end) {
        coords.push_back(Coord(col_offset, row_offset));
        row_offset += 1;
      }
    } break;
    case Direction::L: {
      int64_t col_end = col_offset - di.amt;
      while (col_offset > col_end) {
        coords.push_back(Coord(col_offset, row_offset));
        col_offset -= 1;
      }
    } break;
    case Direction::R: {
      int64_t col_end = col_offset + di.amt;
      while (col_offset < col_end) {
        coords.push_back(Coord(col_offset, row_offset));
        col_offset += 1;
      }
    } break;
    }
  }
  int64_t max_row = std::accumulate(
      coords.begin(), coords.end(), int64_t{0},
      [](int64_t acc, const Coord &c) { return std::max(acc, c.y); });
  int64_t min_row = std::accumulate(
      coords.begin(), coords.end(), int64_t{100000},
      [](int64_t acc, const Coord &c) { return std::min(acc, c.y); });
  int64_t max_col = std::accumulate(
      coords.begin(), coords.end(), int64_t{0},
      [](int64_t acc, const Coord &c) { return std::max(acc, c.x); });
  int64_t min_col = std::accumulate(
      coords.begin(), coords.end(), int64_t{100000},
      [](int64_t acc, const Coord &c) { return std::min(acc, c.x); });

  std::vector<std::string> matrix(max_row - min_row + 1,
                                  std::string(max_col - min_col + 1, ' '));
  for (const Coord &c : coords) {
    matrix[c.y - min_row][c.x - min_col] = '1';
  }
  int64_t area = get_area(matrix, max_row, min_row, max_col, min_col);
  std::cout << std::format("PART1: {}\n", area);

  int64_t y_offset = 0;
  int64_t x_offset = 0;
  std::vector<std::pair<Coord, std::pair<int64_t, InsideSide>>> vertical;
  std::vector<std::pair<Coord, std::pair<int64_t, InsideSide>>> horizontal;
  int64_t max_y = -1000000;
  for (const DigInstruction &di : instructions) {
    int64_t distance = 0;
    for (size_t idx = 2; idx <= 6; ++idx) {
      int64_t value = 0;
      if (di.hex[idx] >= 'a' && di.hex[idx] <= 'f') {
        value = (di.hex[idx] - 'a') + 10;
      } else {
        value = di.hex[idx] - '0';
      }
      distance += (value << (4 * (4 - (idx - 2))));
    }
    switch (di.hex[7] - '0') {
    case 0:
      horizontal.push_back(
          {Coord(x_offset, y_offset), {distance, InsideSide::R}});
      x_offset += distance;
      break;
    case 1:
      vertical.push_back(
          {Coord(x_offset, y_offset), {distance, InsideSide::D}});
      y_offset += distance;
      break;
    case 2:
      horizontal.push_back(
          {Coord(x_offset - distance, y_offset), {distance, InsideSide::L}});
      x_offset -= distance;
      break;
    case 3:
      vertical.push_back(
          {Coord(x_offset, y_offset - distance), {distance, InsideSide::U}});
      y_offset -= distance;
      break;
    }
    max_y = std::max(max_y, y_offset);
  }
  InsideSide inside = InsideSide::L;
  for (size_t idx = 0; idx < horizontal.size(); ++idx) {
    const std::pair<Coord, std::pair<int64_t, InsideSide>> &p = horizontal[idx];
    if (p.first.y == max_y) {
      if (p.second.second == InsideSide::R) {
        inside = InsideSide::L;
      } else {
        inside = InsideSide::R;
      }
      break;
    }
  }
  auto get_inside = [&inside](const InsideSide &turn) -> InsideSide {
    switch (inside) {
    case InsideSide::L:
      switch (turn) {
      case InsideSide::L:
        return InsideSide::D;
      case InsideSide::R:
        return InsideSide::U;
      case InsideSide::U:
        return InsideSide::L;
      case InsideSide::D:
        return InsideSide::R;
      }
      break;
    case InsideSide::R:
      switch (turn) {
      case InsideSide::L:
        return InsideSide::U;
      case InsideSide::R:
        return InsideSide::D;
      case InsideSide::U:
        return InsideSide::R;
      case InsideSide::D:
        return InsideSide::L;
      }
      break;
    default:
      throw;
      break;
    }
    return InsideSide::U;
  };
  std::sort(
      horizontal.begin(), horizontal.end(),
      [](const auto &p, const auto &p2) { return p.first.y < p2.first.y; });
  std::vector<Region> regions;
  {
    for (size_t idx = 0; idx < horizontal.size(); ++idx) {
      if (get_inside(horizontal[idx].second.second) == InsideSide::U) {
        continue;
      }
      std::stack<std::pair<Coord, std::pair<int64_t, InsideSide>>> stack;
      stack.push(horizontal[idx]);
      while (!stack.empty()) {
        auto top_pair = stack.top();
        stack.pop();
        for (size_t idx2 = idx + 1; idx2 < horizontal.size(); ++idx2) {
          const auto &second_pair = horizontal[idx2];
          if (top_pair.first.x <=
                  second_pair.first.x + second_pair.second.first &&
              second_pair.first.x <= top_pair.first.x + top_pair.second.first &&
              get_inside(second_pair.second.second) == InsideSide::U) {
            int64_t start_overlap =
                std::max(top_pair.first.x, second_pair.first.x);
            int64_t end_overlap =
                std::min(top_pair.first.x + top_pair.second.first,
                         second_pair.first.x + second_pair.second.first);
            Region reg(Coord(start_overlap, top_pair.first.y),
                       Coord(end_overlap, top_pair.first.y),
                       Coord(start_overlap, second_pair.first.y),
                       Coord(end_overlap, second_pair.first.y));
            regions.push_back(reg);
            if (top_pair.first.x < start_overlap) {
              stack.push({top_pair.first,
                          {start_overlap - 1 - top_pair.first.x,
                           top_pair.second.second}});
            }
            if (top_pair.first.x + top_pair.second.first > end_overlap) {
              stack.push({Coord(end_overlap + 1, top_pair.first.y),
                          {top_pair.first.x + top_pair.second.first -
                               (end_overlap + 1),
                           top_pair.second.second}});
            }
            break;
          }
        }
      }
    }
  }
  std::sort(
      horizontal.begin(), horizontal.end(),
      [](const auto &p, const auto &p2) { return p.first.y > p2.first.y; });
  {
    for (size_t idx = 0; idx < horizontal.size(); ++idx) {
      if (get_inside(horizontal[idx].second.second) == InsideSide::D) {
        continue;
      }
      std::stack<std::pair<Coord, std::pair<int64_t, InsideSide>>> stack;
      stack.push(horizontal[idx]);
      while (!stack.empty()) {
        auto top_pair = stack.top();
        stack.pop();
        for (size_t idx2 = idx + 1; idx2 < horizontal.size(); ++idx2) {
          const auto &second_pair = horizontal[idx2];
          if (top_pair.first.x <=
                  second_pair.first.x + second_pair.second.first &&
              second_pair.first.x <= top_pair.first.x + top_pair.second.first &&
              get_inside(second_pair.second.second) == InsideSide::D) {
            int64_t start_overlap =
                std::max(top_pair.first.x, second_pair.first.x);
            int64_t end_overlap =
                std::min(top_pair.first.x + top_pair.second.first,
                         second_pair.first.x + second_pair.second.first);
            Region reg(Coord(start_overlap, second_pair.first.y),
                       Coord(end_overlap, second_pair.first.y),
                       Coord(start_overlap, top_pair.first.y),
                       Coord(end_overlap, top_pair.first.y));
            regions.push_back(reg);
            if (top_pair.first.x < start_overlap) {
              stack.push({top_pair.first,
                          {start_overlap - 1 - top_pair.first.x,
                           top_pair.second.second}});
            }
            if (top_pair.first.x + top_pair.second.first > end_overlap) {
              stack.push({Coord(end_overlap + 1, top_pair.first.y),
                          {top_pair.first.x + top_pair.second.first -
                               (end_overlap + 1),
                           top_pair.second.second}});
            }
            break;
          }
        }
      }
    }
  }
  std::sort(
      vertical.begin(), vertical.end(),
      [](const auto &p, const auto &p2) { return p.first.x < p2.first.x; });
  {
    for (size_t idx = 0; idx < vertical.size(); ++idx) {
      if (get_inside(vertical[idx].second.second) == InsideSide::L) {
        continue;
      }
      std::stack<std::pair<Coord, std::pair<int64_t, InsideSide>>> stack;
      stack.push(vertical[idx]);
      while (!stack.empty()) {
        auto top_pair = stack.top();
        stack.pop();
        for (size_t idx2 = idx + 1; idx2 < vertical.size(); ++idx2) {
          const auto &second_pair = vertical[idx2];
          if (top_pair.first.y <=
                  second_pair.first.y + second_pair.second.first &&
              second_pair.first.y <= top_pair.first.y + top_pair.second.first &&
              get_inside(second_pair.second.second) == InsideSide::L) {
            int64_t start_overlap =
                std::max(top_pair.first.y, second_pair.first.y);
            int64_t end_overlap =
                std::min(top_pair.first.y + top_pair.second.first,
                         second_pair.first.y + second_pair.second.first);
            Region reg(Coord(top_pair.first.x, start_overlap),
                       Coord(second_pair.first.x, start_overlap),
                       Coord(top_pair.first.x, end_overlap),
                       Coord(second_pair.first.x, end_overlap));
            regions.push_back(reg);
            if (top_pair.first.y < start_overlap) {
              stack.push({top_pair.first,
                          {start_overlap - 1 - top_pair.first.y,
                           top_pair.second.second}});
            }
            if (top_pair.first.y + top_pair.second.first > end_overlap) {
              stack.push({Coord(top_pair.first.x, end_overlap + 1),
                          {top_pair.first.y + top_pair.second.first -
                               (end_overlap + 1),
                           top_pair.second.second}});
            }
            break;
          }
        }
      }
    }
  }
  std::sort(
      vertical.begin(), vertical.end(),
      [](const auto &p, const auto &p2) { return p.first.x > p2.first.x; });
  {
    std::set<int64_t> already_matched_v;
    for (size_t idx = 0; idx < vertical.size(); ++idx) {
      if (get_inside(vertical[idx].second.second) == InsideSide::R) {
        continue;
      }
      std::stack<std::pair<Coord, std::pair<int64_t, InsideSide>>> stack;
      stack.push(vertical[idx]);
      while (!stack.empty()) {
        auto top_pair = stack.top();
        stack.pop();
        for (size_t idx2 = idx + 1; idx2 < vertical.size(); ++idx2) {
          const auto &second_pair = vertical[idx2];
          if (top_pair.first.y <=
                  second_pair.first.y + second_pair.second.first &&
              second_pair.first.y <= top_pair.first.y + top_pair.second.first &&
              get_inside(second_pair.second.second) == InsideSide::R) {
            int64_t start_overlap =
                std::max(top_pair.first.y, second_pair.first.y);
            int64_t end_overlap =
                std::min(top_pair.first.y + top_pair.second.first,
                         second_pair.first.y + second_pair.second.first);
            Region reg(Coord(second_pair.first.x, start_overlap),
                       Coord(top_pair.first.x, start_overlap),
                       Coord(second_pair.first.x, end_overlap),
                       Coord(top_pair.first.x, end_overlap));
            regions.push_back(reg);
            if (top_pair.first.y < start_overlap) {
              stack.push({top_pair.first,
                          {start_overlap - 1 - top_pair.first.y,
                           top_pair.second.second}});
            }
            if (top_pair.first.y + top_pair.second.first > end_overlap) {
              stack.push({Coord(top_pair.first.x, end_overlap + 1),
                          {top_pair.first.y + top_pair.second.first -
                               (end_overlap + 1),
                           top_pair.second.second}});
            }
            break;
          }
        }
      }
    }
  }

  std::sort(regions.begin(), regions.end(), [](const auto &a, const auto &b) {
    int64_t a_area = (a.top_right.x - a.top_left.x + 1) *
                     (a.bottom_left.y - a.top_left.y + 1);
    int64_t b_area = (b.top_right.x - b.top_left.x + 1) *
                     (b.bottom_left.y - b.top_left.y + 1);
    return a_area > b_area;
  });

  for (size_t idx = 0; idx < regions.size(); ++idx) {
    const Region &r = regions[idx];
    for (size_t idx2 = idx + 1; idx2 < regions.size();) {
      const Region &r2 = regions[idx2];
      if (r2.top_left.x >= r.top_left.x && r2.top_right.x <= r.top_right.x &&
          r2.bottom_left.y <= r.bottom_left.y &&
          r2.top_left.y >= r.top_left.y) {
        // std::cout << "REMOVING: " << std::endl;
        // std::cout << r2.top_left << " " << r2.top_right << std::endl;
        // std::cout << r2.bottom_left << " " << r2.bottom_right << std::endl;
        assert(r2.get_area() <= r.get_area());
        regions.erase(regions.begin() + idx2);
        continue;
      }
      ++idx2;
    }
  }
  // std::cout << std::endl;
  // for (const Region &r : regions) {
  //   std::cout << r.top_left << " " << r.top_right << std::endl;
  //   std::cout << r.bottom_left << " " << r.bottom_right << std::endl;
  //   std::cout << r.get_area() << std::endl;
  // }
  // std::cout << std::endl;
  for (size_t idx = 0; idx < regions.size();) {
    bool removed = false;
    for (size_t idx2 = idx + 1; idx2 < regions.size();) {
      const Region r = regions[idx];
      const Region r2 = regions[idx2];
      if (r.top_left.x <= r2.top_right.x && r2.top_left.x <= r.top_right.x &&
          r.top_left.y <= r2.bottom_left.y &&
          r2.top_left.y <= r.bottom_left.y) {
        int64_t start_overlap_x = std::max(r.top_left.x, r2.top_left.x);
        int64_t end_overlap_x = std::min(r.top_right.x, r2.top_right.x);

        int64_t start_overlap_y = std::max(r.top_left.y, r2.top_left.y);
        int64_t end_overlap_y = std::min(r.bottom_left.y, r2.bottom_left.y);

        removed = true;
        regions.erase(regions.begin() + idx2);
        regions.erase(regions.begin() + idx);

        Region overlap_reg(Coord(start_overlap_x, start_overlap_y),
                           Coord(end_overlap_x, start_overlap_y),
                           Coord(start_overlap_x, end_overlap_y),
                           Coord(end_overlap_x, end_overlap_y));
        regions.push_back(overlap_reg);
        if (r2.top_left.x < start_overlap_x) {
          Region left_over(Coord(r2.top_left.x, r2.top_left.y),
                           Coord(start_overlap_x - 1, r2.top_left.y),
                           Coord(r2.bottom_left.x, r2.bottom_left.y),
                           Coord(start_overlap_x - 1, r2.bottom_left.y));
          regions.push_back(left_over);
        }
        if (r.top_left.x < start_overlap_x) {
          Region left_over(Coord(r.top_left.x, r.top_left.y),
                           Coord(start_overlap_x - 1, r.top_left.y),
                           Coord(r.bottom_left.x, r.bottom_left.y),
                           Coord(start_overlap_x - 1, r.bottom_left.y));
          regions.push_back(left_over);
        }
        if (r2.top_right.x > end_overlap_x) {
          Region left_over(Coord(end_overlap_x + 1, r2.top_left.y),
                           Coord(r2.top_right.x, r2.top_left.y),
                           Coord(end_overlap_x + 1, r2.bottom_left.y),
                           Coord(r2.bottom_right.x, r2.bottom_left.y));
          regions.push_back(left_over);
        }
        if (r.top_right.x > end_overlap_x) {
          Region left_over(Coord(end_overlap_x + 1, r.top_left.y),
                           Coord(r.top_right.x, r.top_left.y),
                           Coord(end_overlap_x + 1, r.bottom_left.y),
                           Coord(r.bottom_right.x, r.bottom_left.y));
          regions.push_back(left_over);
        }

        if (r2.top_left.y < start_overlap_y) {
          Region left_over(r2.top_left, r2.top_right,
                           Coord(r2.bottom_left.x, start_overlap_y - 1),
                           Coord(r2.bottom_right.x, start_overlap_y - 1));
          regions.push_back(left_over);
        }
        if (r.top_left.y < start_overlap_y) {
          Region left_over(r.top_left, r.top_right,
                           Coord(r.bottom_left.x, start_overlap_y - 1),
                           Coord(r.bottom_right.x, start_overlap_y - 1));
          regions.push_back(left_over);
        }
        if (r2.bottom_left.y > end_overlap_y) {
          Region left_over(Coord(r2.top_left.x, end_overlap_y + 1),
                           Coord(r2.top_right.x, end_overlap_y + 1),
                           r2.bottom_left, r2.bottom_right);
          regions.push_back(left_over);
        }
        if (r.bottom_left.y > end_overlap_y) {
          Region left_over(Coord(r.top_left.x, end_overlap_y + 1),
                           Coord(r.top_right.x, end_overlap_y + 1),
                           r.bottom_left, r.bottom_right);
          regions.push_back(left_over);
        }
        continue;
      }
      ++idx2;
    }
    if (removed) {
      continue;
    }
    ++idx;
  }
  // for (const Region &r : regions) {
  //   std::cout << r.top_left << " " << r.top_right << std::endl;
  //   std::cout << r.bottom_left << " " << r.bottom_right << std::endl;
  // }
  int64_t part2 = std::accumulate(
      regions.begin(), regions.end(), int64_t{0},
      [](int64_t acc, const auto &r) { return acc + r.get_area(); });
  std::cout << std::format("PART2: {}\n", part2);

  // 0000000000
  // 0        0
  // 0 00000000
  // 0 0
  // 0 0
  // 0 0
  // 0 0
  // 0 00000000
  // 0        0
  // 0000000000
  //
  // 0000000000
  // 0        0
  // 0        0
  // 0        0
  // 0        0
  // 0        0
  // 0  0000  0
  // 0  0  0  0
  // 0  0  0  0
  // 0000  0000
  //
  //     00000
  //     0   0
  //     0   0
  //     0   0
  // 00000   00000
  // 0           0
  // 0           0
  // 0           0
  // 00000   00000
  //     0   0
  //     0   0
  //     0   0
  //     00000
  //
  //  000000    000000
  //  0    0    0    0
  //  0    0    0    0
  //  0    0    0    0
  //  0    0    0    0
  //  0    000000    0
  //  0              0
  //  0              0
  //  0              0
  //  0              0
  //  000000    000000
  //       0    0
  //       0    0
  //       0    0
  //       0    0
  //       000000

  return 0;
}
