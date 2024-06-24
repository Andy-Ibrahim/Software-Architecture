#include "time_widget.hpp"

TimeWidget::TimeWidget(GtkBox *box, Time initial_time) {
	hour_button = GTK_SPIN_BUTTON(gtk_spin_button_new(gtk_adjustment_new(initial_time.hour, 0, 23, 1, 0, 0), 0, 0));
	gtk_orientable_set_orientation(GTK_ORIENTABLE(hour_button), GTK_ORIENTATION_VERTICAL);
	gtk_box_append(box, GTK_WIDGET(hour_button));

	GtkWidget *seperator = gtk_label_new(":");
	gtk_box_append(box, seperator);

	minute_button = GTK_SPIN_BUTTON(gtk_spin_button_new(gtk_adjustment_new(initial_time.minute, 0, 59, 1, 0, 0), 0, 0));
	gtk_orientable_set_orientation(GTK_ORIENTABLE(minute_button), GTK_ORIENTATION_VERTICAL);
	gtk_box_append(box, GTK_WIDGET(minute_button));
}

Time TimeWidget::get_time() {
	return Time(
		gtk_spin_button_get_value_as_int(hour_button),
		gtk_spin_button_get_value_as_int(minute_button)
	);
}