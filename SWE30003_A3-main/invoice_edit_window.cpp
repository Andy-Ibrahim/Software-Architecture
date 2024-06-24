#include "invoice_edit_window.hpp"
#include "order_edit_window.hpp"
#include "order_search_window.hpp"
#include "payment_add_window.hpp"
#include "card_payment_peripheral.hpp"
#include "customer_handler.hpp"
#include "order_handler.hpp"
#include "booking_handler.hpp"
#include "util.hpp"
#include <string>

InvoiceEditWindow *InvoiceEditWindow::instance = new InvoiceEditWindow();

void InvoiceEditWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->invoice = mp_get<Invoice>(create_cb_data, 1);
	GCallback callback = G_CALLBACK(mp_get<void>(create_cb_data, 2));

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Edit Invoice");
	gtk_window_set_default_size(window, 800, 600);
	g_signal_connect_swapped(
		window,
		"unrealize",
		callback,
		NULL
	);

    instance->populate();
}

void InvoiceEditWindow::populate() {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	CustomerHandler *customer_handler = CustomerHandler::get_instance();
	PaymentHandler *payment_handler = PaymentHandler::get_instance();

	Customer *customer = customer_handler->get_customer(invoice->customer_id);
	gtk_box_append(outer_box, gtk_label_new((invoice->date.to_string() + ", " + customer->name).c_str()));

	int total_price = payment_handler->get_invoice_total_price(invoice);

	GtkBox *payment_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(payment_box));
	if (invoice->payment_id == 0) {
		gtk_box_append(payment_box, gtk_label_new("Not Paid"));

		GtkWidget *add_payment_button = gtk_button_new_with_label("Add Payment");
		gtk_box_append(payment_box, add_payment_button);
		multi_ptr create_cb_data = mp_new({instance->window, instance->invoice, (void*) &repopulate});
		g_signal_connect_swapped(
			add_payment_button,
			"clicked",
			G_CALLBACK(PaymentAddWindow::create),
			create_cb_data
		);
		g_signal_connect_swapped(add_payment_button, "unrealize", G_CALLBACK(free), create_cb_data);
	} else {
		Payment *payment = payment_handler->get_payment(invoice->payment_id);
		gtk_box_append(payment_box, gtk_label_new(payment->to_string().c_str()));

		GtkWidget *remove_payment_button = gtk_button_new_with_label("Remove Payment");
		gtk_box_append(payment_box, remove_payment_button);
		g_signal_connect(
			remove_payment_button,
			"clicked",
			G_CALLBACK(remove_payment),
			NULL
		);
	}

	GtkWidget *create_item_button = gtk_button_new_with_label("Link Order");
	gtk_box_append(outer_box, create_item_button);
	multi_ptr create_cb_data = mp_new({window, invoice, (void*) &link_order});
	g_signal_connect_swapped(
		create_item_button,
		"clicked",
		G_CALLBACK(OrderSearchWindow::create),
		create_cb_data
	);
	g_signal_connect_swapped(create_item_button, "unrealize", G_CALLBACK(free), create_cb_data);

	gtk_box_append(outer_box, gtk_label_new(("Linked Orders: (" + format_price(total_price) + " total)").c_str()));

	OrderHandler *order_handler = OrderHandler::get_instance();
	BookingHandler *booking_handler = BookingHandler::get_instance();
	std::vector<order_id> &linked_orders = invoice->linked_orders;
	for (size_t i = 0; i < linked_orders.size(); ++i) {
		GtkBox *order_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
		gtk_box_append(outer_box, GTK_WIDGET(order_box));

		Order *order = order_handler->get_order(linked_orders[i]);

		Customer *order_customer = customer_handler->get_customer(order->customer_id);
		Table *order_table = booking_handler->get_table(order->table_id);
		std::string order_text =
			order->date.to_string() + ", " +
			order_customer->name + ", " + order_table->name + ", " +
			ORDER_TYPE_NAMES[order->order_type] + ", " +
			format_price(order->get_total_price());
		gtk_box_append(order_box, gtk_label_new(order_text.c_str()));

		GtkWidget* edit_order_button = gtk_button_new_with_label("Edit");
		gtk_box_append(order_box, edit_order_button);
		create_cb_data = mp_new({window, order, (void*) &repopulate});
		g_signal_connect_swapped(
			edit_order_button,
			"clicked",
			G_CALLBACK(OrderEditWindow::create),
			create_cb_data
		);
		g_signal_connect_swapped(edit_order_button, "unrealize", G_CALLBACK(free), create_cb_data);

		GtkWidget* delivered_button = gtk_button_new_with_label(order->delivered ? "Set Not Delivered" : "Set Delivered");
		gtk_box_append(order_box, delivered_button);
		g_signal_connect_swapped(
			delivered_button,
			"clicked",
			G_CALLBACK(flip_order_delivered),
			order
		);

		GtkWidget *unlink_order_button = gtk_button_new_with_label("Unlink");
		gtk_box_append(order_box, unlink_order_button);
		g_signal_connect_swapped(
			unlink_order_button,
			"clicked",
			G_CALLBACK(unlink_order),
			order
		);
	}

	gtk_window_present(GTK_WINDOW(window));
}

void InvoiceEditWindow::unlink_order(Order *order) {
	PaymentHandler::get_instance()->unlink_invoice_order(instance->invoice->id, order->id);
	repopulate();
}

void InvoiceEditWindow::link_order(Order *order) {
	PaymentHandler::get_instance()->link_invoice_order(instance->invoice->id, order->id);
	repopulate();
}

void InvoiceEditWindow::flip_order_delivered(Order *order) {
	order->delivered = !order->delivered;
	OrderHandler::get_instance()->update_order(order->id);
	repopulate();
}

void InvoiceEditWindow::remove_payment() {
	PaymentHandler *payment_handler = PaymentHandler::get_instance();
	Invoice *invoice = instance->invoice;
	payment_id payid = invoice->payment_id;

	Payment *payment = payment_handler->get_payment(payid);
	if (payment->payment_type == PAYMENT_CARD) {
		CardPaymentPeripheral::cancel_payment(PaymentDetails(payment->card_number, payment->transaction_id));
	}

	invoice->payment_id = 0;
	payment_handler->update_invoice(invoice->id);
	payment_handler->delete_payment(payid);
	repopulate();
}

void InvoiceEditWindow::repopulate() {
	instance->clear();
	instance->populate();
}