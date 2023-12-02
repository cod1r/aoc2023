#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <cmath>
int32_t max(int32_t a, int32_t b) {
  return a < b ? b:a;
}
// returned results will always be rgb
std::array<int32_t, 3> parse_set(const std::string& set) {
  int32_t red = 0;
  int32_t green = 0;
  int32_t blue = 0;
  int32_t start_idx = 0;
  for (int32_t idx = 0; idx < (int32_t)set.length(); ++idx) {
    if (set[idx] == ' ' && set[idx + 1] >= 97 && set[idx + 1] <= 123) {
      int32_t val = 0;
      for (int32_t digit_idx = start_idx; digit_idx < idx; ++digit_idx) {
        val += (set[digit_idx] - '0') * std::pow(10, (idx - digit_idx - 1));
      }
      switch (set[idx + 1]) {
        case 'r': red = val;break; 
        case 'g': green = val;break;
        case 'b': blue = val;break;
      }
    } else if (set[idx] == ',') {
      start_idx = idx + 2;
    }
  }
  return {red, green, blue};
}
int32_t main() {
  //A game is impossible if at any point in any game, the number of cubes of any color exceeds 12 red, 13 green, 14 blue.
  //A game is possible if all sets have less than or equal to 12 red, 13 green, 14 blue.
  std::ifstream input("day2_input.txt");
  if (!input.is_open()) {
    std::cerr << "Unable to open file" << std::endl;
    return 1;
  }
  int32_t sum_of_game_ids = 0;
  int32_t sum_of_rgb_multiplied = 0;
  char line[500];
  while (input.getline(line, 500)) {
    int32_t line_length = input.gcount();
    int32_t idx_of_colon = 0;
    int32_t game_id = 0;
    for (int32_t idx = 0; idx < line_length; ++idx) {
      if (line[idx] == ':') {
        idx_of_colon = idx;
        break;
      }
    }
    for (int32_t idx = 0; idx < line_length; ++idx) {
      if (line[idx] == ' ') {
        for (int32_t digit_idx = idx + 1; digit_idx < idx_of_colon; ++digit_idx) {
          game_id += (line[digit_idx] - '0') * std::pow(10, (idx_of_colon - digit_idx - 1));
        }
        break;
      }
    }
    std::vector<std::string> sets;
    int32_t idx_of_start_of_sets = idx_of_colon;
    int32_t prev_idx_of_semi = idx_of_start_of_sets;
    for (int32_t idx = idx_of_start_of_sets; idx < line_length; ++idx) {
      if (line[idx] == ';') {
        sets.push_back(
            // adding 2 because of semi and space
            std::string(line + prev_idx_of_semi + 2, idx - (prev_idx_of_semi + 2))
        );
        prev_idx_of_semi = idx;
      }
    }
    // adding 2 because of semi and space
    sets.push_back(std::string(line + prev_idx_of_semi + 2, line_length - (prev_idx_of_semi + 2)));
    sum_of_game_ids += game_id;
    for (std::string& set : sets) {
      std::array<int32_t, 3> rgb = parse_set(set);
      if (rgb[0] > 12 || rgb[1] > 13 || rgb[2] > 14) {
        sum_of_game_ids -= game_id;
        break;
      }
    }
    int32_t max_red = 0;
    int32_t max_green = 0;
    int32_t max_blue = 0;
    for (std::string& set : sets) {
      std::array<int32_t, 3> rgb = parse_set(set);
      max_red = max(rgb[0], max_red);
      max_green = max(rgb[1], max_green);
      max_blue = max(rgb[2], max_blue);
    }
    sum_of_rgb_multiplied += max_red * max_green * max_blue;
  }
  input.close();
  std::cout << "PART1: " << sum_of_game_ids << std::endl;
  std::cout << "PART2: " << sum_of_rgb_multiplied << std::endl;
}
