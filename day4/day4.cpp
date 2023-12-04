#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <optional>
#include <cmath>
#include <numeric>
#include <execution>
int32_t main(int32_t argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "./day.out input.txt is the format required\n";
    return 1;
  }
  std::ifstream input(argv[1]);
  if (!input.is_open()) {
    std::cerr << "file cannot be opened\n";
    return 1;
  }
  char line[512];
  std::array<std::string_view, 256> winning;
  int32_t winning_idx = 0;
  int32_t part1_sum = 0;
  int32_t num_lines = 0;
  std::vector<int32_t> card_num_winnings;
  card_num_winnings.reserve(500);
  while (input.getline(line, 512)) {
    ++num_lines;
    int32_t line_length = input.gcount() - 1;
    bool past_semi = false;
    bool past_pipe = false;
    std::optional<int32_t> start_of_number;
    int32_t matches = 0;
    for (int32_t idx = 0; idx < line_length; ++idx) {
      if (line[idx] == ':') {
        past_semi = true;
      } else if (line[idx] == '|') {
        past_pipe = true;
      }
      if (past_semi) {
        if (line[idx] >= '0' && line[idx] <= '9' && !start_of_number.has_value()) {
          start_of_number = idx;
        }
        if (start_of_number.has_value() && (line[idx] == ' ' || idx == line_length - 1)) {
          if (!past_pipe) {
            winning[winning_idx] = std::string_view(line + *start_of_number, idx - *start_of_number);
            ++winning_idx;
          } else {
            std::string_view holding_number(
                line + *start_of_number,
                (idx == line_length - 1 ? line_length : idx) - *start_of_number
            );
            for (int32_t curr_winning = 0; curr_winning < winning_idx; ++curr_winning) {
              if (holding_number == winning[curr_winning]) {
                ++matches;
                break;
              }
            }
          }
          start_of_number.reset();
        }
      }
    }
    card_num_winnings.push_back(matches);
    part1_sum += 1 << (matches - 1);
    winning_idx = 0;
  }
  std::vector<int32_t> count_of_each_card(num_lines, 1);
  for (int32_t idx_of_card = 0; idx_of_card < num_lines; ++idx_of_card) {
    for (int32_t idx_of_increment = idx_of_card + 1;
        idx_of_increment < idx_of_card + card_num_winnings[idx_of_card] + 1;
        ++idx_of_increment) {
      count_of_each_card[idx_of_increment] += 1 * count_of_each_card[idx_of_card];
    }
  }
  std::cout << "PART1: " << part1_sum << std::endl;
  std::cout << "PART2: " << std::reduce(std::execution::par, count_of_each_card.begin(), count_of_each_card.end()) << std::endl;
}
