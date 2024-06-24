#include "order_printer_peripheral.hpp"
#include "util.hpp"
#include <iostream>

void OrderPrinterPeripheral::print_order(Order *order) {
	std::cout << "Printing order to kitchen..." << std::endl;

	OrderHandler *order_handler = OrderHandler::get_instance();

	std::cout
		<< std::endl << std::endl
		<< "Order ID: " << order->id << "\n"
		<< "Customer ID: " << order->customer_id << "\n"
		<< "Date: " << order->date.to_string() << "\n"
		<< "Type: " << ORDER_TYPE_NAMES[order->order_type] << "\n"
		<< "Table ID: " << order->table_id << "\n"
		<< std::endl << std::endl;

	int total = 0;
	for (std::pair<int, OrderStats> line : order->items) {
		Product* prod = order_handler->get_product(line.first);
		int linePrice = line.second.get_price();
		total += linePrice;
		std::cout
			<< prod->id << " " << prod->name
			<< " x " << line.second.count
			<< " " << format_price(linePrice)
			<< "(" << format_price(prod->price) << "ea)" << std::endl;
	}

	std::cout << "Total: " << format_price(total) << std::endl << std::endl;
}