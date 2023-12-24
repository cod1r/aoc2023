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
std::vector<int64_t> parse_string_to_vec_numbers(char *line, size_t length,
                                                 char delimiter = ' ') {
  std::vector<int64_t> numbers;
  enum class Sign {
    Pos,
    Neg,
  };
  std::vector<std::pair<Sign, std::string_view>> num_strings;
  std::optional<int32_t> digit_start;
  for (size_t num_parser_idx = 0; num_parser_idx < length; ++num_parser_idx) {
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
  size_t x;
  size_t y;
  Coord &operator=(const Coord &c) {
    this->x = c.x;
    this->y = c.y;
    return *this;
  }
  Coord() = default;
  Coord(const Coord &c) { *this = c; }
  Coord(size_t x, size_t y) {
    this->x = x;
    this->y = y;
  }
};
std::ostream &operator<<(std::ostream &os, const Coord &c) {
  os << std::format("x: {}, y: {}\n", c.x, c.y);
  return os;
}
bool operator==(const Coord &a, const Coord &b) {
  return a.x == b.x && a.y == b.y;
}
struct Range {
  size_t start;
  size_t length;
};

struct Node {
  size_t weight;
  Coord coord;
  size_t lefts_in_a_row;
  size_t rights_in_a_row;
  size_t ups_in_a_row;
  size_t downs_in_a_row;
  Node() = default;
  Node(const Node &n) { *this = n; }
  Node &operator=(const Node &n) {
    this->weight = n.weight;
    this->coord = n.coord;
    this->lefts_in_a_row = n.lefts_in_a_row;
    this->rights_in_a_row = n.rights_in_a_row;
    this->ups_in_a_row = n.ups_in_a_row;
    this->downs_in_a_row = n.downs_in_a_row;
    return *this;
  }
};
bool operator==(const Node &a, const Node &b) { return a.coord == b.coord; }

size_t get_heat_loss(const std::vector<std::string> &heat_map, size_t row,
                     size_t col) {
  return (heat_map[row][col] - '0');
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
  std::vector<std::string> heat_map;
  while (input.getline(line, LINE_CAPACITY)) {
    size_t line_length = input.gcount() - 1;
    heat_map.push_back(std::string(line, line_length));
  }
  auto cmp = [](const Node &a, const Node &b) { return a.weight > b.weight; };
  std::priority_queue<Node, std::vector<Node>, decltype(cmp)> pq(cmp);
  Node first_node;
  first_node.weight = 0;
  first_node.coord = Coord(0, 0);
  first_node.lefts_in_a_row = 0;
  first_node.rights_in_a_row = 0;
  first_node.ups_in_a_row = 0;
  first_node.downs_in_a_row = 0;
  pq.push(first_node);
  std::vector<std::string> visited(heat_map.size(), std::string(heat_map[0].length(), ' '));
  std::vector<Node> final_dest;
  while (!pq.empty()) {
    Node n = pq.top();
    pq.pop();
    if (n.coord.y == heat_map.size() - 1 &&
        n.coord.x == heat_map[0].length() - 1) {
      final_dest.push_back(n);
    }
    visited[n.coord.y][n.coord.x] = 'x';
    if (n.coord.y + 1 < heat_map.size() && n.downs_in_a_row < 3) {
      size_t heat_loss = get_heat_loss(heat_map, n.coord.y + 1, n.coord.x);
      Node new_node = n;
      new_node.weight += heat_loss;
      new_node.coord.y = n.coord.y + 1;
      new_node.downs_in_a_row += 1;
      new_node.lefts_in_a_row = 0;
      new_node.rights_in_a_row = 0;
      new_node.ups_in_a_row = 0;
      if (visited[new_node.coord.y][new_node.coord.x] != 'x') {
        pq.push(new_node);
      }
    }
    if (n.coord.y > 0 && n.ups_in_a_row < 3) {
      size_t heat_loss = get_heat_loss(heat_map, n.coord.y - 1, n.coord.x);
      Node new_node = n;
      new_node.weight += heat_loss;
      new_node.coord.y = n.coord.y - 1;
      new_node.ups_in_a_row += 1;
      new_node.lefts_in_a_row = 0;
      new_node.rights_in_a_row = 0;
      new_node.downs_in_a_row = 0;
      if (visited[new_node.coord.y][new_node.coord.x] != 'x') {
        pq.push(new_node);
      }
    }
    if (n.coord.x + 1 < heat_map[0].length() && n.rights_in_a_row < 3) {
      size_t heat_loss = get_heat_loss(heat_map, n.coord.y, n.coord.x + 1);
      Node new_node = n;
      new_node.weight += heat_loss;
      new_node.coord.x = n.coord.x + 1;
      new_node.downs_in_a_row = 0;
      new_node.lefts_in_a_row = 0;
      new_node.rights_in_a_row += 1;
      new_node.ups_in_a_row = 0;
      if (visited[new_node.coord.y][new_node.coord.x] != 'x') {
        pq.push(new_node);
      }
    }
    if (n.coord.x > 0 && n.lefts_in_a_row < 3) {
      size_t heat_loss = get_heat_loss(heat_map, n.coord.y, n.coord.x - 1);
      Node new_node = n;
      new_node.weight += heat_loss;
      new_node.coord.x = n.coord.x - 1;
      new_node.downs_in_a_row = 0;
      new_node.lefts_in_a_row += 1;
      new_node.rights_in_a_row = 0;
      new_node.ups_in_a_row = 0;
      if (visited[new_node.coord.y][new_node.coord.x] != 'x') {
        pq.push(new_node);
      }
    }
  }
  std::sort(final_dest.begin(), final_dest.end(),
            [](const Node &a, const Node &b) { return a.weight < b.weight; });
  std::cout << std::format("heat_loss: {}\n", final_dest[0].weight);
  return 0;
}
