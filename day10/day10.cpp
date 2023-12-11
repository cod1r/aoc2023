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
std::pair<int32_t, int32_t> find_S(const std::vector<std::string> &pipes) {
  for (size_t idx_row = 0; idx_row < pipes.size(); ++idx_row) {
    for (size_t idx_col = 0; idx_col < pipes[idx_row].length(); ++idx_col) {
      if (pipes[idx_row][idx_col] == 'S') {
        return {idx_row, idx_col};
      }
    }
  }
  return {-1,-1};
}
std::vector<char> find_S_options(
    const std::vector<std::string> &pipes,
    const std::pair<int32_t, int32_t> &S_loc
) {
  std::vector<char> S_options;
  auto [s_row, s_col] = S_loc;
  // connecting top to bottom
  if (s_row - 1 >= 0 &&
    s_row + 1 < (int32_t)pipes.size() &&
    (pipes[s_row - 1][s_col] == '|' || pipes[s_row - 1][s_col] == 'F' || pipes[s_row - 1][s_col] == '7') &&
    (pipes[s_row + 1][s_col] == '|' || pipes[s_row + 1][s_col] == 'L' || pipes[s_row + 1][s_col] == 'J')) {
    S_options.push_back('|');
  }
  // connecting right to left
  if (s_col - 1 >= 0 &&
    s_col + 1 < (int32_t)pipes[0].length() &&
    (pipes[s_row][s_col - 1] == '-' || pipes[s_row][s_col - 1] == 'F' || pipes[s_row][s_col - 1] == 'L') &&
    (pipes[s_row][s_col + 1] == '-' || pipes[s_row][s_col + 1] == 'J' || pipes[s_row][s_col + 1] == '7')) {
    S_options.push_back('-');
  }
  // connecting south to east
  if (s_row + 1 < (int32_t)pipes.size() && s_col + 1 < (int32_t)pipes[0].length() &&
    (pipes[s_row + 1][s_col] == '|' || pipes[s_row + 1][s_col] == 'L' || pipes[s_row + 1][s_col] == 'J') &&
    (pipes[s_row][s_col + 1] == '-' || pipes[s_row][s_col + 1] == 'J' || pipes[s_row][s_col + 1] == '7')) {
    S_options.push_back('F');
  }
  // connecting west to south
  if (s_row + 1 < (int32_t)pipes.size() && s_col - 1 >= 0 &&
    (pipes[s_row + 1][s_col] == '|' || pipes[s_row + 1][s_col] == 'L' || pipes[s_row + 1][s_col] == 'J') &&
    (pipes[s_row][s_col - 1] == '-' || pipes[s_row][s_col - 1] == 'F' || pipes[s_row][s_col - 1] == 'L')) {
    S_options.push_back('7');
  }
  // connecting west to north
  if (s_row - 1 >= 0 && s_col - 1 >= 0 &&
    (pipes[s_row - 1][s_col] == '|' || pipes[s_row - 1][s_col] == 'F' || pipes[s_row - 1][s_col] == '7') &&
    (pipes[s_row][s_col - 1] == '-' || pipes[s_row][s_col - 1] == 'F' || pipes[s_row][s_col - 1] == 'L')) {
    S_options.push_back('J');
  }
  // connecting north to east
  if (s_row - 1 >= 0 && s_col + 1 < (int32_t)pipes[0].length() &&
    (pipes[s_row - 1][s_col] == '|' || pipes[s_row - 1][s_col] == 'F' || pipes[s_row - 1][s_col] == '7') &&
    (pipes[s_row][s_col + 1] == '-' || pipes[s_row][s_col + 1] == 'J' || pipes[s_row][s_col + 1] == '7')) {
    S_options.push_back('L');
  }
  return S_options;
}
struct Step {
  int32_t step_number;
  int32_t row;
  int32_t col;
};
bool operator==(const Step& a, const Step& b) {
  return a.step_number == b.step_number && a.row == b.row && a.col == b.col;
}
std::ostream& operator<<(std::ostream &os, const std::vector<Step> &v) {
  for (const auto &s : v) {
    os << s.step_number << " " << s.row << " " << s.col << std::endl;
  }
  os << std::endl;
  return os;
}
std::vector<Step> travel_path(const std::vector<std::string> &pipes, std::pair<int32_t, int32_t> prev, std::pair<int32_t, int32_t> curr) {
  auto [s_row, s_col] = prev;
  char curr_pipe = pipes[curr.first][curr.second];
  int32_t step_counter = 1;
  std::vector<Step> steps;
  steps.push_back({.step_number = step_counter, .row = curr.first, .col = curr.second});
  while (curr != std::pair<int32_t, int32_t>{s_row, s_col}) {
    auto copy = curr;
    switch (curr_pipe) {
      case '|': if (prev.first < curr.first) { curr.first += 1; } else { curr.first -= 1; } break;
      case '-': if (prev.second < curr.second) { curr.second += 1; } else { curr.second -= 1; } break;
      case 'L': if (prev.first < curr.first) { curr.second += 1; } else { curr.first -= 1; } break;
      case 'J': if (prev.first < curr.first) { curr.second -= 1; } else { curr.first -= 1; } break;
      case '7': if (prev.first > curr.first) { curr.second -= 1; } else { curr.first += 1; } break;
      case 'F': if (prev.first > curr.first) { curr.second += 1; } else { curr.first += 1; } break;
    }
    prev = copy;
    if (curr.first < 0 || curr.first >= (int32_t)pipes.size() || curr.second < 0 || curr.second >= (int32_t)pipes[0].length()) {
      steps.clear();
      break;
    }
    curr_pipe = pipes[curr.first][curr.second];
    step_counter += 1;
    steps.push_back({.step_number = step_counter, .row = curr.first, .col = curr.second});
  }
  return steps;
}
enum class Turns {
  Left,
  Right,
  Up,
  Down,
};
Turns get_current_turn(const std::pair<int32_t, int32_t> prev, const std::pair<int32_t, int32_t> curr) {
  if (prev.first == curr.first && prev.second < curr.second) {
    return Turns::Right;
  } else if (prev.first == curr.first && prev.second > curr.second) {
    return Turns::Left;
  } else if (prev.first < curr.first && prev.second == curr.second) {
    return Turns::Down;
  }
  return Turns::Up;
}
enum class InsideSide {
  Left,
  Right,
};
InsideSide get_inside_side(const std::vector<Step>& first_way, int32_t s_row, int32_t s_col) {
  std::pair<int32_t, int32_t> prev{s_row, s_col};
  auto first_step = first_way.front();
  std::pair<int32_t, int32_t> curr = {first_step.row, first_step.col};
  Turns turn;
  for (const Step &step : first_way) {
    curr = {step.row, step.col};
    turn = get_current_turn(prev, curr);
    switch (turn) {
      case Turns::Up: {
        auto find_res = std::find_if(first_way.begin(), first_way.end(), [step](const Step& a) {
          return (a.row == step.row && a.col < step.col);
        });
        auto find_res2 = std::find_if(first_way.begin(), first_way.end(), [step](const Step& a) {
          return (a.row == step.row && a.col > step.col);
        });
        if (find_res == first_way.end()) {
          return InsideSide::Right;
        } else if (find_res2 == first_way.end()) {
          return InsideSide::Left;
        }
      } break;
      case Turns::Down: {
        auto find_res = std::find_if(first_way.begin(), first_way.end(), [step](const Step& a) {
          return (a.row == step.row && a.col < step.col);
        });
        auto find_res2 = std::find_if(first_way.begin(), first_way.end(), [step](const Step& a) {
          return (a.row == step.row && a.col > step.col);
        });
        if (find_res == first_way.end()) {
          return InsideSide::Left;
        } else if (find_res2 == first_way.end()) {
          return InsideSide::Right;
        }
      } break;
      case Turns::Left: {
        auto find_res = std::find_if(first_way.begin(), first_way.end(), [step](const Step& a) {
          return (a.row < step.row && a.col == step.col);
        });
        auto find_res2 = std::find_if(first_way.begin(), first_way.end(), [step](const Step& a) {
          return (a.row > step.row && a.col == step.col);
        });
        if (find_res == first_way.end()) {
          return InsideSide::Left;
        } else if (find_res2 == first_way.end()) {
          return InsideSide::Right;
        }
      } break;
      case Turns::Right: {
        auto find_res = std::find_if(first_way.begin(), first_way.end(), [step](const Step& a) {
          return (a.row < step.row && a.col == step.col);
        });
        auto find_res2 = std::find_if(first_way.begin(), first_way.end(), [step](const Step& a) {
          return (a.row > step.row && a.col == step.col);
        });
        if (find_res == first_way.end()) {
          return InsideSide::Right;
        } else if (find_res2 == first_way.end()) {
          return InsideSide::Left;
        }
      } break;
    }
    prev = curr;
  }
  return InsideSide::Left;
}
std::ostream& operator<<(std::ostream &os, Turns t) {
  switch (t) {
    case Turns::Left: os << "Left"; break;
    case Turns::Right: os << "Right"; break;
    case Turns::Up: os << "Up"; break;
    case Turns::Down: os << "Down"; break;
  }
  return os;
}
Turns get_looking_turn(const InsideSide& inside_side, const Turns& t) {
  Turns ret{};
  switch (inside_side) {
    case InsideSide::Left:
    {
      switch (t) {
        case Turns::Left:
        ret = Turns::Down;
        break;
        case Turns::Right:
        ret = Turns::Up;
        break;
        case Turns::Up:
        ret = Turns::Left;
        break;
        case Turns::Down:
        ret = Turns::Right;
        break;
      }
    }
    break;
    case InsideSide::Right:
    {
      switch (t) {
        case Turns::Left:
        ret = Turns::Up;
        break;
        case Turns::Right:
        ret = Turns::Down;
        break;
        case Turns::Up:
        ret = Turns::Right;
        break;
        case Turns::Down:
        ret = Turns::Left;
        break;
      }
    }
    break;
  }
  return ret;
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
  std::vector<std::string> pipes;
  std::optional<int32_t> single_size_of_width;
  while (input.getline(line, LINE_CAPACITY)) {
    int32_t line_length = input.gcount() - 1;
    if (single_size_of_width.has_value() && *single_size_of_width != line_length) {
      throw "WIDTHS ARE NOT EQUAL";
    } else {
      single_size_of_width = line_length;
    }
    pipes.push_back(std::string(line, line_length));
  }
  auto [s_row, s_col] = find_S(pipes);
  std::vector<char> S_options = find_S_options(pipes, {s_row, s_col});
  std::vector<Step> first_way;
  int32_t part1 = 0;
  for (const char &s_pipe : S_options) {
    char curr_pipe = s_pipe;
    {
      // Go one direction
      std::pair<int32_t, int32_t> curr{s_row, s_col};
      std::pair<int32_t, int32_t> prev{s_row, s_col};
      switch (curr_pipe) {
        case '|': curr = {curr.first + 1, curr.second}; break;
        case '-': curr = {curr.first, curr.second + 1}; break;
        case 'L': curr = {curr.first, curr.second + 1}; break;
        case 'J': curr = {curr.first, curr.second - 1}; break;
        case '7': curr = {curr.first, curr.second - 1}; break;
        case 'F': curr = {curr.first, curr.second + 1}; break;
      }
      first_way = travel_path(pipes, prev, curr);
    }
    // removing the S location step after
    if (first_way.size() > 0) {
      first_way.pop_back();
      part1 = std::max(part1, first_way[(first_way.size() - 1)/2].step_number);
      break;
    }
  }
  std::cout << "PART1: " << part1 << std::endl;

  InsideSide inside_side = get_inside_side(first_way, s_row, s_col);
  //switch (inside_side) {
  //  case InsideSide::Left: std::cerr << "LEFT\n"; break;
  //  case InsideSide::Right: std::cerr << "RIGHT\n"; break;
  //}
  std::vector<std::pair<int32_t, int32_t>> ranges;
  auto first = first_way.front();
  std::pair<int32_t, int32_t> curr{first.row, first.col};
  std::pair<int32_t, int32_t> prev{s_row, s_col};
  Turns t = get_current_turn(prev, curr);
  Turns p = t;
  struct LookingNodes {
    int32_t row;
    int32_t col;
    Turns looking_turn;
  };
  std::vector<LookingNodes> nodes;
  for (const Step& s : first_way) {
    curr = {s.row, s.col};
    p = t;
    t = get_current_turn(prev, curr);
    Turns lt = get_looking_turn(inside_side, t);
    nodes.push_back({.row = s.row, .col = s.col, .looking_turn = lt});
    if (p != t) {
      nodes.push_back({.row = prev.first, .col = prev.second, .looking_turn = lt});
    }
    prev = curr;
  }
  curr = {s_row, s_col};
  t = get_current_turn(prev, curr);
  Turns lt = get_looking_turn(inside_side, t);
  nodes.push_back({.row = curr.first, .col = curr.second, .looking_turn = lt});
  if (p != t) {
    nodes.push_back({.row = prev.first, .col = prev.second, .looking_turn = lt});
  }

  auto filter_view_right = std::views::filter(nodes, [](const auto& n) {
    return n.looking_turn == Turns::Right;
  });
  std::vector<LookingNodes> right(filter_view_right.begin(), filter_view_right.end());
  //std::ranges::for_each(right.begin(), right.end(), [](const LookingNodes& ln) {
  //  std::cout << ln.row << " " << ln.col << " " << ln.looking_turn << "\n";
  //});

  auto filter_view_left = std::views::filter(nodes, [](const auto& n) {
    return n.looking_turn == Turns::Left;
  });
  std::vector<LookingNodes> left(filter_view_left.begin(), filter_view_left.end());
  //std::ranges::for_each(left.begin(), left.end(), [](const LookingNodes& ln) {
  //  std::cout << ln.row << " " << ln.col << " " << ln.looking_turn << "\n";
  //});

  int32_t part2 = 0;
  for (const LookingNodes& lnr : right) {
    std::vector<LookingNodes> matches;
    for (const LookingNodes& lnl : left) {
      if (lnl.row == lnr.row && lnl.col > lnr.col) {
        matches.push_back(lnl);
      }
    }
    if (matches.size() > 0) {
      LookingNodes m = matches.front();
      for (const auto& lnl : matches) {
        m = std::min(lnl, m, [&lnr](const auto& l, const auto& r) {
            return l.col - lnr.col < r.col - lnr.col;
            });
      }
      auto find_res = std::find_if(nodes.begin(), nodes.end(), [lnr, &m](const LookingNodes& ln) {
        return ln.col > lnr.col && ln.col < m.col && ln.row == lnr.row;
      });
      if (find_res == nodes.end()) {
        part2 += m.col - lnr.col - 1;
      }
    }
  }
  std::cout << "PART2: " << part2 << std::endl;
  return 0;
}
