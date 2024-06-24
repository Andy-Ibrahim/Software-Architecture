#ifndef _103575527SWE300003A3_BOOKING
#define _103575527SWE300003A3_BOOKING 1

#include "customer.hpp"
#include "table.hpp"
#include "date.hpp"
#include "time.hpp"
#include <string>

typedef int booking_id;

class Booking {
	public:
		booking_id id;
		customer_id customer_id;
		table_id table_id;
		Date date;
		Time start;
		Time end;

		Booking(booking_id id, ::customer_id customer_id, ::table_id table_id, Date date, Time start, Time end);
		Booking(::customer_id customer_id, ::table_id table_id, Date date, Time start, Time end);
};

#endif