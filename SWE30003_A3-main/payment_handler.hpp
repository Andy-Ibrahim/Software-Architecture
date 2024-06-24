#ifndef _103575527SWE300003A3_PHANDLER
#define _103575527SWE300003A3_PHANDLER 1

#include "payment.hpp"
#include "invoice.hpp"
#include "order.hpp"
#include <unordered_map>
#include <vector>

class PaymentHandler {
	private:
		static PaymentHandler *instance;
		std::unordered_map<payment_id, Payment*> loaded_payments;
		std::unordered_map<invoice_id, Invoice*> loaded_invoices;

		PaymentHandler() {}
		std::vector<invoice_id> get_date_invoice_ids(Date date);
	public:
		PaymentHandler(PaymentHandler const&) = delete;
        void operator=(PaymentHandler const&) = delete;
        PaymentHandler(PaymentHandler&&) = delete;
        void operator=(PaymentHandler&&) = delete;

		static PaymentHandler *get_instance();

		Payment *get_payment(payment_id id);
		bool update_payment(payment_id id);
		void add_payment(Payment *payment);
		bool delete_payment(payment_id id);

		Invoice *get_invoice(invoice_id id);
		std::vector<Invoice*> get_date_invoices(Date date);
		bool update_invoice(invoice_id id);
		void add_invoice(Invoice *invoice);
		bool delete_invoice(invoice_id id);
		
		bool link_invoice_order(invoice_id iid, order_id oid);
		bool unlink_invoice_order(invoice_id iid, order_id oid);

		int get_invoice_total_price(Invoice *invoice);
};

#endif