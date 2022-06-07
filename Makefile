# Compiler Option, Set to "gcc" for C compilation, "g++" for C++ compilation
CC = gcc

# Target to build to (name.exe)
TARGET = socket

# Source files
SRC = sockets.c

# Compiler Flags
CFLAGS = -pthread

make:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

run:
	./$(TARGET)	$(mode) $(serverPort)