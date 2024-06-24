PRAGMA foreign_keys = ON;
CREATE TABLE customers (
	id INTEGER PRIMARY KEY,
	name TEXT,
	phone_number TEXT,
	deleted INT);
CREATE TABLE products (
	id INTEGER PRIMARY KEY,
	name TEXT,
	price INTEGER,
	deleted INT);
CREATE TABLE tables (
	id INTEGER PRIMARY KEY,
	name TEXT,
	size INTEGER,
	deleted INT);
CREATE TABLE orders (
	id INTEGER PRIMARY KEY,
	cid INTEGER,
	tid INTEGER,
	type INTEGER,
	date TEXT,
	delivered INT,
	deleted INT,
	FOREIGN KEY (cid) REFERENCES customers(id),
	FOREIGN KEY (tid) REFERENCES tables(id));
CREATE TABLE order_items (
	oid INTEGER,
	pid INTEGER,
	count INTEGER,
	price INTEGER,
	PRIMARY KEY (oid, pid),
	FOREIGN KEY (oid) REFERENCES orders(id),
	FOREIGN KEY (pid) REFERENCES products(id));
CREATE TABLE bookings (
	id INTEGER PRIMARY KEY,
	cid INTEGER,
	tid INTEGER,
	date TEXT,
	start TEXT,
	end TEXT,
	FOREIGN KEY (cid) REFERENCES customers(id),
	FOREIGN KEY (tid) REFERENCES tables(id));
CREATE TABLE payments (
	id INTEGER PRIMARY KEY,
	amount INTEGER,
	type INTEGER,
	card_number TEXT,
	transaction_id TEXT
);
CREATE TABLE invoices (
	id INTEGER PRIMARY KEY,
	cid INTEGER,
	date TEXT,
	payid INTEGER NULL,
	FOREIGN KEY (cid) REFERENCES customers(id),
	FOREIGN KEY (payid) REFERENCES payments(id)
);
CREATE TABLE invoice_orders (
	iid INTEGER,
	oid INTEGER,
	PRIMARY KEY (iid, oid),
	FOREIGN KEY (iid) REFERENCES invoices(id),
	FOREIGN KEY (oid) REFERENCES orders(id)
);