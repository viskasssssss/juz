exec = juzc.out
sources = $(wildcard src/*.c)
objects = $(sources:src=work.c=.o)
flags = -g -Iinclude

$(exec): $(objects)
	gcc $(objects) $(flags) -o $(exec)

src/%.o: src/%.c include/%.h
	gcc -c $(flags) $< -o $@

clean:
	-rm -f $(exec)
	-rm -f src/*.o