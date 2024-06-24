#ifndef _103575527SWE300003A3_DATE
#define _103575527SWE300003A3_DATE 1

#include <string>
#include <ctime>

class Date {
	public:
		int day;
		int month;
		int year;

		Date();
		Date(int day, int month, int year);
		Date(std::string string);
		std::string to_string();
		bool operator==(const Date& rhs);
		bool operator!=(const Date& rhs);
	private:
		void init(tm time);
};

#endif