#include "booking_handler.hpp"
#include "database.hpp"
#include <stdexcept>
#include <vector>
#include <list>

BookingHandler *BookingHandler::instance = nullptr;

const int MAX_ERRORS = 10;

BookingHandler::BookingHandler() {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		std::vector<Table*> table_list = instance->get_tables();
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
			 	// Resize first to prevent repeated resizing during loop.
				tables.reserve(table_list.size());
				for (std::size_t i = 0; i < table_list.size(); ++i) {
					Table *table = table_list[i];
					tables.emplace(table->id, table);
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

std::vector<Table*> BookingHandler::get_tables() {
	std::vector<Table*> result;
	for (auto it = tables.begin(); it != tables.end(); ++it) {
		Table *table = it->second;
		if (!table->deleted) { result.push_back(table); }
	}
	return result;
}

BookingHandler *BookingHandler::get_instance() {
	return instance ? instance : instance = new BookingHandler();
}

Table *BookingHandler::get_table(table_id id) {
	return tables.contains(id) ? tables.at(id) : nullptr;
}

Booking *BookingHandler::get_booking(booking_id id) {
	if (loaded_bookings.contains(id)) return loaded_bookings.at(id);
	Database *instance = Database::get_instance();
	Booking *booking = nullptr;
	int err_count = 0;
	while (true) {
		booking = instance->get_booking(id);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				if (booking) {
					loaded_bookings.emplace(id, booking);
					return booking;
				}
				throw std::runtime_error("Failed retrieving booking with id '" + std::to_string(id) + "', database gave OK result but didn't return a booking.");
			case DATABASE_NONE:
				return nullptr;
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed retrieving booking with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

std::vector<booking_id> BookingHandler::get_date_booking_ids(Date date) {
	Database *instance = Database::get_instance();
	std::vector<booking_id> id_list;
	int err_count = 0;
	while (true) {
		id_list = instance->get_date_booking_ids(date);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				return id_list;
			case DATABASE_NONE:
				throw std::runtime_error("Failed retrieving booking ids from date '" + date.to_string() + "', database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed retrieving booking ids from date '" + date.to_string() + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

std::vector<Booking*> BookingHandler::get_date_bookings(Date date) {
	Database *instance = Database::get_instance();
	std::vector<booking_id> id_list = get_date_booking_ids(date);
	std::vector<Booking*> booking_list;
	for (std::size_t i = 0; i < id_list.size(); ++i) {
		booking_id id = id_list[i];
		Booking *booking = get_booking(id);
		if (booking == nullptr) {
			throw std::runtime_error("Failed retrieving bookings from date '" + date.to_string() + "', database gave id '" + std::to_string(id) + "', could not retrieve from database.");
		}
		booking_list.push_back(booking);
	}
	return booking_list;
}

bool BookingHandler::update_booking(booking_id id) {
	if (!loaded_bookings.contains(id)) return false;
	Database *instance = Database::get_instance();
	Booking *booking = loaded_bookings.at(id);
	int err_count = 0;
	while (true) {
		instance->update_booking(booking);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed updating booking with id '" + std::to_string(id) + "', exists in buffer but not in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed updating booking with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

void BookingHandler::add_booking(Booking *booking) {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		booking_id id = instance->add_booking(booking);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				booking->id = id;
				loaded_bookings.emplace(id, booking);
				return;
			case DATABASE_NONE:
				throw std::runtime_error("Failed creating booking with customer id '" + std::to_string(booking->customer_id) + "', database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed creating booking with customer id '" + std::to_string(booking->customer_id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool BookingHandler::delete_booking(booking_id id) {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		instance->delete_booking(id);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				if (loaded_bookings.contains(id)) {
					Booking *booking = loaded_bookings.at(id);
					loaded_bookings.erase(id);
					delete booking;
				}
				return true;
			case DATABASE_NONE:
				return false;
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed deleting booking with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}