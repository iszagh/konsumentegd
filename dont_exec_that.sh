#!/bin/bash
#gcc single_startup.c -Wall -pedantic -pthread -o single_startup
#./single_startup

g++ sender.c -o sender  -Wall -pedantic -pthread -lsfml-graphics -lsfml-window -lsfml-system 
g++ receiver.c -o receiver  -Wall -pedantic -pthread -lsfml-graphics -lsfml-window -lsfml-system
