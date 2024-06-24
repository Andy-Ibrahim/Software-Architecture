#include "table_search_window.hpp"

TableSearchWindow *TableSearchWindow::instance = new TableSearchWindow();

void TableSearchWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->callback = (void(*)(Table*)) mp_get<void>(create_cb_data, 1);

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Select Table");
	gtk_window_set_default_size(window, 800, 600);

    instance->populate();
}

void TableSearchWindow::populate() {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	BookingHandler* booking_handler = BookingHandler::get_instance();
	std::vector<Table*> table_list = booking_handler->get_tables();
	for (Table* table : table_list) {
		// Inner Box
		GtkBox* inner_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
		gtk_box_append(outer_box, GTK_WIDGET(inner_box));
		// Update Button
		GtkWidget* button = gtk_button_new_with_label((table->name + ", " + std::to_string(table->size)).c_str());
		gtk_box_append(inner_box, button);
		g_signal_connect_swapped(
			button,
			"clicked",
			G_CALLBACK(select),
			table
		);
	}

	gtk_window_present(GTK_WINDOW(window));
}

void TableSearchWindow::select(Table *table) {
	gtk_window_close(instance->window);
	instance->callback(table);
}