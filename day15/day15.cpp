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
#include <format>

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
struct Range {
  size_t start;
  size_t length;
};

std::vector<std::string_view> get_steps(const std::string& init_seq) {
  std::vector<std::string_view> steps;
  size_t start = 0;
  for (size_t idx = 0; idx < init_seq.length(); ++idx) {
    if (init_seq[idx] == ',') {
      steps.push_back(std::string_view(init_seq.begin() + start, init_seq.begin() + idx));
      start = idx + 1;
    }
  }
  steps.push_back(std::string_view(init_seq.begin() + start, init_seq.end()));
  return steps;
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
  std::string initialization_seq;
  while (input.getline(line, LINE_CAPACITY)) {
    size_t line_length = input.gcount() - 1;
    initialization_seq = std::string(line, line_length);
  }
  size_t part1 = 0;
  std::vector<std::string_view> steps = get_steps(initialization_seq);
  for (const std::string_view& sv : steps) {
    size_t result = 0;
    for (const char& c : sv) {
      result += (size_t)c;
      result *= size_t{17};
      result %= 256;
    }
    part1 += result;
  }
  std::cout << std::format("PART1: {}\n", part1);


  struct Lens {
    std::string_view label;
    size_t focal_len;
  };
  struct Box {
    std::vector<Lens> lenses;
  };
  std::array<Box, 256> boxes;
  for (const std::string_view& sv : steps) {
    size_t idx_of_minus = sv.find('-');
    size_t idx_of_eql = sv.find('=');

    size_t box_hash = 0;
    for (size_t idx_label = 0; idx_label < std::min(idx_of_minus, idx_of_eql); ++idx_label) {
      char c = sv[idx_label];
      box_hash += (size_t)c;
      box_hash *= size_t{17};
      box_hash %= 256;
    }
    assert(box_hash <= 255);

    std::string_view label(sv.begin(), sv.begin() + std::min(idx_of_minus, idx_of_eql));
    if (idx_of_eql != std::string_view::npos) {
      auto lens_with_same_label = std::find_if(boxes[box_hash].lenses.begin(), boxes[box_hash].lenses.end(), [&label](const Lens& l) {
        return label == l.label;
      });
      size_t focal_len = sv[idx_of_eql + 1] - '0';
      if (lens_with_same_label != boxes[box_hash].lenses.end()) {
        lens_with_same_label->focal_len = focal_len;
      } else {
        boxes[box_hash].lenses.push_back(Lens { .label = label, .focal_len = focal_len });
      }
    } else {
      auto filtered = std::views::filter(boxes[box_hash].lenses, [&label](const Lens& l) {
        return label != l.label;
      });
      boxes[box_hash].lenses = std::vector<Lens>(filtered.begin(), filtered.end());
    }
  }
  size_t part2 = 0;
  for (size_t idx_boxes = 0; idx_boxes < boxes.size(); ++idx_boxes) {
    const Box& b = boxes[idx_boxes];
    size_t focus_power = 0;
    for (size_t idx_lens = 0; idx_lens < b.lenses.size(); ++idx_lens) {
      const Lens& l = b.lenses[idx_lens];
      focus_power += (1 + idx_boxes) * (idx_lens + 1) * l.focal_len;
    }
    part2 += focus_power;
  }
  std::cout << std::format("PART2: {}\n", part2);
  return 0;
}
