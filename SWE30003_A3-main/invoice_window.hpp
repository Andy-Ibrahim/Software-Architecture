#ifndef _103575527SWE300003A3_IWINDOW
#define _103575527SWE300003A3_IWINDOW 1

#include <gtk/gtk.h>
#include "payment_handler.hpp"
#include "multi_ptr.hpp"
#include "date_widget.hpp"

class InvoiceWindow {
	public:
		static InvoiceWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;

		DateWidget *date_widget;

		static void create(GtkWindow *parent);
		static void delete_invoice(Invoice *invoice);
		static void print_invoice(Invoice *invoice);
		static void edit_created_invoice(Invoice *new_invoice);
		static void repopulate();

        InvoiceWindow(InvoiceWindow const&) = delete;
        void operator=(InvoiceWindow const&) = delete;
        InvoiceWindow(InvoiceWindow&&) = delete;
        void operator=(InvoiceWindow&&) = delete;
	private:
		InvoiceWindow() {};
		void populate(Date date);
		void clear();
};

#endif
