#include "invoice_create_window.hpp"
#include "customer_search_window.hpp"
#include <string>

InvoiceCreateWindow *InvoiceCreateWindow::instance = new InvoiceCreateWindow();

void InvoiceCreateWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->callback = (void(*)(Invoice*)) mp_get<void>(create_cb_data, 1);

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Create Invoice");
	gtk_window_set_default_size(window, 800, 600);

	instance->customer = nullptr;

    instance->populate(Date());
}

void InvoiceCreateWindow::populate(Date date) {
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

	// Confirm button.
	GtkWidget *create_button = gtk_button_new_with_label("Create");
	gtk_box_append(outer_box, create_button);
	g_signal_connect(
		create_button,
		"clicked",
		G_CALLBACK(create_invoice),
		NULL
	);

	gtk_window_present(GTK_WINDOW(window));
}

void InvoiceCreateWindow::set_customer(Customer *customer) {
	instance->customer = customer;
	instance->repopulate();
}

void InvoiceCreateWindow::create_invoice() {
	Customer *customer = instance->customer;
	if (customer == nullptr) { return; }
	Date date = instance->date_widget->get_date();

	Invoice *new_invoice = new Invoice(0, customer->id, date);
	PaymentHandler::get_instance()->add_invoice(new_invoice);

	gtk_window_close(instance->window);
	instance->callback(new_invoice);
}

void InvoiceCreateWindow::repopulate() {
	Date date = date_widget->get_date();
	clear();
	populate(date);
}

void InvoiceCreateWindow::clear() {
	delete date_widget;
	gtk_widget_unrealize(GTK_WIDGET(outer_box));
}