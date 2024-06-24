#include "product.hpp"

Product::Product(product_id id, std::string name, int price, bool deleted) {
	this->id = id;
	this->name = name;
	this->price = price;
	this->deleted = deleted;
}

Product::Product(std::string name, int price) {
	this->id = 0;
	this->name = name;
	this->price = price;
	this->deleted = false;
}