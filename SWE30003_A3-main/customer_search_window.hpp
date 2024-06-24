#ifndef _103575527SWE300003A3_CSWINDOW
#define _103575527SWE300003A3_CSWINDOW 1

#include <gtk/gtk.h>
#include "customer_handler.hpp"
#include "multi_ptr.hpp"
#include <string>

class CustomerSearchWindow {
	public:
		static CustomerSearchWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		void (*callback)(Customer*);

		static void create(multi_ptr create_cb_data);
		static void update(GtkEntryBuffer *buffer);
		static void select(Customer *customer);
	private:
		void populate(std::string search);
		inline void clear() { gtk_widget_unrealize(GTK_WIDGET(outer_box)); }
};

#endif