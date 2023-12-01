#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
int32_t main() {
  std::ifstream input("day1_input.txt");
  if (!input.is_open()) {
    std::cerr << "Could not open input\n";
    return 1;
  }
  std::string line(1000, '\0');
  int32_t sum1 = 0;
  int32_t sum2 = 0;
  std::string number_names[10] = {
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
  };
  while (input.getline(&line[0], line.length())) {
    int32_t idx_of_first;
    int32_t idx_of_second;
    for (int32_t idx = 0; idx < input.gcount(); ++idx) {
      if (line[idx] >= '0' && line[idx] <= '9') {
        sum1 += ((line[idx] - '0') * 10);
        idx_of_first = idx;
        break;
      }
    }
    for (int32_t idx = input.gcount() - 1; idx >= 0; --idx) {
      if (line[idx] >= '0' && line[idx] <= '9') {
        sum1 += (line[idx] - '0');
        idx_of_second = idx;
        break;
      }
    }
    int32_t part2_first_digit = -1;
    for (int32_t idx = 0; idx < idx_of_first; ++idx) {
      bool break_out = false;
        for (int32_t names_index = 0; names_index < 9; ++names_index) {
          std::string name = number_names[names_index];
          if (name.length() - 1 + idx < (uint32_t)idx_of_first) {
            bool equal = true;
            int32_t idx_temp = idx;
            for (int32_t name_idx = 0; name_idx < (int32_t)name.length(); ++name_idx) {
              if (name[name_idx] != line[idx_temp]) {
                equal = false;
                break;
              }
              ++idx_temp;
            }
            if (equal) {
              part2_first_digit = (names_index + 1) * 10;
              break_out = true;
              break;
            }
          }
        }
        if (break_out)
          break;
    }
    if (part2_first_digit == -1) {
      sum2 += (line[idx_of_first] - '0') * 10;
    } else {
      sum2 += part2_first_digit;
    }

    int32_t part2_second_digit = -1;
    for (int32_t idx = input.gcount() - 1; idx >= idx_of_second + 1; --idx) {
      bool break_out = false;
      for (int32_t names_index = 0; names_index < 9; ++names_index) {
        std::string name = number_names[names_index];
        if (name.length() - 1 + idx < (uint32_t)input.gcount()) { 
          bool equal = true;
          int32_t idx_temp = idx;
          for (int32_t name_idx = 0; name_idx < (int32_t)name.length(); ++name_idx) {
            if (name[name_idx] != line[idx_temp]) {
              equal = false;
              break;
            }
            ++idx_temp;
          }
          if (equal) {
            part2_second_digit = (names_index + 1);
            break_out = true;
            break;
          }
        }
      }
      if (break_out)
        break;
    }
    if (part2_second_digit == -1) {
      sum2 += (line[idx_of_second] - '0');
    } else {
      sum2 += part2_second_digit;
    }
  }
  std::cout << "sum: " << sum1 << "\n";
  std::cout << "sum2: " << sum2 << "\n";
}
