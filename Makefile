main: main.o 
	clang++ -o main main.o  -lncurses

main.o: ./main.cpp
	clang++ -c -Wno-unused-value ./main.cpp

clear:
	rm *.o main
