#include "date.hpp"
#include <sstream>
#include <iomanip>

Date::Date() {
	time_t now = std::time(0);
	tm local_time = *std::localtime(&now);
	init(local_time);
}

Date::Date(int day, int month, int year) {
	this->day = day;
	this->month = month;
	this->year = year;
}

Date::Date(std::string string) {
	tm time = {};
    std::istringstream ss(string.c_str());
    ss >> std::get_time(&time, "%d/%m/%Y");
	init(time);
}

void Date::init(tm time) {
	day = time.tm_mday;
	month = time.tm_mon + 1;
	year = time.tm_year + 1900;
}

std::string Date::to_string() {
	return std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);
}

bool Date::operator==(const Date& rhs) {
	return day == rhs.day && month == rhs.month && year == rhs.year;
}

bool Date::operator!=(const Date& rhs) {
	return day != rhs.day || month != rhs.month || year != rhs.year;
}