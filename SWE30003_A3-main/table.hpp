#ifndef _103575527SWE300003A3_TABLE
#define _103575527SWE300003A3_TABLE 1

#include <string>

typedef int table_id;

class Table {
	public:
		table_id id;
		std::string name;
		int size;
		bool deleted;

		Table(table_id id, std::string name, int size, bool deleted);
		Table(std::string name, int size);
};

#endif