#include "order_handler.hpp"
#include "database.hpp"
#include <stdexcept>
#include <vector>
#include <list>
#include <iostream>

OrderHandler *OrderHandler::instance = nullptr;

const int MAX_ERRORS = 10;

OrderHandler::OrderHandler() {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		std::vector<Product*> product_list = instance->get_products();
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				for (std::size_t i = 0; i < product_list.size(); ++i) {
					Product *product = product_list[i];
					products.emplace(product->id, product);
				}
				return;
			case DATABASE_NONE:
				throw std::runtime_error("Failed retrieving products, database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed retrieving products, after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

std::vector<Product*> OrderHandler::get_products() {
	std::vector<Product*> result;
	for (auto it = products.begin(); it != products.end(); ++it) {
		Product *product = it->second;
		if (!product->deleted) { result.push_back(product); }
	}
	return result;
}

OrderHandler *OrderHandler::get_instance() {
	return instance ? instance : instance = new OrderHandler();
}

Product *OrderHandler::get_product(product_id id) {
	return products.contains(id) ? products.at(id) : nullptr;
}

bool OrderHandler::update_product(product_id id) {
	if (!products.contains(id)) return false;

	Database *instance = Database::get_instance();
	Product *product = products.at(id);
	int err_count = 0;
	while (true) {
		instance->update_product(product);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed updating product with id '" + std::to_string(id) + "', exists in buffer but not in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed updating product with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

void OrderHandler::add_product(Product *product) {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		product_id id = instance->add_product(product);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				product->id = id;
				products.emplace(id, product);
				return;
			case DATABASE_NONE:
				throw std::runtime_error("Failed creating product with name '" + product->name + "', database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed creating product with name '" + product->name + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool OrderHandler::delete_product(product_id id) {
	if (!products.contains(id)) return false;
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		instance->delete_product(id);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				{ Product *product = products.at(id);
				products.erase(id);
				delete product; }
				return true;
			case DATABASE_OTHER:
				{ Product *product = products.at(id);
				product->deleted = true; }
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed deleting product with id '" + std::to_string(id) + "', exists in buffer but not in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed deleting product with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

Order *OrderHandler::get_order(order_id id) {
	if (loaded_orders.contains(id)) return loaded_orders.at(id);
	Database *instance = Database::get_instance();
	Order *order = nullptr;
	int err_count = 0;
	while (true) {
		order = instance->get_order(id);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				if (order) {
					loaded_orders.emplace(id, order);
					return order;
				}
				throw std::runtime_error("Failed retrieving order with id '" + std::to_string(id) + "', database gave OK result but didn't return an order.");
			case DATABASE_NONE:
				return nullptr;
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed retrieving order with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

std::vector<order_id> OrderHandler::get_date_order_ids(Date date) {
	Database *instance = Database::get_instance();
	std::vector<order_id> id_list;
	int err_count = 0;
	while (true) {
		id_list = instance->get_date_order_ids(date);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				return id_list;
			case DATABASE_NONE:
				throw std::runtime_error("Failed retrieving order ids from date '" + date.to_string() + "', database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed retrieving order ids from date '" + date.to_string() + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

std::vector<Order*> OrderHandler::get_date_orders(Date date) {
	Database *instance = Database::get_instance();
	std::vector<order_id> id_list = get_date_order_ids(date);
	std::vector<Order*> order_list;
	for (std::size_t i = 0; i < id_list.size(); ++i) {
		order_id id = id_list[i];
		Order *order = get_order(id);
		if (order == nullptr) {
			throw std::runtime_error("Failed retrieving orders from date '" + date.to_string() + "', database gave id '" + std::to_string(id) + "', could not retrieve from database.");
		}
		order_list.push_back(order);
	}
	return order_list;
}

std::vector<Order*> OrderHandler::get_date_customer_orders(Date date, customer_id id) {
	std::vector<Order*> order_list = get_date_orders(date);
	std::erase_if(order_list, [id](Order *order){ return order->customer_id != id; });
	return order_list;
}

bool OrderHandler::update_order(order_id id) {
	if (!loaded_orders.contains(id)) return false;
	Database *instance = Database::get_instance();
	Order *order = loaded_orders.at(id);
	int err_count = 0;
	while (true) {
		instance->update_order(order);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed updating order with id '" + std::to_string(id) + "', exists in buffer but not in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed updating order with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

void OrderHandler::add_order(Order *order) {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		order_id id = instance->add_order(order);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				order->id = id;
				loaded_orders.emplace(id, order);
				return;
			case DATABASE_NONE:
				throw std::runtime_error("Failed creating order with customer id '" + std::to_string(order->customer_id) + "', database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed creating order with customer id '" + std::to_string(order->customer_id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool OrderHandler::delete_order(order_id id) {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		instance->delete_order(id);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				if (loaded_orders.contains(id)) {
					Order *order = loaded_orders.at(id);
					loaded_orders.erase(id);
					delete order;
				}
				return true;
			case DATABASE_OTHER:
				if (loaded_orders.contains(id)) {
					Order *order = loaded_orders.at(id);
					order->deleted = true;
				}
				return true;
			case DATABASE_NONE:
				return false;
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed deleting order with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool OrderHandler::add_order_item(order_id oid, product_id pid, OrderStats stats) {
	if (!loaded_orders.contains(oid)) { return false; }
	Order *order = loaded_orders.at(oid);
	if (order->items.contains(pid)) { return false; }
	Product *product = get_product(pid);
	if (product == nullptr) { return false; }
	stats.calculate_price(product->price);
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		instance->add_order_item(oid, pid, stats);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				order->items.emplace(pid, stats);
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed adding order item with order id '" + std::to_string(oid) + "', doesn't exist in buffer but does in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed adding order item with order id '" + std::to_string(oid) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool OrderHandler::update_order_item(order_id oid, product_id pid, OrderStats stats) {
	if (!loaded_orders.contains(oid)) return false;
	Order *order = loaded_orders.at(oid);
	auto it = order->items.find(pid);
	if (it == order->items.end()) { return false; }
	Product *product = get_product(pid);
	if (product == nullptr) { return false; }
	stats.calculate_price(product->price);
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		instance->update_order_item(oid, pid, stats);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				(*it).second = stats;
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed updating order item with order id '" + std::to_string(oid) + "', exists in buffer but not in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed updating order item with order id '" + std::to_string(oid) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool OrderHandler::delete_order_item(order_id oid, product_id pid) {
	if (!loaded_orders.contains(oid)) return false;
	Order *order = loaded_orders.at(oid);
	auto it = order->items.find(pid);
	if (it == order->items.end()) { return false; }
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		instance->delete_order_item(oid, pid);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				order->items.erase(it);
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed deleting order item with order id '" + std::to_string(oid) + "', exists in buffer but not in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed deleting order item with order id '" + std::to_string(oid) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}