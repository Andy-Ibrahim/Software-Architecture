#ifndef _103575527SWE300003A3_CPPERIPHERAL
#define _103575527SWE300003A3_CPPERIPHERAL 1

#include <string>

class PaymentDetails {
	public:
		std::string card_number;
		std::string transaction_id;

		PaymentDetails(std::string card_number, std::string transaction_id);
};

class CardPaymentPeripheral {
	public:
		static PaymentDetails request_payment(int amount);
		static void cancel_previous_payment();
		static void cancel_payment(PaymentDetails details);
	private:
		CardPaymentPeripheral() {}
};

#endif