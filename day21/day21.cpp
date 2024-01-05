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

  auto map2 = map;

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
  map[s.y][s.x] = '.';
  map2[s.y][s.x] = '.';

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
    std::set<std::pair<int64_t, int64_t>> in_q;
    in_q.insert({s.x, s.y});
    const size_t STEP_LIMIT = 1244 + 131 - 131 - 131;
    int64_t current_quad = 4;
    int64_t prev_d = 0;
    const int64_t ACTUAL_LIMIT = 26'501'365;
    std::vector<int64_t> stupid_step(ACTUAL_LIMIT, 0);
    std::vector<int64_t> stupid_diff(ACTUAL_LIMIT, 0);
    for (size_t steps = 0; steps < STEP_LIMIT; ++steps) {
      size_t q_size = q.size();
      while (q_size > 0) {
        Coord t = q.front();
        q.pop();
        {
          int64_t x_rem = t.x % (int64_t)map2[0].length();
          int64_t x_mod =
              ((int64_t)map2[0].length() + x_rem) % (int64_t)map2[0].length();

          int64_t y_rem = (t.y - 1) % (int64_t)map2.size();
          int64_t y_mod = ((int64_t)map2.size() + y_rem) % (int64_t)map2.size();
          assert(x_mod >= 0 && x_mod < (int64_t)map2[0].length());
          assert(y_mod >= 0 && y_mod < (int64_t)map2.size());

          if (map2[y_mod][x_mod] == '.' && !in_q.contains({t.x, t.y - 1})) {
            q.push(Coord(t.x, t.y - 1));
            in_q.insert({t.x, t.y - 1});
          }
        }
        {
          int64_t x_rem = (t.x - 1) % (int64_t)map2[0].length();
          int64_t x_mod =
              ((int64_t)map2[0].length() + x_rem) % (int64_t)map2[0].length();

          int64_t y_rem = t.y % (int64_t)map2.size();
          int64_t y_mod = ((int64_t)map2.size() + y_rem) % (int64_t)map2.size();
          assert(x_mod >= 0 && x_mod < (int64_t)map2[0].length());
          assert(y_mod >= 0 && y_mod < (int64_t)map2.size());

          if (map2[y_mod][x_mod] == '.' && !in_q.contains({t.x - 1, t.y})) {
            q.push(Coord(t.x - 1, t.y));
            in_q.insert({t.x - 1, t.y});
          }
        }
        {
          int64_t x_rem = (t.x + 1) % (int64_t)map2[0].length();
          int64_t x_mod =
              ((int64_t)map2[0].length() + x_rem) % (int64_t)map2[0].length();

          int64_t y_rem = t.y % (int64_t)map2.size();
          int64_t y_mod = ((int64_t)map2.size() + y_rem) % (int64_t)map2.size();
          assert(x_mod >= 0 && x_mod < (int64_t)map2[0].length());
          assert(y_mod >= 0 && y_mod < (int64_t)map2.size());

          if (map2[y_mod][x_mod] == '.' && !in_q.contains({t.x + 1, t.y})) {
            q.push(Coord(t.x + 1, t.y));
            in_q.insert({t.x + 1, t.y});
          }
        }
        {
          int64_t x_rem = t.x % (int64_t)map2[0].length();
          int64_t x_mod =
              ((int64_t)map2[0].length() + x_rem) % (int64_t)map2[0].length();

          int64_t y_rem = (t.y + 1) % (int64_t)map2.size();
          int64_t y_mod = ((int64_t)map2.size() + y_rem) % (int64_t)map2.size();
          assert(x_mod >= 0 && x_mod < (int64_t)map2[0].length());
          assert(y_mod >= 0 && y_mod < (int64_t)map2.size());

          if (map2[y_mod][x_mod] == '.' && !in_q.contains({t.x, t.y + 1})) {
            q.push(Coord(t.x, t.y + 1));
            in_q.insert({t.x, t.y + 1});
          }
        }
        --q_size;
      }
      std::cout << steps << " " << q.size() << " " << current_quad << " "
                << (int64_t)q.size() - current_quad << " "
                << (int64_t)q.size() - current_quad - prev_d << std::endl;
      stupid_step[steps] = q.size();
      stupid_diff[steps] = (int64_t)q.size() - current_quad;
      prev_d = (int64_t)q.size() - current_quad;
      current_quad += 4;
    }
    int64_t start = 0;
    int64_t end = 0;
    for (int64_t idx = 0; idx < 1000; ++idx) {
      if (stupid_step[idx] == (idx + 1) * 4 &&
          stupid_step[idx + 1] == (idx + 2) * 4) {
        if (start == 0) {
          start = idx;
          ++idx;
        } else {
          end = idx;
          break;
        }
      }
    }
    std::cout << start << " " << end << std::endl;
    int64_t start2 = end;
    int64_t end2 = end + 131;
    std::cout << start2 << " " << end2 << std::endl;
    std::vector<int64_t> diffs_offset(131, 0);
    for (int64_t idx = start2; idx < end2; ++idx) {
      diffs_offset[idx - start2] =
          stupid_diff[idx] - stupid_diff[start + (idx - start2)];
      // std::cout << stupid_step[idx] << " "
      //           << stupid_step[start + (idx - start2)] << " "
      //           << stupid_diff[idx] << " "
      //           << stupid_diff[start + (idx - start2)] << std::endl;
    }
    assert(stupid_step.size() == ACTUAL_LIMIT);
    int64_t diffs_idx = 0;
    for (int64_t idx = STEP_LIMIT; idx < (int64_t)stupid_step.size(); ++idx) {
      stupid_diff[idx] = stupid_diff[idx - 131] + diffs_offset[diffs_idx];
      stupid_step[idx] = (idx + 1) * 4 + stupid_diff[idx];
      ++diffs_idx;
      if (diffs_idx == 131) {
        diffs_idx = 0;
      }
      // std::cout << idx << " " << stupid_step[idx] << " " << (idx + 1) * 4 <<
      // " "
      //           << stupid_diff[idx] << std::endl;
    }
    stupid_step.insert(stupid_step.begin(), 1);
    int64_t sum = 0;
    for (int64_t idx = stupid_step.size() - 1; idx >= 0; idx -= 2) {
      sum += stupid_step[idx];
      assert(sum >= 0);
    }
    std::cout << "PART2: " << sum << std::endl;
  }
  return 0;
}

//     4
/*    434
     43#34
    43#3#34
   43#3#3#34
  43#3#3#3#34
   43#3#3#34
    43#3#34
     43#34
      434
       4
 */
