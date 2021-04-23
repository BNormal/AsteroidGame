###############################################################
# Program:
#     Project 13, Asteroids
#     Brother Ercanbrack, CS165
# Author:
#     Benson Norman
# Summary:
#     Creates a spacecraft will particles and there are five
#     large asteroids. You need to shoot the asteroids which
#     will break into smaller pieces with different velocitys
#     and angles. 
# Above and Beyond
#    Added particles behind the ship, the thruster fires when
#    holding down the space bar, you can press the space bar
#    to change weapons (cosmetics only), the asteroids will
#    bounce off each other, there are stars in the background 
#    grow brighter then dim and when they get really dark they
#    move to another new location.
###############################################################


LFLAGS = -lglut -lGLU -lGL

###############################################################
# Build the main game
###############################################################
a.out: game.o uiInteract.o uiDraw.o point.o velocity.o flyingObject.o ship.o bullet.o rocks.o
	g++ game.o uiInteract.o uiDraw.o point.o velocity.o flyingObject.o ship.o bullet.o rocks.o $(LFLAGS)
	tar -cf asteroids.tar makefile *.cpp *.h
###############################################################
# Individual files
#    uiDraw.o       Draw polygons on the screen and do all OpenGL graphics
#    uiInteract.o   Handles input events
#    point.o        The position on the screen
#    game.o         Handles the game interaction
#    velocity.o     Velocity (speed and direction)
#    flyingObject.o Base class for all flying objects
#    ship.o         The player's ship
#    bullet.o       The bullets fired from the ship
#    rocks.o        Contains all of the Rock classes
###############################################################
uiDraw.o: uiDraw.cpp uiDraw.h
	g++ -c uiDraw.cpp

uiInteract.o: uiInteract.cpp uiInteract.h
	g++ -c uiInteract.cpp

point.o: point.cpp point.h
	g++ -c point.cpp

game.o: game.cpp game.h uiDraw.h uiInteract.h point.h flyingObject.h bullet.h rocks.h ship.h
	g++ -c game.cpp

velocity.o: velocity.cpp velocity.h point.h
	g++ -c velocity.cpp

flyingObject.o: flyingObject.cpp flyingObject.h velocity.h uiDraw.h
	g++ -c flyingObject.cpp

ship.o: ship.cpp ship.h bullet.h uiInteract.h
	g++ -c ship.cpp

bullet.o: bullet.cpp bullet.h flyingObject.h
	g++ -c bullet.cpp

rocks.o: rocks.cpp rocks.h flyingObject.h
	g++ -c rocks.cpp


###############################################################
# General rules
###############################################################
clean:
	rm a.out *.o
