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
int64_t parse_number(std::string_view s) {
  int32_t length = s.length();
  int64_t value = 0;
  for (int32_t idx = 0; idx < length; ++idx) {
    value += (s[idx] - '0') * std::pow(10, length - idx - 1);
  }
  return value;
}
std::vector<int64_t> parse_string_to_vec_numbers(char *line, int32_t length) {
  std::vector<int64_t> numbers;
  enum class Sign {
    Pos,
    Neg,
  };
  std::vector<std::pair<Sign, std::string_view>> num_strings;
  std::optional<int32_t> digit_start;
  for (int32_t num_parser_idx = 0; num_parser_idx < length; ++num_parser_idx) {
    if (
      ((line[num_parser_idx] >= '0' && line[num_parser_idx] <= '9') || line[num_parser_idx] == '-') &&
      !digit_start.has_value()) {
      digit_start = num_parser_idx;
    }
    if ((line[num_parser_idx] == ' ' || num_parser_idx == length - 1) && digit_start.has_value()) {
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
std::ostream& operator<<(std::ostream &os, const std::vector<int64_t> &v) {
  for (const int64_t &n : v) {
    os << n << " ";
  }
  os << std::endl;
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
  const int32_t LINE_CAPACITY = 1024 * 2;
  char line[LINE_CAPACITY];
  int64_t part1 = 0;
  int64_t part2 = 0;
  while (input.getline(line, LINE_CAPACITY)) {
    int32_t line_length = input.gcount() - 1;
    std::vector<int64_t> numbers = parse_string_to_vec_numbers(line, line_length);
    std::vector<int64_t> triangle;
    triangle.reserve(2 * (numbers.size() - 1));
    for (int32_t idx = 1; idx < (int32_t)numbers.size(); ++idx) {
      int64_t diff = numbers[idx] - numbers[idx - 1];
      triangle.push_back(diff);
      for (
        int32_t idx_triangle = triangle.size() - 2;
        idx_triangle > (int32_t)(triangle.size() - 1) / 2;
        --idx_triangle
      ) {
        triangle[idx_triangle] = triangle[idx_triangle + 1] - triangle[idx_triangle];
      }
      if (triangle.size() > 1) {
        int64_t diff_middle = triangle[triangle.size() / 2] - triangle[triangle.size() / 2 - 1];
        triangle.insert(triangle.begin() + triangle.size() / 2, diff_middle);
      }
    }
    int64_t next = 0;
    for (int32_t idx = (triangle.size() - 1) / 2; idx < (int32_t)triangle.size(); ++idx) {
      next += triangle[idx];
    }
    part1 += next + numbers.back();
    int64_t two_next = 0;
    for (int32_t idx = (triangle.size() - 1) / 2; idx >= 0; --idx) {
      two_next = triangle[idx] - two_next;
    }
    part2 += numbers.front() - two_next;
  }
  std::cout << "PART1: " << part1 << std::endl;
  std::cout << "PART2: " << part2 << std::endl;
  return 0;
}
