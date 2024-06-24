#ifndef _103575527SWE300003A3_TSWINDOW
#define _103575527SWE300003A3_TSWINDOW 1

#include <gtk/gtk.h>
#include "booking_handler.hpp"
#include "multi_ptr.hpp"

class TableSearchWindow {
	public:
		static TableSearchWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		void (*callback)(Table*);

		static void create(multi_ptr create_cb_data);
		static void select(Table *table);
	private:
		void populate();
};

#endif