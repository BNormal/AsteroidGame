#ifndef flyingObject_h
#define flyingObject_h

#include <cmath>
#include "velocity.h"
#include "uiDraw.h"

#define PI (atan(1) * 4) //The value of pi

/***************************************************************
 * FLYINGOBJECT :: FLYINGOBJECT
 * handles any moving object on the screen. It's the parent class
 * for the ship, asteroids and bullets.
 **************************************************************/
class FlyingObject
{
public:
	FlyingObject() : rotation(0), lives(1) {}
	void setLives(int lives) { this->lives = lives; }
	int getLives() const { return lives; }
	bool isAlive() const { return lives; }
	void kill() { setLives(0); }
	Velocity getVelocity() const { return velocity; }
	void setVelocity(Velocity v) { velocity = v; }
	Point getPosition() const { return pos; }
	void setPosition(Point pos) { this->pos = pos; }
	int getRotation() const { return rotation; }
	void setRotation(int rotation) { this->rotation = rotation; }
	int getSize() const { return size; }
	void setSize(int size) { this->size = size; }
private:
	int lives;
	Velocity velocity;
	Point pos;
	int size;
	int rotation;
};




#endif /* flyingObject_h */
