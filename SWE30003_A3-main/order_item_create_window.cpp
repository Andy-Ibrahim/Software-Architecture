#include "order_item_create_window.hpp"
#include "product_search_window.hpp"
#include "popup_window.hpp"
#include "util.hpp"

OrderItemCreateWindow *OrderItemCreateWindow::instance = new OrderItemCreateWindow();

void OrderItemCreateWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->order = mp_get<Order>(create_cb_data, 1);
	instance->callback = (void(*)()) mp_get<void>(create_cb_data, 2);

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Add Order Item");
	gtk_window_set_default_size(window, 800, 600);

	instance->product = nullptr;

    instance->populate(1);
}

void OrderItemCreateWindow::populate(int count) {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	count_button = GTK_SPIN_BUTTON(gtk_spin_button_new(gtk_adjustment_new(count, 1, 999, 1, 0, 0), 0, 0));
	gtk_box_append(outer_box, GTK_WIDGET(count_button));

	GtkBox *product_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(product_box));
	gtk_box_append(product_box, gtk_label_new(product == nullptr ? "-" : (product->name + ", " + format_price(product->price)).c_str()));
	GtkWidget *select_product_button = gtk_button_new_with_label("Select Product");
	gtk_box_append(product_box, select_product_button);
	multi_ptr create_cb_data = mp_new({window, (void*) &set_product});
	g_signal_connect_swapped(
		select_product_button,
		"clicked",
		G_CALLBACK(ProductSearchWindow::create),
		create_cb_data
	);
	g_signal_connect_swapped(select_product_button, "unrealize", G_CALLBACK(free), create_cb_data);

	GtkWidget *create_button = gtk_button_new_with_label("Create");
	gtk_box_append(outer_box, create_button);
	g_signal_connect(
		create_button,
		"clicked",
		G_CALLBACK(create_order_item),
		NULL
	);
	gtk_window_present(GTK_WINDOW(window));
}

void OrderItemCreateWindow::set_product(Product *product) {
	instance->product = product;
	instance->repopulate();
}

void OrderItemCreateWindow::create_order_item() {
	Product *product = instance->product;
	if (product == nullptr) { return; }
	Order *order = instance->order;
	int count = gtk_spin_button_get_value_as_int(instance->count_button);

	// Item is already in order, update the total.
	OrderHandler* order_handler = OrderHandler::get_instance();
	bool result = order_handler->add_order_item(order->id, product->id, OrderStats(count));
	if (!result) {
		int current_count = order->items.at(product->id).count;
		result = order_handler->update_order_item(order->id, product->id, OrderStats(current_count + count));
	}

	instance->callback();
	gtk_window_close(instance->window);
}

void OrderItemCreateWindow::repopulate() {
	int count = gtk_spin_button_get_value_as_int(count_button);
	clear();
	populate(count);
}