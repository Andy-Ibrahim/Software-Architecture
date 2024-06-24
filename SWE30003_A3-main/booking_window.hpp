#ifndef _103575527SWE300003A3_BWINDOW
#define _103575527SWE300003A3_BWINDOW 1

#include <gtk/gtk.h>
#include "booking_handler.hpp"
#include "multi_ptr.hpp"
#include "date_widget.hpp"

class BookingWindow {
	public:
		static BookingWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;

		DateWidget *date_widget;

		static void create(GtkWindow *parent);
		static void delete_booking(Booking *booking);
		static void repopulate();
        BookingWindow(BookingWindow const&) = delete;
        void operator=(BookingWindow const&) = delete;
        BookingWindow(BookingWindow&&) = delete;
        void operator=(BookingWindow&&) = delete;
	private:
		BookingWindow() {};
		void populate(Date date);
		void clear();
};

#endif
