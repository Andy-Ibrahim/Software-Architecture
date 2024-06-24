#ifndef _103575527SWE300003A3_DATABASE
#define _103575527SWE300003A3_DATABASE 1

#include "customer.hpp"
#include "product.hpp"
#include "table.hpp"
#include "order.hpp"
#include "booking.hpp"
#include "payment.hpp"
#include "invoice.hpp"
#include <sqlite3.h>
#include <vector>
#include <string>
#include <filesystem>

#define DATABASE_OK 0
#define DATABASE_NONE 1
#define DATABASE_ERROR 2
#define DATABASE_OTHER 3

class Database {
	private:
		static std::string database_path;
		static Database *instance;
		sqlite3 *db;
		int db_result_code;

        Database(const char* filename);
        static bool is_unrecoverable_result(int res);
        bool is_fk_error(int res);
        sqlite3_stmt *prepare_statement(std::string sql);
        std::string nullable_id_string(int id);

        void populate_order_items(Order *order);
        void delete_order_items(order_id id);
        
        void populate_invoice_orders(Invoice *invoice);
        void unlink_invoice_orders(invoice_id id);
    public:
        static void init(std::filesystem::path folder);
        static Database *get_instance();
        int get_result_code();

        Customer *get_customer(customer_id id);
        std::vector<customer_id> get_name_customer_ids(std::string match_name);
        void update_customer(Customer *customer);
        customer_id add_customer(Customer *customer);
        void delete_customer(customer_id id);

        std::vector<Product*> get_products();
        void update_product(Product *product);
        product_id add_product(Product *product);
        void delete_product(product_id id);
        std::vector<Table*> get_tables();

        Order *get_order(order_id id);
        std::vector<order_id> get_date_order_ids(Date date);
        void update_order(Order *order);
        order_id add_order(Order *order);
        void delete_order(order_id id);

        void add_order_item(order_id oid, product_id pid, OrderStats stats);
        void update_order_item(order_id oid, product_id pid, OrderStats stats);
        void delete_order_item(order_id oid, product_id pid);

        Booking *get_booking(booking_id id);
        std::vector<booking_id> get_date_booking_ids(Date date);
        void update_booking(Booking *booking);
        booking_id add_booking(Booking *booking);
        void delete_booking(booking_id id);

        Payment *get_payment(payment_id id);
        void update_payment(Payment *payment);
        payment_id add_payment(Payment *payment);
        void delete_payment(payment_id id);

        Invoice *get_invoice(invoice_id id);
        std::vector<invoice_id> get_date_invoice_ids(Date date);
        void update_invoice(Invoice *invoice);
        invoice_id add_invoice(Invoice *invoice);
        void delete_invoice(Invoice *invoice);

        void link_invoice_order(invoice_id iid, order_id oid);
        void unlink_invoice_order(invoice_id iid, order_id oid);
};

#endif