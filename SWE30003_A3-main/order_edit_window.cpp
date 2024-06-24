#include "order_edit_window.hpp"
#include "order_item_create_window.hpp"
#include "table_search_window.hpp"
#include "customer_handler.hpp"
#include "booking_handler.hpp"
#include "util.hpp"
#include <string>

OrderEditWindow *OrderEditWindow::instance = new OrderEditWindow();

void OrderEditWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->order = mp_get<Order>(create_cb_data, 1);
	GCallback callback = G_CALLBACK(mp_get<void>(create_cb_data, 2));

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Edit Order");
	gtk_window_set_default_size(window, 800, 600);
	g_signal_connect_swapped(
		window,
		"unrealize",
		callback,
		NULL
	);

    instance->populate();
}

void OrderEditWindow::populate() {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	CustomerHandler *customer_handler = CustomerHandler::get_instance();
	BookingHandler *booking_handler = BookingHandler::get_instance();

	Customer *customer = customer_handler->get_customer(order->customer_id);
	Table *table = booking_handler->get_table(order->table_id);
	std::string order_text =
		order->date.to_string() + ", " +
		customer->name + ", " + table->name + ", " +
		ORDER_TYPE_NAMES[order->order_type] + ", " +
		format_price(order->get_total_price()) + ", " +
		(order->delivered ? "Delivered" : "Not Delivered");
	gtk_box_append(outer_box, gtk_label_new(order_text.c_str()));

	GtkWidget *create_item_button = gtk_button_new_with_label("Add Item");
	gtk_box_append(outer_box, create_item_button);
	multi_ptr create_cb_data = mp_new({window, order, (void*) &repopulate});
	g_signal_connect_swapped(
		create_item_button,
		"clicked",
		G_CALLBACK(OrderItemCreateWindow::create),
		create_cb_data
	);
	g_signal_connect_swapped(create_item_button, "unrealize", G_CALLBACK(free), create_cb_data);

	gtk_box_append(outer_box, gtk_label_new("Order Items:"));

	OrderHandler *order_handler = OrderHandler::get_instance();
	for (auto it = order->items.begin(); it != order->items.end(); ++it) {
		Product *product = order_handler->get_product(it->first);
		OrderStats stats = it->second;
		GtkBox *item_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
		gtk_box_append(outer_box, GTK_WIDGET(item_box));
		gtk_box_append(item_box, gtk_label_new((
			product->name + " (" +
			std::to_string(stats.count) + "), " +
			format_price(stats.get_price())
		).c_str()));
		GtkWidget *delete_item_button = gtk_button_new_with_label("Delete");
		gtk_box_append(item_box, delete_item_button);
		g_signal_connect_swapped(
			delete_item_button,
			"clicked",
			G_CALLBACK(delete_item),
			product
		);
	}

	gtk_window_present(GTK_WINDOW(window));
}

void OrderEditWindow::delete_item(Product *product) {
	OrderHandler::get_instance()->delete_order_item(instance->order->id, product->id);
	repopulate();
}

void OrderEditWindow::repopulate() {
	instance->clear();
	instance->populate();
}