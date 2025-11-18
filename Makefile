TARGET = bin/a.out
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))



run: clean default
	./$(TARGET) -f ./mynewdb.db -n
	./$(TARGET) -f ./mynewdb.db
	

default: $(TARGET)

clean-all: clean
	rm ./mynewdb.db

clean:
	rm -rf obj/*.o
	rm -rf bin/*.out


$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude
