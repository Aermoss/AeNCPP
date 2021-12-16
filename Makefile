all:
	g++ -o bin/aencpp src/*.cpp -lmingw32
	bin\aencpp test.ncpp