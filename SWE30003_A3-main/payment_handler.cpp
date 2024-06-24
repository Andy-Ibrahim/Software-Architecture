#include "payment_handler.hpp"
#include "database.hpp"
#include "order_handler.hpp"
#include <algorithm>

const int MAX_ERRORS = 10;

PaymentHandler *PaymentHandler::instance = nullptr;

PaymentHandler *PaymentHandler::get_instance() {
	return instance ? instance : instance = new PaymentHandler();
}



Payment *PaymentHandler::get_payment(payment_id id) {
	if (loaded_payments.contains(id)) return loaded_payments.at(id);
	Database *instance = Database::get_instance();
	Payment *payment = nullptr;
	int err_count = 0;
	while (true) {
		payment = instance->get_payment(id);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				if (payment) {
					loaded_payments.emplace(id, payment);
					return payment;
				}
				throw std::runtime_error("Failed retrieving payment with id '" + std::to_string(id) + "', database gave OK result but didn't return a payment.");
			case DATABASE_NONE:
				return nullptr;
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed retrieving payment with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool PaymentHandler::update_payment(payment_id id) {
	if (!loaded_payments.contains(id)) return false;
	Database *instance = Database::get_instance();
	Payment *payment = loaded_payments.at(id);
	int err_count = 0;
	while (true) {
		instance->update_payment(payment);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed updating payment with id '" + std::to_string(id) + "', exists in buffer but not in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed updating payment with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

void PaymentHandler::add_payment(Payment *payment) {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		payment_id id = instance->add_payment(payment);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				payment->id = id;
				loaded_payments.emplace(id, payment);
				return;
			case DATABASE_NONE:
				throw std::runtime_error("Failed creating order with transaction id '" + payment->transaction_id + "', database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed creating order with transaction id '" + payment->transaction_id + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool PaymentHandler::delete_payment(payment_id id) {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		instance->delete_payment(id);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				if (loaded_payments.contains(id)) {
					Payment *payment = loaded_payments.at(id);
					loaded_payments.erase(id);
					delete payment;
				}
				return true;
			case DATABASE_OTHER:
			case DATABASE_NONE:
				return false;
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed deleting payment with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}



Invoice *PaymentHandler::get_invoice(invoice_id id) {
	if (loaded_invoices.contains(id)) return loaded_invoices.at(id);
	Database *instance = Database::get_instance();
	Invoice *invoice = nullptr;
	int err_count = 0;
	while (true) {
		invoice = instance->get_invoice(id);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				if (invoice) {
					loaded_invoices.emplace(id, invoice);
					return invoice;
				}
				throw std::runtime_error("Failed retrieving invoice with id '" + std::to_string(id) + "', database gave OK result but didn't return an invoice.");
			case DATABASE_NONE:
				return nullptr;
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed retrieving invoice with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

std::vector<invoice_id> PaymentHandler::get_date_invoice_ids(Date date) {
	Database *instance = Database::get_instance();
	std::vector<invoice_id> id_list;
	int err_count = 0;
	while (true) {
		id_list = instance->get_date_invoice_ids(date);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				return id_list;
			case DATABASE_NONE:
				throw std::runtime_error("Failed retrieving invoice ids from date '" + date.to_string() + "', database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed retrieving invoice ids from date '" + date.to_string() + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

std::vector<Invoice*> PaymentHandler::get_date_invoices(Date date) {
	Database *instance = Database::get_instance();
	std::vector<invoice_id> id_list = get_date_invoice_ids(date);
	std::vector<Invoice*> invoice_list;
	for (std::size_t i = 0; i < id_list.size(); ++i) {
		invoice_id id = id_list[i];
		Invoice *invoice = get_invoice(id);
		if (invoice == nullptr) {
			throw std::runtime_error("Failed retrieving invoices from date '" + date.to_string() + "', database gave id '" + std::to_string(id) + "', could not retrieve from database.");
		}
		invoice_list.push_back(invoice);
	}
	return invoice_list;
}

bool PaymentHandler::update_invoice(invoice_id id) {
	if (!loaded_invoices.contains(id)) return false;
	Database *instance = Database::get_instance();
	Invoice *invoice = loaded_invoices.at(id);
	int err_count = 0;
	while (true) {
		instance->update_invoice(invoice);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed updating invoice with id '" + std::to_string(id) + "', exists in buffer but not in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed updating invoice with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

void PaymentHandler::add_invoice(Invoice *invoice) {
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		payment_id id = instance->add_invoice(invoice);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				invoice->id = id;
				loaded_invoices.emplace(id, invoice);
				return;
			case DATABASE_NONE:
				throw std::runtime_error("Failed creating order with customer id '" + std::to_string(invoice->customer_id) + "', database gave NONE result?");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed creating order with customer id '" + std::to_string(invoice->customer_id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool PaymentHandler::delete_invoice(invoice_id id) {
	if (!loaded_invoices.contains(id)) return false;
	Database *instance = Database::get_instance();
	Invoice *invoice = loaded_invoices.at(id);
	int err_count = 0;
	while (true) {
		instance->delete_invoice(invoice);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				loaded_invoices.erase(id);

				{ payment_id payid = invoice->payment_id;
				if (loaded_payments.contains(payid)) {
					Payment *payment = loaded_payments.at(payid);
					loaded_payments.erase(payid);
					delete payment;
				} }

				delete invoice;
				return true;
			case DATABASE_OTHER:
			case DATABASE_NONE:
				return false;
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed deleting invoice with id '" + std::to_string(id) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}



bool PaymentHandler::link_invoice_order(invoice_id iid, order_id oid) {
	if (!loaded_invoices.contains(iid)) { return false; }
	Invoice *invoice = loaded_invoices.at(iid);
	std::vector<order_id> &linked_orders = invoice->linked_orders;
	if (std::find(linked_orders.begin(), linked_orders.end(), oid) != linked_orders.end()) { return false; }
	if (OrderHandler::get_instance()->get_order(oid) == nullptr) { return false; }
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		instance->link_invoice_order(iid, oid);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				linked_orders.push_back(oid);
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed linking invoice order with invoice id '" + std::to_string(iid) + "', doesn't exist in buffer but does in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed linking invoice order with invoice id '" + std::to_string(iid) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}

bool PaymentHandler::unlink_invoice_order(invoice_id iid, order_id oid) {
	if (!loaded_invoices.contains(iid)) return false;
	Invoice *invoice = loaded_invoices.at(iid);
	std::vector<order_id> &linked_orders = invoice->linked_orders;
	auto it = std::find(linked_orders.begin(), linked_orders.end(), oid);
	if (it == linked_orders.end()) { return false; }
	Database *instance = Database::get_instance();
	int err_count = 0;
	while (true) {
		instance->unlink_invoice_order(iid, oid);
		int res = instance->get_result_code();
		switch (res) {
			case DATABASE_OK:
				linked_orders.erase(it);
				return true;
			case DATABASE_NONE:
				throw std::runtime_error("Failed unlinking invoice order with invoice id '" + std::to_string(iid) + "', exists in buffer but not in database.");
			case DATABASE_ERROR:
			default:
				err_count += 1;
				if (err_count >= 10) {
					throw std::runtime_error("Failed unlinking invoice order with invoice id '" + std::to_string(iid) + "' after trying " + std::to_string(MAX_ERRORS) + " times.");
				}
		}
	}
}



int PaymentHandler::get_invoice_total_price(Invoice *invoice) {
	std::vector<order_id> &linked_orders = invoice->linked_orders;
	OrderHandler *order_handler = OrderHandler::get_instance();
	int total = 0;
	for (size_t i = 0; i < linked_orders.size(); ++i) {
		total += order_handler->get_order(linked_orders[i])->get_total_price();
	}
	return total;
}