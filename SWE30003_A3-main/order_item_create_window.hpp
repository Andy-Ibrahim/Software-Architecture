#ifndef _103575527SWE300003A3_OICWINDOW
#define _103575527SWE300003A3_OICWINDOW 1

#include <gtk/gtk.h>
#include "order_handler.hpp"
#include "multi_ptr.hpp"

class OrderItemCreateWindow {
	public:
		static OrderItemCreateWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		Order *order;
		void (*callback)();
		GtkSpinButton *count_button;
		Product *product;

		static void create(multi_ptr create_cb_data);
		static void set_product(Product *product);
		static void create_order_item();
	private:
		void repopulate();
		void populate(int count);
		inline void clear() { gtk_widget_unrealize(GTK_WIDGET(outer_box)); }
};

#endif