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
enum class Compare {
  Greater,
  Lesser,
};
struct Rule {
  bool just_workflow;
  char category;
  Compare c;
  std::string workflow;
  int64_t value;
  Rule(char cat, Compare compare, std::string wf, int64_t v, bool jwf = false) {
    category = cat;
    c = compare;
    workflow = wf;
    value = v;
    just_workflow = jwf;
  }
  Rule() = default;
  Rule(bool jwf, std::string wf) {
    just_workflow = jwf;
    workflow = wf;
  }
  Range apply(Range r, bool flip = false) const {
    if (just_workflow) {
      throw;
    }
    switch (c) {
    case Compare::Lesser: {
      if (!flip) {
        if (value < r.start + r.length && value >= r.start) {
          r.length = value - r.start;
          return r;
        }
      } else {
        if (value < r.start + r.length && value >= r.start) {
          r.length = r.start + r.length - value;
          r.start = value;
          return r;
        }
      }
    } break;
    case Compare::Greater: {
      if (!flip) {
        if (value < r.start + r.length && value >= r.start) {
          r.length = r.start + r.length - (value + 1);
          r.start = value + 1;
          return r;
        }
      } else {
        if (value < r.start + r.length && value >= r.start) {
          r.length = value - r.start + 1;
          return r;
        }
      }
    } break;
    }
    return r;
  }
};
struct Part {
  int64_t x, m, a, s;
  Part(int64_t one, int64_t two, int64_t three, int64_t four) {
    x = one;
    m = two;
    a = three;
    s = four;
  }
};
struct WorkFlow {
  std::vector<Rule> rules;
  WorkFlow() = default;
  WorkFlow(std::vector<Rule> rs) { rules = rs; }
  std::string apply_rules(Part p) {
    for (const Rule &rule : rules) {
      if (rule.just_workflow) {
        return rule.workflow;
      }
      switch (rule.c) {
      case Compare::Greater: {
        switch (rule.category) {
        case 'x':
          if (p.x > rule.value)
            return rule.workflow;
          break;
        case 'm':
          if (p.m > rule.value)
            return rule.workflow;
          break;
        case 'a':
          if (p.a > rule.value)
            return rule.workflow;
          break;
        case 's':
          if (p.s > rule.value)
            return rule.workflow;
          break;
        }
      } break;
      case Compare::Lesser: {
        switch (rule.category) {
        case 'x':
          if (p.x < rule.value)
            return rule.workflow;
          break;
        case 'm':
          if (p.m < rule.value)
            return rule.workflow;
          break;
        case 'a':
          if (p.a < rule.value)
            return rule.workflow;
          break;
        case 's':
          if (p.s < rule.value)
            return rule.workflow;
          break;
        }
      } break;
      }
    }
    return "R";
  }
};
struct xmas {
  std::vector<Range> x;
  std::vector<Range> m;
  std::vector<Range> a;
  std::vector<Range> s;
};
void dfs(const std::map<std::string, WorkFlow> workflows,
         const std::string &workflow, Range x, Range m, Range a, Range s,
         std::vector<xmas> &grouped_together) {
  if (workflow == "A") {
    grouped_together.push_back({{x}, {m}, {a}, {s}});
    return;
  }
  if (workflow == "R") {
    return;
  }
  const WorkFlow &wf = workflows.at(workflow);
  for (const Rule &r : wf.rules) {
    if (r.just_workflow) {
      dfs(workflows, r.workflow, x, m, a, s, grouped_together);
    } else {
      Range old_x = x;
      Range old_m = m;
      Range old_a = a;
      Range old_s = s;
      // applying the rule
      switch (r.category) {
      case 'x': {
        x = r.apply(x);
      } break;
      case 'm': {
        m = r.apply(m);
      } break;
      case 'a': {
        a = r.apply(a);
      } break;
      case 's': {
        s = r.apply(s);
      } break;
      }
      dfs(workflows, r.workflow, x, m, a, s, grouped_together);
      // flipping the rule because now the inverse applies
      x = old_x;
      m = old_m;
      a = old_a;
      s = old_s;
      switch (r.category) {
      case 'x': {
        x = r.apply(x, true);
      } break;
      case 'm': {
        m = r.apply(m, true);
      } break;
      case 'a': {
        a = r.apply(a, true);
      } break;
      case 's': {
        s = r.apply(s, true);
      } break;
      }
    }
  }
}
std::ostream &operator<<(std::ostream &os, const Rule &r) {
  if (r.just_workflow) {
    os << r.workflow;
  } else {
    os << r.category << (r.c == Compare::Greater ? '>' : '<') << r.value;
  }
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
  bool begin_parsing_parts = false;
  std::vector<Part> parts;
  std::vector<Part> accepted;
  std::map<std::string, WorkFlow> workflows;
  while (input.getline(line, LINE_CAPACITY)) {
    int64_t line_length = input.gcount() - 1;
    if (line_length == 0) {
      begin_parsing_parts = true;
      continue;
    }
    if (begin_parsing_parts) {
      std::string part_line(line, line_length);
      std::vector<int64_t> categories;
      for (size_t idx = 1; idx < part_line.length();) {
        if (part_line[idx] == 'x' || part_line[idx] == 'm' ||
            part_line[idx] == 'a' || part_line[idx] == 's') {
          size_t idx_of_comma = part_line.find(',', idx);
          if (idx_of_comma != std::string::npos) {
            std::string_view sv(part_line.begin() + idx + 2,
                                part_line.begin() + idx_of_comma);
            categories.push_back(parse_number(sv));
            idx = idx_of_comma + 1;
          } else {
            std::string_view sv(part_line.begin() + idx + 2,
                                part_line.begin() + part_line.length() - 1);
            categories.push_back(parse_number(sv));
            break;
          }
        }
      }
      assert(categories.size() == 4);
      parts.push_back(
          Part(categories[0], categories[1], categories[2], categories[3]));
    } else {
      std::string workflow(line, line_length);
      size_t idx_of_first_curly = workflow.find('{');
      std::string workflow_name = workflow.substr(0, idx_of_first_curly);
      std::vector<Rule> rules;
      for (size_t idx = idx_of_first_curly + 1; idx < workflow.length();) {
        size_t idx_of_comma = workflow.find(',', idx);
        if (idx_of_comma != std::string::npos) {
          size_t idx_of_colon = workflow.find(':', idx);
          std::string_view sv(workflow.begin() + idx + 2,
                              workflow.begin() + idx_of_colon);
          int64_t num = parse_number(sv);
          std::string wf_next = workflow.substr(
              idx_of_colon + 1, idx_of_comma - (idx_of_colon + 1));
          rules.push_back(Rule(workflow[idx],
                               workflow[idx + 1] == '>' ? Compare::Greater
                                                        : Compare::Lesser,
                               wf_next, num));
          idx = idx_of_comma + 1;
        } else {
          rules.push_back(
              Rule(true, workflow.substr(idx, workflow.length() - 1 - idx)));
          break;
        }
      }
      workflows[workflow_name] = WorkFlow(rules);
    }
  }
  input.close();
  for (const Part &p : parts) {
    std::string workflow = "in";
    while (workflow != "A" and workflow != "R") {
      workflow = workflows[workflow].apply_rules(p);
    }
    if (workflow == "A") {
      accepted.push_back(p);
    }
  }
  int64_t part1 = 0;
  for (const Part &p : accepted) {
    part1 += p.x + p.m + p.a + p.s;
  }
  std::cout << std::format("PART1: {}\n", part1);

  std::vector<xmas> grouped_together;
  dfs(workflows, "in", Range(1, 4000), Range(1, 4000), Range(1, 4000),
      Range(1, 4000), grouped_together);

  auto check_for_overlaps = [](xmas &group, xmas &group2) -> bool {
    for (size_t idx_x = 0; idx_x < group.x.size(); ++idx_x) {
      for (size_t idx_m = 0; idx_m < group.m.size(); ++idx_m) {
        for (size_t idx_a = 0; idx_a < group.a.size(); ++idx_a) {
          for (size_t idx_s = 0; idx_s < group.s.size(); ++idx_s) {
            Range &xrngs = group.x[idx_x];
            Range &mrngs = group.m[idx_m];
            Range &arngs = group.a[idx_a];
            Range &srngs = group.s[idx_s];
            for (size_t idx_x2 = 0; idx_x2 < group2.x.size(); ++idx_x2) {
              for (size_t idx_m2 = 0; idx_m2 < group2.m.size(); ++idx_m2) {
                for (size_t idx_a2 = 0; idx_a2 < group2.a.size(); ++idx_a2) {
                  for (size_t idx_s2 = 0; idx_s2 < group2.s.size(); ++idx_s2) {

                    const Range rx = group2.x[idx_x2];
                    const Range rm = group2.m[idx_m2];
                    const Range ra = group2.a[idx_a2];
                    const Range rs = group2.s[idx_s2];

                    if (xrngs.start < rx.start + rx.length &&
                        rx.start < xrngs.start + xrngs.length &&

                        mrngs.start < rm.start + rm.length &&
                        rm.start < mrngs.start + mrngs.length &&

                        arngs.start < ra.start + ra.length &&
                        ra.start < arngs.start + arngs.length &&

                        srngs.start < rs.start + rs.length &&
                        rs.start < srngs.start + srngs.length) {
                      return true;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    return false;
  };
  for (size_t idx_xmas = 0; idx_xmas < grouped_together.size(); ++idx_xmas) {
    xmas &group = grouped_together[idx_xmas];
    for (size_t idx_xmas2 = idx_xmas + 1; idx_xmas2 < grouped_together.size();
         ++idx_xmas2) {
      xmas &group2 = grouped_together[idx_xmas2];
      // std::cerr << idx_xmas << " " << idx_xmas2 << std::endl;
      if (check_for_overlaps(group, group2)) {
        throw;
      }
    }
  }

  int64_t part2 = std::accumulate(
      grouped_together.begin(), grouped_together.end(), int64_t{0},
      [](int64_t acc, const xmas &group) {
        int64_t xs = std::accumulate(
            group.x.begin(), group.x.end(), int64_t{0},
            [](int64_t acc, const Range &r) { return acc + r.length; });
        int64_t ms = std::accumulate(
            group.m.begin(), group.m.end(), int64_t{0},
            [](int64_t acc, const Range &r) { return acc + r.length; });
        int64_t as = std::accumulate(
            group.a.begin(), group.a.end(), int64_t{0},
            [](int64_t acc, const Range &r) { return acc + r.length; });
        int64_t ss = std::accumulate(
            group.s.begin(), group.s.end(), int64_t{0},
            [](int64_t acc, const Range &r) { return acc + r.length; });
        return acc + (xs * ms * as * ss);
      });
  std::cout << std::format("PART2: {}\n", part2);
  return 0;
}
