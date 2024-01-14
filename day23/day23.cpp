#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
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
  Range() = default;
  Range(int64_t s, int64_t l) {
    start = s;
    length = l;
  }
};
std::ostream &operator<<(std::ostream &os, const Range &r) {
  os << std::format("start: {}, length: {}", r.start, r.length);
  return os;
}
bool operator==(const Range &a, const Range &b) {
  return a.start == b.start && a.length == b.length;
}
struct Region {
  Coord top_left;
  Coord top_right;
  Coord bottom_left;
  Coord bottom_right;
  Region(Coord one, Coord two, Coord three, Coord four) {
    top_left = one;
    top_right = two;
    bottom_left = three;
    bottom_right = four;
  }
};
struct Point {
  int64_t x;
  int64_t y;
  int64_t w;
  std::set<std::pair<int64_t, int64_t>> visited;
  Point(int64_t x, int64_t y, int64_t w) {
    this->x = x;
    this->y = y;
    this->w = w;
  }
  bool been_to(std::pair<int64_t, int64_t> p) { return visited.contains(p); }
  void add(std::pair<int64_t, int64_t> p) { visited.insert(p); }
};
template <typename A, typename B>
std::ostream &operator<<(std::ostream &os, const std::pair<A, B> &p) {
  os << p.first << " " << p.second;
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
  const int64_t LINE_CAPACITY = 1 << 16;
  char line[LINE_CAPACITY];
  std::vector<std::string> hiking_trails;
  while (input.getline(line, LINE_CAPACITY)) {
    int64_t line_length = input.gcount() - 1;
    hiking_trails.push_back(std::string(line, line_length));
  }
  input.close();

  Point start(1, 0, 0);
  start.add({0, 1});
  assert(
      hiking_trails[hiking_trails.size() - 1][hiking_trails[0].length() - 2] ==
      '.');
  {
    int64_t ans = 0;
    std::queue<Point> points;
    points.push(start);
    while (points.size() > 0) {
      Point top = points.front();
      // std::cout << top.y << " " << top.x << " " << top.w << std::endl;
      points.pop();
      if (top.y == (int64_t)hiking_trails.size() - 1 &&
          top.x == (int64_t)hiking_trails[0].length() - 2) {
        // std::cout << top.w << std::endl;
        // auto hiking_trails_copy = hiking_trails;
        // for (const auto& p : top.visited) {
        //   hiking_trails_copy[p.first][p.second] = 'O';
        // }
        // for (const std::string& s : hiking_trails_copy) {
        //   std::cout << s << std::endl;
        // }
        // std::cout << std::endl;
        ans = std::max(ans, top.w);
        continue;
      }
      auto process_char = [](char next, const Point &p) -> Point {
        switch (next) {
        case '>':
          return Point(p.x + 1, p.y, p.w + 1);
        case '<':
          return Point(p.x - 1, p.y, p.w + 1);
        case '^':
          return Point(p.x, p.y - 1, p.w + 1);
        case 'v':
          return Point(p.x, p.y + 1, p.w + 1);
        }
        return p;
      };
      int64_t new_row = top.y + 1;
      if (new_row < (int64_t)hiking_trails.size() &&
          hiking_trails[new_row][top.x] != '#' &&
          !top.been_to({new_row, top.x}) &&
          hiking_trails[new_row][top.x] != '^') {
        Point next = process_char(hiking_trails[new_row][top.x],
                                  Point(top.x, new_row, top.w + 1));
        next.visited = top.visited;
        next.add({new_row, top.x});
        next.add({next.y, next.x});
        points.push(next);
      }
      if (top.y > 0 && hiking_trails[top.y - 1][top.x] != '#' &&
          !top.been_to({top.y - 1, top.x}) &&
          hiking_trails[top.y - 1][top.x] != 'v') {
        Point next = process_char(hiking_trails[top.y - 1][top.x],
                                  Point(top.x, top.y - 1, top.w + 1));
        next.visited = top.visited;
        next.add({top.y - 1, top.x});
        next.add({next.y, next.x});
        points.push(next);
      }
      if (top.x + 1 < (int64_t)hiking_trails[0].length() &&
          hiking_trails[top.y][top.x + 1] != '#' &&
          !top.been_to({top.y, top.x + 1}) &&
          hiking_trails[top.y][top.x + 1] != '<') {
        Point next = process_char(hiking_trails[top.y][top.x + 1],
                                  Point(top.x + 1, top.y, top.w + 1));
        next.visited = top.visited;
        next.add({top.y, top.x + 1});
        next.add({next.y, next.x});
        points.push(next);
      }
      if (top.x > 0 && hiking_trails[top.y][top.x - 1] != '#' &&
          !top.been_to({top.y, top.x - 1}) &&
          hiking_trails[top.y][top.x - 1] != '>') {
        Point next = process_char(hiking_trails[top.y][top.x - 1],
                                  Point(top.x - 1, top.y, top.w + 1));
        next.visited = top.visited;
        next.add({top.y, top.x - 1});
        next.add({next.y, next.x});
        points.push(next);
      }
    }
    std::cout << "PART1: " << ans << std::endl;
  }
  for (size_t row = 0; row < hiking_trails.size(); ++row) {
    for (size_t col = 0; col < hiking_trails[row].length(); ++col) {
      if (hiking_trails[row][col] == 'v' || hiking_trails[row][col] == '^' ||
          hiking_trails[row][col] == '>' || hiking_trails[row][col] == '<') {
        hiking_trails[row][col] = '.';
      }
    }
  }
  {
    auto is_intersection = [&hiking_trails](int64_t row, int64_t col)
        -> std::vector<std::pair<int64_t, int64_t>> {
      bool up = row > 0 && hiking_trails[row - 1][col] == '.';
      bool down = row + 1 < (int64_t)hiking_trails.size() &&
                  hiking_trails[row + 1][col] == '.';
      bool vertical = up && down;
      bool left = col > 0 && hiking_trails[row][col - 1] == '.';
      bool right = col + 1 < (int64_t)hiking_trails[row].length() &&
                   hiking_trails[row][col + 1] == '.';
      bool horizontal = left && right;
      if (vertical && (left || right)) {
        std::vector<std::pair<int64_t, int64_t>> coords;
        coords.push_back({row - 1, col});
        coords.push_back({row + 1, col});
        if (left) {
          coords.push_back({row, col - 1});
        }
        if (right) {
          coords.push_back({row, col + 1});
        }
        return coords;
      }
      if (horizontal && (up || down)) {
        std::vector<std::pair<int64_t, int64_t>> coords;
        coords.push_back({row, col - 1});
        coords.push_back({row, col + 1});
        if (up) {
          coords.push_back({row - 1, col});
        }
        if (down) {
          coords.push_back({row + 1, col});
        }
        return coords;
      }
      return {};
    };
    struct Node {
      std::vector<int64_t> w;
      std::vector<std::pair<int64_t, int64_t>> neighbors;
    };
    std::map<std::pair<int64_t, int64_t>, Node> graph;
    std::function<void(const std::pair<int64_t, int64_t> &,
                       std::set<std::pair<int64_t, int64_t>>, int64_t, int64_t,
                       int64_t)>
        dfs_until_intersection = [&dfs_until_intersection, &hiking_trails,
                                  &is_intersection, &graph](
                                     const std::pair<int64_t, int64_t> &orig,
                                     std::set<std::pair<int64_t, int64_t>>
                                         visited,
                                     int64_t row, int64_t col, int64_t steps) {
          if (row > 0 && hiking_trails[row - 1][col] == '.' &&
              !visited.contains({row - 1, col})) {
            auto v = is_intersection(row - 1, col);
            if (v.empty()) {
              visited.insert({row - 1, col});
              dfs_until_intersection(orig, visited, row - 1, col, steps + 1);
            } else {
              if (graph.contains({row - 1, col})) {
                graph[{row - 1, col}].neighbors.push_back(orig);
                graph[{row - 1, col}].w.push_back(steps + 1);
              } else {
                graph.insert(
                    {{row - 1, col},
                     Node{{steps + 1},
                          std::vector<std::pair<int64_t, int64_t>>{orig}}});
              }
            }
          }
          if (row + 1 < (int64_t)hiking_trails.size() &&
              hiking_trails[row + 1][col] == '.' &&
              !visited.contains({row + 1, col})) {
            auto v = is_intersection(row + 1, col);
            if (v.empty()) {
              visited.insert({row + 1, col});
              dfs_until_intersection(orig, visited, row + 1, col, steps + 1);
            } else {
              if (graph.contains({row + 1, col})) {
                graph[{row + 1, col}].neighbors.push_back(orig);
                graph[{row + 1, col}].w.push_back(steps + 1);
              } else {
                graph.insert(
                    {{row + 1, col},
                     Node{{steps + 1},
                          std::vector<std::pair<int64_t, int64_t>>{orig}}});
              }
            }
          }
          if (col > 0 && hiking_trails[row][col - 1] == '.' &&
              !visited.contains({row, col - 1})) {
            auto v = is_intersection(row, col - 1);
            if (v.empty()) {
              visited.insert({row, col - 1});
              dfs_until_intersection(orig, visited, row, col - 1, steps + 1);
            } else {
              if (graph.contains({row, col - 1})) {
                graph[{row, col - 1}].neighbors.push_back(orig);
                graph[{row, col - 1}].w.push_back(steps + 1);
              } else {
                graph.insert(
                    {{row, col - 1},
                     Node{{steps + 1},
                          std::vector<std::pair<int64_t, int64_t>>{orig}}});
              }
            }
          }
          if (col + 1 < (int64_t)hiking_trails[0].length() &&
              hiking_trails[row][col + 1] == '.' &&
              !visited.contains({row, col + 1})) {
            auto v = is_intersection(row, col + 1);
            if (v.empty()) {
              visited.insert({row, col + 1});
              dfs_until_intersection(orig, visited, row, col + 1, steps + 1);
            } else {
              if (v.empty()) {
                visited.insert({row, col + 1});
                dfs_until_intersection(orig, visited, row, col + 1, steps + 1);
              } else {
                if (graph.contains({row, col + 1})) {
                  graph[{row, col + 1}].neighbors.push_back(orig);
                  graph[{row, col + 1}].w.push_back(steps + 1);
                } else {
                  graph.insert(
                      {{row, col + 1},
                       Node{{steps + 1},
                            std::vector<std::pair<int64_t, int64_t>>{orig}}});
                }
              }
            }
          }
        };
    for (size_t row = 0; row < hiking_trails.size(); ++row) {
      for (size_t col = 0; col < hiking_trails[row].size(); ++col) {
        if (hiking_trails[row][col] == '.') {
          auto v = is_intersection(row, col);
          if (v.size() > 0) {
            for (const auto &p : v) {
              dfs_until_intersection({row, col},
                                     {{p.first, p.second}, {row, col}}, p.first,
                                     p.second, 1);
            }
          }
        }
      }
    }
    dfs_until_intersection(
        {hiking_trails.size() - 1, hiking_trails[0].length() - 2},
        {{hiking_trails.size() - 1, hiking_trails[0].length() - 2}},
        hiking_trails.size() - 1, hiking_trails[0].length() - 2, 0);
    dfs_until_intersection({0, 1}, {{0, 1}}, 0, 1, 0);
    for (const auto &p : graph) {
      std::cout << p.first << std::endl;
      for (size_t idx = 0; idx < p.second.w.size(); ++idx) {
        std::cout << p.second.neighbors[idx] << " " << p.second.w[idx]
                  << std::endl;
      }
    }
    std::pair<int64_t, int64_t> starting;
    int64_t starting_w = 0;
    for (const auto &p : graph) {
      auto fd = std::find(p.second.neighbors.begin(), p.second.neighbors.end(),
                          std::pair<int64_t, int64_t>{0, 1});
      if (fd != p.second.neighbors.end()) {
        starting = p.first;
        size_t dist = std::distance(p.second.neighbors.begin(), fd);
        auto start_w = p.second.w.begin();
        std::advance(start_w, dist);
        starting_w = *start_w;
        break;
      }
    }
    std::function<void(std::pair<int64_t, int64_t>, int64_t,
                       std::set<std::pair<int64_t, int64_t>>, int64_t &)>
        dfs_ans = [&dfs_ans, &graph, &hiking_trails](
                      std::pair<int64_t, int64_t> curr, int64_t w,
                      std::set<std::pair<int64_t, int64_t>> visited,
                      int64_t &ans) {
          for (size_t idx = 0; idx < graph[curr].neighbors.size(); ++idx) {
            if (visited.contains(graph[curr].neighbors[idx])) {
              continue;
            }
            if (graph[curr].neighbors[idx] ==
                std::pair<int64_t, int64_t>{hiking_trails.size() - 1,
                                            hiking_trails[0].length() - 2}) {
              ans = std::max(ans, w + graph[curr].w[idx]);
            }
            if (!graph.contains(graph[curr].neighbors[idx])) {
              continue;
            }
            visited.insert(graph[curr].neighbors[idx]);
            dfs_ans(graph[curr].neighbors[idx], w + graph[curr].w[idx], visited,
                    ans);
            visited.erase(graph[curr].neighbors[idx]);
          }
        };
    int64_t ans = 0;
    dfs_ans(starting, starting_w, {starting}, ans);
    std::cout << "PART2: " << ans << std::endl;
  }
  return 0;
}
