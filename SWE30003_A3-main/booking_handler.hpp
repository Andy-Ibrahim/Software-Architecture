#ifndef _103575527SWE300003A3_BHANDLER
#define _103575527SWE300003A3_BHANDLER 1

#include "table.hpp"
#include "booking.hpp"
#include <unordered_map>
#include <vector>
#include <list>

class BookingHandler {
    private:
        static BookingHandler *instance;
        std::unordered_map<table_id, Table*> tables;
        std::unordered_map<booking_id, Booking*> loaded_bookings;
        
        BookingHandler();
        std::vector<booking_id> get_date_booking_ids(Date date);
    public:
        // Copy constructor
        BookingHandler(BookingHandler const&) = delete;
        // Copy assignment
        void operator=(BookingHandler const&) = delete;
        // Move constructor
        BookingHandler(BookingHandler&&) = delete;
        // Move assignment
        void operator=(BookingHandler&&) = delete;

        static BookingHandler *get_instance();
        Table *get_table(table_id id);
        std::vector<Table*> get_tables();
        Booking *get_booking(booking_id id);
        std::vector<Booking*> get_date_bookings(Date date);
        bool update_booking(booking_id id);
        void add_booking(Booking *booking);
        bool delete_booking(booking_id id);
};

#endif