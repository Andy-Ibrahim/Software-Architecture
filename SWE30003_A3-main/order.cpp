#include "order.hpp"

OrderStats::OrderStats(int count, int price) {
	this->count = count;
	this->price = price;
}

OrderStats::OrderStats(int count) {
	this->count = count;
	this->price = 0;
}

void OrderStats::calculate_price(int product_price) { price = count * product_price; }

Order::Order(order_id id, ::customer_id customer_id, ::table_id table_id, ::order_type order_type, Date date, bool delivered, bool deleted) {
	this->id = id;
	this->customer_id = customer_id;
	this->table_id = table_id;
	this->order_type = order_type;
	this->date = date;
	this->delivered = delivered;
	this->deleted = deleted;
}

Order::Order(::customer_id customer_id, ::table_id table_id, ::order_type order_type, Date date, bool delivered) {
	this->id = 0;
	this->customer_id = customer_id;
	this->table_id = table_id;
	this->order_type = order_type;
	this->date = date;
	this->delivered = delivered;
	this->deleted = false;
}

Order::Order(::customer_id customer_id, ::table_id table_id, ::order_type order_type, Date date) {
	this->id = 0;
	this->customer_id = customer_id;
	this->table_id = table_id;
	this->order_type = order_type;
	this->date = date;
	this->delivered = false;
	this->deleted = false;
}

int Order::get_total_price() {
	int total = 0;
	for (auto it = items.begin(); it != items.end(); ++it) {
		total += it->second.get_price();
	}
	return total;
}