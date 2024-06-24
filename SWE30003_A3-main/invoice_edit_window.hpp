#ifndef _103575527SWE300003A3_IEWINDOW
#define _103575527SWE300003A3_IEWINDOW 1

#include <gtk/gtk.h>
#include "payment_handler.hpp"
#include "order_handler.hpp"
#include "multi_ptr.hpp"

class InvoiceEditWindow {
	public:
		static InvoiceEditWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		Invoice *invoice;

		static void create(multi_ptr create_cb_data);
		static void unlink_order(Order *order);
		static void link_order(Order *order);
		static void flip_order_delivered(Order *order);
		static void remove_payment();
		static void repopulate();
	private:
		void populate();
		inline void clear() { gtk_widget_unrealize(GTK_WIDGET(outer_box)); }
};

#endif