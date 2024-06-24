#ifndef _103575527SWE300003A3_IPPERIPHERAL
#define _103575527SWE300003A3_IPPERIPHERAL 1

#include "payment_handler.hpp"

class InvoicePrinterPeripheral {
	public:
		static void print_invoice(Invoice *invoice);
	private:
		InvoicePrinterPeripheral() {}
};

#endif