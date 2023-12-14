#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <fstream>
#include <tuple>
#include <optional>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <utility>
#include <map>
#include <set>
#include <numeric>
#include <cassert>
#include <ranges>

int64_t parse_number(std::string_view s) {
  int32_t length = (int32_t)s.length();
  int64_t value = 0;
  for (int32_t idx = 0; idx < length; ++idx) {
    value += (s[idx] - '0') * (int64_t)std::pow(10, length - idx - 1);
  }
  return value;
}
std::vector<int64_t> parse_string_to_vec_numbers(char *line, size_t length, char delimiter = ' ') {
  std::vector<int64_t> numbers;
  enum class Sign {
    Pos,
    Neg,
  };
  std::vector<std::pair<Sign, std::string_view>> num_strings;
  std::optional<int32_t> digit_start;
  for (size_t num_parser_idx = 0; num_parser_idx < length; ++num_parser_idx) {
    if (
      ((line[num_parser_idx] >= '0' && line[num_parser_idx] <= '9') || line[num_parser_idx] == '-') &&
      !digit_start.has_value()) {
      digit_start = num_parser_idx;
    }
    if ((line[num_parser_idx] == delimiter || num_parser_idx == length - 1) && digit_start.has_value()) {
      if (num_parser_idx == length - 1)
        num_parser_idx = length;
      bool has_neg = line[*digit_start] == '-';
      int32_t actual_digit_start = has_neg ? *digit_start + 1 : *digit_start;
      num_strings.push_back(
      {has_neg ? Sign::Neg : Sign::Pos,
      std::string_view(line + actual_digit_start, num_parser_idx - actual_digit_start)});
      digit_start.reset();
    }
  }
  for (auto [sign, seed_str] : num_strings) {
    auto num = parse_number(seed_str);
    switch (sign) {
      case Sign::Pos: numbers.push_back(num); break;
      case Sign::Neg: numbers.push_back(-num); break;
    }
  }
  return numbers;
}
template<typename T> requires std::is_integral_v<T>
std::ostream& operator<<(std::ostream &os, const std::vector<T> &v) {
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
void roll_north(std::vector<std::string>& platform) {
  for (size_t col = 0; col < platform[0].length(); ++col) {
    Coord north_most; north_most.x = col; north_most.y = 0;
    std::vector<Coord> rocks;
    for (size_t row = 0; row < platform.size(); ++row) {
      if (platform[row][col] == '#') {
        north_most.y = row + 1;
      } else if (platform[row][col] == 'O') {
          rocks.push_back(Coord{ .x = north_most.x, .y = north_most.y });
          platform[row][col] = '.';
          north_most.y += 1;
      }
    }
    for (const Coord& c : rocks) {
      platform[c.y][c.x] = 'O';
    }
  }
}
void roll_west(std::vector<std::string>& platform) {
  for (size_t row = 0; row < platform.size(); ++row) {
    Coord west_most; west_most.x = 0; west_most.y = row;
    std::vector<Coord> rocks;
    for (size_t col = 0; col < platform[0].length(); ++col) {
      if (platform[row][col] == '#') {
        west_most.x = col + 1;
      } else if (platform[row][col] == 'O') {
          rocks.push_back(Coord{ .x = west_most.x, .y = west_most.y });
          platform[row][col] = '.';
          west_most.x += 1;
      }
    }
    for (const Coord& c : rocks) {
      platform[c.y][c.x] = 'O';
    }
  }
}
void roll_south(std::vector<std::string>& platform) {
  for (size_t col = 0; col < platform[0].length(); ++col) {
    Coord south_most; south_most.x = col; south_most.y = platform.size() - 1;
    std::vector<Coord> rocks;
    size_t row = platform.size() - 1;
    while (true) {
      if (platform[row][col] == '#') {
        south_most.y = row - 1;
      } else if (platform[row][col] == 'O') {
          rocks.push_back(Coord{ .x = south_most.x, .y = south_most.y });
          platform[row][col] = '.';
          south_most.y -= 1;
      }
      if (row == 0) {
        break;
      }
      row -= 1;
    }
    for (const Coord& c : rocks) {
      platform[c.y][c.x] = 'O';
    }
  }
}
void roll_east(std::vector<std::string>& platform) {
  for (size_t row = 0; row < platform.size(); ++row) {
    Coord east_most; east_most.x = platform[0].length() - 1; east_most.y = row;
    std::vector<Coord> rocks;
    size_t col = platform[0].length() - 1;
    while (true) {
      if (platform[row][col] == '#') {
        east_most.x = col - 1;
      } else if (platform[row][col] == 'O') {
          rocks.push_back(Coord{ .x = east_most.x, .y = east_most.y });
          platform[row][col] = '.';
          east_most.x -= 1;
      }
      if (col == 0) {
        break;
      }
      col -= 1;
    }
    for (const Coord& c : rocks) {
      platform[c.y][c.x] = 'O';
    }
  }
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
  const int32_t LINE_CAPACITY = 1024 * 2;
  char line[LINE_CAPACITY];
  std::vector<std::string> platform;
  while (input.getline(line, LINE_CAPACITY)) {
    size_t line_length = input.gcount() - 1;
    platform.push_back(std::string(line, line_length));
  }
  roll_north(platform);
  size_t part1 = 0;
  for (size_t row = 0; row < platform.size(); ++row) {
    const std::string& s = platform[row];
    int64_t rock_amt = std::accumulate(s.begin(), s.end(), int64_t{0},
        [](int64_t acc, const char& c) {
          if (c == 'O')
            return acc + 1;
          return acc;
        });
    part1 += (platform.size() - row) * rock_amt;
  }
  std::cout << "PART1: " << part1 << std::endl;
  size_t part2 = 0;
  for (size_t roll_counter = 0; roll_counter < 1'000'000'000; roll_counter += 1) {
    roll_north(platform);
    roll_west(platform);
    roll_south(platform);
    roll_east(platform);
  }
  for (size_t row = 0; row < platform.size(); ++row) {
    const std::string& s = platform[row];
    int64_t rock_amt = std::accumulate(s.begin(), s.end(), int64_t{0},
        [](int64_t acc, const char& c) {
          if (c == 'O')
            return acc + 1;
          return acc;
        });
    part2 += (platform.size() - row) * rock_amt;
  }
  std::cout << "PART2: " << part2 << std::endl;
  return 0;
}
