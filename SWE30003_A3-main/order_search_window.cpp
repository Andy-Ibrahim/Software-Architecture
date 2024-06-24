#include "order_search_window.hpp"
#include "customer_handler.hpp"
#include "booking_handler.hpp"
#include "util.hpp"

OrderSearchWindow *OrderSearchWindow::instance = new OrderSearchWindow();

void OrderSearchWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->invoice = mp_get<Invoice>(create_cb_data, 1);
	instance->callback = (void(*)(Order*)) mp_get<void>(create_cb_data, 2);

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Search Orders");
	gtk_window_set_default_size(window, 800, 600);

    instance->populate(Date());
}

void OrderSearchWindow::populate(Date date) {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	GtkBox* date_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(date_box));
	date_widget = new DateWidget(date_box, date, G_CALLBACK(repopulate));

	OrderHandler* order_handler = OrderHandler::get_instance();
	std::vector<Order*> order_list = order_handler->get_date_orders(date);
	
	std::vector<order_id> &linked_orders = invoice->linked_orders;
	for (size_t i = 0; i < linked_orders.size(); ++i) {
		order_id order_id = linked_orders[i];
		for (auto it = order_list.begin(); it != order_list.end(); ++it) {
			if ((*it)->id == order_id) {
				order_list.erase(it);
				break;
			}
		}
	}

	gtk_box_append(outer_box, gtk_label_new("Orders:"));

	CustomerHandler *customer_handler = CustomerHandler::get_instance();
	BookingHandler *booking_handler = BookingHandler::get_instance();
	for (Order* order : order_list) {
		Customer *order_customer = customer_handler->get_customer(order->customer_id);
		Table *order_table = booking_handler->get_table(order->table_id);
		std::string order_text =
			order->date.to_string() + ", " +
			order_customer->name + ", " + order_table->name + ", " +
			ORDER_TYPE_NAMES[order->order_type] + ", " +
			format_price(order->get_total_price()) + ", " +
			(order->delivered ? "Delivered" : "Not Delivered");
		GtkWidget* button = gtk_button_new_with_label(order_text.c_str());
		gtk_box_append(outer_box, button);
		g_signal_connect_swapped(
			button,
			"clicked",
			G_CALLBACK(select),
			order
		);
	}

	gtk_window_present(GTK_WINDOW(window));
}

void OrderSearchWindow::select(Order *order) {
	gtk_window_close(instance->window);
	instance->callback(order);
}

void OrderSearchWindow::repopulate() {
	Date date = instance->date_widget->get_date();
	instance->clear();
	instance->populate(date);
}

void OrderSearchWindow::clear() {
	delete date_widget;
	gtk_widget_unrealize(GTK_WIDGET(outer_box));
}