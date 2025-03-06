
CC = gcc
CFLAGS = -Wall -O2
LIBS = -lm
TARGET = calcPerformance
SRC = calcPerformance.c

# Compile
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(LIBS)

# Run and clear stuff --> make run
run: $(TARGET)
	./$(TARGET) $(n)
	rm -f $(TARGET)	
