#ifndef _103575527SWE300003A3_INVOICE
#define _103575527SWE300003A3_INVOICE 1

#include "customer.hpp"
#include "date.hpp"
#include "payment.hpp"
#include "order.hpp"
#include <vector>

typedef int invoice_id;

class Invoice {
	public:
		invoice_id id;
		customer_id customer_id;
		Date date;
		payment_id payment_id;
		std::vector<order_id> linked_orders;

		Invoice(invoice_id id, ::customer_id customer_id, Date date, ::payment_id payment_id);
		Invoice(::payment_id payment_id, ::customer_id customer_id, Date date);
};

#endif