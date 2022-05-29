#include "yumecalc.hpp"
#include "Calculator2.hpp"
#include <sstream>
int yumecalc(const std::string& val) {
	return check_digit_calc::calc(val);
}
std::string discribe() {
	std::stringstream ss;
	ss
		<< "gather:" << check_digit_calc::gater << '\n'
		<< "ph1:" << check_digit_calc::ph1 << '\n'
		<< "ph2:" << check_digit_calc::ph2 << '\n'
		<< "acc:" << check_digit_calc::acc << '\n';
	return ss.str();
}