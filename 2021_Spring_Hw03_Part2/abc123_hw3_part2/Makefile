CC = g++
CFLAGS = -I. -O3 -mavx
DEPS = hw3_part2.hpp
OBJ = hw3_part2.o
LIB = lib_hw3_part2.a

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(LIB): $(OBJ)
	ar rcs $@ $(OBJ)
	
.PHONY: clean
clean:
	rm -f $(OBJ) $(LIB)
