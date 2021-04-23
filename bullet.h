#ifndef bullet_h
#define bullet_h

#define BULLET_SPEED 5
#define BULLET_LIFE 40

#include "flyingObject.h"

/*************************************************************
 * BULLET :: BULLET
 * Handles flying dots or weapons for the space ship
 *************************************************************/
class Bullet : public FlyingObject
{
public:
	Bullet() : distance(0), speed(5), type(0), weapon(0) {}
	void advance();
	void draw();
	float getSpeed() const { return speed; }
	void setSpeed(float speed) { this->speed = speed; }
	void setType(int type) { this->type = type; }
	int getDistance() const { return distance; }
	void setDistance(int distance) { this->distance = distance; }
	void setWeapon(int weapon);
	int getWeapon() const { return weapon; }
private:
	int distance;
	float speed;
	int type;
	int weapon;
};




#endif /* bullet_h */
