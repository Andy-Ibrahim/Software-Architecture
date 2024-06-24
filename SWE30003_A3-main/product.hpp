#ifndef _103575527SWE300003A3_PRODUCT
#define _103575527SWE300003A3_PRODUCT 1

#include <string>

typedef int product_id;

class Product {
	public:
		product_id id;
		std::string name;
		int price;
		bool deleted;

		Product(product_id id, std::string name, int price, bool deleted);
		Product(std::string name, int price);
};

#endif