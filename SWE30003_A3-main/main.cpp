#include "database.hpp"
#include "customer_window.hpp"
#include "booking_window.hpp"
#include "order_window.hpp"
#include "invoice_window.hpp"
#include "multi_ptr.hpp"
#include <gtk/gtk.h>
#include <filesystem>
#include <iostream>

static void create_main_window(GtkApplication *app) {
	GtkWindow *window = GTK_WINDOW(gtk_application_window_new(app));

	gtk_window_set_title(window, "Relaxing Koala RIS");
	gtk_window_set_default_size(window, 800, 600);

	GtkBox* outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	GtkWidget* customer_window_button = gtk_button_new_with_label("Manage Customers");
	gtk_box_append(outer_box, customer_window_button);
	g_signal_connect_swapped(
		customer_window_button,
		"clicked",
		G_CALLBACK(CustomerWindow::create),
		window
	);

	GtkWidget* order_window_button = gtk_button_new_with_label("Manage Orders");
	gtk_box_append(outer_box, order_window_button);
	g_signal_connect_swapped(
		order_window_button,
		"clicked",
		G_CALLBACK(OrderWindow::create),
		window
	);

	GtkWidget* booking_window_button = gtk_button_new_with_label("Manage Bookings");
	gtk_box_append(outer_box, booking_window_button);
	g_signal_connect_swapped(
		booking_window_button,
		"clicked",
		G_CALLBACK(BookingWindow::create),
		window
	);

	GtkWidget* invoice_window_button = gtk_button_new_with_label("Manage Invoices");
	gtk_box_append(outer_box, invoice_window_button);
	g_signal_connect_swapped(
		invoice_window_button,
		"clicked",
		G_CALLBACK(InvoiceWindow::create),
		window
	);

	gtk_window_present(window);
}

int main(int argc, char *argv[]) {
	std::filesystem::path dir = argv[0];
	dir.remove_filename();
	Database::init(dir);

	GtkApplication *app = gtk_application_new("org.swe30003group.assign3", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(create_main_window), NULL);
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;
} 
