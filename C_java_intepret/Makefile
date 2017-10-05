CXX = gcc
CXXFLAGS = -Wall -Wextra -pedantic -std=c11
OBJ = ial.o s_table.o code_gen.o error.o lex.o param.o stack.o funcpar.o parser.o main.o expr.o
DEPS = code_gen.h param.h parser.h s_table.h error.h lex.h stack.h funcpar.h interfaces.h ial.h expr.h

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

ifj16: $(OBJ)
		$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -rf *.o ifj16

zip:
	zip xflajs00 *.c *.h readme Makefile rozdeleni rozsireni dokumentace.pdf