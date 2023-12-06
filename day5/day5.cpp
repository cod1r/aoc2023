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
std::vector<std::pair<int64_t, int64_t>> change_range_and_add_new_range(
    const std::vector<std::tuple<int64_t, int64_t, int64_t>> &map,
    std::vector<std::pair<int64_t, int64_t>> &ranges
    ) {
  std::vector<std::pair<int64_t, int64_t>> add_to_ranges;
  for (const auto &t : map) {
    int64_t dest = std::get<0>(t);
    int64_t src = std::get<1>(t);
    int64_t map_range = std::get<2>(t);
    for (int32_t idx_ranges = 0; idx_ranges < (int32_t)ranges.size();) {
      auto p = ranges[idx_ranges];
      int64_t range_start = p.first;
      int64_t len_of_range = p.second;
      // check if there's any overlap
      if (src < range_start + len_of_range && range_start < src + map_range) {
        int64_t overlap_start = std::max(src, range_start);
        int64_t overlap_range = std::min(src + map_range, range_start + len_of_range) - overlap_start;
        // mapping the start value from src to dest
        int64_t overlap_start_mapped = overlap_start + (dest - src);
        add_to_ranges.push_back({overlap_start_mapped, overlap_range});
        ranges.erase(ranges.begin() + idx_ranges);
        // if there's any unmapped values from the left
        if (overlap_start > range_start) {
          ranges.push_back({range_start, overlap_start - range_start});
        }
        // if there's any unmapped values from the right
        if (range_start + len_of_range > overlap_start + overlap_range) {
          ranges.push_back({overlap_start + overlap_range,
          (range_start + len_of_range) - (overlap_start + overlap_range)});
        }
        continue;
      }
      idx_ranges += 1;
    }
  }
  add_to_ranges.insert(add_to_ranges.end(), ranges.begin(), ranges.end());
  return add_to_ranges;
}
std::ostream& operator<<(std::ostream &os, std::vector<std::pair<int64_t, int64_t>> p) {
  for (auto pp : p) {
    os << pp.first << " " << pp.second << std::endl;
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
  const int32_t LINE_CAPACITY = 512;
  char line[LINE_CAPACITY];
  enum class MapType {
    SeedSoil,
    SoilFert,
    FertWater,
    WaterLight,
    LightTemp,
    TempHumid,
    HumidLocation,
  };
  std::optional<enum MapType> current_map;
  std::vector<int64_t> seeds;
  std::vector<std::tuple<int64_t, int64_t, int64_t>> seed_to_soil;
  std::vector<std::tuple<int64_t, int64_t, int64_t>> soil_to_fert;
  std::vector<std::tuple<int64_t, int64_t, int64_t>> fert_to_water;
  std::vector<std::tuple<int64_t, int64_t, int64_t>> water_to_light;
  std::vector<std::tuple<int64_t, int64_t, int64_t>> light_to_temp;
  std::vector<std::tuple<int64_t, int64_t, int64_t>> temp_to_humid;
  std::vector<std::tuple<int64_t, int64_t, int64_t>> humid_to_location;
  while (input.getline(line, LINE_CAPACITY)) {
    int32_t line_length = input.gcount() - 1;
    if (line_length == 0)
      continue;
    bool has_colon = false;
    for (int32_t idx = 0; idx < line_length; ++idx) {
      if (line[idx] == ':') {
        std::string_view string_before_colon(line, idx);
        if (string_before_colon == "seeds") {
          seeds = parse_string_to_vec_numbers(line + idx + 2, line_length - (idx + 2));
        } else if (string_before_colon == "seed-to-soil map") {
          current_map = MapType::SeedSoil;
        } else if (string_before_colon == "soil-to-fertilizer map") {
          current_map = MapType::SoilFert;
        } else if (string_before_colon == "fertilizer-to-water map") {
          current_map = MapType::FertWater;
        } else if (string_before_colon == "water-to-light map") {
          current_map = MapType::WaterLight;
        } else if (string_before_colon == "light-to-temperature map") {
          current_map = MapType::LightTemp;
        } else if (string_before_colon == "temperature-to-humidity map") {
          current_map = MapType::TempHumid;
        } else if (string_before_colon == "humidity-to-location map") {
          current_map = MapType::HumidLocation;
        }
        has_colon = true;
        break;
      }
    }
    if (has_colon)
      continue;
    std::vector<int64_t> mapping_numbers = parse_string_to_vec_numbers(line, line_length);
    std::tuple<int64_t, int64_t, int64_t> t = {
      mapping_numbers[0],
      mapping_numbers[1],
      mapping_numbers[2],
    };
    if (current_map.has_value()) {
      switch (*current_map) {
        case MapType::SeedSoil:
        seed_to_soil.push_back(t);
        break;
        case MapType::SoilFert:
        soil_to_fert.push_back(t);
        break;
        case MapType::FertWater:
        fert_to_water.push_back(t);
        break;
        case MapType::WaterLight:
        water_to_light.push_back(t);
        break;
        case MapType::LightTemp:
        light_to_temp.push_back(t);
        break;
        case MapType::TempHumid:
        temp_to_humid.push_back(t);
        break;
        case MapType::HumidLocation:
        humid_to_location.push_back(t);
        break;
      }
    }
  }
  input.close();
  int64_t min_location_1 = ((uint64_t)1 << 63) - 1;
  for (int64_t seed_num : seeds) {
    for (std::tuple<int64_t, int64_t, int64_t> &t : seed_to_soil) {
      int64_t src = std::get<1>(t);
      int64_t dest = std::get<0>(t);
      int64_t range = std::get<2>(t);
      if (seed_num >= src && seed_num <= src + range - 1) {
        seed_num += dest - src;
        break;
      }
    }
    for (std::tuple<int64_t, int64_t, int64_t> &t : soil_to_fert) {
      int64_t src = std::get<1>(t);
      int64_t dest = std::get<0>(t);
      int64_t range = std::get<2>(t);
      if (seed_num >= src && seed_num <= src + range - 1) {
        seed_num += dest - src;
        break;
      }
    }
    for (std::tuple<int64_t, int64_t, int64_t> &t : fert_to_water) {
      int64_t src = std::get<1>(t);
      int64_t dest = std::get<0>(t);
      int64_t range = std::get<2>(t);
      if (seed_num >= src && seed_num <= src + range - 1) {
        seed_num += dest - src;
        break;
      }
    }
    for (std::tuple<int64_t, int64_t, int64_t> &t : water_to_light) {
      int64_t src = std::get<1>(t);
      int64_t dest = std::get<0>(t);
      int64_t range = std::get<2>(t);
      if (seed_num >= src && seed_num <= src + range - 1) {
        seed_num += dest - src;
        break;
      }
    }
    for (std::tuple<int64_t, int64_t, int64_t> &t : light_to_temp) {
      int64_t src = std::get<1>(t);
      int64_t dest = std::get<0>(t);
      int64_t range = std::get<2>(t);
      if (seed_num >= src && seed_num <= src + range - 1) {
        seed_num += dest - src;
        break;
      }
    }
    for (std::tuple<int64_t, int64_t, int64_t> &t : temp_to_humid) {
      int64_t src = std::get<1>(t);
      int64_t dest = std::get<0>(t);
      int64_t range = std::get<2>(t);
      if (seed_num >= src && seed_num <= src + range - 1) {
        seed_num += dest - src;
        break;
      }
    }
    for (std::tuple<int64_t, int64_t, int64_t> &t : humid_to_location) {
      int64_t src = std::get<1>(t);
      int64_t dest = std::get<0>(t);
      int64_t range = std::get<2>(t);
      if (seed_num >= src && seed_num <= src + range - 1) {
        seed_num += dest - src;
        break;
      }
    }
    min_location_1 = std::min<int64_t>(min_location_1, seed_num);
  }
  std::cout << "PART1: " << min_location_1 << std::endl;

  int64_t part2_min_location = ((uint64_t)1 << 63) - 1;
  for (int32_t idx_seed = 0; idx_seed < (int32_t)seeds.size(); idx_seed += 2) {
    int64_t seed_start = seeds[idx_seed];
    int64_t seed_range = seeds[idx_seed + 1];
    std::vector<std::pair<int64_t, int64_t>> ranges;
    ranges.push_back({seed_start, seed_range});
    ranges = change_range_and_add_new_range(seed_to_soil, ranges);
    ranges = change_range_and_add_new_range(soil_to_fert, ranges);
    ranges = change_range_and_add_new_range(fert_to_water, ranges);
    ranges = change_range_and_add_new_range(water_to_light, ranges);
    ranges = change_range_and_add_new_range(light_to_temp, ranges);
    ranges = change_range_and_add_new_range(temp_to_humid, ranges);
    ranges = change_range_and_add_new_range(humid_to_location, ranges);
    for (const auto &r : ranges) {
      part2_min_location = std::min<int64_t>(r.first, part2_min_location);
    }
  }
  std::cout << "PART2: " << part2_min_location << std::endl;
  return 0;
}
