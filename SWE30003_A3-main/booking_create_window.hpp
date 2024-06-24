#ifndef _103575527SWE300003A3_BCWINDOW
#define _103575527SWE300003A3_BCWINDOW 1

#include <gtk/gtk.h>
#include "booking_handler.hpp"
#include "multi_ptr.hpp"
#include "date_widget.hpp"
#include "time_widget.hpp"

class BookingCreateWindow {
	public:
		static BookingCreateWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		DateWidget *date_widget;
		TimeWidget *start_widget;
		TimeWidget *end_widget;
		void (*callback)();
		Customer *customer;
		Table *table;

		static void create(multi_ptr create_cb_data);
		static void set_customer(Customer *customer);
		static void set_table(Table *table);
		static void create_booking();
	private:
		void repopulate();
		void populate(Date date, Time start, Time end);
		void clear();
};

#endif