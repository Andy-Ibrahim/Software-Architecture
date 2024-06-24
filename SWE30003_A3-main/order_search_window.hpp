#ifndef _103575527SWE300003A3_OSWINDOW
#define _103575527SWE300003A3_OSWINDOW 1

#include <gtk/gtk.h>
#include "order_handler.hpp"
#include "payment_handler.hpp"
#include "multi_ptr.hpp"
#include "date_widget.hpp"
#include <string>

class OrderSearchWindow {
	public:
		static OrderSearchWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		DateWidget *date_widget;
		void (*callback)(Order*);
		Invoice *invoice;

		static void create(multi_ptr create_cb_data);
		static void select(Order *order);
		static void repopulate();
	private:
		void populate(Date date);
		void clear();
};

#endif