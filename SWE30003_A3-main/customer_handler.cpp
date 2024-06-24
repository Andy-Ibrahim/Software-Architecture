#include "customer_handler.hpp"
#include "database.hpp"
#include <stdexcept>
#include <list>

CustomerHandler *CustomerHandler::instance = nullptr;

CustomerHandler *CustomerHandler::get_instance() {
	return instance ? instance : instance = new CustomerHandler();
}

const int MAX_ERRORS = 10;

Customer *CustomerHandler::get_customer(customer_id id) {
	if (loaded_customers.contains(id)) return loaded_customers.at(id);
	Database *instance = Database::get_instance();
	Customer *customer = nullptr;
	int err_count = 0;
	while (true) {
		customer = instance->get_customer(id);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				if (customer) {
					loaded_customers.emplace(id, customer);
					return customer;
				}
				throw std::runtime_error("Failed retrieving customer with id '" + std::to_string(id) + "', database gave OK result but didn't return a customer.");
			case DATABASE_NONE:
				return nullptr;
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed retrieving customer with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

std::vector<customer_id> CustomerHandler::get_name_customer_ids(std::string match_name) {
	Database *instance = Database::get_instance();
	std::vector<customer_id> id_list;
	int err_count = 0;
	while (true) {
		id_list = instance->get_name_customer_ids(match_name);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				return id_list;
			case DATABASE_NONE:
				throw std::runtime_error("Failed retrieving customer ids from name '" + match_name + "', database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed retrieving customer ids from name '" + match_name + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

std::vector<Customer*> CustomerHandler::get_name_customers(std::string match_name) {
	Database *instance = Database::get_instance();
	std::vector<customer_id> id_list = get_name_customer_ids(match_name);
	std::vector<Customer*> customer_list;
	for (std::size_t i = 0; i < id_list.size(); ++i) {
		customer_id id = id_list[i];
		Customer *customer = get_customer(id);
		if (customer == nullptr) {
			throw std::runtime_error("Failed retrieving customers from name '" + match_name + "', database gave id '" + std::to_string(id) + "', could not retrieve from database.");
		}
		if (!customer->deleted) { customer_list.push_back(customer); }
	}
	return customer_list;
}

bool CustomerHandler::update_customer(customer_id id) {
	if (!loaded_customers.contains(id)) return false;

	Database *instance = Database::get_instance();
	Customer *customer = loaded_customers.at(id);
	int err_count = 0;
	while (true) {
		instance->update_customer(customer);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed updating customer with id '" + std::to_string(id) + "', exists in buffer but not in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed updating customer with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

void CustomerHandler::add_customer(Customer *customer) {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		customer_id id = instance->add_customer(customer);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				customer->id = id;
				loaded_customers.emplace(id, customer);
				return;
			case DATABASE_NONE:
				throw std::runtime_error("Failed creating customer with name '" + customer->name + "', database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed creating customer with name '" + customer->name + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool CustomerHandler::delete_customer(customer_id id) {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		instance->delete_customer(id);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				if (loaded_customers.contains(id)) {
					Customer *customer = loaded_customers.at(id);
					loaded_customers.erase(id);
					delete customer;
				}
				return true;
			case DATABASE_OTHER:
				if (loaded_customers.contains(id)) {
					Customer *customer = loaded_customers.at(id);
					customer->deleted = true;
				}
				return true;
			case DATABASE_NONE:
				return false;
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed deleting customer with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}