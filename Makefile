satPuzzle: clean File.o Proof.o Solver.o 15puzzle.o
	g++ -o $@ -g File.o Proof.o Solver.o 15puzzle.o

File.o: File.cpp
	g++ -c -g File.cpp

Proof.o: Proof.cpp
	g++ -c -g Proof.cpp

Solve.o: Solver.cpp
	g++ -c -g Solver.cpp

15puzzle.o: 15puzzle.cpp
	g++ -c -g 15puzzle.cpp

clean:
	rm -f *.o satTest tags
