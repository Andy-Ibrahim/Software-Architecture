#ifndef _103575527SWE300003A3_OHANDLER
#define _103575527SWE300003A3_OHANDLER 1

#include "product.hpp"
#include "order.hpp"
#include "customer.hpp"
#include <unordered_map>
#include <vector>

class OrderHandler {
    private:
        static OrderHandler *instance;
        std::unordered_map<product_id, Product*> products;
        std::unordered_map<order_id, Order*> loaded_orders;

        OrderHandler();
        std::vector<order_id> get_date_order_ids(Date date);
    public:
        OrderHandler(OrderHandler const&) = delete;
        void operator=(OrderHandler const&) = delete;
        OrderHandler(OrderHandler&&) = delete;
        void operator=(OrderHandler&&) = delete;

        static OrderHandler *get_instance();
        Product *get_product(product_id id);
        bool update_product(product_id id);
        void add_product(Product *product);
        std::vector<Product*> get_products();
        bool delete_product(product_id id);
        Order *get_order(order_id id);
        std::vector<Order*> get_date_orders(Date date);
        std::vector<Order*> get_date_customer_orders(Date date, customer_id id);
        bool update_order(order_id id);
        void add_order(Order *order);
        bool delete_order(order_id id);
        bool add_order_item(order_id oid, product_id pid, OrderStats stats);
        bool update_order_item(order_id oid, product_id pid, OrderStats stats);
        bool delete_order_item(order_id oid, product_id pid);
};

#endif