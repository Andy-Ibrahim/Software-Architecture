#ifndef _103575527SWE300003A3_UTIL
#define _103575527SWE300003A3_UTIL 1

#include <string>

inline std::string format_price(int price) {
	std::string decimal = std::to_string(price % 100);
	if (decimal.length() == 1) { decimal = "0" + decimal; }
	return "$" + std::to_string(price / 100) + "." + decimal;
}

#endif