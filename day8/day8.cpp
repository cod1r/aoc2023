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
  const int32_t LINE_CAPACITY = 1024 * 2;
  char line[LINE_CAPACITY];
  std::string directions;
  if (input.getline(line, LINE_CAPACITY)) {
    size_t line_length = input.gcount() - 1;
    directions = std::string(line, line_length);
  }
  std::map<std::string, std::pair<std::string, std::string>> nodes;
  while (input.getline(line, LINE_CAPACITY)) {
    size_t line_length = input.gcount() - 1;
    if (line_length == 0)
      continue;
    std::string node_name = std::string(line, 3);
    std::string node_left = std::string(line + 7, 3);
    std::string node_right = std::string(line + 12, 3);
    nodes.insert(
        {node_name,
        {node_left, node_right}}
    );
  }
  std::string start{"AAA"};
  int32_t counter = 0;
  int32_t direction_idx = 0;
  while (start != "ZZZ") {
    start = directions[direction_idx] == 'L' ? nodes[start].first : nodes[start].second;
    direction_idx += 1;
    direction_idx %= directions.size();
    counter += 1;
  }
  std::cout << "PART1: " << counter << std::endl;

  std::vector<std::string_view> a_nodes;
  for (auto &p : nodes) {
    const std::string& node_name = p.first;
    switch (node_name[2]) {
      case 'A': a_nodes.push_back(std::string_view{node_name}); break;
      default: break;
    }
  }
  std::map<std::string, std::vector<int32_t>> history;
  for (size_t idx = 0; idx < a_nodes.size(); ++idx) {
    std::set<std::string> cycle_detect;
    auto curr_str = std::string(a_nodes[idx]);
    int32_t idx_direction = 0;
    std::string key_check = 
      curr_str +
      std::string{directions[idx_direction]} +
      std::to_string(idx_direction);
    int32_t counter = 0;
    std::vector<int32_t> curr_hist;
    while (!cycle_detect.contains(key_check)) {
      const std::string key =
        curr_str +
        std::string{directions[idx_direction]} +
        std::to_string(idx_direction);
      cycle_detect.insert(key);
      curr_str = directions[idx_direction] == 'L' ? nodes[curr_str].first : nodes[curr_str].second;
      if (curr_str[2] == 'Z') {
        curr_hist.push_back(counter + 1);
      }
      idx_direction += 1;
      idx_direction %= directions.size();
      counter += 1;
      key_check = curr_str + std::string{directions[idx_direction]} + std::to_string(idx_direction);
    }
    history.insert({std::string(a_nodes[idx]), curr_hist});
  }
  int64_t curr_lcm = 1;
  for (auto p : history) {
    int32_t sum = std::reduce(p.second.begin(), p.second.end());
    curr_lcm = std::lcm(curr_lcm, sum);
  }
  std::cout << "PART2: " << curr_lcm << std::endl;
  return 0;
}
