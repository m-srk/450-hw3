CC=g++ 
CFLAGS= -O3 -pthread
DEPS =
OBJ = main.o calcSd.o
EXEC = sd_test
LIB = _sd_thread
LIB_DIR = sxm6373_hw3_part1

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -I. -I./$(LIB_DIR)

$(EXEC): $(OBJ)
	module load gcc; \
	cd $(LIB_DIR) && $(MAKE);
	module load gcc; \
	$(CC) -o $@ $^ $(CFLAGS) -L./$(LIB_DIR)/ -l$(LIB) -L.

.PHONY: clean
clean:
	cd $(LIB_DIR) && $(MAKE) clean
	rm -f $(OBJ) $(EXEC)
