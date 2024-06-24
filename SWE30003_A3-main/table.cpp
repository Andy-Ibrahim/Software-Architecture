#include "table.hpp"

Table::Table(table_id id, std::string name, int size, bool deleted) {
	this->id = id;
	this->name = name;
	this->size = size;
	this->deleted = deleted;
}

Table::Table(std::string name, int size) {
	this->id = 0;
	this->name = name;
	this->size = size;
	this->deleted = false;
}