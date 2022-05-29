#include <string>
#include <regex>

namespace CalculationImpl {
    inline bool AllValueIsHalfNumberText(const std::string& val) {
        static const std::regex r(R"([0-9]{12})");
        return std::regex_match(val, r);
    }

    inline bool CheckArg(const std::string& val) {
        static const std::regex r(R"([0-9０-９]{12,36})");
        return std::regex_match(val, r);
    }

    inline void ConvertNumTextToHalfSizeString(std::string& str) {
        std::string Ret{};
        for (const char& c : str) {
            if (c >= '0' && c <= '9') Ret += c;
            else {
                if (const unsigned int i = static_cast<unsigned int>(c); i < 0xffffff90 || i > 0xffffff99) continue;
                Ret += c - 0xffffff90 + '0';
            }
        }
        str = Ret;
    }

    inline unsigned long CalcCheckDigit(const std::string& val) {
        if (!AllValueIsHalfNumberText(val)) return {};
        unsigned int Total = 0;
        for (auto i = val.begin(); i < val.end(); i += 2) {
            Total += static_cast<unsigned long>(*i - '0') * 2 + static_cast<unsigned long>(*(i + 1) - '0');
        }
        return 9 - (Total % 9);
    }

    inline std::string GenerateCompanyNumber(const unsigned long& CheckDigit, const std::string& val) {
        if (!AllValueIsHalfNumberText(val) || CheckDigit == 0 || CheckDigit > 9) return {};
        return std::to_string(CheckDigit) + "-" + val.substr(0, 4) + "-" + val.substr(4, 4) + "-" + val.substr(8, 4);
    }
}

inline std::string CalcCompanyNumber(std::string arg) {
    if (!CalculationImpl::CheckArg(arg)) return {};
    CalculationImpl::ConvertNumTextToHalfSizeString(arg);
    const unsigned long CheckDigit = CalculationImpl::CalcCheckDigit(arg);
    if (CheckDigit == 0) return {};
    return CalculationImpl::GenerateCompanyNumber(CheckDigit, arg);
}
