#ifndef _103575527SWE300003A3_OCWINDOW
#define _103575527SWE300003A3_OCWINDOW 1

#include <gtk/gtk.h>
#include "order_handler.hpp"
#include "multi_ptr.hpp"
#include "date_widget.hpp"

class OrderCreateWindow {
	public:
		static OrderCreateWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		DateWidget *date_widget;
		void (*callback)(Order*);
		Customer *customer;
		Table *table;
		order_type type;

		static void create(multi_ptr create_cb_data);
		static void set_customer(Customer *customer);
		static void set_table(Table *table);
		static void set_order_type(order_type type);
		static void create_order();
	private:
		void repopulate();
		void populate(Date date);
		void clear();
};

#endif