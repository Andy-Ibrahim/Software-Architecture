#ifndef _103575527SWE300003A3_CWINDOW
#define _103575527SWE300003A3_CWINDOW 1

#include <gtk/gtk.h>
#include "customer_handler.hpp"
#include "multi_ptr.hpp"

class CustomerWindow {
	public:
		static CustomerWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;

		static void create(GtkWindow *parent);
		static void update_customer(multi_ptr update_cb_data);
		static void delete_customer(Customer *customer);
		static void add_customer(multi_ptr add_cb_data);
        CustomerWindow(CustomerWindow const&) = delete;
        void operator=(CustomerWindow const&) = delete;
        CustomerWindow(CustomerWindow&&) = delete;
        void operator=(CustomerWindow&&) = delete;
	private:
		CustomerWindow() {}
		void populate();
		inline void clear() { gtk_widget_unrealize(GTK_WIDGET(outer_box)); }
};

#endif