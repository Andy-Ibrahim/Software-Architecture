#include "invoice.hpp"

Invoice::Invoice(invoice_id id, ::customer_id customer_id, Date date, ::payment_id payment_id) {
	this->id = id;
	this->customer_id = customer_id;
	this->date = date;
	this->payment_id = payment_id;
}

Invoice::Invoice(::payment_id payment_id, ::customer_id customer_id, Date date) {
	this->id = 0;
	this->customer_id = customer_id;
	this->date = date;
	this->payment_id = payment_id;
}