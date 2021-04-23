#ifndef VELOCITY_H
#define VELOCITY_H

#include "point.h"

class Velocity
{
  public:
  Velocity() : pos(0.0, 0.0) {}
   Velocity(Point pos) { this->pos = pos; }
   float getDx() const { return pos.getX(); }
   float getDy() const { return pos.getY(); }
   void setDx(float x) { pos.setX(x); }
   void setDy(float y) { pos.setY(y); }
  private:
   Point pos;
};

#endif /* velocity_h */
