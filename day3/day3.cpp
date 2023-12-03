#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <cmath>
struct Number_Info {
  int32_t row;
  int32_t col;
  int32_t number_of_digits;
  int32_t number;
  Number_Info(int32_t r, int32_t c, int32_t nd, int32_t num): row{r}, col{c}, number_of_digits{nd}, number{num}
  {}
};
int32_t main(int32_t argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Needs to be called like `./day.out input.txt`\n";
    return 1;
  }
  std::ifstream input(argv[1]);
  if (!input.is_open()) {
    std::cerr << "Cannot open file\n";
    return 1;
  }
  char line[256];
  std::vector<Number_Info> numbers;
  std::vector<std::tuple<int32_t, int32_t, char>> symbols;
  int32_t line_number = 0;
  while (input.getline(line, 256)) {
    // minus 1 because null termination character
    int32_t line_length = input.gcount() - 1;
    std::optional<int32_t> digit_start;
    for (int32_t line_idx = 0; line_idx < line_length; ++line_idx) {
      if (line[line_idx] >= '0' && line[line_idx] <= '9' && !digit_start.has_value()) {
        digit_start = line_idx;
      }
      if (line[line_idx] < '0' || line[line_idx] > '9' || line_idx == line_length - 1) {
        if (digit_start.has_value()) {
          int32_t number = 0;
          int32_t digit_end =
            line_idx == line_length - 1 &&
            line[line_idx] >= '0' && line[line_idx] <= '9' ? line_length : line_idx;
          for (int32_t digit_idx = *digit_start; digit_idx < digit_end; ++digit_idx) {
            number += (line[digit_idx] - '0') * std::pow(10, digit_end - digit_idx - 1);
          }
          numbers.push_back(Number_Info(line_number, *digit_start, line_idx - *digit_start, number));
          digit_start.reset();
        }
        if (line[line_idx] != '.' && (line[line_idx] < '0' || line[line_idx] > '9')) {
          symbols.push_back(std::tuple<int32_t, int32_t, char>{line_number, line_idx, line[line_idx]});
        }
      }
    }
    line_number += 1;
  }
  int32_t part1_ans = 0;
  for (Number_Info &ni : numbers) {
    for (std::tuple<int32_t, int32_t, char> &t : symbols) {
      int32_t symbol_row = std::get<0>(t);
      int32_t symbol_col = std::get<1>(t);
      if (
        symbol_col >= ni.col - 1 && symbol_col <= (ni.col + ni.number_of_digits - 1) + 1 &&
        (symbol_row == ni.row || symbol_row - 1 == ni.row || symbol_row + 1 == ni.row)
        ) {
        part1_ans += ni.number;
        break;
      }
    }
  }
  int32_t part2_sum = 0;
  for (std::tuple<int32_t, int32_t, char> &t : symbols) {
    if (std::get<2>(t) != '*')
      continue;
    int32_t symbol_row = std::get<0>(t);
    int32_t symbol_col = std::get<1>(t);
    int32_t adjacent_counter = 0;
    int32_t product = 1;
    for (Number_Info &ni : numbers) {
      if (
        symbol_col >= ni.col - 1 && symbol_col <= (ni.col + ni.number_of_digits - 1) + 1 &&
        (symbol_row == ni.row || symbol_row - 1 == ni.row || symbol_row + 1 == ni.row)
        ) {
        adjacent_counter += 1;
        product *= ni.number;
      }
    }
    if (adjacent_counter == 2) {
      part2_sum += product;
    }
  }
  std::cout << "PART1: " << part1_ans << "\n";
  std::cout << "PART2: " << part2_sum << "\n";
  return 0;
}
