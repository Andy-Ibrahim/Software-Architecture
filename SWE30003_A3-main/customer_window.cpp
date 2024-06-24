#include "customer_window.hpp"
#include "popup_window.hpp"
#include <stdexcept>

CustomerWindow *CustomerWindow::instance = new CustomerWindow();

void CustomerWindow::create(GtkWindow *parent) {
	GtkWindow *window = GTK_WINDOW(gtk_window_new());
	instance->window = window;

	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Customers");
	gtk_window_set_default_size(window, 800, 600);

	instance->populate();
}

void CustomerWindow::update_customer(multi_ptr update_cb_data) {
	Customer *customer = mp_get<Customer>(update_cb_data, 0);
	GtkEntryBuffer *name_buffer = mp_get<GtkEntryBuffer>(update_cb_data, 1);
	GtkEntryBuffer *phone_number_buffer = mp_get<GtkEntryBuffer>(update_cb_data, 2);
	customer->name = gtk_entry_buffer_get_text(name_buffer);
	customer->phone_number = gtk_entry_buffer_get_text(phone_number_buffer);
	CustomerHandler::get_instance()->update_customer(customer->id);

	const char *name = "Confirmation";
	const char *message = "Updated Customer record.";
	multi_ptr create_cb_data = mp_new({instance->window, (void*) name, (void*) message});
	create_popup_window(create_cb_data);
	free(create_cb_data);
}

void CustomerWindow::delete_customer(Customer *customer) {
	CustomerHandler::get_instance()->delete_customer(customer->id);
	instance->clear();
	instance->populate();
}

void CustomerWindow::add_customer(multi_ptr add_cb_data) {
	GtkEntryBuffer *name_buffer = mp_get<GtkEntryBuffer>(add_cb_data, 0);
	GtkEntryBuffer *phone_number_buffer = mp_get<GtkEntryBuffer>(add_cb_data, 1);
	Customer *customer = new Customer(gtk_entry_buffer_get_text(name_buffer), gtk_entry_buffer_get_text(phone_number_buffer));
	CustomerHandler::get_instance()->add_customer(customer);
	instance->clear();
	instance->populate();
}

void CustomerWindow::populate() {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	CustomerHandler* customer_handler = CustomerHandler::get_instance();
	std::vector<Customer*> customer_list = customer_handler->get_name_customers("");
	for (Customer* customer : customer_list) {
		// Inner Box
		GtkBox* inner_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
		gtk_box_append(outer_box, GTK_WIDGET(inner_box));
		// Name
		GtkEntryBuffer* name_buffer = gtk_entry_buffer_new(
			customer->name.c_str(),
			customer->name.length()
		);
		GtkWidget* name = gtk_entry_new_with_buffer(name_buffer);
		gtk_box_append(inner_box, name);
		// Number
		GtkEntryBuffer* phone_number_buffer = gtk_entry_buffer_new(
			customer->phone_number.c_str(),
			customer->phone_number.length()
		);
		GtkWidget* number = gtk_entry_new_with_buffer(phone_number_buffer);
		gtk_box_append(inner_box, number);
		// Update Button
		GtkWidget* button = gtk_button_new_with_label("Update");
		gtk_box_append(inner_box, button);
		multi_ptr update_cb_data = mp_new({customer, name_buffer, phone_number_buffer});
		g_signal_connect_swapped(
			button,
			"clicked",
			G_CALLBACK(update_customer),
			update_cb_data
		);
		g_signal_connect_swapped(button, "unrealize", G_CALLBACK(free), update_cb_data);
		// Delete Button
		button = gtk_button_new_with_label("Delete");
		gtk_box_append(inner_box, button);
		g_signal_connect_swapped(
			button,
			"clicked",
			G_CALLBACK(delete_customer),
			customer
		);
	}
	// Add Box
	GtkBox* inner_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(inner_box));
	// Name
	GtkEntryBuffer* name_buffer = gtk_entry_buffer_new("", 0);
	GtkWidget* name = gtk_entry_new_with_buffer(name_buffer);
	gtk_box_append(inner_box, name);
	// Number
	GtkEntryBuffer* phone_number_buffer = gtk_entry_buffer_new("", 0);
	GtkWidget* number = gtk_entry_new_with_buffer(phone_number_buffer);
	gtk_box_append(inner_box, number);
	// Add Button
	GtkWidget* button = gtk_button_new_with_label("Add");
	gtk_box_append(inner_box, button);
	multi_ptr add_cb_data = mp_new({name_buffer, phone_number_buffer});
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(add_customer), add_cb_data);
	g_signal_connect_swapped(button, "unrealize", G_CALLBACK(free), add_cb_data);

	gtk_window_present(GTK_WINDOW(window));
}