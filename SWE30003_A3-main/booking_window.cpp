#include "booking_window.hpp"
#include "customer_handler.hpp"
#include "booking_create_window.hpp"

BookingWindow *BookingWindow::instance = new BookingWindow();

void BookingWindow::create(GtkWindow *parent) {
	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Bookings");
	gtk_window_set_default_size(window, 800, 600);

    instance->populate(Date());
}

void BookingWindow::populate(Date date) {
    outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	GtkBox* date_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(date_box));

	date_widget = new DateWidget(date_box, date, G_CALLBACK(repopulate));

	GtkWidget* create_button = gtk_button_new_with_label("Create Booking");
	gtk_box_append(outer_box, create_button);
	multi_ptr create_cb_data = mp_new({window, (void*) &repopulate});
	g_signal_connect_swapped(
		create_button,
		"clicked",
		G_CALLBACK(BookingCreateWindow::create),
		create_cb_data
	);
	g_signal_connect_swapped(create_button, "unrealize", G_CALLBACK(free), create_cb_data);

	gtk_box_append(outer_box, gtk_label_new("Bookings:"));

	CustomerHandler *customer_handler = CustomerHandler::get_instance();
	BookingHandler* booking_handler = BookingHandler::get_instance();
	std::vector<Booking*> bookings = booking_handler->get_date_bookings(date);
	for (Booking* booking : bookings) {
		GtkBox* booking_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
		gtk_box_append(outer_box, GTK_WIDGET(booking_box));

		Customer *customer = customer_handler->get_customer(booking->customer_id);
		Table *table = booking_handler->get_table(booking->table_id);
		std::string booking_text =
			booking->date.to_string() + " " + booking->start.to_string() + "-" + booking->end.to_string() + ", " +
			customer->name + ", " + table->name;
		gtk_box_append(booking_box, gtk_label_new(booking_text.c_str()));

		GtkWidget* delete_button = gtk_button_new_with_label("Delete");
		gtk_box_append(booking_box, delete_button);
		g_signal_connect_swapped(
			delete_button,
			"clicked",
			G_CALLBACK(delete_booking),
			booking
		);
	}

	gtk_window_present(GTK_WINDOW(window));
}

void BookingWindow::delete_booking(Booking *booking) {
	BookingHandler::get_instance()->delete_booking(booking->id);
	repopulate();
}

void BookingWindow::repopulate() {
	Date date = instance->date_widget->get_date();
	instance->clear();
	instance->populate(date);
}

void BookingWindow::clear() {
	delete date_widget;
	gtk_widget_unrealize(GTK_WIDGET(outer_box));
}
