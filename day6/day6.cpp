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
  std::vector<std::string_view> num_strings;
  std::optional<int32_t> digit_start;
  for (int32_t num_parser_idx = 0; num_parser_idx < length; ++num_parser_idx) {
    if (line[num_parser_idx] >= '0' && line[num_parser_idx] <= '9' && !digit_start.has_value()) {
      digit_start = num_parser_idx;
    }
    if ((line[num_parser_idx] == ' ' && digit_start.has_value()) || num_parser_idx == length - 1) {
      if (num_parser_idx == length - 1)
        num_parser_idx = length;
      num_strings.push_back(std::string_view(line + *digit_start, num_parser_idx - *digit_start));
      digit_start.reset();
    }
  }
  for (std::string_view &seed_str : num_strings) {
    numbers.push_back(parse_number(seed_str));
  }
  return numbers;
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
  const int32_t LINE_CAPACITY = 512;
  char line[LINE_CAPACITY];
  std::vector<int64_t> times;
  if (input.getline(line, LINE_CAPACITY)) {
    int32_t line_length = input.gcount() - 1;
    int32_t idx_of_digits = 0;
    for (int32_t idx = 0; idx < line_length; ++idx) {
      if (line[idx] >= '0' && line[idx] <= '9') {
        idx_of_digits = idx;
        break;
      }
    }
    times = parse_string_to_vec_numbers(line + idx_of_digits, line_length - idx_of_digits);
  }
  std::vector<int64_t> distances;
  if (input.getline(line, LINE_CAPACITY)) {
    int32_t line_length = input.gcount() - 1;
    int32_t idx_of_digits = 0;
    for (int32_t idx = 0; idx < line_length; ++idx) {
      if (line[idx] >= '0' && line[idx] <= '9') {
        idx_of_digits = idx;
        break;
      }
    }
    distances = parse_string_to_vec_numbers(line + idx_of_digits, line_length - idx_of_digits);
  }
  input.close();
  int64_t part1_product = 1;
  for (int64_t idx_time = 0; idx_time < (int64_t)times.size(); ++idx_time) {
    int64_t count = 0;
    for (int64_t time = 1; time <= times[idx_time]; ++time) {
      if ((times[idx_time] - time) * time > distances[idx_time]) {
        count += 1;
      }
    }
    part1_product *= count;
  }
  std::cout << "PART1: " << part1_product << std::endl;
  std::string one_big_time;
  for (auto n : times) {
    one_big_time += std::to_string(n);
  }
  std::string one_big_distance;
  for (auto d : distances) {
    one_big_distance += std::to_string(d);
  }
  int64_t one_time = parse_number(std::string_view(one_big_time));
  int64_t one_distance = parse_number(std::string_view(one_big_distance));
  int64_t count = 0;
  for (int64_t time = 1; time <= one_time; ++time) {
    if ((one_time - time) * time > one_distance) {
      count += 1;
    }
  }
  std::cout << "PART2: " << count << std::endl;
  return 0;
}
