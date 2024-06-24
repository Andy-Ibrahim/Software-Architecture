#include "customer_search_window.hpp"

CustomerSearchWindow *CustomerSearchWindow::instance = new CustomerSearchWindow();

void CustomerSearchWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->callback = (void(*)(Customer*)) mp_get<void>(create_cb_data, 1);

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Search Customers");
	gtk_window_set_default_size(window, 800, 600);

    instance->populate("");
}

void CustomerSearchWindow::populate(std::string search) {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	GtkBox* search_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(search_box));

	GtkEntryBuffer* search_buffer = gtk_entry_buffer_new(
		search.c_str(),
		search.length()
	);
	GtkWidget* name = gtk_entry_new_with_buffer(search_buffer);
	gtk_box_append(search_box, name);

	GtkWidget* search_button = gtk_button_new_with_label("Search");
	gtk_box_append(search_box, search_button);
	g_signal_connect_swapped(
		search_button,
		"clicked",
		G_CALLBACK(update),
		search_buffer
	);

	CustomerHandler* customer_handler = CustomerHandler::get_instance();
	std::vector<Customer*> customer_list = customer_handler->get_name_customers(search);
	for (Customer* customer : customer_list) {
		// Inner Box
		GtkBox* inner_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
		gtk_box_append(outer_box, GTK_WIDGET(inner_box));
		// Update Button
		GtkWidget* button = gtk_button_new_with_label((customer->name + ", " + customer->phone_number).c_str());
		gtk_box_append(inner_box, button);
		g_signal_connect_swapped(
			button,
			"clicked",
			G_CALLBACK(select),
			customer
		);
	}

	gtk_window_present(GTK_WINDOW(window));
}

void CustomerSearchWindow::update(GtkEntryBuffer *buffer) {
	instance->clear();
	instance->populate(gtk_entry_buffer_get_text(buffer));
}

void CustomerSearchWindow::select(Customer *customer) {
	gtk_window_close(instance->window);
	instance->callback(customer);
}