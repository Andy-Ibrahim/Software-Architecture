#include "popup_window.hpp"
#include <string>

GtkWindow *create_popup_window(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	const char *name = mp_get<const char>(create_cb_data, 1);
	const char *message = mp_get<const char>(create_cb_data, 2);

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, name);
	gtk_window_set_default_size(window, 400, 200);

	GtkBox *outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	gtk_box_append(outer_box, gtk_label_new(message));

	GtkWidget *button = gtk_button_new_with_label("Close");
	gtk_box_append(outer_box, button);
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_close), window);

	gtk_window_present(window);
	return window;
}
