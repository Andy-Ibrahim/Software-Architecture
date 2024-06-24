#include "database.hpp"
#include <stdexcept>
#include <filesystem>

const std::string FILENAME = "database.db";

Database *Database::instance = nullptr;
std::string Database::database_path;

Database::Database(const char* filename) {
	int res = sqlite3_open(filename, &db);
	if (res != SQLITE_OK) {
		throw std::runtime_error(std::string("Could not open database with filename '") + filename + "': " + sqlite3_errmsg(db) + "\n");
	}
	res = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
	if (res != SQLITE_OK) {
		throw std::runtime_error("Could not setup database for foreign keys.");
	}
}

bool Database::is_unrecoverable_result(int res) {
	switch (res) {
		case SQLITE_ROW:
			[[fallthrough]];
		case SQLITE_DONE:
			[[fallthrough]];
		case SQLITE_BUSY:
			return false;
		default:
			return true;
	}
}

bool Database::is_fk_error(int res) {
	return res == SQLITE_CONSTRAINT && sqlite3_extended_errcode(db) == SQLITE_CONSTRAINT_FOREIGNKEY;
}

sqlite3_stmt *Database::prepare_statement(std::string sql) {
	sqlite3_stmt *statement;
	int res = sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, NULL);
	if (res != SQLITE_OK) {
		throw std::runtime_error("Error when preparing statement '" + sql + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n");
	}
	return statement;
}

std::string Database::nullable_id_string(int id) { return id == 0 ? "NULL" : std::to_string(id); }

void Database::init(std::filesystem::path folder_path) {
	std::filesystem::path tmpPath = folder_path.append(FILENAME);
	database_path = (std::string) tmpPath.string();
}

Database *Database::get_instance() {
	return instance ? instance : new Database(database_path.c_str());
}

int Database::get_result_code() { return db_result_code; }

std::vector<customer_id> Database::get_name_customer_ids(std::string match_name) {
	std::vector<customer_id> id_list;
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement(("SELECT id FROM customers WHERE name LIKE '%" + match_name + "%';"));
	bool looping = true;
	while (looping) {
		int res = sqlite3_step(statement);
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving customers for name '" + match_name + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				{ customer_id id = sqlite3_column_int(statement, 0);
				id_list.push_back(id); }
				break;
			default:
				db_result_code = DATABASE_ERROR;
				[[fallthrough]];
			case SQLITE_DONE:
				looping = false;
				break;
		}
	}
	sqlite3_finalize(statement);
	return id_list;
}

Customer *Database::get_customer(customer_id id) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("SELECT name, phone_number, deleted FROM customers WHERE id = " + std::to_string(id) + ";");
	Customer *customer = nullptr;
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving customer with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			{ std::string name = std::string((const char*) sqlite3_column_text(statement, 0));
			std::string phone_number = std::string((const char*) sqlite3_column_text(statement, 1));
			bool deleted = sqlite3_column_int(statement, 2);
			customer = new Customer(id, name, phone_number, deleted); }
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	return customer;
}

void Database::update_customer(Customer *customer) {
	db_result_code = DATABASE_OK;
	customer_id id = customer->id;
	sqlite3_stmt *statement = prepare_statement("UPDATE customers SET name = '" + customer->name + "', phone_number = '" + customer->phone_number + "' WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when updating customer with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

customer_id Database::add_customer(Customer *customer) {
	db_result_code = DATABASE_OK;
	customer_id id = 0;
	sqlite3_stmt *statement = prepare_statement("INSERT INTO customers (name, phone_number) VALUES ('" + customer->name + "', '" + customer->phone_number + "') RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when adding customer with name '" + customer->name + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			id = sqlite3_column_int(statement, 0);
			break;
		case SQLITE_DONE:
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	return id;
}

void Database::delete_customer(customer_id id) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("DELETE FROM customers WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_fk_error(res)) {
		sqlite3_finalize(statement);
		statement = prepare_statement("UPDATE customers SET deleted = 1 WHERE id = " + std::to_string(id) + ";");
		res = sqlite3_step(statement);
		if (is_unrecoverable_result(res) || res == SQLITE_ROW) { throw std::runtime_error("Error when hiding customer with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		db_result_code = (res == SQLITE_DONE) ? DATABASE_OTHER : DATABASE_ERROR;
	} else {
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when deleting customer with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				break;
			case SQLITE_DONE:
				db_result_code = DATABASE_NONE;
				break;
			default:
				db_result_code = DATABASE_ERROR;
				break;
		}
	}
	sqlite3_finalize(statement);
}

std::vector<Product*> Database::get_products() {
	std::vector<Product*> product_list;
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("SELECT id, name, price, deleted FROM products;");
	while (true) {
		int res = sqlite3_step(statement);
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving products, result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				{ product_id id = sqlite3_column_int(statement, 0);
				std::string name = std::string((const char*) sqlite3_column_text(statement, 1));
				int price = sqlite3_column_int(statement, 2);
				bool deleted = sqlite3_column_int(statement, 3);
				product_list.push_back(new Product(id, name, price, deleted)); }
				break;
			default:
				db_result_code = DATABASE_ERROR;
			case SQLITE_DONE:
				sqlite3_finalize(statement);
				return product_list;
		}
	}
}

void Database::update_product(Product *product) {
	db_result_code = DATABASE_OK;
	product_id id = product->id;
	sqlite3_stmt *statement = prepare_statement("UPDATE products SET name = '" + product->name + "', price = " + std::to_string(product->price) + " WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when updating product with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

product_id Database::add_product(Product *product) {
	db_result_code = DATABASE_OK;
	product_id id = 0;
	sqlite3_stmt *statement = prepare_statement("INSERT INTO products (name, price) VALUES ('" + product->name + "', " + std::to_string(product->price) + ") RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when adding product with name '" + product->name + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			id = sqlite3_column_int(statement, 0);
			break;
		case SQLITE_DONE:
			[[fallthrough]];
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	return id;
}

void Database::delete_product(product_id id) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("DELETE FROM products WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_fk_error(res)) {
		sqlite3_finalize(statement);
		statement = prepare_statement("UPDATE products SET deleted = 1 WHERE id = " + std::to_string(id) + ";");
		res = sqlite3_step(statement);
		if (is_unrecoverable_result(res) || res == SQLITE_ROW) { throw std::runtime_error("Error when hiding product with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		db_result_code = (res == SQLITE_DONE) ? DATABASE_OTHER : DATABASE_ERROR;
	} else {
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when deleting product with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				break;
			case SQLITE_DONE:
				db_result_code = DATABASE_NONE;
				break;
			default:
				db_result_code = DATABASE_ERROR;
				break;
		}
	}
	sqlite3_finalize(statement);
}

std::vector<Table*> Database::get_tables() {
	std::vector<Table*> table_list;
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("SELECT id, name, size, deleted FROM tables;");
	while (true) {
		int res = sqlite3_step(statement);
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving tables, result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				{ product_id id = sqlite3_column_int(statement, 0);
				std::string name = std::string((const char*) sqlite3_column_text(statement, 1));
				int size = sqlite3_column_int(statement, 2);
				bool deleted = sqlite3_column_int(statement, 3);
				table_list.push_back(new Table(id, name, size, deleted)); }
				break;
			default:
				db_result_code = DATABASE_ERROR;
				[[fallthrough]];
			case SQLITE_DONE:
				sqlite3_finalize(statement);
				return table_list;
		}
	}
}

void Database::populate_order_items(Order *order) {
	order_id id = order->id;
	sqlite3_stmt *statement = prepare_statement("SELECT pid, count, price FROM order_items WHERE oid = " + std::to_string(id) + ";");
	bool looping = true;
	while (looping) {
		int res = sqlite3_step(statement);
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving order items with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				{ product_id pid = sqlite3_column_int(statement, 0);
				int count = sqlite3_column_int(statement, 1);
				int price = sqlite3_column_int(statement, 2);
				order->items.emplace(pid, OrderStats(count, price)); }
				break;
			default:
				db_result_code = DATABASE_ERROR;
				[[fallthrough]];
			case SQLITE_DONE:
				looping = false;
				break;
		}
	}
	sqlite3_finalize(statement);
}

Order *Database::get_order(order_id id) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("SELECT cid, tid, type, date, delivered, deleted FROM orders WHERE id = " + std::to_string(id) + ";");
	Order *order = nullptr;
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving customer with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			{ customer_id cid = sqlite3_column_int(statement, 0);
			table_id tid = sqlite3_column_int(statement, 1);
			order_type type = sqlite3_column_int(statement, 2);
			Date date = Date((const char*) sqlite3_column_text(statement, 3));
			bool delivered = sqlite3_column_int(statement, 4);
			bool deleted = sqlite3_column_int(statement, 5);
			order = new Order(id, cid, tid, type, date, delivered, deleted); }
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	if (db_result_code == DATABASE_OK) {
		populate_order_items(order);
	}
	return order;
}

std::vector<order_id> Database::get_date_order_ids(Date date) {
	std::vector<order_id> id_list;
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("SELECT id FROM orders WHERE date = '" + date.to_string() + "';");
	bool looping = true;
	while (looping) {
		int res = sqlite3_step(statement);
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving order ids for date '" + date.to_string() + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				{ order_id id = sqlite3_column_int(statement, 0);
				id_list.push_back(id); }
				break;
			default:
				db_result_code = DATABASE_ERROR;
				[[fallthrough]];
			case SQLITE_DONE:
				looping = false;
				break;
		}
	}
	sqlite3_finalize(statement);
	return id_list;
}

void Database::update_order(Order *order) {
	db_result_code = DATABASE_OK;
	order_id id = order->id;
	sqlite3_stmt *statement = prepare_statement("UPDATE orders SET cid = " + std::to_string(order->customer_id) + ", tid = " + std::to_string(order->table_id) + ", type = " + std::to_string((int) order->order_type) + ", date = '" + order->date.to_string() + "', delivered = " + std::to_string((int) order->delivered) + " WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when updating order with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

order_id Database::add_order(Order *order) {
	db_result_code = DATABASE_OK;
	order_id id = 0;
	sqlite3_stmt *statement = prepare_statement("INSERT INTO orders (cid, tid, type, date, delivered) VALUES (" + std::to_string(order->customer_id) + ", " + std::to_string(order->table_id) + ", " + std::to_string((int) order->order_type) + ", '" + order->date.to_string() + "', " + std::to_string((int) order->delivered) + ") RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when adding order with customer id '" + std::to_string(order->customer_id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			id = sqlite3_column_int(statement, 0);
			break;
		case SQLITE_DONE:
			[[fallthrough]];
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	return id;
}

void Database::delete_order_items(order_id id) {
	sqlite3_stmt *statement = prepare_statement("DELETE FROM order_items WHERE oid = " + std::to_string(id) + ";");
	int res = sqlite3_step(statement);
	switch (res) {
		case SQLITE_ROW:
			[[fallthrough]];
		case SQLITE_DONE:
			break;
		default:
			throw std::runtime_error("! MAJOR ERROR ! Error when deleting order items with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n");
	}
	sqlite3_finalize(statement);
}

void Database::delete_order(order_id id) {
	db_result_code = DATABASE_OK;
	delete_order_items(id);

	sqlite3_stmt *statement = prepare_statement("DELETE FROM orders WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_fk_error(res)) {
		sqlite3_finalize(statement);
		statement = prepare_statement("UPDATE orders SET deleted = 1 WHERE id = " + std::to_string(id) + ";");
		res = sqlite3_step(statement);
		if (is_unrecoverable_result(res) || res == SQLITE_ROW) { throw std::runtime_error("Error when hiding order with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		db_result_code = (res == SQLITE_DONE) ? DATABASE_OTHER : DATABASE_ERROR;
	} else {
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when deleting order with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				break;
			case SQLITE_DONE:
				db_result_code = DATABASE_NONE;
				break;
			default:
				db_result_code = DATABASE_ERROR;
				break;
		}
	}
	sqlite3_finalize(statement);
}

void Database::add_order_item(order_id oid, product_id pid, OrderStats stats) {
	db_result_code = DATABASE_OK;
	customer_id id = 0;
	sqlite3_stmt *statement = prepare_statement("INSERT INTO order_items (oid, pid, count, price) VALUES (" + std::to_string(oid) + ", " + std::to_string(pid) + ", " + std::to_string(stats.count) + ", " + std::to_string(stats.get_price()) + ");");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when adding order item with order id '" + std::to_string(oid) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_DONE:
			break;
		case SQLITE_ROW:
			[[fallthrough]];
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

void Database::update_order_item(order_id oid, product_id pid, OrderStats stats) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("UPDATE order_items SET count = " + std::to_string(stats.count) + ", price = " + std::to_string(stats.get_price()) + " WHERE oid = " + std::to_string(oid) + " AND pid = " + std::to_string(pid) + " RETURNING oid;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when updating order item with order id '" + std::to_string(oid) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

void Database::delete_order_item(order_id oid, product_id pid) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("DELETE FROM order_items WHERE oid = " + std::to_string(oid) + " AND pid = " + std::to_string(pid) + " RETURNING oid;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when deleting order item with order id '" + std::to_string(oid) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

Booking *Database::get_booking(booking_id id) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("SELECT cid, tid, date, start, end FROM bookings WHERE id = " + std::to_string(id) + ";");
	Booking *booking = nullptr;
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving booking with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			{ customer_id cid = sqlite3_column_int(statement, 0);
			table_id tid = sqlite3_column_int(statement, 1);
			Date date = Date((const char*) sqlite3_column_text(statement, 2));
			Time start = Time((const char*) sqlite3_column_text(statement, 3));
			Time end = Time((const char*) sqlite3_column_text(statement, 4));
			booking = new Booking(id, cid, tid, date, start, end); }
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	return booking;
}

std::vector<booking_id> Database::get_date_booking_ids(Date date) {
	std::vector<booking_id> id_list;
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("SELECT id FROM bookings WHERE date = '" + date.to_string() + "';");
	bool looping = true;
	while (looping) {
		int res = sqlite3_step(statement);
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving booking ids for date '" + date.to_string() + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				{ booking_id id = sqlite3_column_int(statement, 0);
				id_list.push_back(id); }
				break;
			default:
				db_result_code = DATABASE_ERROR;
				[[fallthrough]];
			case SQLITE_DONE:
				looping = false;
				break;
		}
	}
	sqlite3_finalize(statement);
	return id_list;
}

void Database::update_booking(Booking *booking) {
	db_result_code = DATABASE_OK;
	booking_id id = booking->id;
	sqlite3_stmt *statement = prepare_statement("UPDATE bookings SET cid = " + std::to_string(booking->customer_id) + ", tid = " + std::to_string(booking->table_id) + ", date = '" + booking->date.to_string() + "', start = '" + booking->start.to_string() + "', end = '" + booking->end.to_string() + "' WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when updating booking with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

booking_id Database::add_booking(Booking *booking) {
	db_result_code = DATABASE_OK;
	booking_id id = 0;
	sqlite3_stmt *statement = prepare_statement("INSERT INTO bookings (cid, tid, date, start, end) VALUES (" + std::to_string(booking->customer_id) + ", " + std::to_string(booking->table_id) + ", '" + booking->date.to_string() + "', '" + booking->start.to_string() + "', '" + booking->end.to_string() + "') RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when adding booking with customer id '" + std::to_string(booking->customer_id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			id = sqlite3_column_int(statement, 0);
			break;
		case SQLITE_DONE:
			[[fallthrough]];
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	return id;
}

void Database::delete_booking(booking_id id) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("DELETE FROM bookings WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when deleting booking with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

Payment *Database::get_payment(payment_id id) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("SELECT amount, type, card_number, transaction_id FROM payments WHERE id = " + std::to_string(id) + ";");
	Payment *payment = nullptr;
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving payment with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			{ int amount = sqlite3_column_int(statement, 0);
			payment_type type = sqlite3_column_int(statement, 1);
			std::string card_number = (const char*) sqlite3_column_text(statement, 2);
			std::string transaction_id = (const char*) sqlite3_column_text(statement, 3);
			payment = new Payment(id, amount, type, card_number, transaction_id); }
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	return payment;
}

void Database::update_payment(Payment *payment) {
	db_result_code = DATABASE_OK;
	payment_id id = payment->id;
	sqlite3_stmt *statement = prepare_statement("UPDATE payments SET amount = " + std::to_string(payment->amount) + ", type = " + std::to_string((int) payment->payment_type) + ", card_number = '" + payment->card_number + ", transaction_id = '" + payment->transaction_id + "' WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when updating payment with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

payment_id Database::add_payment(Payment *payment) {
	db_result_code = DATABASE_OK;
	payment_id id = 0;
	sqlite3_stmt *statement = prepare_statement("INSERT INTO payments (amount, type, card_number, transaction_id) VALUES (" + std::to_string(payment->amount) + ", " + std::to_string((int) payment->payment_type) + ", '" + payment->card_number + "', '" + payment->transaction_id + "') RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when adding payment with transaction id '" + payment->transaction_id + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			id = sqlite3_column_int(statement, 0);
			break;
		case SQLITE_DONE:
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	return id;
}

void Database::delete_payment(payment_id id) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("DELETE FROM payments WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_fk_error(res)) {
		db_result_code = DATABASE_OTHER;
	} else {
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when deleting payment with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				break;
			case SQLITE_DONE:
				db_result_code = DATABASE_NONE;
				break;
			default:
				db_result_code = DATABASE_ERROR;
				break;
		}
	}
	sqlite3_finalize(statement);
}

void Database::populate_invoice_orders(Invoice *invoice) {
	invoice_id id = invoice->id;
	sqlite3_stmt *statement = prepare_statement("SELECT oid FROM invoice_orders WHERE iid = " + std::to_string(id) + ";");
	bool looping = true;
	while (looping) {
		int res = sqlite3_step(statement);
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving linked invoice orders with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				{ order_id oid = sqlite3_column_int(statement, 0);
				invoice->linked_orders.push_back(oid); }
				break;
			default:
				db_result_code = DATABASE_ERROR;
			case SQLITE_DONE:
				looping = false;
				break;
		}
	}
	sqlite3_finalize(statement);
}

Invoice *Database::get_invoice(invoice_id id) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("SELECT cid, date, payid FROM invoices WHERE id = " + std::to_string(id) + ";");
	Invoice *invoice = nullptr;
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving invoice with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			{ customer_id cid = sqlite3_column_int(statement, 0);
			Date date = Date((const char*) sqlite3_column_text(statement, 1));
			payment_id payid = sqlite3_column_int(statement, 2);
			invoice = new Invoice(id, cid, date, payid); }
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	if (db_result_code == DATABASE_OK) {
		populate_invoice_orders(invoice);
	}
	return invoice;
}

std::vector<invoice_id> Database::get_date_invoice_ids(Date date) {
	std::vector<invoice_id> id_list;
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("SELECT id FROM invoices WHERE date = '" + date.to_string() + "';");
	bool looping = true;
	while (looping) {
		int res = sqlite3_step(statement);
		if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when retrieving invoice ids for date '" + date.to_string() + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
		switch (res) {
			case SQLITE_ROW:
				{ invoice_id id = sqlite3_column_int(statement, 0);
				id_list.push_back(id); }
				break;
			default:
				db_result_code = DATABASE_ERROR;
			case SQLITE_DONE:
				looping = false;
				break;
		}
	}
	sqlite3_finalize(statement);
	return id_list;
}

void Database::update_invoice(Invoice *invoice) {
	db_result_code = DATABASE_OK;
	invoice_id id = invoice->id;
	sqlite3_stmt *statement = prepare_statement("UPDATE invoices SET cid = " + std::to_string(invoice->customer_id) + ", date = '" + invoice->date.to_string() + "', payid = " + nullable_id_string(invoice->payment_id) + " WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when updating invoice with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

invoice_id Database::add_invoice(Invoice *invoice) {
	db_result_code = DATABASE_OK;
	invoice_id id = 0;
	sqlite3_stmt *statement = prepare_statement("INSERT INTO invoices (cid, date, payid) VALUES (" + std::to_string(invoice->customer_id) + ", '" + invoice->date.to_string() + "', " + nullable_id_string(invoice->payment_id) + ") RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when adding invoice with customer id '" + std::to_string(invoice->customer_id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			id = sqlite3_column_int(statement, 0);
			break;
		case SQLITE_DONE:
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
	return id;
}

void Database::unlink_invoice_orders(invoice_id id) {
	sqlite3_stmt *statement = prepare_statement("DELETE FROM invoice_orders WHERE iid = " + std::to_string(id) + ";");
	int res = sqlite3_step(statement);
	switch (res) {
		case SQLITE_ROW:
		case SQLITE_DONE:
			break;
		default:
			throw std::runtime_error("! MAJOR ERROR ! Error when deleting linked invoice orders with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n");
	}
	sqlite3_finalize(statement);
}

void Database::delete_invoice(Invoice *invoice) {
	db_result_code = DATABASE_OK;
	invoice_id id = invoice->id;
	unlink_invoice_orders(id);

	sqlite3_stmt *statement = prepare_statement("DELETE FROM invoices WHERE id = " + std::to_string(id) + " RETURNING id;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when deleting invoice with id '" + std::to_string(id) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);

	payment_id payid = invoice->payment_id; // Should probably be done with a join between invoices an payments but I'm short on time. --Orson
	if (payid != 0) {
		try {
			delete_payment(payid);
		} catch (std::runtime_error e) {
			throw std::runtime_error(std::string("! MAJOR ERROR ! Error when deleting invoice payment: ") + e.what());
		}
		switch (db_result_code) {
			case DATABASE_NONE:
				throw std::runtime_error("Error when deleting invoice payment, exists in object but not in database.");
			case DATABASE_ERROR:
				throw std::runtime_error("! MAJOR ERROR ! Error when deleting invoice payment.");
			case DATABASE_OTHER:
				throw std::runtime_error("! MAJOR ERROR ! Error when deleting invoice payment, FK error?");
		}
	}
}

void Database::link_invoice_order(invoice_id iid, order_id oid) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("INSERT INTO invoice_orders (iid, oid) VALUES (" + std::to_string(iid) + ", " + std::to_string(oid) + ");");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when linking invoice order with invoice id '" + std::to_string(iid) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_DONE:
			break;
		case SQLITE_ROW:
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}

void Database::unlink_invoice_order(invoice_id iid, order_id oid) {
	db_result_code = DATABASE_OK;
	sqlite3_stmt *statement = prepare_statement("DELETE FROM invoice_orders WHERE iid = " + std::to_string(iid) + " AND oid = " + std::to_string(oid) + " RETURNING iid;");
	int res = sqlite3_step(statement);
	if (is_unrecoverable_result(res)) { throw std::runtime_error("Error when unlinking invoice order with invoice id '" + std::to_string(iid) + "', result code " + std::to_string(res) + ": " + sqlite3_errmsg(db) + "\n"); }
	switch (res) {
		case SQLITE_ROW:
			break;
		case SQLITE_DONE:
			db_result_code = DATABASE_NONE;
			break;
		default:
			db_result_code = DATABASE_ERROR;
			break;
	}
	sqlite3_finalize(statement);
}