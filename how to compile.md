How to Compile
Requirements
GCC (via MSYS2 MinGW 64-bit)
GTK3 library

Install GTK3 (first time only)
Open MSYS2 MinGW 64-bit terminal and run:
pacman -S mingw-w64-x86_64-gtk3

Compile (Manual)
Open MSYS2 MinGW 64-bit terminal, navigate to the project folder:
cd /c/Users/YourName/C_Projects/FinalProject-main

Then run:
gcc gui.c admin_command.c Customer.c Integration.c Inventory.c orders.c 
$(pkg-config --cflags --libs gtk+-3.0) 
-o output/program

Compile (Using Makefile)
make

Run
./output/program.exe
or
./output/program_cli.exe