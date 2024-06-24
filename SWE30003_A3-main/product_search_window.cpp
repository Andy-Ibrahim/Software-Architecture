#include "product_search_window.hpp"
#include "util.hpp"

ProductSearchWindow *ProductSearchWindow::instance = new ProductSearchWindow();

void ProductSearchWindow::create(multi_ptr create_cb_data) {
	GtkWindow *parent = mp_get<GtkWindow>(create_cb_data, 0);
	instance->callback = (void(*)(Product*)) mp_get<void>(create_cb_data, 1);

	GtkWindow* window = GTK_WINDOW(gtk_window_new());
    instance->window = window;
	gtk_window_set_modal(window, true);
	gtk_window_set_transient_for(window, parent);
	gtk_window_set_title(window, "Select Product");
	gtk_window_set_default_size(window, 800, 600);

    instance->populate();
}

void ProductSearchWindow::populate() {
	outer_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
	gtk_window_set_child(window, GTK_WIDGET(outer_box));

	OrderHandler* booking_handler = OrderHandler::get_instance();
	std::vector<Product*> product_list = booking_handler->get_products();
	for (Product* product : product_list) {
		// Inner Box
		GtkBox* inner_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
		gtk_box_append(outer_box, GTK_WIDGET(inner_box));
		// Update Button
		GtkWidget* button = gtk_button_new_with_label((product->name + ", " + format_price(product->price)).c_str());
		gtk_box_append(inner_box, button);
		g_signal_connect_swapped(
			button,
			"clicked",
			G_CALLBACK(select),
			product
		);
	}

	gtk_window_present(GTK_WINDOW(window));
}

void ProductSearchWindow::select(Product *product) {
	gtk_window_close(instance->window);
	instance->callback(product);
}