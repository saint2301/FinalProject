CC = gcc
CFLAGS = -Wall -g $(shell pkg-config --cflags gtk+-3.0)
LIBS = $(shell pkg-config --libs gtk+-3.0)

CLI_CFLAGS = -Wall -g
CLI_LIBS =

GUI_TARGET = output/program
CLI_TARGET = output/program_cli

GUI_SRCS = gui.c admin_command.c Customer.c Integration.c Inventory.c orders.c
CLI_SRCS = main.c admin_command.c Customer.c Integration.c Inventory.c orders.c

all: $(GUI_TARGET) $(CLI_TARGET)

$(GUI_TARGET): $(GUI_SRCS)
	$(CC) $(CFLAGS) $(GUI_SRCS) -o $(GUI_TARGET) $(LIBS)

$(CLI_TARGET): $(CLI_SRCS)
	$(CC) $(CLI_CFLAGS) $(CLI_SRCS) -o $(CLI_TARGET) $(CLI_LIBS)

clean:
	del output\program.exe output\program_cli.exe