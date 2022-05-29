#include "calc.hpp"
#include "Calculator.hpp"

unsigned long calc(const std::string& val) {
	std::string s = val;
	if (!CalculationImpl::AllValueIsHalfNumberText(s)) CalculationImpl::ConvertNumTextToHalfSizeString(s);
	return CalculationImpl::CalcCheckDigit(s);
}
