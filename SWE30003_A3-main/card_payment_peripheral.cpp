#include "card_payment_peripheral.hpp"
#include <iostream>

PaymentDetails::PaymentDetails(std::string card_number, std::string transaction_id) {
	this->card_number = card_number;
	this->transaction_id = transaction_id;
}



PaymentDetails CardPaymentPeripheral::request_payment(int amount) {
	std::cout << "Requested payment from peripheral." << std::endl;
	std::cout << "Recieved payment from peripheral." << std::endl;
	return PaymentDetails("0", "0");
}

void CardPaymentPeripheral::cancel_previous_payment() {
	std::cout << "Cancelled previous payment." << std::endl;
}

void CardPaymentPeripheral::cancel_payment(PaymentDetails details) {
	std::cout << "Cancelled payment with card number: " << details.card_number << std::endl;
}