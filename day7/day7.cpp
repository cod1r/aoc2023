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
  std::vector<std::string_view> num_strings;
  std::optional<int32_t> digit_start;
  for (int32_t num_parser_idx = 0; num_parser_idx < length; ++num_parser_idx) {
    if (line[num_parser_idx] >= '0' && line[num_parser_idx] <= '9' && !digit_start.has_value()) {
      digit_start = num_parser_idx;
    }
    if ((line[num_parser_idx] == ' ' && digit_start.has_value()) || num_parser_idx == length - 1) {
      if (num_parser_idx == length - 1)
        num_parser_idx = length;
      num_strings.push_back(std::string_view(line + *digit_start, num_parser_idx - *digit_start));
      digit_start.reset();
    }
  }
  for (std::string_view &seed_str : num_strings) {
    numbers.push_back(parse_number(seed_str));
  }
  return numbers;
}
constexpr std::array<char, 13> card_ranks = {
'A',
'K',
'Q',
'J',
'T',
'9',
'8',
'7',
'6',
'5', 
'4',
'3',
'2'
};
constexpr std::array<char, 13> card_ranks_part2 = {
'A',
'K',
'Q',
'T',
'9',
'8',
'7',
'6',
'5', 
'4',
'3',
'2',
'J',
};
enum class HandType {
  FiveKind,
  FourKind,
  FullHouse,
  ThreeKind,
  TwoPair,
  OnePair,
  HighCard,
};
bool compare_hand(HandType a, HandType b) {
  return a > b;
}
HandType get_type(const std::string &hand, bool is_part2) {
  std::array<int32_t, 13> count = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  };
  for (char c : hand) {
    switch (c) {
      case 'A': count[0]++;break;
      case 'K': count[1]++;break;
      case 'Q': count[2]++;break;
      case 'J': count[3]++;break;
      case 'T': count[4]++;break;
      case '9': count[5]++;break;
      case '8': count[6]++;break;
      case '7': count[7]++;break;
      case '6': count[8]++;break;
      case '5': count[9]++;break; 
      case '4': count[10]++;break;
      case '3': count[11]++;break;
      case '2': count[12]++;break;
    }
  }
  if (is_part2) {
    int32_t j_count = count[3];
    count[3] = 0;
    int32_t idx_of_max = 0;
    for (int32_t idx_of_count = 0; idx_of_count < 13; ++idx_of_count) {
      if (count[idx_of_max] < count[idx_of_count]) {
        idx_of_max = idx_of_count;
      }
    }
    count[idx_of_max] += j_count;
  }
  std::array<int32_t, 5> count_of_counts = {
    0,
    0,
    0,
    0,
    0,
  };;
  for (int32_t c : count) {
    count_of_counts[c - 1]++;
  }
  if (count_of_counts[4] == 1) {
    return HandType::FiveKind;
  } else if (count_of_counts[3] == 1 && count_of_counts[0] == 1) {
    return HandType::FourKind;
  } else if (count_of_counts[2] == 1 && count_of_counts[1] == 1) {
    return HandType::FullHouse;
  } else if (count_of_counts[2] == 1 && count_of_counts[0] == 2) {
    return HandType::ThreeKind;
  } else if (count_of_counts[1] == 2 && count_of_counts[0] == 1) {
    return HandType::TwoPair;
  } else if (count_of_counts[1] == 1 && count_of_counts[0] == 3) {
    return HandType::OnePair;
  }
  return HandType::HighCard;
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
  const int32_t LINE_CAPACITY = 512;
  char line[LINE_CAPACITY];
  std::vector<std::pair<std::string, int64_t>> card_and_bid;
  while (input.getline(line, LINE_CAPACITY)) {
    size_t line_length = input.gcount() - 1;
    for (size_t idx = 0; idx < line_length; ++idx) {
      if (line[idx] == ' ') {
        card_and_bid.push_back({
          std::string(line, idx + 1),
          parse_number(std::string_view(line + idx + 1, line_length - (idx + 1)))
        });
        break;
      }
    }
  }
  auto card_and_bid_part2 = card_and_bid;
  std::sort(card_and_bid.begin(), card_and_bid.end(), [&](const auto &a, const auto &b) {
    const std::string &a_hand = a.first;
    HandType a_hand_type = get_type(a_hand, false);

    const std::string &b_hand = b.first;
    HandType b_hand_type = get_type(b_hand, false);

    if (a_hand_type == b_hand_type) {
      for (int32_t idx = 0; idx < 5; ++idx) {
        if (a_hand[idx] != b_hand[idx]) {
          int32_t a_hand_card_rank = 0;
          for (int32_t idx_card_ranking = 0; idx_card_ranking < 13; ++idx_card_ranking) {
            if (card_ranks[idx_card_ranking] == a_hand[idx]) {
              a_hand_card_rank = idx_card_ranking;
              break;
            }
          }
          int32_t b_hand_card_rank = 0;
          for (int32_t idx_card_ranking = 0; idx_card_ranking < 13; ++idx_card_ranking) {
            if (card_ranks[idx_card_ranking] == b_hand[idx]) {
              b_hand_card_rank = idx_card_ranking;
              break;
            }
          }
          return a_hand_card_rank > b_hand_card_rank;
        }
      }
    }

    return compare_hand(a_hand_type, b_hand_type);
  });
  int64_t part1_ans = 0;
  for (size_t idx = 0; idx < card_and_bid.size(); ++idx) {
    part1_ans += (idx + 1) * card_and_bid[idx].second;
  }
  std::cout << "PART1: " << part1_ans << "\n";

  std::sort(card_and_bid_part2.begin(), card_and_bid_part2.end(), [&](const auto& a, const auto& b) {
    const std::string &a_hand = a.first;
    HandType a_hand_type = get_type(a_hand, true);

    const std::string &b_hand = b.first;
    HandType b_hand_type = get_type(b_hand, true);

    if (a_hand_type == b_hand_type) {
      for (int32_t idx = 0; idx < 5; ++idx) {
        if (a_hand[idx] != b_hand[idx]) {
          int32_t a_hand_card_rank = 0;
          for (int32_t idx_card_ranking = 0; idx_card_ranking < 13; ++idx_card_ranking) {
            if (card_ranks_part2[idx_card_ranking] == a_hand[idx]) {
              a_hand_card_rank = idx_card_ranking;
              break;
            }
          }
          int32_t b_hand_card_rank = 0;
          for (int32_t idx_card_ranking = 0; idx_card_ranking < 13; ++idx_card_ranking) {
            if (card_ranks_part2[idx_card_ranking] == b_hand[idx]) {
              b_hand_card_rank = idx_card_ranking;
              break;
            }
          }
          return a_hand_card_rank > b_hand_card_rank;
        }
      }
    }

    return compare_hand(a_hand_type, b_hand_type);
  });
  int64_t part2_ans = 0;
  for (size_t idx = 0; idx < card_and_bid_part2.size(); ++idx) {
    part2_ans += (idx + 1) * card_and_bid_part2[idx].second;
  }
  std::cout << "PART2: " << part2_ans << "\n";
  return 0;
}
