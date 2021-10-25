main: main.cpp
	g++ -std=c++11 main.cpp -o main display.cpp -lglut -lGL -lGLU `pkg-config opencv --libs`
create_text_image: create_text_image.cpp
	g++ -std=c++11 create_text_image.cpp -o create_text_image -lglut -lGL -lGLU `pkg-config opencv --libs`
display: display.cpp
	g++ -std=c++11 display.cpp -o display main.cpp -lglut -lGL -lGLU `pkg-config opencv --libs`