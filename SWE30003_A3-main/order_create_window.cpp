#include "order_create_window.hpp"
#include "customer_search_window.hpp"
#include "table_search_window.hpp"
#include "order_window.hpp"
#include <string>

OrderCreateWindow *OrderCreateWindow::instance = new OrderCreateWindow();

void OrderCreateWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->callback = (void(*)(Order*)) mp_get<void>(create_cb_data, 1);

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Create Order");
	gtk_window_set_default_size(window, 800, 600);

	instance->customer = nullptr;
	instance->table = nullptr;
	instance->type = ORDER_DINE_IN;

    instance->populate(Date());
}

void OrderCreateWindow::populate(Date date) {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	// Date selelection.
	GtkBox *date_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(date_box));
	date_widget = new DateWidget(date_box, date);

	// Customer selection.
	GtkBox *customer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(customer_box));
	gtk_box_append(customer_box, gtk_label_new(customer == nullptr ? "-" : (customer->name + ", " + customer->phone_number).c_str()));
	GtkWidget *select_customer_button = gtk_button_new_with_label("Select Customer");
	gtk_box_append(customer_box, select_customer_button);
	multi_ptr create_cb_data = mp_new({window, (void*) &set_customer});
	g_signal_connect_swapped(
		select_customer_button,
		"clicked",
		G_CALLBACK(CustomerSearchWindow::create),
		create_cb_data
	);
	g_signal_connect_swapped(select_customer_button, "unrealize", G_CALLBACK(free), create_cb_data);

	// Table selection.
	GtkBox *table_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(table_box));
	gtk_box_append(table_box, gtk_label_new(table == nullptr ? "-" : (table->name + ": " + std::to_string(table->size)).c_str()));
	GtkWidget *select_table_button = gtk_button_new_with_label("Select Table");
	gtk_box_append(table_box, select_table_button);
	create_cb_data = mp_new({window, (void*) &set_table});
	g_signal_connect_swapped(
		select_table_button,
		"clicked",
		G_CALLBACK(TableSearchWindow::create),
		create_cb_data
	);
	g_signal_connect_swapped(select_table_button, "unrealize", G_CALLBACK(free), create_cb_data);

	// Order type selection.
	GtkBox *type_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(type_box));
	gtk_box_append(type_box, gtk_label_new(ORDER_TYPE_NAMES[type]));
	size_t i = 0;
	for (size_t i = 0; i < ORDER_TYPE_COUNT; ++i) {
		GtkWidget *type_button = gtk_button_new_with_label(ORDER_TYPE_NAMES[i]);
		gtk_box_append(type_box, type_button);
		g_signal_connect_swapped(
			type_button,
			"clicked",
			G_CALLBACK(set_order_type),
			(gpointer) i
		);
	}

	// Confirm button.
	GtkWidget *create_button = gtk_button_new_with_label("Create");
	gtk_box_append(outer_box, create_button);
	g_signal_connect(
		create_button,
		"clicked",
		G_CALLBACK(create_order),
		NULL
	);

	gtk_window_present(GTK_WINDOW(window));
}

void OrderCreateWindow::set_customer(Customer *customer) {
	instance->customer = customer;
	instance->repopulate();
}

void OrderCreateWindow::set_table(Table *table) {
	instance->table = table;
	instance->repopulate();
}

void OrderCreateWindow::set_order_type(order_type type) {
	instance->type = type;
	instance->repopulate();
}

void OrderCreateWindow::create_order() {
	Customer *customer = instance->customer;
	Table *table = instance->table;
	if (!customer || !table) return;
	Date date = instance->date_widget->get_date();

	// Create new order.
	Order* new_order = new Order(customer->id, table->id, instance->type, date);
	OrderHandler::get_instance()->add_order(new_order);
	gtk_window_close(instance->window);
	instance->callback(new_order);
}

void OrderCreateWindow::repopulate() {
	Date date = date_widget->get_date();
	clear();
	populate(date);
}

void OrderCreateWindow::clear() {
	delete date_widget;
	gtk_widget_unrealize(GTK_WIDGET(outer_box));
}