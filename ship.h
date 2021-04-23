#ifndef ship_h
#define ship_h

#define SHIP_SIZE 10

#define ROTATE_AMOUNT 6
#define THRUST_AMOUNT 0.5

#include "bullet.h"
#include "uiInteract.h"
#include <list>

/*********************************************************************
 * SHIP :: SHIP
 * The space craft that you control as a player
 *********************************************************************/
class Ship : public Bullet
{
  public:
   Ship() { setSize(10); }
   void advance();
   void draw(Interface ui);
   void thrust();
   void turnRight();
   void turnLeft();
   void setX(float x);
   void setY(float y);
  private:
   float speed;
   std::list<Bullet*> trail;
};

#endif /* ship_h */
