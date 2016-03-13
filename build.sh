rm main.o
rm sfml-app
g++ -c receiver.c
g++ receiver.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
g++ sender.c -o sender  -Wall -pedantic -pthread -lsfml-graphics -lsfml-window -lsfml-system 
