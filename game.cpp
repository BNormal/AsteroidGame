/*********************************************************************
 * File: game.cpp
 * Author: Br. Burton
 *
 * This file is provided as the instructor's "half" of the project.
 *
 * Please DO NOT provide, share, or distribute this file to students
 * of other sections or semesters.
 *********************************************************************/

#include "game.h"
#include <limits>

#define WINDOW_X_SIZE 200
#define WINDOW_Y_SIZE 200

Point Game :: topLeft;
Point Game :: bottomRight;

/***************************************
* GAME :: MIN
* returns the smaller float of the two parameters
***************************************/
float Game::min(float d1, float d2) const
{
   return d1 < d2 ? d1 : d2;
}

/***************************************
* GAME :: MAX
* return the bigger float of the two parameters
***************************************/
float Game::max(float d1, float d2) const
{
   return d1 > d2 ? d1 : d2;
}

/***************************************
 * GAME :: getRandomPoint
 * Gets a random point within the boundaries of the world.
 ***************************************/
Point Game :: getRandomPoint() const
{
   int x = random(topLeft.getX(), bottomRight.getX());
   int y = random(bottomRight.getY(), topLeft.getY());
   Point p(x, y);  
   return p;
}


/***************************************
 * GAME :: ADVANCE
 * advance the game one unit of time
 ***************************************/
void Game :: advance()
{
   pShip->advance();
   for (list<Bullet*>::iterator starIt = stars.begin();
        starIt != stars.end();
        starIt++)
   {
      (*starIt)->advance();
   }
   for (list<Bullet*>::iterator debrisIt = debris.begin();
        debrisIt != debris.end();
        debrisIt++)
   {
      (*debrisIt)->advance();
   }
   for (list<Bullet*>::iterator bulletIt = bullets.begin();
        bulletIt != bullets.end();
        bulletIt++)
   {
      (*bulletIt)->advance();
   }
   for (list<Rocks*>::iterator rockIt = rocks.begin();
        rockIt != rocks.end();
        rockIt++)
   {
      (*rockIt)->advance();
   }
   checkForCollisions();
   cleanUpZombies();
   
}

/***************************************
 * GAME :: input
 * accept input from the user
 ***************************************/
void Game :: handleInput(const Interface & ui)
{
   if (pShip->isAlive())
   {
      if (ui.isLeft())
      {
         pShip->turnLeft();
      }
      
      if (ui.isRight())
      {
         pShip->turnRight();
      }
      
      if (ui.isUp())
      {
         pShip->thrust();
      }
      
      if (ui.isSpace())
      {
         Bullet* pBullet = new Bullet(*pShip);
         bullets.push_back(pBullet);
      }
      if (ui.isR())
         pShip->setWeapon(pShip->getWeapon() + 1);
   }
   if (ui.isDown())
      if (!pShip->isAlive())
      {
         pShip->setPosition(Point(0, 0));
         pShip->setVelocity(Velocity(Point(0, 0)));
         pShip->setLives(1);
      }
}

/*********************************************
 * GAME :: DRAW
 * Draw everything on the screen
 *********************************************/
void Game :: draw(const Interface & ui)
{
   pShip->draw(ui);

   //stars on screen
   for (list<Bullet*>::iterator starIt = stars.begin();
        starIt != stars.end();
        starIt++)
   {
      (*starIt)->draw();
   }

   //explosion on screen
   for (list<Bullet*>::iterator debrisIt = debris.begin();
        debrisIt != debris.end();
        debrisIt++)
   {
      (*debrisIt)->draw();
   }
   
   for (list<Bullet*>::iterator bulletIt = bullets.begin();
        bulletIt != bullets.end();
        bulletIt++)
   {
      (*bulletIt)->draw();
   }
   
   for (list<Rocks*>::iterator rockIt = rocks.begin();
        rockIt != rocks.end();
        rockIt++)
   {
      (*rockIt)->draw();
   }
   if (rocks.size() == 0)
   {
      drawFunny(Point(10, 50), 180);
      drawText(Point(-60, -50), "Thanks for playing :)");
      drawText(Point(-70, -70), "Stay classy Ercanbrack!");
      Rocks rock;
      rock.setPosition(Point(-5, 10));
      rock.setSize(2);
      createDebris(rock.getPosition(), rock.getSize(), 1);
   }
   else
   {
      //displays a message on the screen if there are any asteroids alive
      drawText(Point(-98, -170), "Shoot all the asteroids for the suprise!");
      drawText(Point(-150, -185), "Down arrow key to respawn and r key to switch weapon.");
   }
}

/*********************************************
 * GAME :: checkForCollisions
 * Check for collisions between any two objects.
 *********************************************/
void Game::checkForCollisions()
{
   // go through each rock
   for (list<Rocks*>::iterator rockIt = rocks.begin();
        rockIt != rocks.end();
        rockIt++)
   {
      int collisionCount = 0;
      for (list<Rocks*>::iterator rockIt2 = rocks.begin(); rockIt2 != rocks.end(); rockIt2++)
      {
         if (rockIt2 != rockIt)
         {
            if (isCollision(**rockIt2, **rockIt))
            {
               if ((*rockIt)->isCollision())
               {
                  (*rockIt)->setCollision(false);
                  (*rockIt)->setAngle((*rockIt)->getAngle() + 180);
                  (*rockIt2)->setAngle((*rockIt2)->getAngle() + random(-45, 45));
               }
               if (!(*rockIt)->isCollision())
                  collisionCount++;
            }
         }
      }
      if (!(*rockIt)->isCollision())
         if (collisionCount == 0)
            (*rockIt)->setCollision(true);
      // check for collision with the ship
      if (isCollision(*pShip, **rockIt))
      {
         pShip->kill();
         createDebris(pShip->getPosition(), pShip->getSize(), 3);
         (*rockIt)->kill();
         (*rockIt)->breakApart(rocks);
         createDebris((**rockIt).getPosition(), (**rockIt).getSize(), 1);
      }
      
      // go through each bullet
      for (list<Bullet*>::iterator bulletIt = bullets.begin();
           bulletIt != bullets.end();
           bulletIt++)
      {
         // check for collision between this rock and this bullet
         if (isCollision(**bulletIt, **rockIt))
         {
            (*bulletIt)->kill();
            (*rockIt)->kill();
            (*rockIt)->breakApart(rocks);
            createDebris((**rockIt).getPosition(), (**rockIt).getSize(), 1);
         }
      }
   }
}

/******************************************************
 * Function: isCollision
 * Description: Determine if two objects are colliding
 ******************************************************/
bool Game :: isCollision(const FlyingObject &obj1, const FlyingObject &obj2) const
{
   bool collision = false;
   
   // we only collide if we're both alive
   if (obj1.isAlive() && obj2.isAlive())
   {
      float diff = getClosestDistance(obj1, obj2);
      float tooClose = obj1.getSize() + obj2.getSize();
      
      if (diff < tooClose)
      {
         // we have a hit!
         collision = true;
      }
   }
   
   return collision;
}

/**********************************************************
 * Function: getClosestDistance
 * Description: Determine how close these two objects will
 * get in between the frames.
 **********************************************************/
float Game :: getClosestDistance(const FlyingObject &obj1, const FlyingObject &obj2) const
{
   // from Br. Helfrich:
   // find the maximum distance traveled
   float dMax = max(abs(obj1.getVelocity().getDx()), abs(obj1.getVelocity().getDy()));
   dMax = max(dMax, abs(obj2.getVelocity().getDx()));
   dMax = max(dMax, abs(obj2.getVelocity().getDy()));
   dMax = max(dMax, 0.1f); // when dx and dy are 0.0. Go through the loop once.
   
   // we will advance by this number
   float distMin = std::numeric_limits<float>::max();
   for (float i = 0.0; i <= dMax; i++)
   {
      Point point1(obj1.getPosition().getX() + (obj1.getVelocity().getDx() * i / dMax),
                   obj1.getPosition().getY() + (obj1.getVelocity().getDy() * i / dMax));
      Point point2(obj2.getPosition().getX() + (obj2.getVelocity().getDx() * i / dMax),
                   obj2.getPosition().getY() + (obj2.getVelocity().getDy() * i / dMax));
      
      float xDiff = point1.getX() - point2.getX();
      float yDiff = point1.getY() - point2.getY();
      
      float distSquared = (xDiff * xDiff) +(yDiff * yDiff);
      
      distMin = min(distMin, distSquared);
   }
   return sqrt(distMin);
}

/*********************************************
 * GAME :: cleanUpZombies()
 * Look for and remove any objects that are dead.
 *********************************************/
void Game::cleanUpZombies()
{
   // Look for dead debris
   list<Bullet*>::iterator debrisIt = debris.begin();
   while (debrisIt != debris.end())
   {
      Bullet* pdebris = *debrisIt;
      
      if (!pdebris->isAlive())
      {
         // first deallocate
         delete pdebris;
         
         // now remove from list and advance
         debrisIt = debris.erase(debrisIt);
      }
      else
      {
         debrisIt++; // advance
      }
   }
   
   // Look for dead bullets
   list<Bullet*>::iterator bulletIt = bullets.begin();
   while (bulletIt != bullets.end())
   {
      Bullet* pBullet = *bulletIt;
      
      if (!pBullet->isAlive())
      {
         // first deallocate
         delete pBullet;
         
         // now remove from list and advance
         bulletIt = bullets.erase(bulletIt);
      }
      else
      {
         bulletIt++; // advance
      }
   }
   
   // Look for dead rocks
   list<Rocks*>::iterator rockIt = rocks.begin();
   while (rockIt != rocks.end())
   {
      Rocks* pRock = *rockIt;
      
      if (!pRock->isAlive())
      {
         // first deallocate
         delete pRock;
         
         // now remove from list and advance
         rockIt = rocks.erase(rockIt);
      }
      else
      {
         rockIt++; // advance
      }
   }
}

/***************************************
* GAME :: CREATEDEBRIS
* Creates a list of bullets that will be\
* used to show an explosion on screen.
***************************************/
void Game::createDebris(Point point, int size, int type)
{
   for (int i = 0; i < size * 15; i++)
   {
      Bullet *pDebris = new Bullet();
      pDebris->setPosition(point);
      pDebris->setRotation(random(0, 360));
      pDebris->setSpeed(random(0.1, 3.0));
      pDebris->setDistance(random(-10, 10));
      pDebris->setType(type);
      debris.push_back(pDebris);
   }
}


/*************************************
 * All the interesting work happens here, when
 * I get called back from OpenGL to draw a frame.
 * When I am finished drawing, then the graphics
 * engine will wait until the proper amount of
 * time has passed and put the drawing on the screen.
 **************************************/
void callBack(const Interface *pUI, void *p)
{
   Game *pGame = (Game *)p;
   
   pGame->advance();
   pGame->handleInput(*pUI);
   pGame->draw(*pUI);
}


/*********************************
 * Main is pretty sparse.  Just initialize
 * the game and call the display engine.
 * That is all!
 *********************************/
int main(int argc, char ** argv)
{
   Point topLeft(-WINDOW_X_SIZE, WINDOW_Y_SIZE);
   Point bottomRight(WINDOW_X_SIZE, -WINDOW_Y_SIZE);

   Interface ui(argc, argv, "Asteroids", topLeft, bottomRight);
   Game game(topLeft, bottomRight);
   ui.run(callBack, &game);
   
   return 0;
}
