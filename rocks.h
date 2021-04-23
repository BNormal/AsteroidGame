#ifndef rocks_h
#define rocks_h

#define BIG_ROCK_SIZE 16
#define MEDIUM_ROCK_SIZE 8
#define SMALL_ROCK_SIZE 4

#define BIG_ROCK_SPIN 2
#define MEDIUM_ROCK_SPIN 5
#define SMALL_ROCK_SPIN 10

#include <list>
#include "flyingObject.h"

class Rocks : public FlyingObject
{
  public:
  Rocks() : direction(random(0, 1)), angle(random(0, 360)), collision(false) { setVelocity(Velocity(Point(1, 1))); }
   virtual void advance();
   void globalAdvance();
   virtual void draw();
   virtual void breakApart(std::list<Rocks*>& rocks);
   bool getDirection() const { return direction; }
   void setDirection(bool dir) { direction = dir; }
   int getAngle() const { return angle; }
   void setAngle(int angle) { this->angle = angle; }
   bool isCollision() const { return collision; }
   void setCollision(bool collision) { this->collision = collision; }
  private:
   bool direction;
   int angle;
   bool collision;
};

class SmallRock : public Rocks
{
  public:
   SmallRock(Point pos) { setPosition(pos); setSize(SMALL_ROCK_SIZE); }
      void advance()
         {
            setRotation(getRotation() + (getDirection() ? SMALL_ROCK_SPIN : SMALL_ROCK_SPIN * -1));
            globalAdvance();
         }
         void draw() {
            drawSmallAsteroid(getPosition(), getRotation());
         }
         void breakApart(std::list<Rocks*>& rocks) {}
};

class MediumRock : public Rocks
{
  public:
   MediumRock(Point pos) { setPosition(pos); setSize(MEDIUM_ROCK_SIZE); }
   void advance()
      {
           setRotation(getRotation() + (getDirection() ? MEDIUM_ROCK_SPIN : MEDIUM_ROCK_SPIN * -1));
           globalAdvance();
	}
      void draw() {
         drawMediumAsteroid(getPosition(), getRotation());
      }
      void breakApart(std::list<Rocks*>& rocks)
	{
           for (int i = 0; i < 2; i++)
           {
                   Rocks *pRock = new SmallRock(getPosition());
                   if (i == 0)
                   {
                           pRock->setVelocity(Velocity(Point(getVelocity().getDx() + 3, getVelocity().getDy())));
			}
                   else if (i == 1)
			{
                           pRock->setVelocity(Velocity(Point(getVelocity().getDx() - 3, getVelocity().getDy())));
			}
                   pRock->setAngle(getAngle());
			rocks.push_back(pRock);
		}
	}
};

class BigRock : public Rocks
{
  public:
   BigRock(Point pos) { setPosition(pos); setSize(BIG_ROCK_SIZE); }
   void advance()
      {
         setRotation(getRotation() + (getDirection() ? BIG_ROCK_SPIN : BIG_ROCK_SPIN * -1));
         globalAdvance();
      }
      void draw()
	{
           drawLargeAsteroid(getPosition(), getRotation());
        }
	void breakApart(std::list<Rocks*>& rocks)
	{
           for (int i = 0; i < 3; i++)
           {
			Rocks *pRock = new MediumRock(getPosition());
			if (i == 0)
			{
                           pRock->setVelocity(Velocity(Point(getVelocity().getDx(), getVelocity().getDy() + 1)));
			}
			else if (i == 1)
			{
                           pRock->setVelocity(Velocity(Point(getVelocity().getDx(), getVelocity().getDy() - 1)));
			}
			else
			{
                           pRock = new SmallRock(getPosition());
				pRock->setVelocity(Velocity(Point(getVelocity().getDx() + 2, getVelocity().getDy())));
			}
			pRock->setAngle(getAngle());
			rocks.push_back(pRock);
		}
	}
};

#endif /* rocks_h */
