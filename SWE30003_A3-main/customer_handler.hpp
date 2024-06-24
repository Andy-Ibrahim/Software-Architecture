#ifndef _103575527SWE300003A3_CHANDLER
#define _103575527SWE300003A3_CHANDLER 1

#include "customer.hpp"
#include <unordered_map>
#include <vector>
#include <string>

class CustomerHandler {
	private:
		static CustomerHandler *instance;
		std::unordered_map<customer_id, Customer*> loaded_customers;

        CustomerHandler() {}
		std::vector<customer_id> get_name_customer_ids(std::string match_name);
	public:
        CustomerHandler(CustomerHandler const&) = delete;
        void operator=(CustomerHandler const&) = delete;
        CustomerHandler(CustomerHandler&&) = delete;
        void operator=(CustomerHandler&&) = delete;

		static CustomerHandler *get_instance();
		Customer *get_customer(customer_id id);
		std::vector<Customer*> get_name_customers(std::string match_name);
        bool update_customer(customer_id id);
        void add_customer(Customer *customer);
		bool delete_customer(customer_id id);
};

#endif