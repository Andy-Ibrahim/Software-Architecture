mkdir build
g++ -std=c++20 -o "build/swe30003_a3" *.cpp `pkg-config gtk4 --cflags pkg-config sqlite3 --cflags pkg-config gtk4 --libs pkg-config sqlite3 --libs` -O3