#include "payment_add_window.hpp"
#include "popup_window.hpp"
#include "card_payment_peripheral.hpp"
#include "util.hpp"

PaymentAddWindow *PaymentAddWindow::instance = new PaymentAddWindow();

void PaymentAddWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->invoice = mp_get<Invoice>(create_cb_data, 1);
	instance->callback = (void(*)()) mp_get<void>(create_cb_data, 2);

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Add Payment");
	gtk_window_set_default_size(window, 800, 600);

	instance->payment_done = false;
	instance->card_number = "";
	instance->transaction_id = "";
	instance->type = PAYMENT_CASH;

    instance->populate();
}

void PaymentAddWindow::populate() {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	PaymentHandler *payment_handler = PaymentHandler::get_instance();
	int total_price = payment_handler->get_invoice_total_price(invoice);

	gtk_box_append(outer_box, gtk_label_new(("Payment Amount: " + format_price(total_price)).c_str()));

	GtkBox *type_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(type_box));
	gtk_box_append(type_box, gtk_label_new(PAYMENT_TYPE_NAMES[type]));
	size_t i = 0;
	for (size_t i = 0; i < PAYMENT_TYPE_COUNT; ++i) {
		GtkWidget *type_button = gtk_button_new_with_label(PAYMENT_TYPE_NAMES[i]);
		gtk_box_append(type_box, type_button);
		g_signal_connect_swapped(
			type_button,
			"clicked",
			G_CALLBACK(set_payment_type),
			(gpointer) i
		);
	}

	switch (type) {
		case PAYMENT_CASH:
			break;
		case PAYMENT_CARD:
			gtk_box_append(outer_box, gtk_label_new(payment_done ? "Paid" : "Not Paid"));
			gtk_box_append(outer_box, gtk_label_new(("Card Number: " + card_number).c_str()));
			gtk_box_append(outer_box, gtk_label_new(("Transaction ID: " + transaction_id).c_str()));

			{ GtkWidget *request_card_payment_button = gtk_button_new_with_label("Request Card Payment");
			gtk_box_append(outer_box, request_card_payment_button);
			g_signal_connect(
				request_card_payment_button,
				"clicked",
				G_CALLBACK(request_card_payment),
				NULL
			); }
			break;
	}

	GtkWidget *create_button = gtk_button_new_with_label("Add Payment");
	gtk_box_append(outer_box, create_button);
	g_signal_connect(
		create_button,
		"clicked",
		G_CALLBACK(add_payment),
		NULL
	);

	gtk_window_present(GTK_WINDOW(window));
}

void PaymentAddWindow::set_payment_type(payment_type type) {
	if (type != PAYMENT_CARD && instance->payment_done) {
		CardPaymentPeripheral::cancel_previous_payment();
		instance->payment_done = false;
		instance->card_number = "";
		instance->transaction_id = "";
	}
	instance->type = type;
	instance->repopulate();
}

void PaymentAddWindow::finish_card_payment(std::string card_number, std::string transaction_id) {
	payment_done = true;
	this->card_number = card_number;
	this->transaction_id = transaction_id;
	repopulate();
}

void PaymentAddWindow::request_card_payment() {
	PaymentHandler *payment_handler = PaymentHandler::get_instance();
	int amount = payment_handler->get_invoice_total_price(instance->invoice);
	PaymentDetails details = CardPaymentPeripheral::request_payment(amount);
	instance->finish_card_payment(details.card_number, details.transaction_id);
}

void PaymentAddWindow::add_payment() {
	PaymentHandler *payment_handler = PaymentHandler::get_instance();
	Invoice *invoice = instance->invoice;
	int amount = payment_handler->get_invoice_total_price(invoice);

	Payment *payment;
	switch (instance->type) {
		case PAYMENT_CASH:
			payment = new Payment(amount, PAYMENT_CASH, "", "");
			break;
		case PAYMENT_CARD:
			if (!instance->payment_done) {
				const char *name = "Error";
				const char *message = "Could not create payment, card payment not completed.";
				multi_ptr create_cb_data = mp_new({instance->window, (void*) name, (void*) message});
				create_popup_window(create_cb_data);
				free(create_cb_data);
				return;
			}
			payment = new Payment(amount, PAYMENT_CARD, instance->card_number, instance->transaction_id);
			break;
	}

	payment_handler->add_payment(payment);
	invoice->payment_id = payment->id;
	payment_handler->update_invoice(invoice->id);

	instance->callback();
	gtk_window_close(instance->window);
}

void PaymentAddWindow::repopulate() {
	clear();
	populate();
}