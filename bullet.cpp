#include "bullet.h"

/***************************************
* BULLET :: ADVANCE
* Controls the type of bullets moves or
* action.
* type : 0 = Normal bullet that moves across the screen
*      : 1 = Exploding bullet when an asteroid breaks
*      : 2 = Stars that don't moves, but change brightness depending on lives
*      : 3 = Exploding bullet when ship breaks
***************************************/
void Bullet::advance()
{
	float defualtSpeed = sqrt(pow(getVelocity().getDx(), 2) +
                                  pow(getVelocity().getDy(), 2));
	float x = getPosition().getX() +
           (speed + defualtSpeed) * cos((getRotation() + 90) * PI / 180);
	float y = getPosition().getY() +
           (speed + defualtSpeed) * sin((getRotation() + 90) * PI / 180);
	distance++;
	if (x < -200)
           if (type == 0)
              x = 200;
           else
              kill();
	else if (x > 200)
           if(type == 0)
              x = -200;
           else
              kill();
	if (y < -200)
           if(type == 0)
              y = 200;
           else
              kill();
	else if (y > 200)
           if(type == 0)
              y = -200;
           else
              kill();
	setPosition(Point(x, y));
	if (distance >= 40 && type != 2)
           setLives(0);
	if (type == 2)
	{
           setLives(getLives() - random(1, 4));
           if (getLives() <= 0)
              setLives(250);
           if (getLives() <= 103 && getLives() >= 98)
              setPosition(Point(random(-200, 200), random(-200, 200)));
	}
}

/***************************************
* BULLET :: DRAW
* Bullets for the ships type of weapon and other colors for specific use.
* type : 0 = normal bullets with a weapon of choice
* (white dot, spinning star, random number, or a pizza)
*      : 1 = asteroids explosion dot with a brownish color
*      : 2 = white dot that gets dimmer and brighter for stars
*      : 3 = blue dot when ship explodes
***************************************/
void Bullet::draw()
{
	if (type == 0)
	{
		if (getWeapon() == 0)
			drawDot(getPosition());
		else if (getWeapon() == 1)
			drawSacredBird(getPosition(), 10);
		else if (getWeapon() == 2)
			drawNumber(getPosition(), random(0, 9));
		else if (getWeapon() == 3)
			drawPizza(getPosition(), getRotation());
	}
	else if (type == 1)
	{
		changeColor(random(0.5, 0.8), random(0.3, 0.6), 0.0);
		drawDot(getPosition());
		changeColor(1, 1, 1);
	}
	else if (type == 2)
	{
		float num = 0;
		if (getLives() >= 100)
			num = (getLives() - 100.0) / 100.0;
		else
			num = ((getLives() * -1 + 100) / 100.0);
		changeColor(num, num, num);
		drawDot(getPosition());
		changeColor(1, 1, 1);
	}
	else if (type == 3)
	{
		changeColor(0.0, 0.0, random(0.1, 1.0));
		drawDot(getPosition());
		changeColor(1, 1, 1);
	}
}

/***************************************
* BULLET :: SETWEAPON
* Changes the weapon type of bullet for
* the ship to shoot different objects
***************************************/
void Bullet::setWeapon(int weapon)
{
	if (weapon >= 4)
		weapon = 0;
	this->weapon = weapon;
}
