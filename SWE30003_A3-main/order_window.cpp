#include "order_window.hpp"
#include "order_create_window.hpp"
#include "order_edit_window.hpp"
#include "order_printer_peripheral.hpp"
#include "customer_handler.hpp"
#include "booking_handler.hpp"
#include "util.hpp"

OrderWindow *OrderWindow::instance = new OrderWindow();

void OrderWindow::create(GtkWindow *parent) {
	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Orders");
	gtk_window_set_default_size(window, 800, 600);

    instance->populate(Date());
}

void OrderWindow::populate(Date date) {
    outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	GtkBox* date_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(date_box));

	date_widget = new DateWidget(date_box, date, G_CALLBACK(repopulate));

	GtkWidget* create_button = gtk_button_new_with_label("Create Order");
	gtk_box_append(outer_box, create_button);
	multi_ptr create_cb_data = mp_new({window, (void*) &edit_created_order});
	g_signal_connect_swapped(
		create_button,
		"clicked",
		G_CALLBACK(OrderCreateWindow::create),
		create_cb_data
	);
	g_signal_connect_swapped(create_button, "unrealize", G_CALLBACK(free), create_cb_data);

	gtk_box_append(outer_box, gtk_label_new("Orders:"));

	CustomerHandler *customer_handler = CustomerHandler::get_instance();
	BookingHandler *booking_handler = BookingHandler::get_instance();

	OrderHandler *order_handler = OrderHandler::get_instance();
	std::vector<Order*> orders = order_handler->get_date_orders(date);
	for (Order* order : orders) {
		GtkBox* order_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
		gtk_box_append(outer_box, GTK_WIDGET(order_box));

		Customer *customer = customer_handler->get_customer(order->customer_id);
		Table *table = booking_handler->get_table(order->table_id);
		std::string order_text =
			order->date.to_string() + ", " +
			customer->name + ", " + table->name + ", " +
			ORDER_TYPE_NAMES[order->order_type] + ", " +
			format_price(order->get_total_price());
		gtk_box_append(order_box, gtk_label_new(order_text.c_str()));

		GtkWidget* edit_button = gtk_button_new_with_label("Edit");
		gtk_box_append(order_box, edit_button);
		create_cb_data = mp_new({window, order, (void*) &repopulate});
		g_signal_connect_swapped(
			edit_button,
			"clicked",
			G_CALLBACK(OrderEditWindow::create),
			create_cb_data
		);
		g_signal_connect_swapped(edit_button, "unrealize", G_CALLBACK(free), create_cb_data);

		GtkWidget* delivered_button = gtk_button_new_with_label(order->delivered ? "Set Not Delivered" : "Set Delivered");
		gtk_box_append(order_box, delivered_button);
		g_signal_connect_swapped(
			delivered_button,
			"clicked",
			G_CALLBACK(flip_order_delivered),
			order
		);

		GtkWidget* print_button = gtk_button_new_with_label("Print");
		gtk_box_append(order_box, print_button);
		g_signal_connect_swapped(
			print_button,
			"clicked",
			G_CALLBACK(OrderPrinterPeripheral::print_order),
			order
		);

		GtkWidget* delete_button = gtk_button_new_with_label("Delete");
		gtk_box_append(order_box, delete_button);
		g_signal_connect_swapped(
			delete_button,
			"clicked",
			G_CALLBACK(delete_order),
			order
		);
	}

	gtk_window_present(GTK_WINDOW(window));
}

void OrderWindow::edit_created_order(Order* new_order) {
	multi_ptr cb_data = mp_new({instance->window, new_order, (void*) &repopulate});
	OrderEditWindow::create(cb_data);
	free(cb_data);
}

void OrderWindow::delete_order(Order *order) {
	OrderHandler::get_instance()->delete_order(order->id);
	repopulate();
}

void OrderWindow::flip_order_delivered(Order *order) {
	order->delivered = !order->delivered;
	OrderHandler* orderHandler = OrderHandler::get_instance();
	orderHandler->update_order(order->id);
	repopulate();
}

void OrderWindow::repopulate() {
	Date date = instance->date_widget->get_date();
	instance->clear();
	instance->populate(date);
}

void OrderWindow::clear() {
	delete date_widget;
	gtk_widget_unrealize(GTK_WIDGET(outer_box));
}
