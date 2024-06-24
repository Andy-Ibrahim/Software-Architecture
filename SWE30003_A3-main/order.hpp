#ifndef _103575527SWE300003A3_ORDER
#define _103575527SWE300003A3_ORDER 1

#include "customer.hpp"
#include "table.hpp"
#include "date.hpp"
#include "product.hpp"
#include <unordered_map>

typedef int order_id;
typedef char order_type;

#define ORDER_DINE_IN '\0'
#define ORDER_TAKEAWAY '\1'
#define ORDER_DELIVERY '\2'

static const size_t ORDER_TYPE_COUNT = 3;
static const char *const ORDER_TYPE_NAMES[ORDER_TYPE_COUNT] = {
	"Dine-In",
	"Takeaway",
	"Delivery"
};

class OrderStats {
	public:
		int count;

		OrderStats(int count, int price);
		OrderStats(int count);
		inline int get_price() { return price; }
		void calculate_price(int product_price);
	private:
		int price;
};

class Order {
	public:
		order_id id;
		customer_id customer_id;
		table_id table_id;
		order_type order_type;
		Date date;
		bool delivered;
		bool deleted;
		std::unordered_map<product_id, OrderStats> items;

		Order(order_id id, ::customer_id customer_id, ::table_id table_id, ::order_type order_type, Date date, bool delivered, bool deleted);
		Order(::customer_id customer_id, ::table_id table_id, ::order_type order_type, Date date, bool delivered);
		Order(::customer_id customer_id, ::table_id table_id, ::order_type order_type, Date date);
		int get_total_price();
};

#endif