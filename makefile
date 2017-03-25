1730ed: main.o editor.o buffer.o
	g++ -lncurses -o 1730ed main.o buffer.o editor.o
    
main.o: main.cpp
	g++ -c -lncurses -Wall -std=c++14 -g -O0 -pedantic-errors main.cpp

editor.o: editor.cpp
	g++ -c -lncurses -Wall -std=c++14 -g -O0 -pedantic-errors editor.cpp

buffer.o: buffer.cpp 
	g++ -c -lncurses -Wall -std=c++14 -g -O0 -pedantic-errors buffer.cpp

clean:
	rm -f *.o
	rm -f 1730ed