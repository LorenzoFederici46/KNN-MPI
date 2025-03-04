
CC = gcc
CFLAGS = -Wall -O2
LIBS = -lm
TARGET = sequential
SRC = sequential.c

# Compile
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(LIBS)

# Run and clear stuff --> make run n=1000	
run: $(TARGET)
	./$(TARGET) $(n)
	rm -f $(TARGET)	
