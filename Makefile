all:
	g++ -std=c++0x -Wall main.cpp -o astar.out

clean:
	$(rm) astar.out
