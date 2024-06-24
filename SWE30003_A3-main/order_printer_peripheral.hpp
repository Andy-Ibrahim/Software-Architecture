#ifndef _103575527SWE300003A3_OPPERIPHERAL
#define _103575527SWE300003A3_OPPERIPHERAL 1

#include "order_handler.hpp"

class OrderPrinterPeripheral {
	public:
		static void print_order(Order *order);
	private:
		OrderPrinterPeripheral() {}
};

#endif