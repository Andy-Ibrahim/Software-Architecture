#ifndef _103575527SWE300003A3_DATEWIDGET
#define _103575527SWE300003A3_DATEWIDGET 1

#include <gtk/gtk.h>
#include "date.hpp"

class DateWidget {
	private:
		GtkSpinButton *day_button;
		GtkSpinButton *month_button;
		GtkSpinButton *year_button;

		void init(GtkBox *box, Date initial_date);
	public:
		DateWidget(GtkBox *box, Date initial_date, GCallback callback);
		DateWidget(GtkBox *box, Date initial_date);
		Date get_date();
};

#endif