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
std::vector<std::vector<Rule>>
dfs(const std::map<std::string, WorkFlow> &workflows,
    const std::string &workflow, std::vector<Rule> &rules) {
  if (workflow == "A") {
    return std::vector<std::vector<Rule>>{rules};
  }
  if (workflow == "R") {
    return std::vector<std::vector<Rule>>{};
  }
  std::vector<std::vector<Rule>> paths;
  const WorkFlow &wf = workflows.at(workflow);
  for (const Rule &r : wf.rules) {
    rules.push_back(r);
    auto extended = dfs(workflows, r.workflow, rules);
    if (extended.size() > 0) {
      paths.insert(paths.end(), extended.begin(), extended.end());
    }
    rules.pop_back();
  }
  return paths;
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

  std::vector<Rule> rules;
  std::vector<std::vector<Rule>> filters = dfs(workflows, "in", rules);
  int64_t part2 = 0;
  for (const auto &v : filters) {
    std::vector<Range> xx;
    std::vector<Range> mm;
    std::vector<Range> aa;
    std::vector<Range> ss;
    for (const Rule &rules_up_until : workflows["in"].rules) {
      if (rules_up_until.workflow == v[0].workflow) {
        break;
      }
      // all rules up until a certain rule shall change the range of a
      // category by making it the opposite of what the rule's condition is.
      // so greater than will be less than bc the range didn't qualify.
      switch (rules_up_until.category) {
      case 'x':
        switch (rules_up_until.c) {
        case Compare::Greater: {
          auto find = std::find_if(
              xx.begin(), xx.end(), [&rules_up_until](const Range &xr) {
                return rules_up_until.value >= xr.start &&
                       rules_up_until.value < xr.start + xr.length;
              });
          if (find == xx.end()) {
            xx.push_back(Range(1, rules_up_until.value));
          } else {
            find->length = rules_up_until.value - find->start;
          }
        } break;
        case Compare::Lesser: {
          auto find = std::find_if(
              xx.begin(), xx.end(), [&rules_up_until](const Range &xr) {
                return rules_up_until.value >= xr.start &&
                       rules_up_until.value < xr.start + xr.length;
              });
          if (find == xx.end()) {
            xx.push_back(
                Range(rules_up_until.value, 4000 - (rules_up_until.value) + 1));
          } else {
            find->length =
                find->start + find->length - (rules_up_until.value + 1);
            find->start = rules_up_until.value + 1;
          }
        } break;
        }
        break;
      case 'm':
        switch (rules_up_until.c) {
        case Compare::Greater: {
          auto find = std::find_if(
              mm.begin(), mm.end(), [&rules_up_until](const Range &xr) {
                return rules_up_until.value >= xr.start &&
                       rules_up_until.value < xr.start + xr.length;
              });
          if (find == mm.end()) {
            mm.push_back(Range(1, rules_up_until.value));
          } else {
            find->length = rules_up_until.value - find->start;
          }
        } break;
        case Compare::Lesser: {
          auto find = std::find_if(
              mm.begin(), mm.end(), [&rules_up_until](const Range &xr) {
                return rules_up_until.value >= xr.start &&
                       rules_up_until.value < xr.start + xr.length;
              });
          if (find == mm.end()) {
            mm.push_back(
                Range(rules_up_until.value, 4000 - (rules_up_until.value) + 1));
          } else {
            find->length =
                find->start + find->length - (rules_up_until.value + 1);
            find->start = rules_up_until.value + 1;
          }
        } break;
        }
        break;
      case 'a':
        switch (rules_up_until.c) {
        case Compare::Greater: {
          auto find = std::find_if(
              aa.begin(), aa.end(), [&rules_up_until](const Range &xr) {
                return rules_up_until.value >= xr.start &&
                       rules_up_until.value < xr.start + xr.length;
              });
          if (find == aa.end()) {
            aa.push_back(Range(1, rules_up_until.value));
          } else {
            find->length = rules_up_until.value - find->start;
          }
        } break;
        case Compare::Lesser: {
          auto find = std::find_if(
              aa.begin(), aa.end(), [&rules_up_until](const Range &xr) {
                return rules_up_until.value >= xr.start &&
                       rules_up_until.value < xr.start + xr.length;
              });
          if (find == aa.end()) {
            aa.push_back(
                Range(rules_up_until.value, 4000 - (rules_up_until.value) + 1));
          } else {
            find->length =
                find->start + find->length - (rules_up_until.value + 1);
            find->start = rules_up_until.value + 1;
          }
        } break;
        }
        break;
      case 's':
        switch (rules_up_until.c) {
        case Compare::Greater: {
          auto find = std::find_if(
              ss.begin(), ss.end(), [&rules_up_until](const Range &xr) {
                return rules_up_until.value >= xr.start &&
                       rules_up_until.value < xr.start + xr.length;
              });
          if (find == ss.end()) {
            ss.push_back(Range(1, rules_up_until.value));
          } else {
            find->length = rules_up_until.value - find->start;
          }
        } break;
        case Compare::Lesser: {
          auto find = std::find_if(
              ss.begin(), ss.end(), [&rules_up_until](const Range &xr) {
                return rules_up_until.value >= xr.start &&
                       rules_up_until.value < xr.start + xr.length;
              });
          if (find == ss.end()) {
            ss.push_back(
                Range(rules_up_until.value, 4000 - (rules_up_until.value) + 1));
          } else {
            find->length =
                find->start + find->length - (rules_up_until.value + 1);
            find->start = rules_up_until.value + 1;
          }
        } break;
        }
        break;
      }
    }
    for (size_t idx = 0; idx < v.size(); ++idx) {
      const Rule &current_rule = v[idx];
      std::cout << current_rule << std::endl;
      if (!current_rule.just_workflow) {
        switch (current_rule.category) {
        case 'x':
          switch (current_rule.c) {
          case Compare::Greater: {
            auto find = std::find_if(
                xx.begin(), xx.end(), [&current_rule](const Range &xr) {
                  return current_rule.value >= xr.start &&
                         current_rule.value < xr.start + xr.length;
                });
            if (find == xx.end()) {
              xx.push_back(Range(current_rule.value + 1,
                                 4000 - (current_rule.value + 1) + 1));
            } else {
              find->length =
                  find->start + find->length - (current_rule.value + 1);
              find->start = current_rule.value + 1;
            }
          } break;
          case Compare::Lesser: {
            auto find = std::find_if(
                xx.begin(), xx.end(), [&current_rule](const Range &xr) {
                  return current_rule.value >= xr.start &&
                         current_rule.value < xr.start + xr.length;
                });
            if (find == xx.end()) {
              xx.push_back(Range(1, current_rule.value - 1));
            } else {
              find->length = current_rule.value - find->start;
            }
          } break;
          }
          break;
        case 'm':
          switch (current_rule.c) {
          case Compare::Greater: {
            auto find = std::find_if(
                mm.begin(), mm.end(), [&current_rule](const Range &xr) {
                  return current_rule.value >= xr.start &&
                         current_rule.value < xr.start + xr.length;
                });
            if (find == mm.end()) {
              mm.push_back(Range(current_rule.value + 1,
                                 4000 - (current_rule.value + 1) + 1));
            } else {
              find->length =
                  find->start + find->length - (current_rule.value + 1);
              find->start = current_rule.value + 1;
            }
          } break;
          case Compare::Lesser: {
            auto find = std::find_if(
                mm.begin(), mm.end(), [&current_rule](const Range &xr) {
                  return current_rule.value >= xr.start &&
                         current_rule.value < xr.start + xr.length;
                });
            if (find == mm.end()) {
              mm.push_back(Range(1, current_rule.value - 1));
            } else {
              find->length = current_rule.value - find->start;
            }
          } break;
          }
          break;
        case 'a':
          switch (current_rule.c) {
          case Compare::Greater: {
            auto find = std::find_if(
                aa.begin(), aa.end(), [&current_rule](const Range &xr) {
                  return current_rule.value >= xr.start &&
                         current_rule.value < xr.start + xr.length;
                });
            if (find == aa.end()) {
              aa.push_back(Range(current_rule.value + 1,
                                 4000 - (current_rule.value + 1) + 1));
            } else {
              find->length =
                  find->start + find->length - (current_rule.value + 1);
              find->start = current_rule.value + 1;
            }
          } break;
          case Compare::Lesser: {
            auto find = std::find_if(
                aa.begin(), aa.end(), [&current_rule](const Range &xr) {
                  return current_rule.value >= xr.start &&
                         current_rule.value < xr.start + xr.length;
                });
            if (find == aa.end()) {
              aa.push_back(Range(1, current_rule.value - 1));
            } else {
              find->length = current_rule.value - find->start;
            }
          } break;
          }
          break;
        case 's':
          switch (current_rule.c) {
          case Compare::Greater: {
            auto find = std::find_if(
                ss.begin(), ss.end(), [&current_rule](const Range &xr) {
                  return current_rule.value >= xr.start &&
                         current_rule.value < xr.start + xr.length;
                });
            if (find == ss.end()) {
              ss.push_back(Range(current_rule.value + 1,
                                 4000 - (current_rule.value + 1) + 1));
            } else {
              find->length =
                  find->start + find->length - (current_rule.value + 1);
              find->start = current_rule.value + 1;
            }
          } break;
          case Compare::Lesser: {
            auto find = std::find_if(
                ss.begin(), ss.end(), [&current_rule](const Range &xr) {
                  return current_rule.value >= xr.start &&
                         current_rule.value < xr.start + xr.length;
                });
            if (find == ss.end()) {
              ss.push_back(Range(1, current_rule.value - 1));
            } else {
              find->length = current_rule.value - find->start;
            }
          } break;
          }
          break;
        }
      }
      if (idx + 1 < v.size()) {
        for (const Rule &rules_up_until :
             workflows[current_rule.workflow].rules) {
          if (rules_up_until.workflow == v[idx + 1].workflow) {
            break;
          }
          // all rules up until a certain rule shall change the range of a
          // category by making it the opposite of what the rule's condition is.
          // so greater than will be less than bc the range didn't qualify.
          switch (rules_up_until.category) {
          case 'x':
            switch (rules_up_until.c) {
            case Compare::Greater: {
              auto find = std::find_if(
                  xx.begin(), xx.end(), [&rules_up_until](const Range &xr) {
                    return rules_up_until.value >= xr.start &&
                           rules_up_until.value < xr.start + xr.length;
                  });
              if (find == xx.end()) {
                xx.push_back(Range(1, rules_up_until.value));
              } else {
                find->length = rules_up_until.value - find->start;
              }
            } break;
            case Compare::Lesser: {
              auto find = std::find_if(
                  xx.begin(), xx.end(), [&rules_up_until](const Range &xr) {
                    return rules_up_until.value >= xr.start &&
                           rules_up_until.value < xr.start + xr.length;
                  });
              if (find == xx.end()) {
                xx.push_back(Range(rules_up_until.value,
                                   4000 - (rules_up_until.value) + 1));
              } else {
                find->length =
                    find->start + find->length - (rules_up_until.value + 1);
                find->start = rules_up_until.value + 1;
              }
            } break;
            }
            break;
          case 'm':
            switch (rules_up_until.c) {
            case Compare::Greater: {
              auto find = std::find_if(
                  mm.begin(), mm.end(), [&rules_up_until](const Range &xr) {
                    return rules_up_until.value >= xr.start &&
                           rules_up_until.value < xr.start + xr.length;
                  });
              if (find == mm.end()) {
                mm.push_back(Range(1, rules_up_until.value));
              } else {
                find->length = rules_up_until.value - find->start;
              }
            } break;
            case Compare::Lesser: {
              auto find = std::find_if(
                  mm.begin(), mm.end(), [&rules_up_until](const Range &xr) {
                    return rules_up_until.value >= xr.start &&
                           rules_up_until.value < xr.start + xr.length;
                  });
              if (find == mm.end()) {
                mm.push_back(Range(rules_up_until.value,
                                   4000 - (rules_up_until.value) + 1));
              } else {
                find->length =
                    find->start + find->length - (rules_up_until.value + 1);
                find->start = rules_up_until.value + 1;
              }
            } break;
            }
            break;
          case 'a':
            switch (rules_up_until.c) {
            case Compare::Greater: {
              auto find = std::find_if(
                  aa.begin(), aa.end(), [&rules_up_until](const Range &xr) {
                    return rules_up_until.value >= xr.start &&
                           rules_up_until.value < xr.start + xr.length;
                  });
              if (find == aa.end()) {
                aa.push_back(Range(1, rules_up_until.value));
              } else {
                find->length = rules_up_until.value - find->start;
              }
            } break;
            case Compare::Lesser: {
              auto find = std::find_if(
                  aa.begin(), aa.end(), [&rules_up_until](const Range &xr) {
                    return rules_up_until.value >= xr.start &&
                           rules_up_until.value < xr.start + xr.length;
                  });
              if (find == aa.end()) {
                aa.push_back(Range(rules_up_until.value,
                                   4000 - (rules_up_until.value) + 1));
              } else {
                find->length =
                    find->start + find->length - (rules_up_until.value + 1);
                find->start = rules_up_until.value + 1;
              }
            } break;
            }
            break;
          case 's':
            switch (rules_up_until.c) {
            case Compare::Greater: {
              auto find = std::find_if(
                  ss.begin(), ss.end(), [&rules_up_until](const Range &xr) {
                    return rules_up_until.value >= xr.start &&
                           rules_up_until.value < xr.start + xr.length;
                  });
              if (find == ss.end()) {
                ss.push_back(Range(1, rules_up_until.value));
              } else {
                find->length = rules_up_until.value - find->start;
              }
            } break;
            case Compare::Lesser: {
              auto find = std::find_if(
                  ss.begin(), ss.end(), [&rules_up_until](const Range &xr) {
                    return rules_up_until.value >= xr.start &&
                           rules_up_until.value < xr.start + xr.length;
                  });
              if (find == ss.end()) {
                ss.push_back(Range(rules_up_until.value,
                                   4000 - (rules_up_until.value) + 1));
              } else {
                find->length =
                    find->start + find->length - (rules_up_until.value + 1);
                find->start = rules_up_until.value + 1;
              }
            } break;
            }
            break;
          }
        }
      }
    }
    std::cout << std::endl;
    std::cout << "X\n";
    int64_t amt_x = xx.empty() ? 4000:0;
    for (const Range &range : xx) {
      std::cout << range << std::endl;
      amt_x += range.length;
    }
    std::cout << "M\n";
    int64_t amt_m = mm.empty() ? 4000:0;
    for (const Range &range : mm) {
      std::cout << range << std::endl;
      amt_m += range.length;
    }
    std::cout << "A\n";
    int64_t amt_a = aa.empty() ? 4000:0;
    for (const Range &range : aa) {
      std::cout << range << std::endl;
      amt_a += range.length;
    }
    std::cout << "S\n";
    int64_t amt_s = ss.empty() ? 4000:0;
    for (const Range &range : ss) {
      std::cout << range << std::endl;
      amt_s += range.length;
    }
    std::cout << std::endl;
    part2 += amt_x * amt_m * amt_a * amt_s;
  }
  std::cout << std::format("PART2: {}\n", part2);
  return 0;
}
