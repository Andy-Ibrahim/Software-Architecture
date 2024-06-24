#ifndef _103575527SWE300003A3_TIMEWIDGET
#define _103575527SWE300003A3_TIMEWIDGET 1

#include <gtk/gtk.h>
#include "time.hpp"

class TimeWidget {
	private:
		GtkSpinButton *hour_button;
		GtkSpinButton *minute_button;
	public:
		TimeWidget(GtkBox *box, Time initial_time);
		Time get_time();
};

#endif