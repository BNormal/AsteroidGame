#include "rocks.h"

using namespace std;

/***************************************
* GAME :: ADVANCE
* defualt advance function
***************************************/
void Rocks::advance()
{

}

/***************************************
* GAME :: GLOBALADVANCE
* a function that any inherited class will use
***************************************/
void Rocks::globalAdvance()
{
	if (getRotation() >= 360)
		setRotation(0);
	else if (getRotation() <= 0)
		setRotation(360);
	float x = getPosition().getX() + getVelocity().getDx() * cos((getAngle() + 90) * PI / 180);
	float y = getPosition().getY() + getVelocity().getDy() * sin((getAngle() + 90) * PI / 180);
	if (x < -200 - getSize())
	{
		x = 200 + getSize();
		y = y * -1;
	}
	else if (x > 200 + getSize())
	{
		x = -200 - getSize();
		y = y * -1;
	}
	if (y < -200 - getSize())
	{
		y = 200 + getSize();
		x = x * -1;
	}
	else if (y > 200 + getSize())
	{
		y = -200 - getSize();
		x = x * -1;
	}
	setPosition(Point(x, y));
}

/***************************************
* GAME :: DRAW
* default draw function
***************************************/
void Rocks::draw()
{

}

/***************************************
* GAME :: BREAKAPART
* default break apart function
***************************************/
void Rocks::breakApart(list<Rocks*>& rocks)
{

}