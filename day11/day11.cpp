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
template<typename T> requires std::is_integral_v<T>
std::ostream& operator<<(std::ostream &os, const std::vector<T> &v) {
  for (const T &n : v) {
    os << n << " ";
  }
  os << std::endl;
  return os;
}
std::vector<int64_t> generate_seq(const int32_t times, const int32_t range, std::vector<int32_t> seq) {
  std::vector<int64_t> new_seq;
  int64_t curr_seq = 0;
  for (int32_t c = 0; c < range; ++c) {
    auto find_res = std::find_if(seq.begin(), seq.end(), [c](const int32_t empty) {
      return empty == c;
    });
    new_seq.push_back(curr_seq);
    if (find_res != seq.end()) {
      curr_seq += (int64_t)times;
    } else {
      curr_seq += 1;
    }
  }
  return new_seq;
}
int64_t get_ans(
const int32_t times,
const std::vector<std::array<std::pair<int32_t, int32_t>, 2>>& pairs,
const std::vector<std::string>& image,
const std::vector<int32_t>& rows,
const std::vector<int32_t>& cols
) {
  std::vector<int64_t> row_seq = generate_seq(times, (int32_t)image.size(), rows);
  std::vector<int64_t> col_seq = generate_seq(times, (int32_t)image[0].length(), cols);
  int64_t sum = std::accumulate(pairs.begin(), pairs.end(), (int64_t)0, [&row_seq, &col_seq](auto acc, const auto& p) {
    int64_t converted_row_p0 = (int64_t)row_seq[p[0].first];
    int64_t converted_row_p1 = (int64_t)row_seq[p[1].first];
    int64_t converted_col_p0 = (int64_t)col_seq[p[0].second];
    int64_t converted_col_p1 = (int64_t)col_seq[p[1].second];
    return acc + std::abs(converted_row_p0 - converted_row_p1) +
      std::abs(converted_col_p0 - converted_col_p1);
  });
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
  std::vector<std::string> image;
  while (input.getline(line, LINE_CAPACITY)) {
    int32_t line_length = (int32_t)input.gcount() - 1;
    image.push_back(std::string(line, line_length));
  }
  std::vector<int32_t> rows;
  for (int32_t idx = 0; idx < (int32_t)image.size(); ++idx) {
    auto find_res = std::find_if(image[idx].begin(), image[idx].end(), [](char c){return c == '#';});
    if (find_res == image[idx].end()) {
      rows.push_back(idx);
    }
  }
  std::vector<int32_t> cols;
  for (int32_t idx = 0; idx < (int32_t)image[0].length(); ++idx) {
    bool found = false;
    for (int32_t idx_row = 0; idx_row < (int32_t)image.size(); ++idx_row) {
      if (image[idx_row][idx] == '#') {
        found = true;
        break;
      }
    }
    if (!found) {
      cols.push_back(idx);
    }
  }
  std::vector<std::pair<int32_t, int32_t>> galaxies;
  for (int32_t r = 0; r < (int32_t)image.size(); ++r) {
    for (int32_t c = 0; c < (int32_t)image[r].length(); ++c) {
      if (image[r][c] == '#') {
        galaxies.push_back({r, c});
      }
    }
  }
  std::vector<std::array<std::pair<int32_t, int32_t>, 2>> pairs;
  for (int32_t first = 0; first < (int32_t)galaxies.size(); ++first) {
    for (int32_t second = first + 1; second < (int32_t)galaxies.size(); ++second) {
      pairs.push_back({galaxies[first], galaxies[second]});
    }
  }
  assert(pairs.size() == galaxies.size() * (galaxies.size() - 1) / 2);
  int64_t part1 = get_ans(2, pairs, image, rows, cols);
  std::cout << "PART1: " << part1 << std::endl;
  int64_t part2 = get_ans(1'000'000, pairs, image, rows, cols);
  std::cout << "PART2: " << part2 << std::endl;
  return 0;
}
