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
enum class Pulse { High, Low };
enum class ModuleType { Conjunction, FlipFlop, Untyped };
struct Module {
  int64_t low_sent;
  int64_t high_sent;
  ModuleType mt;
  std::vector<Module *> inputs;
  std::vector<Module *> outputs;
  std::string name;
  Pulse prev_sent_pulse;
  virtual void reset() = 0;
  virtual void send() = 0;
  virtual void send_low() {
    prev_sent_pulse = Pulse::Low;
    for (Module *m : outputs) {
      switch (m->mt) {
      case ModuleType::FlipFlop:
        m->receive(Pulse::Low);
        break;
      case ModuleType::Conjunction:
        m->receive(name, Pulse::Low);
        break;
      case ModuleType::Untyped:
        break;
      }
    }
  }
  virtual void send_high() {
    prev_sent_pulse = Pulse::High;
    for (Module *m : outputs) {
      switch (m->mt) {
      case ModuleType::FlipFlop:
        m->receive(Pulse::High);
        break;
      case ModuleType::Conjunction:
        m->receive(name, Pulse::High);
        break;
      case ModuleType::Untyped:
        break;
      }
    }
  }
  virtual void receive(Pulse) = 0;
  virtual void receive(std::string, Pulse){};
  Module() = default;
  ~Module() = default;
};
struct FlipFlop : public Module {
  bool did_nothing;
  bool on_bool;
  bool was_on() const { return !on_bool; }
  void receive(Pulse p) {
    switch (p) {
    case Pulse::High:
      did_nothing = true;
      break;
    case Pulse::Low:
      did_nothing = false;
      on_bool = !on_bool;
      break;
    }
  }
  void reset() {
    on_bool = false;
    did_nothing = false;
    low_sent = 0;
    high_sent = 0;
  }
  void send() {
    if (was_on()) {
      send_low();
      low_sent += outputs.size();
    } else {
      send_high();
      high_sent += outputs.size();
    }
  }
  FlipFlop() = default;
  FlipFlop(std::string n) {
    mt = ModuleType::FlipFlop;
    name = n;
    on_bool = false;
    did_nothing = false;
    inputs = std::vector<Module *>();
    outputs = std::vector<Module *>();
    low_sent = 0;
    high_sent = 0;
  }
  FlipFlop(const FlipFlop &ff) : Module() {
    mt = ff.mt;
    name = ff.name;
    on_bool = ff.on_bool;
    did_nothing = ff.did_nothing;
    inputs = ff.inputs;
    outputs = ff.outputs;
  }
  ~FlipFlop() = default;
};
struct Conjunction : public Module {
  std::vector<std::pair<std::string, Pulse>> memory;
  void receive(Pulse) {}
  void receive(std::string input, Pulse p) override {
    auto fd =
        std::find_if(memory.begin(), memory.end(),
                     [&input](const auto &p) { return p.first == input; });
    fd->second = p;
  }
  void reset() {
    for (auto &p : memory) {
      p.second = Pulse::Low;
    }
    low_sent = 0;
    high_sent = 0;
  }
  void send() {
    bool all_is_high = std::accumulate(
        memory.begin(), memory.end(), true,
        [](bool acc, const auto &p) { return acc && p.second == Pulse::High; });
    if (all_is_high) {
      send_low();
      low_sent += outputs.size();
    } else {
      send_high();
      high_sent += outputs.size();
    }
  }
  Conjunction() = default;
  Conjunction(std::string n) {
    mt = ModuleType::Conjunction;
    name = n;
    memory = std::vector<std::pair<std::string, Pulse>>();
    inputs = std::vector<Module *>();
    outputs = std::vector<Module *>();
    low_sent = 0;
    high_sent = 0;
  }
  Conjunction(const Conjunction &c) : Module() {
    name = c.name;
    memory = c.memory;
    inputs = c.inputs;
    outputs = c.outputs;
  }
  ~Conjunction() = default;
};
struct Broadcaster : public Module {
  void reset() {
    low_sent = 0;
  }
  void send() {
    send_low();
    low_sent += outputs.size();
  }
  void receive(Pulse) {}
  Broadcaster() {
    name = "broadcaster";
    low_sent = 0;
    high_sent = 0;
  }
  ~Broadcaster() = default;
};
struct UntypedModule : public Module {
  void reset() {
  }
  void send() {}
  void receive(Pulse p) {
    if (p == Pulse::Low)
      throw;
  }
  UntypedModule() = default;
  UntypedModule(std::string n) {
    name = n;
    mt = ModuleType::Untyped;
  }
  ~UntypedModule() = default;
};
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
  std::vector<std::string> modules_strings;
  while (input.getline(line, LINE_CAPACITY)) {
    int64_t line_length = input.gcount() - 1;
    modules_strings.push_back(std::string(line, line_length));
  }
  input.close();

  std::vector<Module *> modules;
  std::vector<FlipFlop *> ffs;
  std::vector<Conjunction *> cs;
  for (const std::string &ms : modules_strings) {
    size_t pos_of_arrow = ms.find(" -> ");
    if (ms[0] == '%') {
      FlipFlop *ff = new FlipFlop(ms.substr(1, pos_of_arrow - 1));
      modules.push_back(ff);
      ffs.push_back(ff);
    } else if (ms[0] == '&') {
      Conjunction *c = new Conjunction(ms.substr(1, pos_of_arrow - 1));
      modules.push_back(c);
      cs.push_back(c);
    } else {
      modules.push_back(new Broadcaster());
    }
  }
  for (const std::string &ms : modules_strings) {
    size_t pos_of_arrow = ms.find(" -> ");
    std::string_view sv(ms.begin() + 1, ms.begin() + pos_of_arrow);
    auto module_find =
        std::find_if(modules.begin(), modules.end(),
                     [&sv](const Module *m) { return m->name == sv; });
    if (module_find == modules.end()) {
      // broadcaster
      module_find =
          std::find_if(modules.begin(), modules.end(), [](const Module *m) {
            return m->name == "broadcaster";
          });
    }
    std::vector<std::string_view> outputs;
    std::optional<size_t> start = pos_of_arrow + 4;
    for (size_t idx = pos_of_arrow + 4; idx < ms.length(); ++idx) {
      if (ms[idx] >= 'a' && ms[idx] <= 'z' && !start.has_value()) {
        start = idx;
      }
      if (ms[idx] == ',' || idx == ms.length() - 1) {
        if (idx == ms.length() - 1) {
          idx = ms.length();
        }
        outputs.push_back(
            std::string_view(ms.begin() + *start, ms.begin() + idx));
        start.reset();
      }
    }
    for (const std::string_view &msv : outputs) {
      auto output_find =
          std::find_if(modules.begin(), modules.end(),
                       [&msv](const Module *m) { return m->name == msv; });

      if (output_find == modules.end()) {
        (*module_find)->outputs.push_back(new UntypedModule(std::string(msv)));
      } else {
        (*module_find)->outputs.push_back(*output_find);
        (*output_find)->inputs.push_back(*module_find);
      }
    }
  }
  for (Conjunction *c : cs) {
    for (const Module *m : c->inputs) {
      c->memory.push_back({m->name, Pulse::Low});
    }
  }
  auto broadcaster =
      std::find_if(modules.begin(), modules.end(),
                   [](const Module *m) { return m->name == "broadcaster"; });
  std::queue<Module *> sends;
  const size_t PUSH_LIMIT = 1'000;
  for (size_t push = 0; push < PUSH_LIMIT; ++push) {
    sends.push(*broadcaster);
    while (!sends.empty()) {
      Module *o = sends.front();
      sends.pop();
      o->send();
      for (Module *m : o->outputs) {
        if (m->mt == ModuleType::FlipFlop) {
          auto find_ff =
              std::find_if(ffs.begin(), ffs.end(), [m](const FlipFlop *ff) {
                return ff->name == m->name;
              });
          if (!(*find_ff)->did_nothing) {
            sends.push(m);
          }
        } else {
          sends.push(m);
        }
      }
    }
  }
  int64_t total_low = std::accumulate(
      modules.begin(), modules.end(), int64_t{0},
      [](int64_t acc, const Module *m) { return acc + m->low_sent; });
  int64_t total_high = std::accumulate(
      modules.begin(), modules.end(), int64_t{0},
      [](int64_t acc, const Module *m) { return acc + m->high_sent; });
  std::cout << "PART1: " << (total_low + PUSH_LIMIT) * (total_high)
            << std::endl;

  for (Module *m : modules) {
    m->reset();
  }

  size_t push_amt = 0;
  bool final_machine_on = false;
  while (!final_machine_on) {
    ++push_amt;
    sends.push(*broadcaster);
    while (!sends.empty()) {
      Module *o = sends.front();
      sends.pop();
      o->send();
      for (Module *m : o->outputs) {
        if (o->prev_sent_pulse == Pulse::Low && m->mt == ModuleType::Untyped) {
          std::cerr << m->name << std::endl;
          final_machine_on = true;
          break;
        }
        if (m->mt == ModuleType::FlipFlop) {
          auto find_ff =
              std::find_if(ffs.begin(), ffs.end(), [m](const FlipFlop *ff) {
                return ff->name == m->name;
              });
          if (!(*find_ff)->did_nothing) {
            sends.push(m);
          }
        } else {
          sends.push(m);
        }
      }
    }
  }
  std::cout << "PART2: " << push_amt << std::endl;
  return 0;
}
