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
#include <ranges>
#include <set>
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
std::vector<int64_t> parse_string_to_vec_numbers(char *line, size_t length,
                                                 char delimiter = ' ') {
  std::vector<int64_t> numbers;
  enum class Sign {
    Pos,
    Neg,
  };
  std::vector<std::pair<Sign, std::string_view>> num_strings;
  std::optional<int32_t> digit_start;
  for (size_t num_parser_idx = 0; num_parser_idx < length; ++num_parser_idx) {
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
void dfs_concat_groups(
    const std::vector<std::vector<std::string>> &variants_of_groups,
    size_t length_of_springs, const std::string &filter_str,
    std::string curr_concat, size_t idx_group, std::vector<std::string> &out) {
  for (size_t idx_filter_str = 0; idx_filter_str < filter_str.length() &&
                                  idx_filter_str < curr_concat.length();
       ++idx_filter_str) {
    if ((filter_str[idx_filter_str] == '.' ||
         filter_str[idx_filter_str] == '#') &&
        curr_concat[idx_filter_str] != filter_str[idx_filter_str])
      return;
  }
  if (curr_concat.length() == length_of_springs &&
      idx_group == variants_of_groups.size()) {
    out.push_back(curr_concat);
    return;
  }
  if (idx_group == variants_of_groups.size())
    return;
  for (size_t idx_variant_of_group = 0;
       idx_variant_of_group < variants_of_groups[idx_group].size();
       ++idx_variant_of_group) {
    const std::string &variant_group =
        variants_of_groups[idx_group][idx_variant_of_group];
    if (curr_concat.length() + variant_group.length() <= length_of_springs) {
      dfs_concat_groups(variants_of_groups, length_of_springs, filter_str,
                        curr_concat.length() > 0
                            ? curr_concat + "." + variant_group
                            : variant_group,
                        idx_group + 1, out);
    }
  }
}
std::vector<std::string>
filter_down_variant_group(const std::vector<std::string> &variant_group,
                          const std::string &record, size_t idx_start) {
  auto filtered = std::views::filter(
      variant_group, [&idx_start, record](const std::string &group) {
        for (size_t start = idx_start; start < idx_start + group.length();
             ++start) {
          if ((record[start] == '.' || record[start] == '#') &&
              group[start - idx_start] != record[start])
            return false;
        }
        return true;
      });
  return std::vector<std::string>(filtered.begin(), filtered.end());
}
std::vector<std::string> generate_arrangements_based_off_of_group_size(
    const std::vector<int64_t> &group_size, const std::string &filter_str,
    size_t length_of_springs) {
  std::vector<std::string> arrangements;
  std::vector<std::string> groups;
  size_t summed_up_sizes = 0;
  for (const int64_t &size : group_size) {
    summed_up_sizes += (size_t)size;
    groups.push_back(std::string(size, '#'));
  }
  std::vector<std::vector<std::string>> variants_of_groups(group_size.size());
  if (length_of_springs < (groups.size() - 1 + summed_up_sizes)) {
    return arrangements;
  }
  size_t left_over = length_of_springs - (groups.size() - 1 + summed_up_sizes);
  for (size_t idx_groups = 0; idx_groups < groups.size(); ++idx_groups) {
    std::string cpy_group = groups[idx_groups];
    size_t summed_up_sizes_except_for_this_group =
        summed_up_sizes - cpy_group.length();
    for (size_t left_amt = 0; left_amt <= left_over; ++left_amt) {
      for (size_t right_amt = 0; right_amt <= left_over - left_amt;
           ++right_amt) {
        if (left_amt + right_amt + cpy_group.length() + groups.size() - 1 +
                summed_up_sizes_except_for_this_group <=
            length_of_springs) {
          if (right_amt > 0) {
            std::string right(right_amt, '.');
            cpy_group.append(right);
          }
          if (left_amt > 0) {
            std::string left(left_amt, '.');
            cpy_group = left + cpy_group;
          }
          variants_of_groups[idx_groups].push_back(cpy_group);
          cpy_group = groups[idx_groups];
        }
      }
    }
  }
  variants_of_groups[0] =
      filter_down_variant_group(variants_of_groups[0], filter_str, 0);
  dfs_concat_groups(variants_of_groups, length_of_springs, filter_str,
                    std::string{}, 0, arrangements);
  return arrangements;
}
size_t dfs_with_group_size(const std::string &record, size_t idx_record,
                           const std::vector<int64_t> &group_sizes,
                           size_t idx_group_size,
                           std::vector<std::vector<int64_t>> &memo) {
  if (idx_group_size == group_sizes.size()) {
    bool good_start = true;
    for (size_t checking_for_dysfunctional_after_last = idx_record;
         checking_for_dysfunctional_after_last < record.length();
         ++checking_for_dysfunctional_after_last) {
      if (record[checking_for_dysfunctional_after_last] == '#') {
        good_start = false;
        break;
      }
    }
    if (!good_start) {
      return 0;
    }
    return 1;
  }
  if (memo[idx_record][idx_group_size] != -1) {
    return memo[idx_record][idx_group_size];
  }
  size_t current_size = group_sizes[idx_group_size];
  size_t total_group_sizes = std::accumulate(
      group_sizes.begin() + idx_group_size, group_sizes.end(), size_t{0});
  size_t idx_end =
      record.length() -
      (total_group_sizes + ((group_sizes.size() - idx_group_size) - 1));
  size_t for_sure_dysfunctional = idx_end;
  for (size_t idx_find_for_sure_dysfunctional = idx_record;
       idx_find_for_sure_dysfunctional <= idx_end;
       ++idx_find_for_sure_dysfunctional) {
    if (record[idx_find_for_sure_dysfunctional] == '#') {
      for_sure_dysfunctional = idx_find_for_sure_dysfunctional;
      break;
    }
  }
  size_t return_val = 0;
  for (size_t idx_start = idx_record; idx_start <= for_sure_dysfunctional;
       ++idx_start) {
    bool good_start = true;
    for (size_t idx_check = idx_start; idx_check < idx_start + current_size;
         ++idx_check) {
      if (record[idx_check] == '.') {
        good_start = false;
        break;
      }
    }
    if (idx_start + current_size < record.length() &&
        record[idx_start + current_size] == '#') {
      good_start = false;
    }
    if (good_start) {
          return_val += dfs_with_group_size(record, idx_start + current_size + 1, group_sizes,
                              idx_group_size + 1, memo);
    }
  }
  memo[idx_record][idx_group_size] = return_val;
  return return_val;
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
  std::vector<std::string> condition_records;
  std::vector<std::vector<int64_t>> group_sizes;
  while (input.getline(line, LINE_CAPACITY)) {
    size_t line_length = input.gcount() - 1;
    for (size_t idx_space = 0; idx_space < line_length; ++idx_space) {
      if (line[idx_space] == ' ') {
        condition_records.push_back(std::string(line, idx_space));
        group_sizes.push_back(parse_string_to_vec_numbers(
            line + idx_space, line_length - idx_space, ','));
        break;
      }
    }
  }
  int64_t part1 = 0;
  std::vector<size_t> right_ans(condition_records.size(), 0);
  for (size_t idx_condition_record = 0;
       idx_condition_record < condition_records.size();
       ++idx_condition_record) {
    std::vector<std::string> a = generate_arrangements_based_off_of_group_size(
        group_sizes[idx_condition_record],
        condition_records[idx_condition_record],
        condition_records[idx_condition_record].length());
    const std::string &record = condition_records[idx_condition_record];
    auto filter =
        std::views::filter(a, [record](const std::string &arrangement) {
          for (size_t idx_record = 0; idx_record < record.length();
               ++idx_record) {
            if ((record[idx_record] == '.' || record[idx_record] == '#') &&
                arrangement[idx_record] != record[idx_record]) {
              return false;
            }
          }
          return true;
        });
    std::set<std::string> filtered(filter.begin(), filter.end());
    right_ans[idx_condition_record] = filtered.size();
    part1 += filtered.size();
  }
  std::cout << "PART1: " << part1 << std::endl;

  std::transform(condition_records.begin(), condition_records.end(),
                 condition_records.begin(), [](const std::string &cr) {
                   return cr + "?" + cr + "?" + cr + "?" + cr + "?" + cr;
                 });
  std::transform(group_sizes.begin(), group_sizes.end(), group_sizes.begin(),
                 [](const std::vector<int64_t> &gs) {
                   std::vector<int64_t> gs_cpy;
                   gs_cpy.insert(gs_cpy.begin(), gs.begin(), gs.end());
                   gs_cpy.insert(gs_cpy.begin(), gs.begin(), gs.end());
                   gs_cpy.insert(gs_cpy.begin(), gs.begin(), gs.end());
                   gs_cpy.insert(gs_cpy.begin(), gs.begin(), gs.end());
                   gs_cpy.insert(gs_cpy.begin(), gs.begin(), gs.end());
                   return gs_cpy;
                 });

  size_t part2 = 0;
  for (size_t idx = 0; idx < condition_records.size(); ++idx) {
    const std::string &record = condition_records[idx];
    std::vector<std::vector<int64_t>> memo(
        record.length(), std::vector<int64_t>(group_sizes[idx].size(), -1));
    part2 += dfs_with_group_size(record, 0, group_sizes[idx], 0, memo);
  }
  std::cout << "PART2: " << part2 << std::endl;

  return 0;
}
