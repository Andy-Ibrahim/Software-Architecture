## Setup Instructions
### Windows
#### Installing Dependencies
1. Install MSYS2: 'https://www.msys2.org/'. After installing, run MSYS2.
2. Install MinGW-w64 toolchain (GCC compiler) by pasting `pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain` into MSYS2 terminal.
3. Install GTK-4.0 (graphics library) by pasting `pacman -S mingw-w64-x86_64-gtk4` into MSYS2 terminal.
4. Install SQLite3 (local database) by pasting `pacman -S mingw-w64-x86_64-sqlite` into MSYS2 terminal.
5. Add MSYS2 binaries to Path.
   1. Open 'Settings'.
   2. Search 'environment' and select "Edit environment variable for your account".
   3. In the new window, under "User variables for \<user\>", double click the 'Path' entry.
   4. Add two new entries to 'Path': `C:\msys64\ucrt64\bin` and `C:\msys64\mingw64\bin`.
   5. Exit by clicking "Ok" on both windows.
6. Check that at least GCC was installed by typing `g++ --version` into Command Prompt.
#### Building and Running
1. Open the root folder of the repository in a terminal of some sort (I use VSCode's integrated terminal as it puts me there by default).
2. Run `start .\build.cmd` to build the application (if this doesn't work then please message me). The application will be created inside `<project root>/build`.
3. To run the application, open the build folder in File Explorer and double-click it. It should create a window with 'Hello World' in the center.
#### VSCode Setup
VSCode is more complex because it's stupid. I just setup IntelliSense but you can probably set up debug/build tasks.

To get IntelliSense working:
1. Install the `C/C++` extension.
2. Open the root folder of the project in VSCode.
3. Press `CTRL + Shift + P` and search for `C/C++: Edit Configurations (JSON)` (you might have to press right-shift to get the search bar). Clicking this should create a `c_cpp_properties.json` in `<project root>/.vscode` and open it.
4. In `c_cpp_properties.json`, add these entries to `includePath`:
    1. `"C:/msys64/mingw64/include/gtk-4.0"`
    2. `"C:/msys64/mingw64/include/glib-2.0"`
    3. `"C:/msys64/mingw64/include/**"`
    4. `"C:/msys64/mingw64/lib/**"`
5. Open `<project root>/main.cpp` to check that IntelliSense is working (message me if it doesn't work).

### MacOS
Setup is much easier on MacOS because of brew. It was also easier to set up because `pkg-config` can be easily used.
#### Installing Dependencies
1. Install Homebrew (if you don't have this you're a lost cause): 'https://brew.sh/'
2. Launch Terminal and install `gcc` (compiler), `gtk4` (graphics library), `sqlite` (local database), and `pkg-config` by running: `brew install <name>`.
3. Check that GCC works by running `g++ --version` in Terminal.
#### Building and Running
1. Open the root folder of the repository in a terminal of some sort (I use VSCode's integrated terminal as it puts me there by default).
2. Run `sh build.sh` to build the application (if this doesn't work then please message me). The application will be created inside `<project root>/build`.
3. To run the application either open the build folder in Finder and double-click, or run `./build/swe30003_a3` in a terminal from the project root folder.
#### VSCode Setup
To get IntelliSense working:
1. Install the `C/C++` extension.
2. Open the root folder of the project in VSCode.
3. Press `CMD + Shift + P` and search for `C/C++: Edit Configurations (JSON)`. Clicking this should create a `c_cpp_properties.json` in `<project root>/.vscode` and open it.
4. In `c_cpp_properties.json`, add these entries to `includePath`:
    1. `"/usr/local/lib/**"`
    2. `"/usr/local/include/**"`
    3. `"/usr/local/include/gtk-4.0"`
    4. `"/usr/local/include/glib-2.0"`
5. Open `<project root>/main.cpp` to check that IntelliSense is working (message me if it doesn't work).

## Setup Database
To setup the database (if not already there).
1. Open the project root in a terminal of some kind.
2. Run `sqlite3` in the terminal.
3. Run `.open build/database.db` in the terminal.
4. Run `.read db_create.sql` in the terminal.
1. Run `.read db_populate.sql` in the terminal.