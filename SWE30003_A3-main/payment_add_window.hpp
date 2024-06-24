#ifndef _103575527SWE300003A3_PAWINDOW
#define _103575527SWE300003A3_PAWINDOW 1

#include <gtk/gtk.h>
#include "payment_handler.hpp"
#include "multi_ptr.hpp"

class PaymentAddWindow {
	public:
		static PaymentAddWindow *instance;
		GtkWindow *window;
		GtkBox *outer_box;
		void (*callback)();
		Invoice *invoice;
		payment_type type;
		bool payment_done;
		std::string card_number;
		std::string transaction_id;

		static void create(multi_ptr create_cb_data);
		static void set_payment_type(payment_type type);
		static void request_card_payment();
		static void add_payment();
	private:
		void finish_card_payment(std::string card_number, std::string transaction_id);
		void repopulate();
		void populate();
		inline void clear() { gtk_widget_unrealize(GTK_WIDGET(outer_box)); }
};

#endif