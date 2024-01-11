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
typedef std::tuple<int64_t, int64_t, int64_t> BrickEnd;
typedef std::pair<BrickEnd, BrickEnd> Brick;
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
  std::vector<Brick> bricks;
  while (input.getline(line, LINE_CAPACITY)) {
    int64_t line_length = input.gcount() - 1;
    std::string line_str(line, line_length);
    size_t pos = line_str.find('~');
    auto v = parse_string_to_vec_numbers(line, pos, ',');
    assert(v.size() == 3);
    BrickEnd be1{v[0], v[1], v[2]};
    auto v2 = parse_string_to_vec_numbers(line + pos + 1,
                                          line_length - (pos + 1), ',');
    BrickEnd be2{v2[0], v2[1], v2[2]};
    bricks.push_back({be1, be2});
  }
  input.close();

  int64_t min_z_overall = 1'000'000;
  int64_t max_z_overall = -1'000'000;
  int64_t min_y_overall = 1'000'000;
  int64_t max_y_overall = -1'000'000;
  int64_t min_x_overall = 1'000'000;
  int64_t max_x_overall = -1'000'000;
  std::for_each(bricks.begin(), bricks.end(), [&](const Brick &b) {
    min_z_overall =
        std::min({min_z_overall, std::get<2>(b.first), std::get<2>(b.second)});
    max_z_overall =
        std::max({max_z_overall, std::get<2>(b.first), std::get<2>(b.second)});

    min_y_overall =
        std::min({min_y_overall, std::get<1>(b.first), std::get<1>(b.second)});
    max_y_overall =
        std::max({max_y_overall, std::get<1>(b.first), std::get<1>(b.second)});

    min_x_overall =
        std::min({min_x_overall, std::get<0>(b.first), std::get<0>(b.second)});
    max_x_overall =
        std::max({max_x_overall, std::get<0>(b.first), std::get<0>(b.second)});
  });
  assert(min_z_overall > 0);
  std::vector<std::vector<std::vector<int64_t>>> falling(
      max_x_overall + 1,
      std::vector<std::vector<int64_t>>(
          max_y_overall + 1, std::vector<int64_t>(max_z_overall + 2, 0)));
  for (int64_t x = min_x_overall; x <= max_x_overall; ++x) {
    for (int64_t y = min_y_overall; y <= max_y_overall; ++y) {
      assert((int64_t)falling.size() > x);
      assert((int64_t)falling[x].size() > y);
      assert((int64_t)falling[x][y].size() > max_z_overall + 1);
      falling[x][y][max_z_overall + 1] = 1'000'000;
    }
  }
  size_t brick_number = 1;
  for (const Brick &b : bricks) {
    int64_t min_z = std::min({std::get<2>(b.first), std::get<2>(b.second)});
    int64_t max_z = std::max({std::get<2>(b.first), std::get<2>(b.second)});

    int64_t min_y = std::min({std::get<1>(b.first), std::get<1>(b.second)});
    int64_t min_x = std::min({std::get<0>(b.first), std::get<0>(b.second)});

    int64_t max_y = std::max({std::get<1>(b.first), std::get<1>(b.second)});
    int64_t max_x = std::max({std::get<0>(b.first), std::get<0>(b.second)});
    for (int64_t y = min_y; y <= max_y; ++y) {
      for (int64_t x = min_x; x <= max_x; ++x) {
        for (int64_t z = min_z; z <= max_z; ++z) {
          falling[x][y][max_z_overall + 1 - z] = brick_number;
        }
      }
    }
    ++brick_number;
  }
  std::sort(bricks.begin(), bricks.end(), [](const Brick &b, const Brick &b2) {
    return std::get<2>(b.first) < std::get<2>(b2.first);
  });
  // Z AXIS IS FLIPPED SO THE "MINIMUM Z" IS ACTUALLY THE MAXIMUM INDEX
  for (Brick &b : bricks) {
    int64_t min_z = std::min({std::get<2>(b.first), std::get<2>(b.second)});
    int64_t max_z = std::max({std::get<2>(b.first), std::get<2>(b.second)});
    const int64_t old_min_z = min_z;

    int64_t min_y = std::min({std::get<1>(b.first), std::get<1>(b.second)});
    int64_t min_x = std::min({std::get<0>(b.first), std::get<0>(b.second)});

    int64_t max_y = std::max({std::get<1>(b.first), std::get<1>(b.second)});
    int64_t max_x = std::max({std::get<0>(b.first), std::get<0>(b.second)});

    auto is_clear_below = [&]() -> bool {
      for (int64_t y = min_y; y <= max_y; ++y) {
        for (int64_t x = min_x; x <= max_x; ++x) {
          if (falling[x][y][max_z_overall + 1 - (min_z - 1)]) {
            return false;
          }
        }
      }
      return true;
    };
    while (is_clear_below()) {
      min_z -= 1;
    }
    std::optional<int64_t> brick_number;
    for (int64_t y = min_y; y <= max_y; ++y) {
      for (int64_t x = min_x; x <= max_x; ++x) {
        for (int64_t z = old_min_z; z <= max_z; ++z) {
          if (!brick_number.has_value()) {
            brick_number = falling[x][y][max_z_overall + 1 - z];
          }
          falling[x][y][max_z_overall + 1 - z] = 0;
        }
      }
    }
    std::get<2>(b.first) -= old_min_z - min_z;
    std::get<2>(b.second) -= old_min_z - min_z;
    int64_t new_min_z = std::min({std::get<2>(b.first), std::get<2>(b.second)});
    int64_t new_max_z = std::max({std::get<2>(b.first), std::get<2>(b.second)});
    for (int64_t y = min_y; y <= max_y; ++y) {
      for (int64_t x = min_x; x <= max_x; ++x) {
        for (int64_t z = new_min_z; z <= new_max_z; ++z) {
          assert(brick_number.has_value());
          falling[x][y][max_z_overall + 1 - z] = *brick_number;
        }
      }
    }
    std::set<int64_t> bricks_under;
    for (int64_t x = min_x; x <= max_x; ++x) {
      for (int64_t y = min_y; y <= max_y; ++y) {
        int64_t bu = falling[x][y][max_z_overall + 1 - (new_min_z - 1)];
        if (bu != 1'000'000 && bu > 0) {
          bricks_under.insert(bu);
        }
      }
    }
    if (new_min_z > 1) {
      assert(bricks_under.size() > 0);
    }
  }
  // for (const Brick &b : bricks) {
  //   std::cout << std::get<0>(b.first) << " " << std::get<1>(b.first) << " "
  //             << std::get<2>(b.first) << std::endl;
  //   std::cout << std::get<0>(b.second) << " " << std::get<1>(b.second) << " "
  //             << std::get<2>(b.second) << std::endl;
  //   std::cout << std::endl;
  // }
  //  brick numbers that cannot be disintegrated
  std::set<size_t> brick_numbers;
  for (size_t bn : std::views::iota((size_t)1, brick_number)) {
    brick_numbers.insert(bn);
  }
  for (const Brick &b : bricks) {
    int64_t min_z = std::min({std::get<2>(b.first), std::get<2>(b.second)});

    int64_t min_y = std::min({std::get<1>(b.first), std::get<1>(b.second)});
    int64_t min_x = std::min({std::get<0>(b.first), std::get<0>(b.second)});

    int64_t max_y = std::max({std::get<1>(b.first), std::get<1>(b.second)});
    int64_t max_x = std::max({std::get<0>(b.first), std::get<0>(b.second)});

    std::set<int64_t> bricks_under;
    for (int64_t y = min_y; y <= max_y; ++y) {
      for (int64_t x = min_x; x <= max_x; ++x) {
        int64_t b_num = falling[x][y][max_z_overall + 1 - (min_z - 1)];
        if (b_num > 0 && !bricks_under.contains(b_num) && b_num != 1'000'000) {
          bricks_under.insert(b_num);
        }
      }
    }
    if (min_z > 1) {
      assert(bricks_under.size() > 0);
    }
    if (bricks_under.size() == 1) {
      size_t cannot = (size_t)*bricks_under.begin();
      brick_numbers.erase(cannot);
    }
  }
  std::cout << "PART1: " << brick_numbers.size() << std::endl;

  std::map<int64_t, Brick> num_to_brick;
  for (const Brick &b : bricks) {
    int64_t min_z = std::min({std::get<2>(b.first), std::get<2>(b.second)});
    int64_t min_y = std::min({std::get<1>(b.first), std::get<1>(b.second)});
    int64_t min_x = std::min({std::get<0>(b.first), std::get<0>(b.second)});
    num_to_brick[falling[min_x][min_y][max_z_overall + 1 - min_z]] = b;
  }
  auto get_supports = [&num_to_brick, &falling,
                       &max_z_overall](int64_t brick_num) -> std::set<int64_t> {
    const Brick &b = num_to_brick[brick_num];
    int64_t min_z = std::min({std::get<2>(b.first), std::get<2>(b.second)});

    int64_t min_y = std::min({std::get<1>(b.first), std::get<1>(b.second)});
    int64_t min_x = std::min({std::get<0>(b.first), std::get<0>(b.second)});

    int64_t max_y = std::max({std::get<1>(b.first), std::get<1>(b.second)});
    int64_t max_x = std::max({std::get<0>(b.first), std::get<0>(b.second)});
    std::set<int64_t> supports;
    for (int64_t y = min_y; y <= max_y; ++y) {
      for (int64_t x = min_x; x <= max_x; ++x) {
        int64_t b_num = falling[x][y][max_z_overall + 1 - (min_z - 1)];
        if (b_num > 0 && !supports.contains(b_num) && b_num != 1'000'000) {
          supports.insert(b_num);
        }
      }
    }
    return supports;
  };
  std::map<int64_t, std::set<int64_t>> above_brick;
  for (const auto &p : num_to_brick) {
    auto v = get_supports(p.first);
    for (int64_t bn : v) {
      if (above_brick.contains(bn)) {
        above_brick[bn].insert(p.first);
      } else {
        above_brick.insert({bn, {p.first}});
      }
    }
  }
  int64_t part2 = 0;
  for (const Brick &b : bricks) {
    auto falling_copy = falling;
    auto bricks_copy = bricks;
    auto get_brick_num = [&falling, &max_z_overall](const Brick& b) {
      int64_t min_z = std::min({std::get<2>(b.first), std::get<2>(b.second)});
      int64_t min_y = std::min({std::get<1>(b.first), std::get<1>(b.second)});
      int64_t min_x = std::min({std::get<0>(b.first), std::get<0>(b.second)});
      int64_t the_brick_number = falling[min_x][min_y][max_z_overall + 1 - min_z];
      return the_brick_number;
    };
    auto reset = [&falling_copy, &max_z_overall](const Brick &b) {
      int64_t min_z = std::min({std::get<2>(b.first), std::get<2>(b.second)});
      int64_t max_z = std::max({std::get<2>(b.first), std::get<2>(b.second)});

      int64_t min_y = std::min({std::get<1>(b.first), std::get<1>(b.second)});
      int64_t min_x = std::min({std::get<0>(b.first), std::get<0>(b.second)});

      int64_t max_y = std::max({std::get<1>(b.first), std::get<1>(b.second)});
      int64_t max_x = std::max({std::get<0>(b.first), std::get<0>(b.second)});

      for (int64_t y = min_y; y <= max_y; ++y) {
        for (int64_t x = min_x; x <= max_x; ++x) {
          for (int64_t z = min_z; z <= max_z; ++z) {
            falling_copy[x][y][max_z_overall + 1 - z] = 0;
          }
        }
      }
    };
    reset(b);
    auto is_clear_below = [&falling_copy,
                           max_z_overall](const Brick &brick) -> bool {
      int64_t min_z =
          std::min({std::get<2>(brick.first), std::get<2>(brick.second)});

      int64_t min_y =
          std::min({std::get<1>(brick.first), std::get<1>(brick.second)});
      int64_t min_x =
          std::min({std::get<0>(brick.first), std::get<0>(brick.second)});

      int64_t max_y =
          std::max({std::get<1>(brick.first), std::get<1>(brick.second)});
      int64_t max_x =
          std::max({std::get<0>(brick.first), std::get<0>(brick.second)});

      for (int64_t y = min_y; y <= max_y; ++y) {
        for (int64_t x = min_x; x <= max_x; ++x) {
          if (falling_copy[x][y][max_z_overall + 1 - (min_z - 1)]) {
            return false;
          }
        }
      }
      return true;
    };
    int64_t moved = 0;
    for (Brick &b2 : bricks_copy) {
      if (is_clear_below(b2)) {
        moved += 1;
        int64_t min_z =
            std::min({std::get<2>(b2.first), std::get<2>(b2.second)});
        int64_t max_z =
            std::max({std::get<2>(b2.first), std::get<2>(b2.second)});

        int64_t min_y =
            std::min({std::get<1>(b2.first), std::get<1>(b2.second)});
        int64_t min_x =
            std::min({std::get<0>(b2.first), std::get<0>(b2.second)});

        int64_t max_y =
            std::max({std::get<1>(b2.first), std::get<1>(b2.second)});
        int64_t max_x =
            std::max({std::get<0>(b2.first), std::get<0>(b2.second)});
        int64_t num_of_brick = get_brick_num(b2);
        reset(b2);
        std::get<2>(b2.first) -= 1;
        std::get<2>(b2.second) -= 1;
        min_z = std::min({std::get<2>(b2.first), std::get<2>(b2.second)});
        max_z = std::max({std::get<2>(b2.first), std::get<2>(b2.second)});
        for (int64_t y = min_y; y <= max_y; ++y) {
          for (int64_t x = min_x; x <= max_x; ++x) {
            for (int64_t z = min_z; z <= max_z; ++z) {
              falling_copy[x][y][max_z_overall + 1 - z] = num_of_brick;
            }
          }
        }
      }
    }
    part2 += moved;
  }
  std::cout << "PART2: " << part2 << std::endl;
  return 0;
}
