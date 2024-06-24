#include "customer.hpp"

Customer::Customer(customer_id id, std::string name, std::string phone_number, bool deleted) {
	this->id = id;
	this->name = name;
	this->phone_number = phone_number;
	this->deleted = deleted;
}

Customer::Customer(std::string name, std::string phone_number) {
	this->id = 0;
	this->name = name;
	this->phone_number = phone_number;
	this->deleted = false;
}

Customer::Customer(std::string name) {
	this->id = 0;
	this->name = name;
	null_phone_number();
	this->deleted = false;
}

void Customer::null_phone_number() {
	phone_number = "-";
}