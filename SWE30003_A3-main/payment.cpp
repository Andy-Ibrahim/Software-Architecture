#include "payment.hpp"

Payment::Payment(payment_id id, int amount, ::payment_type payment_type, std::string card_number, std::string transaction_id) {
	this->id = id;
	this->amount = amount;
	this->payment_type = payment_type;
	this->card_number = card_number;
	this->transaction_id = transaction_id;
}

Payment::Payment(int amount, ::payment_type payment_type, std::string card_number, std::string transaction_id) {
	this->id = 0;
	this->amount = amount;
	this->payment_type = payment_type;
	this->card_number = card_number;
	this->transaction_id = transaction_id;
}

std::string Payment::to_string() {
	std::string payment_text = 
		format_price(amount) + ", " +
		PAYMENT_TYPE_NAMES[payment_type];
	switch (payment_type) {
		case PAYMENT_CASH:
			break;
		case PAYMENT_CARD:
			payment_text += ", Card No.: " + card_number + ", Transaction ID: " + transaction_id;
			break;
	}
	return payment_text;
}