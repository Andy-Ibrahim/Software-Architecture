#ifndef _103575527SWE300003A3_CUSTOMER
#define _103575527SWE300003A3_CUSTOMER 1

#include <string>

typedef int customer_id;

class Customer {
	public:
		customer_id id;
		std::string name;
		std::string phone_number;
		bool deleted;

		Customer(customer_id id, std::string name, std::string phone_number, bool deleted);
		Customer(std::string name, std::string phone_number);
		Customer(std::string name);
		void null_phone_number();
};

#endif