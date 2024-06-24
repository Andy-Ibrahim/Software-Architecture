#ifndef _103575527SWE300003A3_PAYMENT
#define _103575527SWE300003A3_PAYMENT 1

#include "util.hpp"
#include <string>

typedef int payment_id;
typedef char payment_type;

#define PAYMENT_CASH '\0'
#define PAYMENT_CARD '\1'

static const size_t PAYMENT_TYPE_COUNT = 2;
static const char *const PAYMENT_TYPE_NAMES[PAYMENT_TYPE_COUNT] = {
	"Cash",
	"Card"
};

class Payment {
	public:
		payment_id id;
		int amount;
		payment_type payment_type;
		std::string card_number;
		std::string transaction_id;

		Payment(payment_id id, int amount, ::payment_type payment_type, std::string card_number, std::string transaction_id);
		Payment(int amount, ::payment_type payment_type, std::string card_number, std::string transaction_id);
		std::string to_string();
};

#endif