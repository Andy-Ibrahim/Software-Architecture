mkdir build;
g++ -std=c++20 *.cpp -o "build/swe30003_a3" -IC:/msys64/mingw64/include -IC:/msys64/mingw64/include/gtk-4.0 -IC:/msys64/mingw64/include/pango-1.0 -IC:/msys64/mingw64/include/harfbuzz -IC:/msys64/mingw64/include/gdk-pixbuf-2.0 -IC:/msys64/mingw64/include/cairo -IC:/msys64/mingw64/include/glib-2.0 -IC:/msys64/mingw64/lib/glib-2.0/include -IC:/msys64/mingw64/include/freetype2 -IC:/msys64/mingw64/include/graphene-1.0 -IC:/msys64/mingw64/lib/graphene-1.0/include -mfpmath=sse -msse -msse2 -IC:/msys64/mingw64/include/fribidi -IC:/msys64/mingw64/include/webp -DLIBDEFLATE_DLL -IC:/msys64/mingw64/include/libpng16 -IC:/msys64/mingw64/include/pixman-1 -LC:/msys64/mingw64/lib -lgtk-4 -lpangowin32-1.0 -lharfbuzz -lpangocairo-1.0 -lpango-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lcairo -lgraphene-1.0 -lgio-2.0 -lglib-2.0 -lintl -lgobject-2.0 -lsqlite3 -O3
pause