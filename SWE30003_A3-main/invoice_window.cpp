#include "invoice_window.hpp"
#include "invoice_create_window.hpp"
#include "invoice_edit_window.hpp"
#include "invoice_printer_peripheral.hpp"
#include "customer_handler.hpp"
#include "util.hpp"

InvoiceWindow *InvoiceWindow::instance = new InvoiceWindow();

void InvoiceWindow::create(GtkWindow *parent) {
	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Invoices");
	gtk_window_set_default_size(window, 800, 600);

    instance->populate(Date());
}

void InvoiceWindow::populate(Date date) {
    outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	GtkBox* date_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(date_box));

	date_widget = new DateWidget(date_box, date, G_CALLBACK(repopulate));

	GtkWidget* create_button = gtk_button_new_with_label("Create Invoice");
	gtk_box_append(outer_box, create_button);
	multi_ptr create_cb_data = mp_new({window, (void*) &edit_created_invoice});
	g_signal_connect_swapped(
		create_button,
		"clicked",
		G_CALLBACK(InvoiceCreateWindow::create),
		create_cb_data
	);
	g_signal_connect_swapped(create_button, "unrealize", G_CALLBACK(free), create_cb_data);

	gtk_box_append(outer_box, gtk_label_new("Invoices:"));

	CustomerHandler *customer_handler = CustomerHandler::get_instance();

	PaymentHandler *payment_handler = PaymentHandler::get_instance();
	std::vector<Invoice*> invoices = payment_handler->get_date_invoices(date);
	for (Invoice* invoice : invoices) {
		GtkBox* order_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
		gtk_box_append(outer_box, GTK_WIDGET(order_box));

		Customer *customer = customer_handler->get_customer(invoice->customer_id);
		std::string payment_string = invoice->payment_id == 0 ? "Not Paid" : PAYMENT_TYPE_NAMES[payment_handler->get_payment(invoice->payment_id)->payment_type];
		std::string invoice_text =
			invoice->date.to_string() + ", " +
			customer->name + ", " +
			format_price(payment_handler->get_invoice_total_price(invoice)) + ", " +
			payment_string;
		gtk_box_append(order_box, gtk_label_new(invoice_text.c_str()));

		GtkWidget* edit_button = gtk_button_new_with_label("Edit");
		gtk_box_append(order_box, edit_button);
		create_cb_data = mp_new({window, invoice, (void*) &repopulate});
		g_signal_connect_swapped(
			edit_button,
			"clicked",
			G_CALLBACK(InvoiceEditWindow::create),
			create_cb_data
		);
		g_signal_connect_swapped(edit_button, "unrealize", G_CALLBACK(free), create_cb_data);

		GtkWidget* print_button = gtk_button_new_with_label("Print");
		gtk_box_append(order_box, print_button);
		g_signal_connect_swapped(
			print_button,
			"clicked",
			G_CALLBACK(InvoicePrinterPeripheral::print_invoice),
			invoice
		);

		GtkWidget* delete_button = gtk_button_new_with_label("Delete");
		gtk_box_append(order_box, delete_button);
		g_signal_connect_swapped(
			delete_button,
			"clicked",
			G_CALLBACK(delete_invoice),
			invoice
		);
	}

	gtk_window_present(GTK_WINDOW(window));
}

void InvoiceWindow::delete_invoice(Invoice *invoice) {
	PaymentHandler::get_instance()->delete_invoice(invoice->id);
	repopulate();
}

void InvoiceWindow::print_invoice(Invoice *invoice) {
	
}

void InvoiceWindow::edit_created_invoice(Invoice *new_invoice) {
	multi_ptr cb_data = mp_new({instance->window, new_invoice, (void*) &repopulate});
	InvoiceEditWindow::create(cb_data);
	free(cb_data);
}

void InvoiceWindow::repopulate() {
	Date date = instance->date_widget->get_date();
	instance->clear();
	instance->populate(date);
}

void InvoiceWindow::clear() {
	delete date_widget;
	gtk_widget_unrealize(GTK_WIDGET(outer_box));
}
