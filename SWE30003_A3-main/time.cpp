#include "time.hpp"
#include <sstream>
#include <iomanip>

Time::Time() {
	time_t now = std::time(0);
	tm local_time = *std::localtime(&now);
	init(local_time);
}

Time::Time(int hour, int minute) {
	this->hour = hour;
	this->minute = minute;
}

Time::Time(std::string string) {
	tm time = {};
    std::istringstream ss(string.c_str());
    ss >> std::get_time(&time, "%H:%M");
	init(time);
}

void Time::init(tm time) {
	hour = time.tm_hour;
	minute = time.tm_min;
}

std::string Time::to_string() {
	std::string mins = std::to_string(minute);
	if (mins.size() == 1) {
		mins = "0" + mins;
	}
	return std::to_string(hour) + ":" + mins;
}

int Time::getMinutesOfDay() const {
	int result = hour * 60;
	result += minute;
	return result;
}
