#ifndef _103575527SWE300003A3_OEWINDOW
#define _103575527SWE300003A3_OEWINDOW 1

#include <gtk/gtk.h>
#include "order_handler.hpp"
#include "multi_ptr.hpp"

class OrderEditWindow {
	public:
		static OrderEditWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		Order *order;

		static void create(multi_ptr create_cb_data);
		static void delete_item(Product *product);
		static void repopulate();
	private:
		void populate();
		inline void clear() { gtk_widget_unrealize(GTK_WIDGET(outer_box)); }
};

#endif