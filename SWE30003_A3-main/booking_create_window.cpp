#include "booking_create_window.hpp"
#include "customer_search_window.hpp"
#include "table_search_window.hpp"
#include "popup_window.hpp"
#include <string>

BookingCreateWindow *BookingCreateWindow::instance = new BookingCreateWindow();

void BookingCreateWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->callback = (void(*)()) mp_get<void>(create_cb_data, 1);

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Create Booking");
	gtk_window_set_default_size(window, 800, 600);

	instance->customer = nullptr;
	instance->table = nullptr;

    instance->populate(Date(), Time(), Time());
}

void BookingCreateWindow::populate(Date date, Time start, Time end) {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	GtkBox *datetime_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
	gtk_box_append(outer_box, GTK_WIDGET(datetime_box));
	date_widget = new DateWidget(datetime_box, date);
	gtk_box_append(datetime_box, gtk_label_new("from"));
	start_widget = new TimeWidget(datetime_box, start);
	gtk_box_append(datetime_box, gtk_label_new("to"));
	end_widget = new TimeWidget(datetime_box, end);

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

	GtkWidget *create_button = gtk_button_new_with_label("Create");
	gtk_box_append(outer_box, create_button);
	g_signal_connect(
		create_button,
		"clicked",
		G_CALLBACK(create_booking),
		NULL
	);
	gtk_window_present(GTK_WINDOW(window));
}

void BookingCreateWindow::set_customer(Customer *customer) {
	instance->customer = customer;
	instance->repopulate();
}

void BookingCreateWindow::set_table(Table *table) {
	instance->table = table;
	instance->repopulate();
}

void BookingCreateWindow::create_booking() {
	Customer *customer = instance->customer;
	Table *table = instance->table;
	Date date = instance->date_widget->get_date();
	Time start = instance->start_widget->get_time();
	Time end = instance->end_widget->get_time();

	// Complain about nonsense input.
	if (!customer || !table) {
		const char* name = "Error";
		const char* message = "Please select both a customer and a table.";
		multi_ptr cb_data = mp_new({
			instance->window,
			(void*) name,
			(void*) message
		});
		create_popup_window(cb_data);
		free(cb_data);
		return;
	}
	if (start >= end) {
		const char* name = "Error";
		const char* message = "Booking must start before it finishes.";
		multi_ptr cb_data = mp_new({
			instance->window,
			(void*) name,
			(void*) message
		});
		create_popup_window(cb_data);
		free(cb_data);
		return;
	}

	BookingHandler* bookingHandler = BookingHandler::get_instance();
	std::vector<Booking*> booksOnDay = bookingHandler->get_date_bookings(date);
	for (Booking* booking : booksOnDay)
	{
		if (booking->table_id != table->id) continue;

		// Check for conflicts on that day.
		if (start < booking->end
		&& end > booking->start) {
			const char* name = "Error";
			const char* message = "Conflicts with existing booking.";
			multi_ptr cb_data = mp_new({
				instance->window,
				(void*) name,
				(void*) message
			});
			create_popup_window(cb_data);
			free(cb_data);
			return;
		}
	}

	Booking* newBook = new Booking(customer->id, table->id, date, start, end);
	bookingHandler->add_booking(newBook);
	instance->callback();
	gtk_window_close(instance->window);
}

void BookingCreateWindow::repopulate() {
	Date date = date_widget->get_date();
	Time start = start_widget->get_time();
	Time end = end_widget->get_time();
	clear();
	populate(date, start, end);
}

void BookingCreateWindow::clear() {
	delete date_widget;
	delete start_widget;
	delete end_widget;
	gtk_widget_unrealize(GTK_WIDGET(outer_box));
}
