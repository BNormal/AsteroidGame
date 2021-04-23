#include "ship.h"

/***************************************
* GAME :: DRAW
* Draws the ship and blue particles
***************************************/
void Ship::draw(Interface ui)
{
   for (std::list<Bullet*>::iterator trailIt = trail.begin(); trailIt != trail.end(); trailIt++)
   {
      changeColor(0, random(0.0, 0.5), random(0.0, 1.0));
      drawDot((*trailIt)->getPosition());
   }
   changeColor(1, 1, 1);
   if (isAlive())
   {
      drawShip(getPosition(), getRotation(), ui.isUp());
   }
}

/***************************************
* GAME :: ADVANCE
* Handles the ships movement and it handles
* the particles behind the ship
***************************************/
void Ship::advance()
{
   speed = sqrt(pow(getVelocity().getDx(), 2) + pow(getVelocity().getDy(), 2));
   setX(getPosition().getX() + getVelocity().getDx());
   setY(getPosition().getY() + getVelocity().getDy());
   std::list<Bullet*>::iterator trailIt = trail.begin();
   while (trailIt != trail.end())
   {
      Bullet* pTrail = *trailIt;
      pTrail->setLives(pTrail->getLives() - 1);
      pTrail->advance();
      if (!pTrail->isAlive())
      {
         delete pTrail;
         trailIt = trail.erase(trailIt);
      }
      else
         trailIt++;
   }
   if (isAlive())
   {
      Bullet *pTrail = new Bullet();
      pTrail->setLives(60);
      pTrail->setPosition(getPosition());
      pTrail->setRotation(getRotation() + (random(0, 1) ? random(120, 180) : random(120, 180) * -1));
      pTrail->setSpeed(0.5);
      trail.push_back(pTrail);
   }
}

/***************************************
* GAME :: THRUST
* Changes the ships velocity
***************************************/
void Ship::thrust()
{
   Velocity v = getVelocity();
   float xSpeed = v.getDx() + (cos((getRotation() + 90) * PI / 180) / 2);
   float ySpeed = v.getDy() + (sin((getRotation() + 90) * PI / 180) / 2);
   int limit = 30;
   if (xSpeed > limit)
      xSpeed = limit;
   else if (xSpeed < limit * -1)
      xSpeed = limit * -1;
   if (ySpeed > limit)
      ySpeed = limit;
   else if (ySpeed < limit * -1)
      ySpeed = limit * -1;
   v.setDx(xSpeed);
   v.setDy(ySpeed);
   setVelocity(v);
}

/***************************************
* GAME :: TURNRIGHT
* turn the ship towards the right
***************************************/
void Ship::turnRight()
{
   int rotation = getRotation() - 6;
   if (rotation == -360)
      rotation = 0;
   setRotation(rotation);
}

/***************************************
* GAME :: TURNLEFT
* turn the ship towards the left
***************************************/
void Ship::turnLeft()
{
   int rotation = getRotation() + 6;
   if (rotation == 360)
      rotation = 0;
   setRotation(rotation);
}

/***************************************
* GAME :: SETX
* sets the ships position and it handles
* the screen wrapping
***************************************/
void Ship::setX(float x)
{
   if (x > 200 + getSize())
      x = -200 - getSize();
   else if (x < -200 - getSize())
      x = 200 + getSize();
   setPosition(Point(x, getPosition().getY()));
}

/***************************************
* GAME :: SETY
* sets the ships position and it handles
* the screen wrapping
***************************************/
void Ship::setY(float y)
{
   if (y > 200 + getSize())
      y = -200 - getSize();
   else if (y < -200 - getSize())
      y = 200 + getSize();
   setPosition(Point(getPosition().getX(), y));
}
