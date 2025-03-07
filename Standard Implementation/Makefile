CC = mpicc                
CFLAGS = -Wall -O2        
LIBS = -lm   

SRC = knn-standard.c util.c
OBJ = knn-standard.o util.o
TARGET = kd    

NP_DEFAULT = 2            
NP_4 = 4                  
NP_8 = 8                  
NP_12 = 12                

# Compiling
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Running using NP = 2 --> make run2 n=1000	
run2: $(TARGET)
	mpirun -np $(NP_DEFAULT) --oversubscribe ./$(TARGET) $(n)
	rm -f $(OBJ) $(TARGET)

# Running using NP = 4 --> make run4 n=1000	
run4: $(TARGET)
	mpirun -np $(NP_4) --oversubscribe ./$(TARGET) $(n)
	rm -f $(OBJ) $(TARGET)

# Running using NP = 8 --> make run8 n=1000	
run8: $(TARGET)
	mpirun -np $(NP_8) --oversubscribe ./$(TARGET) $(n)
	rm -f $(OBJ) $(TARGET)

# Running using NP = 12 --> make run12 n=1000	
run12: $(TARGET)
	mpirun -np $(NP_12) --oversubscribe ./$(TARGET) $(n)
	rm -f $(OBJ) $(TARGET)
