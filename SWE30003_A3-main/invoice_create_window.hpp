#ifndef _103575527SWE300003A3_ICWINDOW
#define _103575527SWE300003A3_ICWINDOW 1

#include <gtk/gtk.h>
#include "payment_handler.hpp"
#include "multi_ptr.hpp"
#include "date_widget.hpp"

class InvoiceCreateWindow {
	public:
		static InvoiceCreateWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		DateWidget *date_widget;
		void (*callback)(Invoice*);
		Customer *customer;

		static void create(multi_ptr create_cb_data);
		static void set_customer(Customer *customer);
		static void create_invoice();
	private:
		void repopulate();
		void populate(Date date);
		void clear();
};

#endif