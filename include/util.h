#ifndef UTIL_H
#define UTIL_H
#include <vector>
#include <cstdint>

void collect_digits(std::vector<uint8_t>& digits, uint8_t num) {
    if (num > 9) {
        collect_digits(digits, num / 10);
    }
    digits.push_back(num % 10);
}

#endif
