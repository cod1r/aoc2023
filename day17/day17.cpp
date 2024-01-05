#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <deque>
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
#include <unistd.h>
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
};

struct Node {
  bool visited;
  int64_t weight;
  Coord coord;
  std::set<std::pair<int64_t, int64_t>> prev;
  int64_t lefts_in_a_row;
  int64_t rights_in_a_row;
  int64_t ups_in_a_row;
  int64_t downs_in_a_row;
  Node() = default;
  Node(int64_t w, Coord c, int64_t l, int64_t r, int64_t u, int64_t d) {
    weight = w;
    coord = c;
    lefts_in_a_row = l;
    rights_in_a_row = r;
    ups_in_a_row = u;
    downs_in_a_row = d;
    visited = false;
  }
  Node(const Node &n) { *this = n; }
  Node &operator=(const Node &n) {
    this->weight = n.weight;
    this->coord = n.coord;
    this->lefts_in_a_row = n.lefts_in_a_row;
    this->rights_in_a_row = n.rights_in_a_row;
    this->ups_in_a_row = n.ups_in_a_row;
    this->downs_in_a_row = n.downs_in_a_row;
    this->prev = n.prev;
    return *this;
  }
};
bool operator==(const Node &a, const Node &b) { return a.coord == b.coord; }

int64_t get_heat_loss(const std::vector<std::string> &heat_map, int64_t row,
                      int64_t col) {
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
  const int64_t LINE_CAPACITY = 1 << 16;
  char line[LINE_CAPACITY];
  std::vector<std::string> heat_map;
  while (input.getline(line, LINE_CAPACITY)) {
    int64_t line_length = input.gcount() - 1;
    heat_map.push_back(std::string(line, line_length));
  }
  auto cmp = [](const Node &a, const Node &b) { return a.weight > b.weight; };
  std::priority_queue<Node, std::deque<Node>, decltype(cmp)> pq;
  Node start(0, Coord(0, 0), 0, 0, 0, 0);
  pq.push(start);
  std::vector<std::vector<int64_t>> heat_losses(
      heat_map.size(), std::vector<int64_t>(heat_map[0].length(), 1'000'000));
  while (pq.size() > 0) {
    Node current_node = pq.top();
    heat_losses[current_node.coord.y][current_node.coord.x] =
        current_node.weight;
        std::cout << current_node.coord << std::endl;
    pq.pop();
    if (current_node.coord ==
        Coord(heat_map[0].length() - 1, heat_map.size() - 1)) {
      std::cout << current_node.weight << std::endl;
      break;
    }
    if (current_node.coord.x + 1 < (int64_t)heat_map[0].length() &&
        current_node.rights_in_a_row + 1 <= 3 &&
        !current_node.prev.contains(
            {current_node.coord.x + 1, current_node.coord.y})) {
      int64_t heat_loss = get_heat_loss(heat_map, current_node.coord.y,
                                        current_node.coord.x + 1);
      Node new_node(current_node.weight + heat_loss,
                    Coord(current_node.coord.x + 1, current_node.coord.y), 0,
                    current_node.rights_in_a_row + 1, 0, 0);
      new_node.prev = current_node.prev;
      new_node.prev.insert({current_node.coord.x, current_node.coord.y});
      if (new_node.weight <=
          heat_losses[new_node.coord.y][new_node.coord.x]) {
        pq.push(new_node);
      }
    }
    if (current_node.coord.x > 0 && current_node.lefts_in_a_row + 1 <= 3 &&
        !current_node.prev.contains(
            {current_node.coord.x - 1, current_node.coord.y})) {
      int64_t heat_loss = get_heat_loss(heat_map, current_node.coord.y,
                                        current_node.coord.x - 1);
      Node new_node(current_node.weight + heat_loss,
                    Coord(current_node.coord.x - 1, current_node.coord.y),
                    current_node.lefts_in_a_row + 1, 0, 0, 0);
      new_node.prev = current_node.prev;
      new_node.prev.insert({current_node.coord.x, current_node.coord.y});
      if (new_node.weight <=
          heat_losses[new_node.coord.y][new_node.coord.x]) {
        pq.push(new_node);
      }
    }
    if (current_node.coord.y + 1 < (int64_t)heat_map.size() &&
        current_node.downs_in_a_row + 1 <= 3 &&
        !current_node.prev.contains(
            {current_node.coord.x, current_node.coord.y + 1})) {
      int64_t heat_loss = get_heat_loss(heat_map, current_node.coord.y + 1,
                                        current_node.coord.x);
      Node new_node(current_node.weight + heat_loss,
                    Coord(current_node.coord.x, current_node.coord.y + 1), 0, 0,
                    0, current_node.downs_in_a_row + 1);
      new_node.prev = current_node.prev;
      new_node.prev.insert({current_node.coord.x, current_node.coord.y});
      if (new_node.weight <=
          heat_losses[new_node.coord.y][new_node.coord.x]) {
        pq.push(new_node);
      }
    }
    if (current_node.coord.y > 0 && current_node.ups_in_a_row + 1 <= 3 &&
        !current_node.prev.contains(
            {current_node.coord.x, current_node.coord.y - 1})) {
      int64_t heat_loss = get_heat_loss(heat_map, current_node.coord.y - 1,
                                        current_node.coord.x);
      Node new_node(current_node.weight + heat_loss,
                    Coord(current_node.coord.x, current_node.coord.y - 1), 0, 0,
                    current_node.ups_in_a_row + 1, 0);
      new_node.prev = current_node.prev;
      new_node.prev.insert({current_node.coord.x, current_node.coord.y});
      if (new_node.weight <=
          heat_losses[new_node.coord.y][new_node.coord.x]) {
        pq.push(new_node);
      }
    }
  }
  return 0;
}
