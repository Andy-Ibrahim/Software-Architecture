#include "booking.hpp"

Booking::Booking(booking_id id, ::customer_id customer_id, ::table_id table_id, Date date, Time start, Time end) {
	this->id = id;
	this->customer_id = customer_id;
	this->table_id = table_id;
	this->date = date;
	this->start = start;
	this->end = end;
}

Booking::Booking(::customer_id customer_id, ::table_id table_id, Date date, Time start, Time end) {
	this->id = 0;
	this->customer_id = customer_id;
	this->table_id = table_id;
	this->date = date;
	this->start = start;
	this->end = end;
}