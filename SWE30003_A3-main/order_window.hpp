#ifndef _103575527SWE300003A3_OWINDOW
#define _103575527SWE300003A3_OWINDOW 1

#include <gtk/gtk.h>
#include "order_handler.hpp"
#include "multi_ptr.hpp"
#include "date_widget.hpp"

class OrderWindow {
	public:
		static OrderWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;

		DateWidget *date_widget;

		static void create(GtkWindow *parent);
		static void delete_order(Order *booking);
		static void flip_order_delivered(Order *booking);
		static void repopulate();
		static void edit_created_order(Order* new_order);

        OrderWindow(OrderWindow const&) = delete;
        void operator=(OrderWindow const&) = delete;
        OrderWindow(OrderWindow&&) = delete;
        void operator=(OrderWindow&&) = delete;
	private:
		OrderWindow() {};
		void populate(Date date);
		void clear();
};

#endif
