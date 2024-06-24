#ifndef _103575527SWE300003A3_PSWINDOW
#define _103575527SWE300003A3_PSWINDOW 1

#include <gtk/gtk.h>
#include "order_handler.hpp"
#include "multi_ptr.hpp"

class ProductSearchWindow {
	public:
		static ProductSearchWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		void (*callback)(Product*);

		static void create(multi_ptr create_cb_data);
		static void select(Product *table);
	private:
		void populate();
};

#endif