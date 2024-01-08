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
  explicit operator std::pair<int64_t, int64_t>() const {
    return std::pair<int64_t, int64_t>{x, y};
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

enum class Dir {
  L,
  R,
  U,
  D,
};
struct Node {
  int64_t weight;
  Coord coord;
  Dir facing;
  int64_t num_steps;
  Node() = default;
  Node(int64_t w, Coord c, Dir facing, int ns) {
    weight = w;
    coord = c;
    this->facing = facing;
    num_steps = ns;
  }
  Node(const Node &n) { *this = n; }
  Node &operator=(const Node &n) {
    this->weight = n.weight;
    this->coord = n.coord;
    this->facing = n.facing;
    this->num_steps = n.num_steps;
    return *this;
  }
  void rotate_cw_and_step() {
    switch (facing) {
    case Dir::U:
      facing = Dir::R;
      coord.x += 1;
      break;
    case Dir::D:
      facing = Dir::L;
      coord.x -= 1;
      break;
    case Dir::L:
      facing = Dir::U;
      coord.y -= 1;
      break;
    case Dir::R:
      facing = Dir::D;
      coord.y += 1;
      break;
    }
    num_steps = 1;
  }
  void rotate_ccw_and_step() {
    switch (facing) {
    case Dir::U:
      facing = Dir::L;
      coord.x -= 1;
      break;
    case Dir::D:
      facing = Dir::R;
      coord.x += 1;
      break;
    case Dir::L:
      facing = Dir::D;
      coord.y += 1;
      break;
    case Dir::R:
      facing = Dir::U;
      coord.y -= 1;
      break;
    }
    num_steps = 1;
  }
  void forward_and_step() {
    switch (facing) {
    case Dir::L:
      coord.x -= 1;
      break;
    case Dir::R:
      coord.x += 1;
      break;
    case Dir::U:
      coord.y -= 1;
      break;
    case Dir::D:
      coord.y += 1;
      break;
    }
    num_steps += 1;
  }
};
bool operator==(const Node &a, const Node &b) { return a.coord == b.coord; }

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
  // I had the right idea to use dijkstra's but failed to see how I could optimize it
  // by just using one outer set and hashing something that would make it unique for certain paths
  // I also didn't see that this solution would be fast enough
  // credit to xavdid ( I took their solution in python and converted it to c++; the problem was that i didn't use a good enough hashing method to make things fast when I tried dijkstra's )
  auto calc = [&heat_map](int64_t min_steps, int64_t max_steps) -> int64_t {
    auto cmp = [](const Node &a, const Node &b) { return a.weight > b.weight; };
    std::priority_queue<Node, std::deque<Node>, decltype(cmp)> pq(cmp);
    pq.push(Node(0, {0, 0}, Dir::R, 0));
    pq.push(Node(0, {0, 0}, Dir::D, 0));
    std::set<std::tuple<int64_t, int64_t, Dir, int64_t>> seen;
    while (pq.size() > 0) {
      Node n = pq.top();
      pq.pop();
      if (n.coord == Coord(heat_map[0].length() - 1, heat_map.size() - 1) && n.num_steps >= min_steps) {
        return n.weight;
      }
      if (seen.contains({n.coord.x, n.coord.y, n.facing, n.num_steps})) {
        continue;
      }
      seen.insert({n.coord.x, n.coord.y, n.facing, n.num_steps});

      Node cwn = n;
      cwn.rotate_cw_and_step();
      Node ccwn = n;
      ccwn.rotate_ccw_and_step();
      assert(cwn.num_steps == 1 && ccwn.num_steps == 1);
      Node fn = n;
      fn.forward_and_step();

      if (ccwn.coord.x >= 0 && ccwn.coord.x < (int64_t)heat_map[0].length() &&
          ccwn.coord.y >= 0 && ccwn.coord.y < (int64_t)heat_map.size() && n.num_steps >= min_steps) {
        ccwn.weight = n.weight + (heat_map[ccwn.coord.y][ccwn.coord.x] - '0');
        pq.push(ccwn);
      }
      if (cwn.coord.x >= 0 && cwn.coord.x < (int64_t)heat_map[0].length() &&
          cwn.coord.y >= 0 && cwn.coord.y < (int64_t)heat_map.size() && n.num_steps >= min_steps) {
        cwn.weight = n.weight + (heat_map[cwn.coord.y][cwn.coord.x] - '0');
        pq.push(cwn);
      }
      if (fn.coord.x >= 0 && fn.coord.x < (int64_t)heat_map[0].length() &&
          fn.coord.y >= 0 && fn.coord.y < (int64_t)heat_map.size() &&
          fn.num_steps <= max_steps) {
        fn.weight = n.weight + (heat_map[fn.coord.y][fn.coord.x] - '0');
        pq.push(fn);
      }
    }
    return -1;
  };
  std::cout << "PART1: " << calc(0, 3) << std::endl;
  std::cout << "PART2: " << calc(4, 10) << std::endl;
  return 0;
}
