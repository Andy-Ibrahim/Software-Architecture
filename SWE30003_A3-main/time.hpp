#ifndef _103575527SWE300003A3_TIME
#define _103575527SWE300003A3_TIME 1

#include <string>
#include <ctime>

class Time {
	public:
		int hour;
		int minute;

		Time();
		Time(int hour, int minute);
		Time(std::string string);
		std::string to_string();

		inline int operator<=>(const Time& b) const {
			return getMinutesOfDay() - b.getMinutesOfDay();
		}

		int getMinutesOfDay() const;
	private:
		void init(tm time);
};

#endif