#include "../CalculatorDll/calc.hpp"
#include "../CalculationDll2/yumecalc.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <chrono>
#pragma comment(lib, "CalculatorDll.lib")
#pragma comment(lib, "CalculationDll2.lib")
constexpr unsigned long TryCount = 10000;
std::vector<unsigned long> CheckDigits1{};
std::vector<unsigned long> CheckDigits2{};
const std::string Target[3] = { u8"700110005901", u8"７００１１０００５９０１", u8"70011０００5901" };

void self() {
    std::array<unsigned long long, 3> times = { 0, 0, 0 };

    for (unsigned long n = 0; n < TryCount; n++) {
        for (int i = 0; i < 3; i++) {
            const std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
            const unsigned long CheckDigit = calc(Target[i]);
            const std::chrono::time_point<std::chrono::system_clock> last = std::chrono::system_clock::now();
            times[i] += std::chrono::duration_cast<std::chrono::nanoseconds>(last - start).count();
            CheckDigits1.push_back(CheckDigit);
        }
    }

    for (int i = 0; i < 3; i++) {
        std::cout << "パターン" << i << std::endl;
        std::cout << "合計時間: " << times[i] << "ns" << std::endl;
        std::cout << "平均時間: " << (times[i] / TryCount) << "ns" << std::endl;
    }
}

void yumetodo() {
    std::array<unsigned long long, 3> times = { 0, 0, 0 };
    for (unsigned long n = 0; n < TryCount; n++) {
        for (int i = 0; i < 3; i++) {
            const std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
            const unsigned long CheckDigit = yumecalc(Target[i]);
            const std::chrono::time_point<std::chrono::system_clock> last = std::chrono::system_clock::now();
            times[i] += std::chrono::duration_cast<std::chrono::nanoseconds>(last - start).count();
            CheckDigits2.push_back(CheckDigit);
        }
    }

    for (int i = 0; i < 3; i++) {
        std::cout << "パターン" << i << std::endl;
        std::cout << "合計時間: " << times[i] << "ns" << std::endl;
        std::cout << "平均時間: " << (times[i] / TryCount) << "ns" << std::endl;
    }
    std::cout << discribe() << std::endl;
}


int main() {
    std::cout << "self code" << std::endl;
    self();
    std::cout << std::endl;
    std::cout << "yumetodo code" << std::endl;
    yumetodo();
    std::cout << "Self Failed Case: " << std::count_if(CheckDigits1.begin(), CheckDigits1.end(), [](const unsigned long& i) { return i != 8; }) << std::endl;
    std::cout << "yume Failed Case: " << std::count_if(CheckDigits2.begin(), CheckDigits2.end(), [](const unsigned long& i) { return i != 8; }) << std::endl;
    return 0;
}

