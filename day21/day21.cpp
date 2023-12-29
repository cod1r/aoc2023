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
std::ostream &operator<<(std::ostream &os, const Range &r) {
  os << std::format("start: {}, length: {}", r.start, r.length);
  return os;
}
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
};
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
  std::vector<std::string> map;
  while (input.getline(line, LINE_CAPACITY)) {
    int64_t line_length = input.gcount() - 1;
    map.push_back(std::string(line, line_length));
  }
  input.close();

  auto find_s_loc = [](const std::vector<std::string> &map) -> Coord {
    Coord s;
    for (size_t r = 0; r < map.size(); ++r) {
      for (size_t c = 0; c < map[r].size(); ++c) {
        if (map[r][c] == 'S') {
          s = Coord(c, r);
          return s;
        }
      }
    }
    return s;
  };

  Coord s = find_s_loc(map);

  {
    std::queue<Coord> q;
    q.push(s);
    for (size_t steps = 0; steps < 64; ++steps) {
      size_t q_size = q.size();
      while (q_size > 0) {
        Coord t = q.front();
        map[t.y][t.x] = '.';
        q.pop();
        if (t.x + 1 < (int64_t)map[0].length() && map[t.y][t.x + 1] == '.') {
          q.push(Coord(t.x + 1, t.y));
          map[t.y][t.x + 1] = 'O';
        }
        if (t.x > 0 && map[t.y][t.x - 1] == '.') {
          q.push(Coord(t.x - 1, t.y));
          map[t.y][t.x - 1] = 'O';
        }
        if (t.y + 1 < (int64_t)map.size() && map[t.y + 1][t.x] == '.') {
          q.push(Coord(t.x, t.y + 1));
          map[t.y + 1][t.x] = 'O';
        }
        if (t.y > 0 && map[t.y - 1][t.x] == '.') {
          q.push(Coord(t.x, t.y - 1));
          map[t.y - 1][t.x] = 'O';
        }
        --q_size;
      }
    }
    std::cout << "PART1: " << q.size() << std::endl;
  }
  {
    std::queue<Coord> q;
    q.push(s);
    for (size_t steps = 0; steps < 26501365; ++steps) {
      size_t q_size = q.size();
      while (q_size > 0) {
        Coord t = q.front();
        map[t.y][t.x] = '.';
        q.pop();
        int64_t x_mod = (t.x + 1) % (int64_t)map[0].length();
        int64_t y_mod = (t.y + 1) % (int64_t)map.size();

        int64_t x_mod_sub = 
        if (map[t.y][x_mod] == '.') {
          q.push(Coord(t.x + 1, t.y));
        }
        if (t.x > 0 && map[t.y][t.x - 1] == '.') {
          q.push(Coord(t.x - 1, t.y));
        }
        if (map[y_mod][t.x] == '.') {
          q.push(Coord(t.x, t.y + 1));
        }
        if (t.y > 0 && map[t.y - 1][t.x] == '.') {
          q.push(Coord(t.x, t.y - 1));
        }
        --q_size;
      }
    }
    std::cout << "PART2: " << q.size() << std::endl;
  }
  return 0;
}
