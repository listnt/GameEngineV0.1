.PHONY: compile_mac
compile_mac:
	g++ -O3 -std=c++17 **.cpp -o main -framework GLUT -framework OpenGL -framework Cocoa