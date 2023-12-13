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
int64_t get_diff_row(const std::string& a, const std::string& b) {
  int64_t diff = 0;
  for (size_t idx = 0; idx < a.length(); ++idx) {
    if (a[idx] != b[idx])
      diff += 1;
  }
  return diff;
}
int64_t get_diff_col(size_t col_a, size_t col_b, const std::vector<std::string>& pattern) {
  int64_t diff = 0;
  for (size_t row = 0; row < pattern.size(); ++row) {
    if (pattern[row][col_a] != pattern[row][col_b]) {
      diff += 1;
    }
  }
  return diff;
}
template<bool part1>
int64_t check_reflections(const std::vector<std::string>& pattern) {
  int64_t sum = 0;
  for (size_t row = 1; row < pattern.size(); ++row) {
    size_t row_check = row;
    size_t row_check_behind = row - 1;
    size_t counter = 0;
    size_t total_diffs = 0;
    while (row_check < pattern.size()) {
      if (part1) {
        if (pattern[row_check] != pattern[row_check_behind]) {
          break;
        }
      } else {
        total_diffs += get_diff_row(pattern[row_check], pattern[row_check_behind]);
      }
      counter += 1;
      if (row_check_behind == 0) {
        break;
      }
      row_check_behind -= 1;
      row_check += 1;
    }
    if (part1) {
      if (counter == row || counter == pattern.size() - row) {
        sum += 100 * row;
        break;
      }
    } else {
      if (total_diffs == 1) {
        sum += 100 * row;
        break;
      }
    }
  }
  for (size_t col = 1; col < pattern[0].length(); ++col) {
    size_t col_check = col;
    size_t col_check_behind = col - 1;
    size_t counter = 0;
    size_t total_diffs = 0;
    while (col_check < pattern[0].length()) {
      bool is_equal = true;
      for (size_t row = 0; row < pattern.size(); ++row) {
        if (pattern[row][col_check] != pattern[row][col_check_behind]) {
          is_equal = false;
          break;
        }
      }
      if (part1) {
        if (!is_equal)
          break;
      } else {
        total_diffs += get_diff_col(col_check, col_check_behind, pattern);
      }
      counter += 1;
      if (col_check_behind == 0) {
        break;
      }
      col_check += 1;
      col_check_behind -= 1;
    }
    if (part1) {
      if (counter == col || counter == pattern[0].length() - col) {
        sum += col;
        break;
      }
    } else {
      if (total_diffs == 1) {
        sum += col; 
        break;
      }
    }
  }
  return sum;
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
  std::vector<std::vector<std::string>> patterns;
  std::vector<std::string> pattern;
  while (input.getline(line, LINE_CAPACITY)) {
    size_t line_length = input.gcount() - 1;
    if (line_length != 0) {
      pattern.push_back(std::string(line, line_length));
    } else {
      patterns.push_back(pattern);
      pattern.clear();
    }
  }
  patterns.push_back(pattern);
  int64_t part1 = 0;
  for (const std::vector<std::string>& pattern : patterns) {
    int64_t sum = check_reflections<true>(pattern);
    if (sum > int64_t{0}) {
      part1 += sum;
    }
  }
  std::cout << "PART1: " << part1 << std::endl;
  int64_t part2 = 0;
  for (const std::vector<std::string>& pattern : patterns) {
    int64_t sum = check_reflections<false>(pattern);
    if (sum > int64_t{0}) {
      part2 += sum;
    }
  }
  std::cout << "PART2: " << part2 << std::endl;
  return 0;
}
