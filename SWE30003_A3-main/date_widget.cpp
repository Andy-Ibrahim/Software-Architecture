#include "date_widget.hpp"

void DateWidget::init(GtkBox *box, Date initial_date) {
	day_button = GTK_SPIN_BUTTON(gtk_spin_button_new(gtk_adjustment_new(initial_date.day, 1, 31, 1, 0, 0), 0, 0));
	gtk_orientable_set_orientation(GTK_ORIENTABLE(day_button), GTK_ORIENTATION_VERTICAL);
	gtk_box_append(box, GTK_WIDGET(day_button));

	gtk_box_append(box, gtk_label_new("/"));

	month_button = GTK_SPIN_BUTTON(gtk_spin_button_new(gtk_adjustment_new(initial_date.month, 1, 12, 1, 0, 0), 0, 0));
	gtk_orientable_set_orientation(GTK_ORIENTABLE(month_button), GTK_ORIENTATION_VERTICAL);
	gtk_box_append(box, GTK_WIDGET(month_button));

	gtk_box_append(box, gtk_label_new("/"));

	year_button = GTK_SPIN_BUTTON(gtk_spin_button_new(gtk_adjustment_new(initial_date.year, 1900, 3000, 1, 0, 0), 0, 0));
	gtk_orientable_set_orientation(GTK_ORIENTABLE(year_button), GTK_ORIENTATION_VERTICAL);
	gtk_box_append(box, GTK_WIDGET(year_button));
}

DateWidget::DateWidget(GtkBox *box, Date initial_date, GCallback callback) {
	init(box, initial_date);
	g_signal_connect(day_button, "value-changed", callback, NULL);
	g_signal_connect(month_button, "value-changed", callback, NULL);
	g_signal_connect(year_button, "value-changed", callback, NULL);
}

DateWidget::DateWidget(GtkBox *box, Date initial_date) {
	init(box, initial_date);
}

Date DateWidget::get_date() {
	return Date(
		gtk_spin_button_get_value_as_int(day_button),
		gtk_spin_button_get_value_as_int(month_button),
		gtk_spin_button_get_value_as_int(year_button)
	);
}