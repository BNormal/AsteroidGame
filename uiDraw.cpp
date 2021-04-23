/***********************************************************************
 * Source File:
 *    User Interface Draw : put pixels on the screen
 * Author:
 *    Br. Helfrich
 * Summary:
 *    This is the code necessary to draw on the screen. We have a collection
 *    of procedural functions here because each draw function does not
 *    retain state. In other words, they are verbs (functions), not nouns
 *    (variables) or a mixture (objects)
 ************************************************************************/

#include <string>     // need you ask?
#include <sstream>    // convert an integer into text
#include <cassert>    // I feel the need... the need for asserts
#include <time.h>     // for clock


#ifdef __APPLE__
#include <openGL/gl.h>    // Main OpenGL library
#include <GLUT/glut.h>    // Second OpenGL library
#endif // __APPLE__

#ifdef __linux__
#include <GL/gl.h>        // Main OpenGL library
#include <GL/glut.h>      // Second OpenGL library
#endif // __linux__

#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>         // OpenGL library we copied 
#define _USE_MATH_DEFINES
#include <math.h>
#endif // _WIN32

#include "point.h"
#include "uiDraw.h"

using namespace std;

#define deg2rad(value) ((M_PI / 180) * (value))

/*********************************************
 * NUMBER OUTLINES
 * We are drawing the text for score and things
 * like that by hand to make it look "old school."
 * These are how we render each individual charactger.
 * Note how -1 indicates "done".  These are paired
 * coordinates where the even are the x and the odd
 * are the y and every 2 pairs represents a point
 ********************************************/
const char NUMBER_OUTLINES[10][20] =
{
  {0, 0,  7, 0,   7, 0,  7,10,   7,10,  0,10,   0,10,  0, 0,  -1,-1, -1,-1},//0
  {7, 0,  7,10,  -1,-1, -1,-1,  -1,-1, -1,-1,  -1,-1, -1,-1,  -1,-1, -1,-1},//1
  {0, 0,  7, 0,   7, 0,  7, 5,   7, 5,  0, 5,   0, 5,  0,10,   0,10,  7,10},//2
  {0, 0,  7, 0,   7, 0,  7,10,   7,10,  0,10,   4, 5,  7, 5,  -1,-1, -1,-1},//3
  {0, 0,  0, 5,   0, 5,  7, 5,   7, 0,  7,10,  -1,-1, -1,-1,  -1,-1, -1,-1},//4
  {7, 0,  0, 0,   0, 0,  0, 5,   0, 5,  7, 5,   7, 5,  7,10,   7,10,  0,10},//5
  {7, 0,  0, 0,   0, 0,  0,10,   0,10,  7,10,   7,10,  7, 5,   7, 5,  0, 5},//6
  {0, 0,  7, 0,   7, 0,  7,10,  -1,-1, -1,-1,  -1,-1, -1,-1,  -1,-1, -1,-1},//7
  {0, 0,  7, 0,   0, 5,  7, 5,   0,10,  7,10,   0, 0,  0,10,   7, 0,  7,10},//8
  {0, 0,  7, 0,   7, 0,  7,10,   0, 0,  0, 5,   0, 5,  7, 5,  -1,-1, -1,-1} //9
};

/************************************************************************
 * DRAW DIGIT
 * Draw a single digit in the old school line drawing style.  The
 * size of the glyph is 8x11 or x+(0..7), y+(0..10)
 *   INPUT  topLeft   The top left corner of the character
 *          digit     The digit we are rendering: '0' .. '9'
 *************************************************************************/
void drawDigit(const Point & topLeft, char digit)
{
   // we better be only drawing digits
   assert(isdigit(digit));
   if (!isdigit(digit))
      return;

   // compute the row as specified by the digit
   int r = digit - '0';
   assert(r >= 0 && r <= 9);

   // go through each segment.
   for (int c = 0; c < 20 && NUMBER_OUTLINES[r][c] != -1; c += 4)
   {
      assert(NUMBER_OUTLINES[r][c    ] != -1 &&
             NUMBER_OUTLINES[r][c + 1] != -1 &&
             NUMBER_OUTLINES[r][c + 2] != -1 &&
             NUMBER_OUTLINES[r][c + 3] != -1);

      //Draw a line based off of the num structure for each number
      Point start;
      start.setX(topLeft.getX() + NUMBER_OUTLINES[r][c]);
      start.setY(topLeft.getY() - NUMBER_OUTLINES[r][c + 1]);
      Point end;
      end.setX(topLeft.getX() + NUMBER_OUTLINES[r][c + 2]);
      end.setY(topLeft.getY() - NUMBER_OUTLINES[r][c + 3]);
         
      drawLine(start, end);
   }
}

/*************************************************************************
 * DRAW NUMBER
 * Display an integer on the screen using the 7-segment method
 *   INPUT  topLeft   The top left corner of the character
 *          digit     The digit we are rendering: '0' .. '9'
 *************************************************************************/
void drawNumber(const Point & topLeft, int number)
{
   // our cursor, if you will. It will advance as we output digits
   Point point = topLeft;
   
   // is this negative
   bool isNegative = (number < 0);
   number *= (isNegative ? -1 : 1);
   
   // render the number as text
   ostringstream sout;
   sout << number;
   string text = sout.str();

   // handle the negative
   if (isNegative)
   {
      glBegin(GL_LINES);
      glVertex2f(point.getX() + 1, point.getY() - 5);
      glVertex2f(point.getX() + 5, point.getY() - 5);
      glEnd();
      point.addX(11);
   }
   
   // walk through the text one digit at a time
   for (const char *p = text.c_str(); *p; p++)
   {
      assert(isdigit(*p));
      drawDigit(point, *p);
      point.addX(11);
   }
}


/*************************************************************************
 * DRAW TEXT
 * Draw text using a simple bitmap font
 *   INPUT  topLeft   The top left corner of the text
 *          text      The text to be displayed
 ************************************************************************/
void drawText(const Point & topLeft, const char * text)
{
   void *pFont = GLUT_BITMAP_HELVETICA_12;  // also try _18

   // prepare to draw the text from the top-left corner
   glRasterPos2f(topLeft.getX(), topLeft.getY());

   // loop through the text
   for (const char *p = text; *p; p++)
      glutBitmapCharacter(pFont, *p);
}

/************************************************************************
 * DRAW POLYGON
 * Draw a POLYGON from a given location (center) of a given size (radius).
 *  INPUT   center   Center of the polygon
 *          radius   Size of the polygon
 *          points   How many points will we draw it.  Larger the number,
 *                   the more line segments we will use
 *          rotation True circles are rotation independent.  However, if you
 *                   are drawing a 3-sided polygon (triangle), this matters!
 *************************************************************************/
void drawPolygon(const Point & center, int radius, int points, int rotation)
{
   // begin drawing
   glBegin(GL_LINE_LOOP);

   //loop around a circle the given number of times drawing a line from
   //one point to the next
   for (double i = 0; i < 2 * M_PI; i += (2 * M_PI) / points)
   {
      Point temp(false /*check*/);
      temp.setX(center.getX() + (radius * cos(i)));
      temp.setY(center.getY() + (radius * sin(i)));
      rotate(temp, center, rotation);
      glVertex2f(temp.getX(), temp.getY());
   }

   // complete drawing
   glEnd();

}


/************************************************************************
 * ROTATE
 * Rotate a given point (point) around a given origin (center) by a given
 * number of degrees (angle).
 *    INPUT  point    The point to be moved
 *           center   The center point we will rotate around
 *           rotation Rotation in degrees
 *    OUTPUT point    The new position
 *************************************************************************/
void rotate(Point & point, const Point & origin, int rotation)
{
   // because sine and cosine are expensive, we want to call them only once
   double cosA = cos(deg2rad(rotation));
   double sinA = sin(deg2rad(rotation));

   // remember our original point
   Point tmp(false /*check*/);
   tmp.setX(point.getX() - origin.getX());
   tmp.setY(point.getY() - origin.getY());

   // find the new values
   point.setX(static_cast<int> (tmp.getX() * cosA -
                                tmp.getY() * sinA) +
              origin.getX());
   point.setY(static_cast<int> (tmp.getX() * sinA +
                                tmp.getY() * cosA) +
              origin.getY());
}

Point getRotate(Point & point, const Point & origin, int rotation)
{
	// because sine and cosine are expensive, we want to call them only once
	double cosA = cos(deg2rad(rotation));
	double sinA = sin(deg2rad(rotation));

	// remember our original point
	Point tmp(false /*check*/);
	tmp.setX(point.getX() - origin.getX());
	tmp.setY(point.getY() - origin.getY());

	// find the new values
	point.setX(static_cast<int> (tmp.getX() * cosA -
		tmp.getY() * sinA) +
		origin.getX());
	point.setY(static_cast<int> (tmp.getX() * sinA +
		tmp.getY() * cosA) +
		origin.getY());
	return point;
}

/************************************************************************
 * DRAW LINE
 * Draw a line on the screen from the beginning to the end.
 *   INPUT  begin     The position of the beginning of the line
 *          end       The position of the end of the line
 *************************************************************************/
void drawLine(const Point & begin, const Point & end,
              float red, float green, float blue)
{
   // Get ready...
   glBegin(GL_LINES);
   glColor3f(red, green, blue);

   // Draw the actual line
   glVertex2f(begin.getX(), begin.getY());
   glVertex2f(  end.getX(),   end.getY());

   // Complete drawing
   glColor3f(1.0 /* red % */, 1.0 /* green % */, 1.0 /* blue % */);
   glEnd();
}

/***********************************************************************
 * DRAW Lander
 * Draw a moon-lander spaceship on the screen at a given point
 ***********************************************************************/
void drawLander(const Point & point)
{
   // ultra simple point
   struct PT
   {
      int x;
      int y;
   } points[] = 
   {
      {-6, 0}, {-10,0}, {-8, 0}, {-8, 3},  // left foot
      {-5, 4}, {-5, 7}, {-8, 3}, {-5, 4},  // left leg
      {-1, 4}, {-3, 2}, { 3, 2}, { 1, 4}, {-1, 4}, // bottom
      { 5, 4}, { 5, 7}, {-5, 7}, {-3, 7},  // engine square
      {-6,10}, {-6,13}, {-3,16}, { 3,16},   // left of habitat
      { 6,13}, { 6,10}, { 3, 7}, { 5, 7},   // right of habitat
      { 5, 4}, { 8, 3}, { 5, 7}, { 5, 4},  // right leg
      { 8, 3}, { 8, 0}, {10, 0}, { 6, 0}   // right foot
   };

   // draw it
   glBegin(GL_LINE_STRIP);
   for (int i = 0; i < sizeof(points) / sizeof(points[0]); i++)
        glVertex2f(point.getX() + points[i].x,
                   point.getY() + points[i].y);

   // complete drawing
   glEnd();
   
   
}


/***********************************************************************
 * DRAW Lander Flame
 * Draw the flames coming out of a moonlander for thrust
 ***********************************************************************/
void drawLanderFlames(const Point & point, 
                      bool bottom,
                      bool left,
                      bool right)
{
   // simple point
   struct PT
   {
      int x;
      int y;
   };

   int iFlame = random(0, 3);  // so the flame flickers
   
   // draw it
   glBegin(GL_LINE_LOOP);
   glColor3f(1.0 /* red % */, 0.0 /* green % */, 0.0 /* blue % */);
   
   // bottom thrust
   if (bottom)
   {
      PT points[3][3] =
      {
         { {-5,  -6}, { 0, -1}, { 3, -10} },
         { {-3,  -6}, {-1, -2}, { 0, -15} },
         { { 2, -12}, { 1,  0}, { 6,  -4} }
      };
      
      glVertex2f(point.getX() - 2, point.getY() + 2);
      for (int i = 0; i < 3; i++)
         glVertex2f(point.getX() + points[iFlame][i].x,
                   point.getY() + points[iFlame][i].y);
      glVertex2f(point.getX() + 2, point.getY() + 2);
   }

   // right thrust
   if (right)
   {
      PT points[3][3] =
      {
         { {10, 14}, { 8, 12}, {12, 12} },
         { {12, 10}, { 8, 10}, {10,  8} },
         { {14, 11}, {14, 11}, {14, 11} }
      };
      
      glVertex2f(point.getX() + 6, point.getY() + 12);
      for (int i = 0; i < 3; i++)
         glVertex2f(point.getX() + points[iFlame][i].x,
                    point.getY() + points[iFlame][i].y);
      glVertex2f(point.getX() + 6, point.getY() + 10);
   }

   // left thrust
   if (left)
   {
      PT points[3][3] =
      {
         { {-10, 14}, { -8, 12}, {-12, 12} },
         { {-12, 10}, { -8, 10}, {-10,  8} },
         { {-14, 11}, {-14, 11}, {-14, 11} }
      };
      
      glVertex2f(point.getX() - 6, point.getY() + 12);
      for (int i = 0; i < 3; i++)
         glVertex2f(point.getX() + points[iFlame][i].x,
                    point.getY() + points[iFlame][i].y);
      glVertex2f(point.getX() - 6, point.getY() + 10);
   }

   glColor3f(1.0 /* red % */, 1.0 /* green % */, 1.0 /* blue % */);
   glEnd();
}


/******************************************************************
 * RANDOM
 * This function generates a random number.  
 *
 *    INPUT:   min, max : The number of values (min <= num <= max)
 *    OUTPUT   <return> : Return the integer
 ****************************************************************/
int random(int min, int max)
{
   assert(min <= max);
   int num = (rand() % (max + 1 - min)) + min;
   assert(min <= num && num <= max);

   return num;
}

/******************************************************************
 * RANDOM
 * This function generates a random number.  
 *
 *    INPUT:   min, max : The number of values (min <= num <= max)
 *    OUTPUT   <return> : Return the double
 ****************************************************************/
double random(double min, double max)
{
   assert(min <= max);
   double num = min + ((double)rand() / (double)RAND_MAX * (max - min));
   
   assert(min <= num && num <= max);

   return num;
}


/************************************************************************
 * DRAW RECTANGLE
 * Draw a rectangle on the screen centered on a given point (center) of
 * a given size (width, height), and at a given orientation (rotation)
 *  INPUT  center    Center of the rectangle
 *         width     Horizontal size
 *         height    Vertical size
 *         rotation  Orientation
 *************************************************************************/
void drawRect(const Point & center, int width, int height, int rotation)
{
   Point tl(false /*check*/); // top left
   Point tr(false /*check*/); // top right 
   Point bl(false /*check*/); // bottom left
   Point br(false /*check*/); // bottom right

   //Top Left point
   tl.setX(center.getX() - (width  / 2));
   tl.setY(center.getY() + (height / 2));

   //Top right point
   tr.setX(center.getX() + (width  / 2));
   tr.setY(center.getY() + (height / 2));

   //Bottom left point
   bl.setX(center.getX() - (width  / 2));
   bl.setY(center.getY() - (height / 2));

   //Bottom right point
   br.setX(center.getX() + (width  / 2));
   br.setY(center.getY() - (height / 2));

   //Rotate all points the given degrees
   rotate(tl, center, rotation);
   rotate(tr, center, rotation);
   rotate(bl, center, rotation);
   rotate(br, center, rotation);

   //Finally draw the rectangle
   glBegin(GL_LINE_STRIP);
   glVertex2f(tl.getX(), tl.getY());
   glVertex2f(tr.getX(), tr.getY());
   glVertex2f(br.getX(), br.getY());
   glVertex2f(bl.getX(), bl.getY());
   glVertex2f(tl.getX(), tl.getY());
   glEnd();
}

/************************************************************************
 * DRAW CIRCLE
 * Draw a circle from a given location (center) of a given size (radius).
 *  INPUT   center   Center of the circle
 *          radius   Size of the circle
 *************************************************************************/
void drawCircle(const Point & center, int radius)
{
   assert(radius > 1.0);
   const double increment = 1.0 / (double)radius;

   // begin drawing
   glBegin(GL_LINE_LOOP);

   // go around the circle
   for (double radians = 0; radians < M_PI * 2.0; radians += increment)
      glVertex2f(center.getX() + (radius * cos(radians)),
                 center.getY() + (radius * sin(radians)));
   
   // complete drawing
   glEnd();   
}

/************************************************************************
 * DRAW DOT
 * Draw a single point on the screen, 2 pixels by 2 pixels
 *  INPUT point   The position of the dow
 *************************************************************************/
void drawDot(const Point & point)
{
   // Get ready, get set...
   glBegin(GL_POINTS);

   // Go...
   glVertex2f(point.getX(),     point.getY()    );
   glVertex2f(point.getX() + 1, point.getY()    );
   glVertex2f(point.getX() + 1, point.getY() + 1);
   glVertex2f(point.getX(),     point.getY() + 1);

   // Done!  OK, that was a bit too dramatic
   glEnd();
}

/************************************************************************
 * DRAW Tough Bird
 * Draw a tough bird on the screen
 *  INPUT point   The position of the sacred
 *        radius  The size of the bird
 *        hits    How many its remaining to kill the bird 
 *************************************************************************/
void drawToughBird(const Point & center, float radius, int hits)
{
   assert(radius > 1.0);
   const double increment = M_PI / 6.0;
   
   // begin drawing
   glBegin(GL_TRIANGLES);   

   // three points: center, pt1, pt2
   Point pt1(false /*check*/);
   pt1.setX(center.getX() + (radius * cos(0.0)));
   pt1.setY(center.getY() + (radius * sin(0.0)));   
   Point pt2(pt1);

   // go around the circle
   for (double radians = increment;
        radians <= M_PI * 2.0 + .5;
        radians += increment)
   {
      pt2.setX(center.getX() + (radius * cos(radians)));
      pt2.setY(center.getY() + (radius * sin(radians)));

      glVertex2f(center.getX(), center.getY());
      glVertex2f(pt1.getX(),    pt1.getY()   );
      glVertex2f(pt2.getX(),    pt2.getY()   );
      
      pt1 = pt2;
   }
      
   // complete drawing
   glEnd();   

   // draw the score in the center
   if (hits > 0 && hits < 10)
   {
      glColor3f(0.0 /* red % */, 0.0 /* green % */, 0.0 /* blue % */);
      glRasterPos2f(center.getX() - 4, center.getY() - 3);
      glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (char)(hits + '0'));
      glColor3f(1.0, 1.0, 1.0); // reset to white
   }
}

/************************************************************************
 * DRAW Sacred Bird
 * Draw a sacred bird on the screen
 *  INPUT point   The position of the sacred
 *        radius  The size of the bird
 *************************************************************************/
void drawSacredBird(const Point & center, float radius)
{
   // handle auto-rotation
   static float rotation = 0.0;   
   rotation += 5.0;

   
   // begin drawing
   glBegin(GL_LINE_LOOP);
   glColor3f(1.0 /* red % */, 0.0 /* green % */, 0.0 /* blue % */);

   
   //loop around a circle the given number of times drawing a line from
   //one point to the next
   for (int i = 0; i < 5; i++)
   {
      Point temp(false /*check*/);
      float radian = (float)i * (M_PI * 2.0) * 0.4;
      temp.setX(center.getX() + (radius * cos(radian)));
      temp.setY(center.getY() + (radius * sin(radian)));
      rotate(temp, center, rotation);
      glVertex2f(temp.getX(), temp.getY());
   }
   
   // complete drawing
   glColor3f(1.0, 1.0, 1.0); // reset to white
   glEnd();   
}

/**********************************************************************
 * DRAW SMALL ASTEROID
 **********************************************************************/
void drawSmallAsteroid( const Point & center, int rotation)
{
   // ultra simple point
   struct PT
   {
      int x;
      int y;
   } points[] = 
   {
      {-5, 9},  {4, 8},   {8, 4},   
      {8, -5},  {-2, -8}, {-2, -3}, 
      {-8, -4}, {-8, 4},  {-5, 10}
   };
   
   glBegin(GL_LINE_STRIP);
   for (int i = 0; i < sizeof(points)/sizeof(PT); i++)
   {
	  changeColor(random(0.6, 0.8), random(0.4, 0.6), 0.0);
      Point pt(center.getX() + points[i].x, 
               center.getY() + points[i].y);
      rotate(pt, center, rotation);
      glVertex2f(pt.getX(), pt.getY());
   }
   changeColor(1, 1, 1);
   glEnd();
}

/**********************************************************************
 * DRAW MEDIUM ASTEROID
 **********************************************************************/
void drawMediumAsteroid( const Point & center, int rotation)
{
   // ultra simple point
   struct PT
   {
      int x;
      int y;
   } points[] = 
   {
      {2, 8},    {8, 15},    {12, 8}, 
      {6, 2},    {12, -6},   {2, -15},
      {-6, -15}, {-14, -10}, {-15, 0},
      {-4, 15},  {2, 8}
   };
   
   glBegin(GL_LINE_STRIP);
   for (int i = 0; i < sizeof(points)/sizeof(PT); i++)
   {
	  changeColor(random(0.4, 0.6), random(0.2, 0.4), random(0.05, 0.2));
      Point pt(center.getX() + points[i].x, 
               center.getY() + points[i].y);
      rotate(pt, center, rotation);
      glVertex2f(pt.getX(), pt.getY());
   }
   changeColor(1, 1, 1);
   glEnd();
}

/**********************************************************************
 * DRAW LARGE ASTEROID
 **********************************************************************/
void drawLargeAsteroid( const Point & center, int rotation)
{
   // ultra simple point
   struct PT
   {
      int x;
      int y;
   } points[] = 
   {
      {0, 12},    {8, 20}, {16, 14},
      {10, 12},   {20, 0}, {0, -20},
      {-18, -10}, {-20, -2}, {-20, 14},
      {-10, 20},  {0, 12}
   };
   
   glBegin(GL_LINE_STRIP);
   for (int i = 0; i < sizeof(points)/sizeof(PT); i++)
   {
	  changeColor(random(0.4, 0.6), random(0.2, 0.4), 0.0);
      Point pt(center.getX() + points[i].x, 
               center.getY() + points[i].y);
      rotate(pt, center, rotation);
      glVertex2f(pt.getX(), pt.getY());
   }
   changeColor(1, 1, 1);
   glEnd();
}


/************************************************************************       
 * DRAW Ship                                                                    
 * Draw a spaceship on the screen                                               
 *  INPUT point   The position of the ship                                      
 *        angle   Which direction it is ponted                                  
 *************************************************************************/
void drawShip(const Point & center, int rotation, bool thrust)
{
   // ultra simple point
   struct PT
   {
      int x;
      int y;
   };
   
   // draw the ship                                                 
   const PT pointsShip[] = 
   { // top   r.wing   r.engine l.engine  l.wing    top
      {0, 6}, {6, -6}, {2, -3}, {-2, -3}, {-6, -6}, {0, 6}  
   };
   
   glBegin(GL_LINE_STRIP);
   for (int i = 0; i < sizeof(pointsShip)/sizeof(PT); i++)
   {
      Point pt(center.getX() + pointsShip[i].x, 
               center.getY() + pointsShip[i].y);
      rotate(pt, center, rotation);
      glVertex2f(pt.getX(), pt.getY());
   }
   glEnd();

   // draw the flame if necessary
   if (thrust)
   {
      const PT pointsFlame[3][5] =
      {
         { {-2, -3}, {-2, -13}, { 0, -6}, { 2, -13}, {2, -3} },
         { {-2, -3}, {-4,  -9}, {-1, -7}, { 1, -14}, {2, -3} },
         { {-2, -3}, {-1, -14}, { 1, -7}, { 4,  -9}, {2, -3} }
      };
      
      glBegin(GL_LINE_STRIP);
      glColor3f(0.0 /* red % */, 0.0 /* green % */, 1.0 /* blue % */);
      int iFlame = random(0, 3);
      for (int i = 0; i < 5; i++)
      {
         Point pt(center.getX() + pointsFlame[iFlame][i].x, 
                  center.getY() + pointsFlame[iFlame][i].y);
         rotate(pt, center, rotation);
		 float x = pt.getX();
		 float y = pt.getY();
		 if (!(x > 200 || x < -200 || y > 200 || y < -200))
			glVertex2f(x, y);
      }
      glColor3f(1.0, 1.0, 1.0); // reset to white                                  
      glEnd();
   }
}


/************************************************************************
* CHANGE COLOR
* Change graphics color
*************************************************************************/
void changeColor(float red, float green, float blue)
{
	glColor3f(red, green, blue);
}

/***************************************
* GAME :: DRAWPIZZA
* draws a pizza
***************************************/
void drawPizza(Point center, int rotation)
{
	float colorsRGB[48][48][3] = { { { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 38, 30, 20 },{ 93, 75, 49 },{ 106, 86, 55 },{ 109, 88, 56 },{ 116, 93, 60 },{ 118, 95, 62 },{ 116, 93, 59 },{ 121, 97, 63 },{ 117, 95, 60 },{ 116, 93, 60 },{ 113, 91, 59 },{ 105, 84, 54 },{ 89, 72, 46 },{ 29, 23, 15 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 3, 2, 1 },{ 95, 76, 49 },{ 112, 90, 58 },{ 119, 96, 63 },{ 117, 94, 61 },{ 110, 88, 56 },{ 106, 85, 54 },{ 105, 84, 55 },{ 100, 80, 51 },{ 99, 80, 51 },{ 101, 81, 52 },{ 100, 80, 51 },{ 106, 85, 56 },{ 108, 87, 55 },{ 116, 93, 61 },{ 121, 98, 63 },{ 121, 97, 62 },{ 116, 94, 61 },{ 91, 73, 47 },{ 1, 1, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 31, 24, 16 },{ 102, 82, 53 },{ 115, 92, 59 },{ 113, 90, 57 },{ 106, 85, 54 },{ 106, 85, 55 },{ 135, 107, 65 },{ 168, 132, 74 },{ 195, 152, 81 },{ 214, 164, 83 },{ 226, 171, 82 },{ 232, 174, 81 },{ 231, 172, 78 },{ 223, 167, 75 },{ 208, 158, 71 },{ 188, 143, 66 },{ 161, 125, 61 },{ 129, 101, 54 },{ 107, 85, 54 },{ 115, 92, 60 },{ 125, 101, 65 },{ 124, 100, 65 },{ 99, 80, 52 },{ 18, 14, 9 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 94, 76, 49 },{ 117, 94, 62 },{ 114, 92, 60 },{ 103, 82, 53 },{ 127, 103, 65 },{ 188, 152, 94 },{ 237, 184, 109 },{ 240, 176, 97 },{ 231, 155, 78 },{ 223, 137, 63 },{ 217, 125, 52 },{ 214, 117, 47 },{ 213, 113, 44 },{ 213, 113, 43 },{ 215, 117, 45 },{ 218, 123, 48 },{ 222, 134, 55 },{ 224, 147, 62 },{ 225, 160, 68 },{ 216, 162, 70 },{ 174, 134, 63 },{ 124, 98, 55 },{ 109, 87, 56 },{ 122, 98, 63 },{ 120, 96, 62 },{ 81, 65, 42 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 25, 20, 12 },{ 108, 87, 56 },{ 118, 95, 62 },{ 108, 87, 57 },{ 126, 105, 69 },{ 206, 173, 115 },{ 245, 191, 120 },{ 234, 157, 86 },{ 224, 125, 58 },{ 219, 102, 42 },{ 216, 96, 42 },{ 213, 101, 47 },{ 198, 86, 36 },{ 198, 89, 35 },{ 212, 106, 39 },{ 212, 105, 34 },{ 215, 100, 37 },{ 218, 92, 24 },{ 219, 86, 30 },{ 218, 95, 36 },{ 215, 105, 38 },{ 216, 120, 46 },{ 221, 142, 58 },{ 224, 162, 68 },{ 186, 142, 64 },{ 120, 95, 54 },{ 113, 91, 59 },{ 125, 101, 64 },{ 108, 87, 56 },{ 13, 10, 6 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 61, 49, 31 },{ 113, 91, 59 },{ 113, 90, 59 },{ 107, 88, 58 },{ 183, 160, 111 },{ 247, 202, 136 },{ 233, 156, 89 },{ 217, 114, 49 },{ 211, 99, 38 },{ 216, 92, 38 },{ 220, 101, 48 },{ 209, 101, 55 },{ 171, 35, 25 },{ 161, 7, 7 },{ 161, 8, 8 },{ 170, 46, 28 },{ 204, 123, 47 },{ 227, 130, 53 },{ 226, 103, 27 },{ 227, 96, 45 },{ 226, 105, 45 },{ 222, 104, 41 },{ 218, 95, 18 },{ 218, 93, 25 },{ 218, 111, 42 },{ 221, 139, 57 },{ 222, 162, 68 },{ 163, 126, 59 },{ 107, 86, 54 },{ 123, 100, 64 },{ 119, 95, 61 },{ 40, 32, 21 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 70, 56, 35 },{ 112, 90, 57 },{ 109, 87, 56 },{ 122, 103, 69 },{ 224, 198, 141 },{ 245, 184, 119 },{ 225, 124, 61 },{ 213, 95, 44 },{ 217, 111, 64 },{ 225, 138, 78 },{ 230, 145, 55 },{ 231, 157, 83 },{ 178, 57, 40 },{ 162, 9, 9 },{ 165, 18, 18 },{ 161, 5, 5 },{ 162, 9, 9 },{ 172, 59, 33 },{ 235, 155, 70 },{ 236, 144, 59 },{ 235, 138, 63 },{ 233, 134, 58 },{ 230, 131, 49 },{ 228, 113, 29 },{ 224, 87, 28 },{ 220, 85, 34 },{ 216, 96, 32 },{ 220, 116, 44 },{ 222, 150, 62 },{ 197, 149, 65 },{ 113, 90, 51 },{ 121, 98, 63 },{ 124, 100, 65 },{ 47, 37, 23 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 60, 48, 31 },{ 113, 91, 58 },{ 107, 86, 55 },{ 133, 114, 79 },{ 240, 211, 151 },{ 241, 165, 101 },{ 222, 103, 45 },{ 219, 92, 51 },{ 221, 109, 64 },{ 230, 153, 90 },{ 241, 191, 106 },{ 248, 222, 127 },{ 239, 224, 137 },{ 163, 15, 13 },{ 165, 17, 17 },{ 164, 16, 16 },{ 162, 10, 10 },{ 163, 12, 12 },{ 163, 26, 21 },{ 222, 149, 75 },{ 239, 161, 78 },{ 242, 172, 80 },{ 240, 162, 66 },{ 236, 146, 50 },{ 237, 146, 38 },{ 223, 91, 30 },{ 221, 88, 32 },{ 224, 90, 29 },{ 221, 90, 30 },{ 218, 103, 39 },{ 219, 137, 56 },{ 210, 156, 67 },{ 120, 94, 52 },{ 115, 93, 59 },{ 122, 98, 62 },{ 38, 31, 20 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 25, 19, 12 },{ 111, 89, 56 },{ 108, 86, 55 },{ 132, 114, 78 },{ 244, 214, 153 },{ 237, 153, 90 },{ 222, 91, 39 },{ 221, 89, 36 },{ 229, 146, 75 },{ 239, 190, 105 },{ 247, 224, 114 },{ 253, 246, 139 },{ 254, 252, 155 },{ 242, 222, 108 },{ 168, 32, 26 },{ 162, 9, 9 },{ 161, 6, 6 },{ 163, 12, 12 },{ 162, 9, 9 },{ 157, 20, 12 },{ 224, 149, 74 },{ 238, 156, 57 },{ 240, 169, 79 },{ 244, 188, 80 },{ 233, 164, 78 },{ 191, 81, 38 },{ 169, 31, 26 },{ 162, 12, 9 },{ 174, 44, 21 },{ 213, 102, 32 },{ 220, 97, 31 },{ 214, 100, 37 },{ 216, 130, 52 },{ 213, 157, 66 },{ 119, 94, 51 },{ 120, 96, 62 },{ 122, 99, 64 },{ 10, 8, 5 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 106, 85, 54 },{ 111, 89, 57 },{ 122, 103, 69 },{ 239, 210, 150 },{ 233, 151, 87 },{ 218, 97, 44 },{ 220, 90, 47 },{ 229, 135, 70 },{ 243, 203, 96 },{ 242, 212, 99 },{ 244, 214, 76 },{ 254, 241, 124 },{ 249, 230, 119 },{ 234, 202, 63 },{ 184, 98, 40 },{ 162, 9, 9 },{ 167, 23, 23 },{ 162, 9, 9 },{ 164, 15, 15 },{ 173, 82, 33 },{ 239, 169, 70 },{ 241, 174, 86 },{ 243, 186, 97 },{ 248, 206, 81 },{ 193, 88, 45 },{ 162, 10, 10 },{ 162, 8, 8 },{ 161, 8, 8 },{ 162, 9, 9 },{ 170, 53, 27 },{ 227, 149, 67 },{ 220, 107, 33 },{ 217, 96, 33 },{ 219, 131, 52 },{ 207, 154, 66 },{ 112, 89, 51 },{ 127, 102, 66 },{ 107, 87, 56 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 91, 72, 46 },{ 116, 93, 60 },{ 108, 88, 58 },{ 223, 198, 140 },{ 237, 163, 99 },{ 216, 99, 39 },{ 219, 106, 53 },{ 230, 156, 88 },{ 244, 206, 124 },{ 251, 223, 78 },{ 252, 224, 80 },{ 254, 251, 147 },{ 250, 237, 134 },{ 231, 203, 96 },{ 236, 198, 103 },{ 234, 196, 83 },{ 183, 96, 35 },{ 161, 34, 18 },{ 158, 28, 14 },{ 178, 93, 53 },{ 231, 171, 102 },{ 242, 182, 84 },{ 245, 199, 87 },{ 246, 199, 84 },{ 241, 186, 61 },{ 166, 23, 9 },{ 160, 3, 3 },{ 163, 12, 12 },{ 159, 2, 2 },{ 164, 14, 14 },{ 162, 17, 14 },{ 210, 157, 53 },{ 225, 139, 56 },{ 221, 88, 32 },{ 220, 96, 33 },{ 218, 137, 55 },{ 191, 144, 63 },{ 102, 82, 50 },{ 128, 103, 65 },{ 73, 59, 38 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 30, 24, 15 },{ 114, 92, 58 },{ 109, 88, 57 },{ 184, 160, 111 },{ 242, 182, 117 },{ 218, 104, 45 },{ 218, 102, 45 },{ 220, 145, 64 },{ 240, 196, 100 },{ 232, 189, 81 },{ 227, 162, 30 },{ 229, 191, 61 },{ 232, 203, 76 },{ 230, 200, 84 },{ 233, 187, 91 },{ 239, 173, 85 },{ 239, 172, 95 },{ 237, 157, 58 },{ 234, 156, 59 },{ 229, 127, 42 },{ 236, 146, 63 },{ 243, 180, 99 },{ 243, 186, 90 },{ 244, 190, 73 },{ 250, 209, 78 },{ 241, 190, 57 },{ 168, 30, 19 },{ 162, 8, 8 },{ 160, 5, 5 },{ 160, 4, 4 },{ 162, 8, 8 },{ 164, 21, 19 },{ 208, 145, 62 },{ 232, 147, 57 },{ 228, 110, 43 },{ 221, 101, 43 },{ 215, 104, 36 },{ 216, 146, 60 },{ 154, 119, 55 },{ 113, 91, 58 },{ 126, 102, 66 },{ 12, 10, 6 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 103, 83, 53 },{ 116, 93, 60 },{ 128, 106, 70 },{ 246, 202, 135 },{ 222, 123, 58 },{ 217, 98, 45 },{ 226, 126, 56 },{ 240, 186, 92 },{ 241, 195, 99 },{ 197, 99, 35 },{ 166, 21, 11 },{ 165, 18, 18 },{ 163, 26, 9 },{ 198, 116, 44 },{ 239, 166, 57 },{ 236, 143, 44 },{ 231, 114, 44 },{ 233, 115, 55 },{ 235, 134, 75 },{ 233, 109, 45 },{ 233, 123, 44 },{ 237, 144, 50 },{ 232, 118, 36 },{ 235, 132, 35 },{ 241, 172, 60 },{ 243, 185, 56 },{ 182, 66, 22 },{ 163, 11, 11 },{ 163, 12, 12 },{ 162, 8, 8 },{ 164, 15, 15 },{ 162, 42, 22 },{ 232, 154, 56 },{ 237, 149, 63 },{ 235, 139, 53 },{ 230, 139, 57 },{ 215, 100, 25 },{ 212, 113, 43 },{ 213, 155, 65 },{ 110, 87, 48 },{ 127, 103, 66 },{ 93, 76, 50 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 3, 2, 1 },{ 120, 97, 63 },{ 107, 86, 56 },{ 207, 174, 115 },{ 234, 156, 89 },{ 215, 97, 40 },{ 222, 113, 56 },{ 233, 155, 76 },{ 250, 222, 108 },{ 219, 147, 83 },{ 162, 9, 8 },{ 164, 15, 15 },{ 165, 18, 18 },{ 160, 5, 5 },{ 166, 26, 22 },{ 216, 151, 90 },{ 235, 131, 47 },{ 235, 97, 27 },{ 234, 80, 31 },{ 235, 67, 30 },{ 237, 61, 24 },{ 236, 77, 28 },{ 235, 103, 27 },{ 234, 94, 22 },{ 234, 92, 35 },{ 235, 136, 54 },{ 237, 148, 36 },{ 224, 120, 37 },{ 173, 44, 25 },{ 159, 11, 9 },{ 160, 14, 12 },{ 161, 44, 22 },{ 212, 134, 49 },{ 241, 163, 60 },{ 237, 148, 63 },{ 236, 141, 56 },{ 233, 143, 39 },{ 224, 125, 46 },{ 209, 99, 34 },{ 214, 134, 54 },{ 173, 132, 59 },{ 104, 84, 53 },{ 128, 103, 68 },{ 1, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 99, 80, 52 },{ 118, 95, 62 },{ 128, 104, 66 },{ 245, 191, 120 },{ 217, 113, 49 },{ 218, 109, 54 },{ 231, 158, 93 },{ 243, 203, 109 },{ 251, 238, 129 },{ 195, 103, 59 },{ 161, 5, 5 },{ 162, 10, 10 },{ 165, 16, 16 },{ 162, 9, 9 },{ 163, 12, 12 },{ 188, 109, 62 },{ 236, 141, 64 },{ 233, 103, 38 },{ 238, 97, 37 },{ 239, 58, 22 },{ 234, 47, 20 },{ 192, 41, 21 },{ 167, 16, 5 },{ 173, 31, 17 },{ 200, 57, 26 },{ 237, 134, 58 },{ 237, 134, 35 },{ 235, 112, 41 },{ 232, 110, 58 },{ 219, 131, 68 },{ 216, 140, 57 },{ 235, 161, 78 },{ 241, 165, 49 },{ 241, 162, 38 },{ 240, 163, 29 },{ 242, 176, 52 },{ 241, 184, 67 },{ 232, 156, 82 },{ 215, 103, 37 },{ 213, 109, 39 },{ 213, 154, 64 },{ 110, 87, 48 },{ 129, 104, 67 },{ 83, 67, 45 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 114, 91, 59 },{ 109, 87, 55 },{ 190, 153, 95 },{ 231, 157, 85 },{ 213, 97, 30 },{ 225, 133, 59 },{ 240, 188, 88 },{ 248, 217, 118 },{ 251, 222, 113 },{ 192, 99, 41 },{ 160, 4, 4 },{ 163, 13, 13 },{ 162, 10, 10 },{ 164, 15, 15 },{ 162, 9, 9 },{ 187, 113, 32 },{ 238, 179, 65 },{ 233, 137, 50 },{ 237, 109, 40 },{ 234, 71, 26 },{ 177, 19, 10 },{ 167, 24, 24 },{ 165, 18, 18 },{ 163, 12, 12 },{ 163, 15, 15 },{ 198, 90, 39 },{ 238, 130, 50 },{ 237, 131, 52 },{ 236, 140, 60 },{ 246, 200, 96 },{ 245, 196, 82 },{ 240, 164, 56 },{ 239, 153, 44 },{ 239, 154, 44 },{ 244, 182, 51 },{ 251, 219, 91 },{ 250, 220, 69 },{ 232, 170, 89 },{ 219, 114, 51 },{ 215, 95, 31 },{ 216, 138, 56 },{ 156, 120, 55 },{ 110, 89, 56 },{ 118, 96, 64 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 37, 30, 19 },{ 118, 95, 61 },{ 109, 88, 56 },{ 237, 184, 109 },{ 222, 125, 58 },{ 218, 94, 31 },{ 231, 143, 54 },{ 240, 194, 54 },{ 238, 206, 93 },{ 245, 200, 87 },{ 216, 145, 70 },{ 162, 12, 10 },{ 162, 10, 10 },{ 167, 22, 22 },{ 163, 11, 11 },{ 163, 30, 22 },{ 203, 165, 38 },{ 234, 191, 63 },{ 232, 120, 42 },{ 234, 82, 35 },{ 215, 64, 28 },{ 159, 2, 1 },{ 163, 12, 12 },{ 162, 10, 10 },{ 166, 20, 20 },{ 163, 12, 12 },{ 169, 45, 24 },{ 232, 116, 41 },{ 238, 144, 54 },{ 239, 165, 70 },{ 250, 226, 112 },{ 247, 212, 74 },{ 215, 133, 53 },{ 200, 95, 48 },{ 213, 129, 65 },{ 243, 196, 76 },{ 244, 198, 77 },{ 235, 189, 81 },{ 229, 147, 60 },{ 225, 99, 36 },{ 221, 88, 29 },{ 217, 119, 47 },{ 200, 149, 63 },{ 97, 78, 49 },{ 132, 107, 70 },{ 17, 14, 10 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 93, 74, 47 },{ 119, 96, 62 },{ 139, 111, 67 },{ 240, 177, 97 },{ 221, 101, 40 },{ 220, 83, 27 },{ 195, 81, 22 },{ 170, 43, 13 },{ 171, 48, 24 },{ 195, 99, 43 },{ 230, 150, 73 },{ 200, 112, 55 },{ 166, 42, 24 },{ 161, 19, 13 },{ 162, 49, 25 },{ 189, 144, 59 },{ 224, 177, 67 },{ 229, 147, 49 },{ 233, 106, 32 },{ 235, 94, 39 },{ 209, 92, 55 },{ 167, 23, 23 },{ 164, 16, 16 },{ 163, 13, 13 },{ 162, 9, 9 },{ 163, 13, 13 },{ 168, 53, 35 },{ 231, 139, 53 },{ 240, 161, 64 },{ 245, 189, 76 },{ 245, 208, 76 },{ 187, 87, 36 },{ 164, 14, 14 },{ 159, 1, 1 },{ 160, 5, 5 },{ 178, 73, 29 },{ 226, 142, 60 },{ 232, 129, 58 },{ 236, 136, 32 },{ 231, 120, 34 },{ 224, 103, 36 },{ 215, 104, 39 },{ 213, 150, 62 },{ 112, 87, 46 },{ 129, 104, 67 },{ 76, 63, 43 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 110, 88, 57 },{ 117, 95, 62 },{ 171, 135, 77 },{ 234, 157, 80 },{ 223, 88, 33 },{ 188, 46, 19 },{ 161, 6, 6 },{ 166, 21, 21 },{ 161, 8, 8 },{ 164, 16, 16 },{ 182, 79, 42 },{ 236, 146, 52 },{ 235, 173, 57 },{ 222, 161, 57 },{ 231, 176, 53 },{ 232, 191, 64 },{ 227, 135, 37 },{ 231, 97, 27 },{ 233, 103, 40 },{ 237, 142, 80 },{ 230, 167, 106 },{ 164, 24, 16 },{ 161, 6, 6 },{ 160, 3, 3 },{ 161, 7, 7 },{ 161, 8, 8 },{ 177, 92, 49 },{ 241, 175, 74 },{ 245, 187, 63 },{ 241, 171, 42 },{ 211, 128, 29 },{ 160, 5, 5 },{ 162, 8, 8 },{ 162, 10, 10 },{ 164, 14, 14 },{ 163, 11, 11 },{ 194, 84, 47 },{ 234, 107, 37 },{ 239, 146, 33 },{ 238, 161, 47 },{ 227, 137, 68 },{ 213, 99, 38 },{ 213, 138, 56 },{ 140, 108, 51 },{ 120, 97, 63 },{ 106, 88, 61 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 117, 94, 61 },{ 113, 91, 59 },{ 197, 153, 82 },{ 228, 140, 66 },{ 218, 81, 29 },{ 164, 15, 11 },{ 164, 14, 14 },{ 162, 9, 9 },{ 160, 3, 3 },{ 164, 15, 15 },{ 162, 21, 17 },{ 218, 128, 65 },{ 242, 172, 82 },{ 240, 168, 74 },{ 242, 172, 42 },{ 237, 163, 45 },{ 228, 112, 41 },{ 224, 104, 46 },{ 235, 144, 78 },{ 240, 164, 78 },{ 240, 168, 82 },{ 208, 146, 65 },{ 162, 26, 13 },{ 160, 5, 5 },{ 163, 16, 15 },{ 160, 51, 10 },{ 221, 162, 54 },{ 241, 174, 64 },{ 238, 170, 72 },{ 235, 145, 45 },{ 190, 72, 23 },{ 164, 15, 15 },{ 162, 9, 9 },{ 160, 2, 2 },{ 162, 10, 10 },{ 162, 10, 10 },{ 176, 56, 20 },{ 233, 120, 34 },{ 238, 152, 56 },{ 241, 175, 56 },{ 229, 139, 65 },{ 211, 96, 37 },{ 212, 127, 50 },{ 164, 125, 56 },{ 108, 87, 57 },{ 117, 96, 66 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 121, 98, 64 },{ 110, 88, 57 },{ 216, 166, 84 },{ 225, 127, 56 },{ 215, 80, 27 },{ 162, 11, 9 },{ 162, 9, 9 },{ 163, 12, 12 },{ 161, 7, 7 },{ 161, 7, 7 },{ 161, 14, 13 },{ 211, 133, 77 },{ 241, 173, 85 },{ 242, 174, 68 },{ 235, 145, 42 },{ 193, 80, 28 },{ 166, 23, 17 },{ 164, 19, 17 },{ 173, 51, 29 },{ 224, 165, 83 },{ 243, 182, 101 },{ 245, 194, 63 },{ 230, 194, 23 },{ 202, 156, 33 },{ 205, 166, 43 },{ 226, 190, 65 },{ 233, 140, 47 },{ 235, 138, 56 },{ 236, 143, 61 },{ 235, 134, 51 },{ 196, 82, 27 },{ 161, 6, 6 },{ 161, 7, 7 },{ 160, 4, 4 },{ 160, 3, 3 },{ 163, 11, 11 },{ 180, 69, 20 },{ 236, 138, 38 },{ 240, 165, 59 },{ 243, 183, 83 },{ 232, 145, 37 },{ 214, 98, 17 },{ 212, 119, 45 },{ 184, 138, 60 },{ 96, 77, 49 },{ 126, 104, 72 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 124, 100, 65 },{ 108, 86, 56 },{ 228, 172, 83 },{ 223, 119, 50 },{ 222, 79, 24 },{ 175, 39, 23 },{ 164, 15, 15 },{ 163, 12, 12 },{ 164, 13, 13 },{ 165, 18, 18 },{ 162, 45, 25 },{ 233, 164, 77 },{ 246, 197, 80 },{ 243, 177, 76 },{ 199, 89, 42 },{ 162, 8, 8 },{ 163, 11, 11 },{ 160, 3, 3 },{ 164, 15, 15 },{ 166, 39, 22 },{ 234, 178, 82 },{ 242, 179, 47 },{ 237, 184, 61 },{ 232, 183, 86 },{ 235, 198, 74 },{ 236, 195, 80 },{ 240, 168, 55 },{ 237, 141, 43 },{ 236, 149, 55 },{ 240, 164, 81 },{ 231, 177, 97 },{ 171, 51, 37 },{ 159, 1, 1 },{ 169, 28, 28 },{ 163, 11, 11 },{ 163, 33, 23 },{ 215, 115, 49 },{ 242, 168, 69 },{ 246, 195, 102 },{ 246, 196, 105 },{ 232, 149, 26 },{ 209, 102, 31 },{ 211, 114, 45 },{ 199, 148, 62 },{ 90, 72, 47 },{ 132, 109, 76 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 125, 101, 66 },{ 107, 86, 56 },{ 234, 175, 81 },{ 222, 116, 46 },{ 224, 76, 24 },{ 210, 80, 32 },{ 166, 35, 21 },{ 163, 12, 12 },{ 165, 17, 17 },{ 160, 36, 20 },{ 206, 137, 62 },{ 242, 174, 71 },{ 245, 185, 63 },{ 241, 176, 82 },{ 173, 45, 27 },{ 163, 13, 13 },{ 161, 7, 7 },{ 165, 18, 18 },{ 162, 10, 10 },{ 162, 9, 9 },{ 208, 149, 53 },{ 242, 175, 65 },{ 239, 161, 68 },{ 235, 147, 69 },{ 240, 175, 57 },{ 240, 177, 67 },{ 239, 167, 62 },{ 237, 143, 38 },{ 236, 145, 62 },{ 242, 190, 105 },{ 253, 249, 160 },{ 236, 209, 110 },{ 189, 112, 52 },{ 167, 58, 14 },{ 172, 64, 22 },{ 210, 97, 41 },{ 235, 114, 52 },{ 223, 137, 52 },{ 206, 122, 57 },{ 209, 132, 72 },{ 223, 131, 51 },{ 219, 100, 40 },{ 215, 112, 44 },{ 207, 153, 64 },{ 84, 68, 43 },{ 135, 112, 79 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 124, 100, 64 },{ 106, 85, 55 },{ 233, 174, 79 },{ 224, 115, 47 },{ 224, 80, 30 },{ 231, 127, 54 },{ 237, 209, 114 },{ 212, 185, 124 },{ 205, 158, 84 },{ 231, 188, 94 },{ 246, 194, 83 },{ 237, 151, 49 },{ 241, 167, 44 },{ 237, 165, 49 },{ 172, 45, 28 },{ 163, 11, 11 },{ 166, 20, 20 },{ 165, 17, 17 },{ 164, 15, 15 },{ 161, 7, 6 },{ 204, 158, 51 },{ 240, 181, 79 },{ 241, 172, 59 },{ 237, 151, 41 },{ 233, 127, 40 },{ 235, 139, 67 },{ 225, 129, 51 },{ 186, 63, 17 },{ 175, 48, 31 },{ 178, 66, 39 },{ 220, 178, 80 },{ 238, 206, 72 },{ 236, 198, 70 },{ 236, 170, 37 },{ 237, 154, 58 },{ 232, 94, 34 },{ 201, 58, 24 },{ 165, 18, 16 },{ 165, 17, 17 },{ 162, 8, 8 },{ 165, 27, 14 },{ 208, 75, 19 },{ 219, 113, 42 },{ 207, 153, 64 },{ 85, 68, 44 },{ 136, 113, 81 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 121, 97, 61 },{ 106, 84, 54 },{ 224, 168, 76 },{ 224, 118, 49 },{ 223, 79, 27 },{ 234, 143, 39 },{ 251, 225, 107 },{ 254, 246, 162 },{ 245, 214, 80 },{ 245, 195, 71 },{ 242, 177, 79 },{ 236, 141, 41 },{ 234, 131, 36 },{ 231, 102, 33 },{ 190, 56, 25 },{ 162, 10, 10 },{ 165, 18, 18 },{ 167, 22, 22 },{ 161, 5, 5 },{ 162, 43, 22 },{ 215, 142, 28 },{ 231, 130, 59 },{ 232, 117, 44 },{ 234, 119, 37 },{ 237, 125, 45 },{ 229, 111, 34 },{ 172, 33, 18 },{ 161, 6, 6 },{ 161, 8, 8 },{ 160, 4, 4 },{ 162, 16, 11 },{ 206, 162, 96 },{ 240, 191, 69 },{ 239, 170, 51 },{ 234, 138, 63 },{ 221, 67, 25 },{ 167, 19, 17 },{ 160, 3, 3 },{ 165, 17, 17 },{ 160, 4, 4 },{ 162, 11, 11 },{ 173, 40, 17 },{ 221, 117, 44 },{ 198, 147, 62 },{ 88, 70, 46 },{ 135, 112, 82 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 119, 95, 61 },{ 108, 86, 54 },{ 209, 158, 72 },{ 222, 125, 52 },{ 221, 84, 26 },{ 229, 119, 21 },{ 247, 200, 74 },{ 254, 227, 101 },{ 244, 205, 32 },{ 245, 197, 76 },{ 242, 175, 83 },{ 238, 149, 61 },{ 234, 112, 48 },{ 234, 64, 20 },{ 231, 94, 34 },{ 180, 65, 21 },{ 159, 15, 9 },{ 161, 15, 13 },{ 164, 61, 39 },{ 206, 145, 45 },{ 236, 145, 34 },{ 232, 105, 42 },{ 232, 67, 26 },{ 233, 107, 43 },{ 237, 134, 51 },{ 208, 98, 31 },{ 161, 6, 6 },{ 165, 17, 17 },{ 162, 11, 11 },{ 161, 8, 8 },{ 165, 16, 16 },{ 174, 79, 38 },{ 234, 155, 50 },{ 234, 137, 39 },{ 234, 119, 26 },{ 209, 53, 17 },{ 166, 21, 21 },{ 162, 8, 8 },{ 161, 7, 7 },{ 166, 20, 20 },{ 162, 10, 10 },{ 159, 21, 10 },{ 215, 120, 48 },{ 182, 136, 59 },{ 94, 76, 51 },{ 132, 110, 82 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 116, 93, 60 },{ 114, 91, 58 },{ 189, 144, 67 },{ 221, 134, 55 },{ 219, 91, 33 },{ 228, 116, 41 },{ 245, 191, 71 },{ 253, 223, 84 },{ 242, 206, 74 },{ 221, 165, 85 },{ 219, 157, 77 },{ 238, 168, 74 },{ 235, 120, 47 },{ 235, 72, 22 },{ 238, 92, 39 },{ 236, 122, 46 },{ 221, 134, 40 },{ 224, 186, 63 },{ 246, 230, 95 },{ 254, 235, 111 },{ 250, 210, 77 },{ 235, 131, 50 },{ 233, 77, 29 },{ 235, 100, 44 },{ 241, 163, 74 },{ 208, 124, 41 },{ 165, 16, 16 },{ 163, 11, 11 },{ 162, 10, 10 },{ 163, 11, 11 },{ 167, 24, 24 },{ 165, 44, 24 },{ 232, 115, 45 },{ 234, 106, 31 },{ 235, 95, 23 },{ 216, 69, 18 },{ 162, 11, 10 },{ 165, 16, 16 },{ 166, 20, 20 },{ 161, 6, 6 },{ 164, 15, 15 },{ 158, 28, 10 },{ 213, 129, 51 },{ 159, 120, 53 },{ 104, 85, 57 },{ 125, 106, 81 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 110, 89, 57 },{ 121, 97, 62 },{ 163, 127, 62 },{ 224, 147, 62 },{ 219, 92, 30 },{ 224, 95, 23 },{ 235, 150, 57 },{ 211, 143, 68 },{ 170, 35, 27 },{ 159, 0, 0 },{ 165, 18, 17 },{ 176, 61, 21 },{ 225, 124, 41 },{ 235, 79, 31 },{ 236, 70, 27 },{ 238, 118, 42 },{ 240, 158, 69 },{ 245, 191, 87 },{ 250, 214, 52 },{ 251, 219, 62 },{ 247, 214, 58 },{ 228, 176, 72 },{ 219, 116, 52 },{ 225, 109, 50 },{ 239, 160, 92 },{ 227, 171, 80 },{ 164, 19, 16 },{ 166, 19, 19 },{ 164, 15, 15 },{ 164, 16, 16 },{ 159, 2, 2 },{ 180, 52, 18 },{ 236, 89, 18 },{ 235, 91, 30 },{ 238, 130, 49 },{ 236, 133, 32 },{ 183, 74, 35 },{ 163, 13, 12 },{ 163, 12, 12 },{ 162, 8, 8 },{ 159, 12, 9 },{ 184, 75, 27 },{ 213, 138, 57 },{ 132, 102, 48 },{ 118, 97, 68 },{ 113, 97, 76 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 91, 73, 47 },{ 127, 102, 65 },{ 134, 105, 57 },{ 227, 161, 68 },{ 221, 102, 35 },{ 223, 81, 20 },{ 221, 127, 50 },{ 164, 18, 13 },{ 164, 16, 16 },{ 163, 11, 11 },{ 166, 19, 19 },{ 167, 24, 24 },{ 181, 64, 33 },{ 236, 99, 38 },{ 236, 74, 25 },{ 235, 101, 39 },{ 238, 148, 66 },{ 240, 165, 57 },{ 242, 187, 37 },{ 238, 205, 67 },{ 191, 101, 32 },{ 165, 21, 14 },{ 165, 18, 18 },{ 162, 14, 5 },{ 203, 116, 52 },{ 245, 193, 79 },{ 196, 110, 30 },{ 162, 18, 13 },{ 163, 12, 12 },{ 160, 8, 7 },{ 164, 37, 14 },{ 225, 78, 24 },{ 237, 98, 32 },{ 237, 124, 38 },{ 239, 158, 54 },{ 244, 177, 63 },{ 230, 148, 58 },{ 190, 92, 42 },{ 169, 54, 25 },{ 167, 49, 21 },{ 190, 75, 19 },{ 215, 103, 36 },{ 209, 147, 61 },{ 103, 81, 44 },{ 137, 115, 86 },{ 80, 69, 56 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 30, 24, 15 },{ 128, 103, 66 },{ 111, 88, 55 },{ 218, 163, 70 },{ 224, 121, 48 },{ 225, 80, 25 },{ 201, 75, 35 },{ 167, 23, 23 },{ 160, 4, 4 },{ 164, 16, 16 },{ 163, 11, 11 },{ 165, 17, 17 },{ 164, 31, 15 },{ 232, 87, 34 },{ 236, 81, 30 },{ 238, 122, 26 },{ 241, 156, 30 },{ 244, 180, 55 },{ 243, 186, 70 },{ 203, 117, 47 },{ 161, 7, 7 },{ 166, 19, 19 },{ 161, 7, 7 },{ 162, 10, 10 },{ 161, 14, 8 },{ 216, 151, 30 },{ 240, 169, 62 },{ 219, 143, 67 },{ 201, 123, 58 },{ 202, 123, 46 },{ 224, 91, 18 },{ 236, 86, 23 },{ 238, 116, 42 },{ 237, 128, 59 },{ 242, 173, 83 },{ 244, 178, 53 },{ 240, 156, 34 },{ 239, 158, 55 },{ 233, 124, 29 },{ 227, 99, 19 },{ 222, 91, 23 },{ 217, 118, 45 },{ 193, 143, 61 },{ 88, 71, 47 },{ 146, 125, 98 },{ 16, 14, 12 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 118, 95, 61 },{ 116, 93, 59 },{ 175, 134, 63 },{ 227, 145, 61 },{ 226, 87, 32 },{ 201, 59, 19 },{ 162, 10, 9 },{ 165, 18, 18 },{ 160, 3, 3 },{ 160, 5, 5 },{ 163, 13, 13 },{ 165, 32, 15 },{ 233, 73, 26 },{ 236, 89, 36 },{ 233, 107, 32 },{ 234, 134, 33 },{ 247, 200, 80 },{ 252, 247, 157 },{ 179, 67, 35 },{ 162, 9, 9 },{ 164, 15, 15 },{ 162, 9, 9 },{ 163, 11, 11 },{ 161, 7, 7 },{ 189, 105, 22 },{ 243, 180, 62 },{ 241, 166, 58 },{ 238, 154, 60 },{ 234, 125, 59 },{ 236, 102, 35 },{ 236, 100, 34 },{ 229, 105, 34 },{ 216, 95, 31 },{ 234, 173, 72 },{ 242, 178, 70 },{ 238, 154, 45 },{ 236, 136, 55 },{ 233, 110, 32 },{ 228, 100, 25 },{ 219, 96, 31 },{ 212, 135, 55 },{ 143, 110, 50 },{ 109, 90, 65 },{ 136, 118, 96 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 92, 73, 46 },{ 126, 102, 64 },{ 127, 100, 56 },{ 226, 163, 69 },{ 226, 109, 44 },{ 221, 77, 25 },{ 169, 30, 16 },{ 161, 8, 8 },{ 164, 14, 14 },{ 165, 17, 17 },{ 162, 20, 17 },{ 193, 97, 53 },{ 237, 129, 58 },{ 238, 133, 62 },{ 233, 114, 50 },{ 236, 138, 36 },{ 245, 186, 64 },{ 253, 228, 113 },{ 180, 70, 40 },{ 163, 11, 11 },{ 166, 19, 19 },{ 162, 9, 9 },{ 164, 15, 15 },{ 162, 8, 8 },{ 186, 107, 24 },{ 243, 180, 51 },{ 242, 178, 79 },{ 234, 141, 61 },{ 231, 101, 50 },{ 236, 112, 52 },{ 203, 79, 34 },{ 165, 16, 11 },{ 166, 22, 21 },{ 166, 26, 17 },{ 194, 110, 34 },{ 240, 176, 82 },{ 236, 132, 58 },{ 230, 102, 24 },{ 218, 99, 27 },{ 213, 110, 40 },{ 204, 146, 61 },{ 97, 77, 45 },{ 144, 124, 97 },{ 89, 79, 66 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 1, 1, 1 },{ 128, 104, 67 },{ 116, 93, 60 },{ 188, 144, 65 },{ 223, 141, 58 },{ 223, 93, 32 },{ 213, 83, 27 },{ 178, 57, 26 },{ 162, 32, 14 },{ 167, 57, 24 },{ 200, 143, 55 },{ 240, 176, 86 },{ 243, 183, 89 },{ 243, 176, 60 },{ 239, 156, 58 },{ 237, 142, 26 },{ 239, 160, 59 },{ 246, 209, 82 },{ 198, 132, 56 },{ 162, 11, 10 },{ 162, 10, 10 },{ 164, 15, 15 },{ 162, 9, 9 },{ 159, 15, 9 },{ 213, 167, 36 },{ 246, 198, 53 },{ 243, 182, 89 },{ 233, 122, 50 },{ 233, 96, 33 },{ 215, 92, 35 },{ 166, 19, 17 },{ 161, 6, 6 },{ 162, 8, 8 },{ 161, 6, 6 },{ 164, 17, 15 },{ 202, 116, 48 },{ 233, 110, 52 },{ 229, 91, 23 },{ 215, 99, 32 },{ 210, 131, 53 },{ 158, 120, 54 },{ 100, 83, 61 },{ 151, 133, 110 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 102, 83, 53 },{ 129, 104, 66 },{ 123, 97, 55 },{ 222, 163, 68 },{ 222, 118, 45 },{ 224, 80, 26 },{ 224, 83, 21 },{ 227, 112, 45 },{ 235, 153, 47 },{ 240, 168, 45 },{ 240, 184, 97 },{ 204, 117, 57 },{ 186, 75, 38 },{ 184, 69, 31 },{ 212, 124, 34 },{ 243, 178, 65 },{ 244, 195, 75 },{ 234, 194, 105 },{ 184, 90, 48 },{ 159, 13, 7 },{ 161, 9, 8 },{ 164, 42, 28 },{ 196, 142, 59 },{ 247, 208, 58 },{ 249, 214, 72 },{ 248, 204, 78 },{ 240, 156, 35 },{ 236, 136, 31 },{ 193, 77, 25 },{ 161, 6, 6 },{ 166, 20, 20 },{ 164, 14, 14 },{ 162, 10, 10 },{ 163, 11, 11 },{ 176, 53, 23 },{ 233, 89, 34 },{ 230, 80, 25 },{ 218, 116, 45 },{ 202, 146, 61 },{ 94, 75, 45 },{ 142, 123, 98 },{ 104, 93, 79 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 18, 14, 9 },{ 125, 101, 64 },{ 120, 97, 62 },{ 166, 128, 61 },{ 224, 151, 63 },{ 225, 102, 39 },{ 227, 74, 24 },{ 230, 82, 31 },{ 232, 101, 24 },{ 235, 148, 33 },{ 185, 70, 35 },{ 162, 8, 8 },{ 162, 10, 10 },{ 162, 10, 10 },{ 160, 8, 4 },{ 207, 122, 44 },{ 243, 177, 40 },{ 245, 190, 69 },{ 241, 173, 46 },{ 222, 153, 67 },{ 208, 135, 55 },{ 227, 172, 74 },{ 250, 220, 87 },{ 253, 242, 121 },{ 252, 229, 86 },{ 253, 225, 97 },{ 241, 185, 78 },{ 235, 140, 47 },{ 194, 76, 25 },{ 165, 18, 18 },{ 163, 11, 11 },{ 166, 19, 19 },{ 164, 15, 15 },{ 166, 21, 21 },{ 175, 40, 18 },{ 236, 75, 28 },{ 232, 101, 40 },{ 212, 141, 59 },{ 135, 104, 49 },{ 107, 91, 69 },{ 154, 137, 116 },{ 10, 9, 8 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 84, 68, 44 },{ 132, 106, 67 },{ 112, 90, 55 },{ 198, 150, 66 },{ 224, 140, 58 },{ 227, 94, 35 },{ 229, 76, 26 },{ 232, 79, 20 },{ 214, 83, 22 },{ 166, 21, 20 },{ 163, 13, 13 },{ 164, 14, 14 },{ 165, 17, 17 },{ 166, 20, 20 },{ 168, 53, 26 },{ 243, 191, 70 },{ 247, 201, 69 },{ 247, 197, 59 },{ 242, 175, 80 },{ 242, 183, 104 },{ 212, 134, 66 },{ 188, 89, 43 },{ 191, 102, 46 },{ 215, 168, 53 },{ 251, 222, 94 },{ 237, 199, 87 },{ 237, 156, 68 },{ 219, 137, 52 },{ 161, 15, 9 },{ 162, 10, 10 },{ 164, 15, 15 },{ 164, 16, 16 },{ 160, 16, 13 },{ 201, 73, 33 },{ 233, 95, 39 },{ 219, 135, 57 },{ 173, 130, 57 },{ 88, 73, 52 },{ 159, 141, 118 },{ 80, 71, 62 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 112, 90, 58 },{ 130, 105, 68 },{ 119, 94, 54 },{ 211, 157, 67 },{ 224, 133, 54 },{ 226, 94, 35 },{ 230, 77, 20 },{ 206, 64, 18 },{ 164, 13, 13 },{ 162, 10, 10 },{ 161, 6, 6 },{ 165, 16, 16 },{ 163, 11, 11 },{ 166, 50, 29 },{ 244, 207, 63 },{ 253, 234, 99 },{ 247, 203, 57 },{ 245, 193, 88 },{ 201, 117, 67 },{ 159, 2, 1 },{ 162, 10, 10 },{ 160, 3, 3 },{ 163, 18, 14 },{ 211, 161, 74 },{ 248, 214, 88 },{ 251, 230, 107 },{ 251, 215, 78 },{ 208, 130, 62 },{ 166, 44, 23 },{ 159, 19, 11 },{ 160, 30, 15 },{ 184, 61, 16 },{ 224, 96, 36 },{ 219, 129, 53 },{ 190, 140, 60 },{ 91, 74, 47 },{ 143, 125, 103 },{ 130, 118, 102 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 13, 11, 7 },{ 125, 101, 65 },{ 126, 101, 65 },{ 124, 98, 54 },{ 214, 158, 67 },{ 223, 133, 54 },{ 226, 95, 32 },{ 215, 73, 18 },{ 163, 14, 10 },{ 162, 9, 9 },{ 162, 10, 10 },{ 160, 4, 4 },{ 161, 6, 6 },{ 168, 53, 15 },{ 245, 189, 52 },{ 254, 225, 81 },{ 244, 208, 87 },{ 233, 171, 47 },{ 168, 28, 20 },{ 166, 20, 20 },{ 162, 10, 10 },{ 163, 11, 11 },{ 166, 20, 20 },{ 171, 62, 38 },{ 246, 222, 113 },{ 252, 227, 101 },{ 244, 211, 70 },{ 240, 188, 67 },{ 231, 150, 44 },{ 216, 111, 29 },{ 218, 89, 27 },{ 222, 96, 33 },{ 217, 128, 52 },{ 195, 143, 61 },{ 97, 78, 47 },{ 131, 114, 93 },{ 156, 140, 122 },{ 7, 7, 6 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 40, 33, 21 },{ 130, 105, 68 },{ 123, 99, 63 },{ 123, 97, 54 },{ 208, 155, 66 },{ 223, 139, 57 },{ 223, 102, 38 },{ 188, 62, 27 },{ 162, 14, 11 },{ 163, 11, 11 },{ 162, 10, 9 },{ 162, 37, 23 },{ 209, 96, 30 },{ 242, 174, 61 },{ 252, 217, 67 },{ 253, 222, 87 },{ 232, 185, 76 },{ 164, 16, 15 },{ 159, 1, 1 },{ 160, 5, 5 },{ 163, 13, 13 },{ 160, 3, 3 },{ 160, 26, 15 },{ 220, 184, 48 },{ 228, 185, 65 },{ 220, 156, 73 },{ 222, 124, 55 },{ 225, 99, 33 },{ 223, 86, 24 },{ 218, 104, 39 },{ 214, 133, 54 },{ 189, 140, 60 },{ 97, 78, 47 },{ 128, 111, 91 },{ 165, 149, 131 },{ 33, 29, 26 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 50, 40, 26 },{ 130, 105, 69 },{ 124, 100, 63 },{ 116, 92, 53 },{ 193, 146, 64 },{ 220, 148, 61 },{ 220, 116, 46 },{ 199, 79, 30 },{ 182, 56, 20 },{ 185, 68, 19 },{ 215, 93, 35 },{ 232, 117, 40 },{ 235, 152, 56 },{ 245, 195, 43 },{ 250, 207, 47 },{ 239, 201, 83 },{ 168, 36, 24 },{ 164, 14, 14 },{ 164, 15, 15 },{ 164, 14, 14 },{ 162, 8, 8 },{ 162, 40, 16 },{ 224, 132, 38 },{ 225, 123, 35 },{ 223, 102, 44 },{ 225, 83, 31 },{ 227, 91, 34 },{ 222, 116, 45 },{ 211, 141, 58 },{ 172, 130, 57 },{ 92, 76, 49 },{ 133, 117, 96 },{ 167, 151, 132 },{ 44, 40, 36 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 39, 31, 21 },{ 127, 103, 69 },{ 133, 107, 70 },{ 108, 87, 54 },{ 157, 121, 57 },{ 215, 156, 66 },{ 218, 137, 56 },{ 220, 111, 41 },{ 218, 95, 34 },{ 222, 94, 37 },{ 223, 106, 47 },{ 227, 137, 71 },{ 230, 147, 51 },{ 232, 152, 54 },{ 225, 152, 77 },{ 190, 94, 34 },{ 166, 25, 23 },{ 162, 9, 9 },{ 163, 11, 11 },{ 161, 23, 18 },{ 195, 69, 29 },{ 227, 95, 32 },{ 224, 91, 28 },{ 222, 94, 32 },{ 222, 111, 43 },{ 217, 135, 55 },{ 203, 147, 62 },{ 136, 105, 51 },{ 92, 77, 57 },{ 149, 132, 111 },{ 161, 146, 129 },{ 33, 30, 27 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 11, 9, 6 },{ 113, 93, 63 },{ 139, 112, 74 },{ 119, 96, 62 },{ 115, 91, 51 },{ 174, 133, 59 },{ 214, 154, 65 },{ 217, 139, 57 },{ 216, 118, 46 },{ 214, 103, 42 },{ 211, 97, 39 },{ 208, 94, 25 },{ 210, 96, 31 },{ 211, 95, 40 },{ 221, 82, 25 },{ 204, 67, 22 },{ 182, 46, 18 },{ 181, 47, 16 },{ 209, 74, 27 },{ 229, 92, 34 },{ 225, 103, 39 },{ 221, 119, 48 },{ 215, 137, 56 },{ 205, 147, 62 },{ 159, 121, 55 },{ 96, 78, 49 },{ 115, 99, 80 },{ 169, 151, 131 },{ 137, 125, 110 },{ 8, 7, 6 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 79, 65, 45 },{ 132, 108, 73 },{ 132, 107, 70 },{ 111, 89, 58 },{ 113, 89, 49 },{ 158, 121, 56 },{ 201, 149, 64 },{ 213, 150, 62 },{ 212, 139, 56 },{ 209, 128, 50 },{ 212, 119, 45 },{ 217, 114, 43 },{ 224, 113, 43 },{ 226, 114, 45 },{ 227, 116, 47 },{ 223, 124, 51 },{ 221, 132, 54 },{ 217, 141, 59 },{ 211, 148, 62 },{ 194, 143, 61 },{ 145, 111, 52 },{ 99, 80, 49 },{ 106, 91, 71 },{ 155, 137, 116 },{ 166, 150, 132 },{ 84, 77, 68 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 13, 11, 8 },{ 100, 83, 58 },{ 134, 110, 76 },{ 137, 111, 75 },{ 116, 93, 61 },{ 100, 80, 51 },{ 115, 90, 48 },{ 141, 109, 53 },{ 164, 125, 56 },{ 185, 139, 60 },{ 200, 148, 63 },{ 208, 153, 65 },{ 207, 153, 65 },{ 199, 147, 63 },{ 183, 137, 59 },{ 160, 122, 55 },{ 134, 103, 50 },{ 105, 83, 47 },{ 92, 76, 54 },{ 119, 102, 80 },{ 157, 139, 117 },{ 168, 151, 132 },{ 115, 104, 92 },{ 11, 10, 9 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 1, 1, 0 },{ 87, 72, 53 },{ 127, 105, 75 },{ 141, 116, 82 },{ 139, 114, 79 },{ 125, 102, 69 },{ 113, 92, 62 },{ 102, 83, 56 },{ 94, 77, 52 },{ 91, 74, 51 },{ 89, 72, 50 },{ 92, 75, 53 },{ 102, 85, 61 },{ 113, 95, 70 },{ 128, 109, 84 },{ 151, 132, 107 },{ 166, 147, 125 },{ 150, 135, 117 },{ 98, 89, 78 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 19, 16, 12 },{ 84, 71, 54 },{ 115, 98, 75 },{ 131, 111, 85 },{ 139, 119, 91 },{ 147, 126, 98 },{ 148, 127, 99 },{ 152, 131, 104 },{ 153, 133, 107 },{ 150, 131, 107 },{ 141, 124, 104 },{ 126, 112, 96 },{ 90, 81, 70 },{ 18, 16, 14 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
	{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } }
};
	for (int i = 0; i < 48; i++)
	{
		for (int j = 0; j < 48; j++)
		{
			float red = (1.0 / 255.0) * colorsRGB[i][j][0];
			float green = (1.0 / 255.0) * colorsRGB[i][j][1];
			float blue = (1.0 / 255.0) * colorsRGB[i][j][2];
			if (red != 0.0 && green != 0.0 && blue != 0.0)
			{
				changeColor(red, green, blue);
				Point pt(center.getX() + j - 24, center.getY() + i - 24);
				rotate(pt, center, rotation);
				drawDot(Point(pt.getX(), pt.getY()));
			}
		}
	}
	changeColor(1, 1, 1);
}

/***************************************
* GAME :: DRAWFUNNY
* draws something funny
***************************************/
void drawFunny(Point center, int rotation)
{
	float colorsRGB[120][170][3] = {
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 103, 79, 67 },{ 104, 73, 53 },{ 116, 82, 57 },{ 126, 88, 65 },{ 132, 90, 66 },{ 139, 95, 66 },{ 143, 100, 66 },{ 151, 110, 78 },{ 162, 121, 89 },{ 173, 132, 102 },{ 181, 144, 117 },{ 181, 151, 127 },{ 168, 149, 134 },{ 145, 131, 128 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 99, 73, 60 },{ 98, 63, 43 },{ 104, 69, 41 },{ 111, 71, 46 },{ 117, 76, 54 },{ 128, 90, 67 },{ 141, 106, 78 },{ 145, 109, 77 },{ 144, 103, 71 },{ 139, 95, 66 },{ 141, 96, 63 },{ 147, 104, 70 },{ 155, 112, 78 },{ 158, 118, 83 },{ 176, 136, 101 },{ 187, 147, 112 },{ 203, 163, 128 },{ 207, 164, 132 },{ 187, 162, 140 },{ 155, 136, 129 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 92, 75, 67 },{ 93, 64, 46 },{ 102, 64, 41 },{ 110, 73, 47 },{ 112, 76, 52 },{ 110, 74, 48 },{ 111, 74, 47 },{ 122, 83, 54 },{ 138, 97, 67 },{ 150, 107, 73 },{ 148, 104, 69 },{ 138, 92, 56 },{ 135, 89, 53 },{ 140, 95, 56 },{ 150, 106, 69 },{ 159, 116, 81 },{ 171, 130, 98 },{ 188, 149, 120 },{ 197, 158, 129 },{ 200, 161, 130 },{ 201, 162, 131 },{ 202, 159, 127 },{ 198, 155, 123 },{ 198, 157, 127 },{ 187, 152, 124 },{ 158, 132, 115 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 95, 62, 45 },{ 99, 63, 41 },{ 107, 71, 47 },{ 110, 70, 45 },{ 103, 66, 40 },{ 110, 73, 47 },{ 108, 66, 42 },{ 107, 65, 41 },{ 108, 66, 42 },{ 112, 70, 45 },{ 116, 74, 49 },{ 120, 78, 53 },{ 123, 81, 56 },{ 129, 85, 58 },{ 134, 91, 59 },{ 145, 100, 69 },{ 150, 105, 72 },{ 158, 115, 81 },{ 169, 128, 96 },{ 183, 144, 113 },{ 196, 157, 128 },{ 205, 163, 138 },{ 207, 167, 141 },{ 204, 165, 136 },{ 198, 159, 128 },{ 191, 152, 119 },{ 194, 151, 117 },{ 184, 140, 103 },{ 154, 120, 93 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 99, 75, 63 },{ 100, 67, 48 },{ 107, 70, 44 },{ 116, 79, 52 },{ 113, 76, 50 },{ 111, 71, 46 },{ 101, 60, 38 },{ 93, 56, 37 },{ 92, 57, 35 },{ 94, 58, 36 },{ 101, 65, 39 },{ 103, 69, 42 },{ 107, 72, 50 },{ 117, 81, 59 },{ 130, 94, 70 },{ 141, 106, 76 },{ 154, 120, 95 },{ 160, 126, 101 },{ 166, 132, 105 },{ 176, 142, 115 },{ 187, 152, 124 },{ 188, 153, 125 },{ 191, 154, 125 },{ 185, 148, 119 },{ 182, 145, 116 },{ 179, 138, 106 },{ 188, 145, 111 },{ 187, 146, 116 },{ 184, 145, 116 },{ 181, 141, 106 },{ 165, 121, 82 },{ 154, 109, 70 },{ 131, 91, 65 },{ 117, 89, 77 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 103, 74, 58 },{ 121, 84, 58 },{ 128, 86, 61 },{ 123, 86, 59 },{ 116, 78, 55 },{ 105, 64, 42 },{ 96, 58, 35 },{ 94, 53, 31 },{ 93, 58, 36 },{ 95, 59, 37 },{ 102, 66, 44 },{ 110, 74, 52 },{ 124, 86, 63 },{ 132, 94, 71 },{ 139, 103, 79 },{ 153, 117, 91 },{ 155, 118, 91 },{ 159, 123, 91 },{ 173, 138, 108 },{ 182, 151, 123 },{ 187, 155, 130 },{ 189, 154, 132 },{ 196, 162, 137 },{ 201, 165, 139 },{ 200, 165, 137 },{ 194, 159, 129 },{ 185, 150, 120 },{ 166, 131, 103 },{ 159, 119, 93 },{ 156, 112, 83 },{ 150, 109, 77 },{ 149, 109, 74 },{ 152, 108, 73 },{ 149, 101, 63 },{ 141, 92, 59 },{ 124, 86, 65 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 105, 87, 75 },{ 122, 81, 61 },{ 130, 93, 64 },{ 130, 90, 65 },{ 117, 77, 52 },{ 115, 77, 54 },{ 116, 75, 55 },{ 99, 61, 40 },{ 91, 55, 31 },{ 93, 57, 33 },{ 96, 60, 36 },{ 99, 63, 39 },{ 105, 67, 44 },{ 111, 70, 48 },{ 119, 79, 54 },{ 123, 83, 57 },{ 140, 99, 71 },{ 152, 111, 81 },{ 160, 119, 89 },{ 166, 125, 95 },{ 170, 129, 97 },{ 183, 142, 110 },{ 198, 155, 123 },{ 212, 168, 139 },{ 215, 174, 146 },{ 208, 171, 144 },{ 200, 166, 138 },{ 187, 152, 122 },{ 173, 136, 107 },{ 164, 124, 98 },{ 159, 118, 90 },{ 152, 109, 77 },{ 139, 98, 68 },{ 128, 87, 59 },{ 131, 84, 56 },{ 136, 90, 54 },{ 132, 87, 56 },{ 127, 81, 47 },{ 123, 82, 52 },{ 100, 80, 71 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 118, 91, 74 },{ 127, 87, 62 },{ 123, 86, 59 },{ 109, 68, 46 },{ 112, 71, 49 },{ 117, 77, 52 },{ 122, 80, 56 },{ 112, 72, 47 },{ 99, 61, 38 },{ 87, 52, 30 },{ 82, 50, 27 },{ 90, 54, 32 },{ 94, 58, 34 },{ 96, 59, 33 },{ 102, 61, 33 },{ 114, 70, 41 },{ 121, 76, 47 },{ 130, 83, 55 },{ 135, 89, 56 },{ 146, 100, 66 },{ 162, 117, 84 },{ 176, 133, 99 },{ 186, 146, 111 },{ 193, 150, 116 },{ 202, 157, 124 },{ 203, 158, 125 },{ 206, 161, 128 },{ 202, 159, 125 },{ 200, 161, 130 },{ 192, 151, 119 },{ 177, 138, 107 },{ 156, 120, 88 },{ 140, 101, 70 },{ 127, 87, 61 },{ 110, 72, 49 },{ 100, 60, 35 },{ 105, 63, 39 },{ 114, 72, 48 },{ 100, 60, 35 },{ 104, 60, 33 },{ 118, 71, 43 },{ 107, 78, 64 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 128, 96, 75 },{ 124, 79, 56 },{ 113, 75, 52 },{ 104, 63, 41 },{ 105, 69, 47 },{ 122, 80, 58 },{ 123, 86, 60 },{ 118, 77, 55 },{ 98, 62, 40 },{ 83, 48, 28 },{ 77, 40, 22 },{ 82, 47, 27 },{ 91, 53, 32 },{ 95, 57, 34 },{ 104, 66, 43 },{ 117, 77, 52 },{ 123, 79, 52 },{ 126, 82, 53 },{ 129, 85, 56 },{ 130, 86, 57 },{ 138, 93, 62 },{ 146, 106, 71 },{ 158, 118, 83 },{ 162, 122, 86 },{ 166, 124, 86 },{ 176, 132, 97 },{ 186, 140, 106 },{ 188, 143, 110 },{ 188, 143, 110 },{ 186, 143, 108 },{ 178, 132, 98 },{ 177, 131, 97 },{ 177, 134, 99 },{ 170, 127, 92 },{ 161, 118, 83 },{ 146, 105, 73 },{ 126, 87, 58 },{ 107, 73, 46 },{ 98, 60, 37 },{ 108, 66, 41 },{ 110, 70, 44 },{ 101, 64, 38 },{ 113, 69, 40 },{ 112, 80, 57 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 134, 101, 82 },{ 125, 83, 59 },{ 105, 68, 42 },{ 99, 61, 40 },{ 101, 66, 44 },{ 117, 75, 53 },{ 112, 74, 51 },{ 111, 73, 50 },{ 98, 62, 40 },{ 90, 55, 35 },{ 85, 50, 31 },{ 86, 49, 31 },{ 96, 60, 38 },{ 106, 69, 42 },{ 112, 71, 43 },{ 125, 79, 55 },{ 133, 87, 61 },{ 139, 96, 64 },{ 141, 98, 64 },{ 139, 96, 62 },{ 146, 103, 71 },{ 149, 108, 76 },{ 155, 112, 80 },{ 159, 116, 84 },{ 161, 120, 88 },{ 163, 122, 90 },{ 174, 133, 101 },{ 175, 132, 98 },{ 170, 125, 92 },{ 163, 117, 83 },{ 163, 118, 79 },{ 158, 113, 74 },{ 151, 106, 67 },{ 152, 106, 70 },{ 150, 104, 68 },{ 152, 104, 68 },{ 151, 101, 66 },{ 139, 93, 59 },{ 130, 84, 50 },{ 133, 83, 50 },{ 139, 90, 57 },{ 123, 80, 46 },{ 115, 74, 44 },{ 111, 72, 43 },{ 107, 67, 41 },{ 112, 72, 47 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 117, 90, 73 },{ 122, 78, 53 },{ 105, 64, 42 },{ 111, 75, 51 },{ 111, 71, 45 },{ 106, 66, 41 },{ 109, 74, 54 },{ 124, 86, 65 },{ 118, 76, 54 },{ 114, 73, 51 },{ 113, 73, 48 },{ 102, 66, 40 },{ 105, 69, 45 },{ 103, 63, 38 },{ 95, 58, 31 },{ 95, 55, 30 },{ 103, 63, 38 },{ 116, 76, 50 },{ 124, 83, 55 },{ 127, 86, 58 },{ 135, 93, 68 },{ 140, 98, 73 },{ 148, 106, 81 },{ 144, 107, 80 },{ 150, 110, 84 },{ 148, 107, 79 },{ 146, 102, 75 },{ 143, 102, 74 },{ 140, 99, 71 },{ 149, 105, 78 },{ 153, 109, 82 },{ 139, 92, 62 },{ 135, 88, 60 },{ 135, 86, 54 },{ 142, 94, 58 },{ 141, 95, 59 },{ 146, 98, 60 },{ 143, 93, 56 },{ 132, 86, 50 },{ 124, 78, 44 },{ 120, 74, 41 },{ 121, 75, 42 },{ 116, 71, 38 },{ 112, 67, 38 },{ 115, 70, 41 },{ 107, 63, 36 },{ 95, 57, 34 },{ 94, 56, 33 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 101, 75, 60 },{ 89, 52, 33 },{ 98, 61, 35 },{ 101, 64, 38 },{ 100, 59, 39 },{ 91, 54, 36 },{ 89, 54, 35 },{ 112, 75, 56 },{ 117, 79, 58 },{ 114, 78, 56 },{ 118, 77, 57 },{ 120, 82, 59 },{ 114, 73, 51 },{ 106, 64, 40 },{ 102, 65, 39 },{ 103, 63, 38 },{ 104, 64, 39 },{ 112, 70, 45 },{ 119, 78, 50 },{ 123, 79, 50 },{ 129, 85, 56 },{ 135, 92, 60 },{ 144, 101, 69 },{ 154, 111, 79 },{ 156, 112, 83 },{ 162, 121, 93 },{ 162, 123, 94 },{ 150, 115, 85 },{ 141, 104, 75 },{ 148, 107, 79 },{ 149, 103, 77 },{ 142, 98, 71 },{ 134, 87, 59 },{ 126, 79, 53 },{ 119, 69, 44 },{ 118, 67, 38 },{ 116, 65, 34 },{ 121, 71, 38 },{ 123, 73, 40 },{ 127, 78, 45 },{ 123, 77, 44 },{ 116, 71, 42 },{ 107, 65, 40 },{ 109, 67, 42 },{ 104, 62, 37 },{ 101, 59, 35 },{ 98, 57, 35 },{ 85, 49, 27 },{ 76, 44, 23 },{ 76, 45, 25 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 98, 74, 61 },{ 84, 49, 29 },{ 82, 49, 30 },{ 89, 54, 34 },{ 90, 52, 31 },{ 98, 62, 40 },{ 97, 62, 42 },{ 101, 65, 49 },{ 97, 64, 47 },{ 110, 72, 53 },{ 116, 80, 58 },{ 121, 80, 58 },{ 115, 75, 50 },{ 117, 75, 50 },{ 111, 67, 42 },{ 109, 67, 42 },{ 113, 68, 45 },{ 115, 69, 45 },{ 116, 70, 44 },{ 117, 71, 45 },{ 124, 78, 52 },{ 127, 83, 56 },{ 133, 92, 64 },{ 146, 105, 77 },{ 153, 111, 86 },{ 143, 103, 77 },{ 143, 101, 76 },{ 143, 104, 75 },{ 145, 104, 76 },{ 137, 95, 70 },{ 147, 101, 75 },{ 145, 98, 70 },{ 144, 93, 64 },{ 141, 87, 59 },{ 136, 82, 54 },{ 139, 83, 56 },{ 149, 94, 64 },{ 142, 89, 58 },{ 129, 80, 50 },{ 113, 68, 39 },{ 108, 64, 35 },{ 109, 63, 37 },{ 103, 57, 33 },{ 108, 62, 36 },{ 111, 67, 40 },{ 105, 65, 39 },{ 93, 55, 32 },{ 87, 52, 32 },{ 80, 48, 27 },{ 75, 44, 26 },{ 71, 38, 23 },{ 68, 39, 23 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 98, 80, 70 },{ 95, 62, 43 },{ 102, 68, 43 },{ 106, 65, 45 },{ 103, 62, 44 },{ 97, 60, 41 },{ 96, 60, 38 },{ 96, 62, 37 },{ 110, 74, 50 },{ 117, 75, 53 },{ 114, 69, 46 },{ 110, 68, 44 },{ 114, 69, 46 },{ 106, 64, 40 },{ 110, 66, 41 },{ 115, 69, 45 },{ 114, 68, 44 },{ 111, 63, 40 },{ 114, 67, 39 },{ 122, 75, 47 },{ 125, 78, 50 },{ 127, 80, 54 },{ 133, 87, 61 },{ 139, 93, 69 },{ 146, 102, 77 },{ 149, 104, 81 },{ 144, 104, 79 },{ 123, 83, 58 },{ 121, 79, 55 },{ 127, 81, 58 },{ 136, 88, 65 },{ 137, 90, 64 },{ 143, 93, 66 },{ 143, 89, 63 },{ 137, 81, 54 },{ 137, 81, 54 },{ 137, 80, 51 },{ 137, 78, 48 },{ 142, 83, 53 },{ 140, 83, 53 },{ 135, 82, 51 },{ 132, 83, 53 },{ 129, 85, 58 },{ 112, 66, 40 },{ 106, 59, 31 },{ 110, 63, 35 },{ 107, 61, 35 },{ 97, 55, 31 },{ 88, 51, 32 },{ 78, 43, 24 },{ 70, 37, 20 },{ 66, 37, 21 },{ 60, 30, 19 },{ 62, 38, 25 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 89, 56, 37 },{ 89, 56, 37 },{ 98, 62, 40 },{ 102, 61, 43 },{ 87, 52, 32 },{ 94, 57, 38 },{ 99, 62, 43 },{ 108, 76, 53 },{ 124, 82, 58 },{ 124, 74, 51 },{ 109, 65, 40 },{ 106, 61, 38 },{ 108, 63, 40 },{ 107, 61, 38 },{ 107, 61, 38 },{ 115, 67, 44 },{ 129, 77, 53 },{ 131, 80, 53 },{ 129, 75, 49 },{ 125, 69, 42 },{ 126, 70, 43 },{ 136, 80, 53 },{ 144, 90, 64 },{ 154, 104, 77 },{ 159, 112, 84 },{ 155, 110, 81 },{ 145, 100, 71 },{ 148, 101, 73 },{ 145, 99, 73 },{ 142, 92, 69 },{ 139, 87, 63 },{ 145, 88, 61 },{ 152, 97, 67 },{ 157, 101, 76 },{ 157, 103, 79 },{ 154, 104, 77 },{ 152, 98, 72 },{ 152, 94, 70 },{ 148, 91, 62 },{ 142, 85, 56 },{ 138, 81, 52 },{ 136, 79, 50 },{ 134, 77, 48 },{ 135, 75, 47 },{ 129, 73, 46 },{ 119, 69, 42 },{ 115, 68, 40 },{ 106, 62, 35 },{ 94, 52, 28 },{ 82, 44, 23 },{ 76, 39, 21 },{ 74, 41, 24 },{ 64, 35, 21 },{ 63, 35, 23 },{ 65, 33, 18 },{ 75, 58, 51 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 89, 56, 37 },{ 70, 44, 29 },{ 77, 44, 27 },{ 82, 47, 28 },{ 78, 45, 26 },{ 81, 50, 30 },{ 97, 62, 42 },{ 121, 76, 53 },{ 124, 74, 49 },{ 118, 68, 45 },{ 109, 63, 40 },{ 106, 64, 40 },{ 114, 68, 45 },{ 108, 60, 38 },{ 111, 61, 38 },{ 118, 66, 42 },{ 131, 75, 50 },{ 136, 80, 53 },{ 141, 85, 58 },{ 148, 92, 67 },{ 148, 91, 62 },{ 146, 86, 58 },{ 149, 89, 61 },{ 152, 92, 64 },{ 153, 96, 67 },{ 161, 104, 75 },{ 157, 102, 72 },{ 148, 92, 65 },{ 143, 86, 59 },{ 140, 82, 58 },{ 147, 91, 68 },{ 157, 101, 76 },{ 160, 109, 80 },{ 171, 120, 93 },{ 186, 140, 117 },{ 198, 155, 136 },{ 195, 152, 135 },{ 201, 158, 141 },{ 196, 153, 134 },{ 189, 142, 122 },{ 182, 130, 108 },{ 168, 112, 89 },{ 159, 102, 75 },{ 151, 94, 65 },{ 144, 83, 54 },{ 130, 70, 44 },{ 122, 66, 41 },{ 115, 63, 39 },{ 113, 65, 42 },{ 101, 56, 35 },{ 82, 44, 25 },{ 70, 35, 16 },{ 67, 35, 20 },{ 64, 34, 23 },{ 70, 37, 18 },{ 72, 35, 17 },{ 73, 51, 40 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 76, 52, 40 },{ 84, 51, 32 },{ 83, 50, 31 },{ 88, 51, 33 },{ 90, 55, 36 },{ 86, 51, 32 },{ 96, 58, 37 },{ 114, 68, 44 },{ 117, 67, 40 },{ 116, 66, 39 },{ 109, 61, 38 },{ 114, 64, 41 },{ 116, 65, 44 },{ 122, 70, 46 },{ 134, 80, 56 },{ 136, 85, 58 },{ 145, 91, 65 },{ 156, 99, 72 },{ 162, 105, 78 },{ 165, 104, 76 },{ 164, 102, 77 },{ 162, 103, 73 },{ 161, 102, 70 },{ 159, 100, 70 },{ 162, 103, 73 },{ 168, 109, 79 },{ 168, 111, 81 },{ 166, 111, 81 },{ 159, 104, 74 },{ 153, 96, 66 },{ 156, 97, 67 },{ 164, 104, 76 },{ 168, 112, 85 },{ 178, 127, 100 },{ 189, 137, 116 },{ 198, 150, 130 },{ 209, 162, 146 },{ 220, 177, 160 },{ 232, 189, 173 },{ 225, 182, 166 },{ 212, 169, 152 },{ 209, 166, 149 },{ 200, 153, 135 },{ 191, 143, 121 },{ 177, 125, 101 },{ 162, 100, 75 },{ 150, 86, 59 },{ 145, 84, 56 },{ 137, 77, 49 },{ 116, 65, 38 },{ 100, 54, 31 },{ 93, 50, 31 },{ 88, 47, 27 },{ 78, 39, 22 },{ 73, 36, 20 },{ 89, 48, 28 },{ 97, 56, 36 },{ 99, 70, 54 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 76, 52, 40 },{ 61, 33, 19 },{ 67, 39, 25 },{ 78, 43, 23 },{ 79, 41, 22 },{ 80, 43, 24 },{ 88, 50, 31 },{ 104, 58, 34 },{ 115, 64, 37 },{ 119, 67, 43 },{ 118, 66, 44 },{ 116, 64, 42 },{ 122, 70, 46 },{ 127, 77, 52 },{ 141, 87, 61 },{ 151, 95, 70 },{ 161, 107, 81 },{ 169, 113, 86 },{ 170, 110, 82 },{ 175, 112, 81 },{ 174, 105, 74 },{ 174, 105, 74 },{ 172, 109, 76 },{ 171, 106, 74 },{ 175, 109, 77 },{ 177, 111, 79 },{ 176, 110, 78 },{ 167, 100, 71 },{ 170, 103, 74 },{ 172, 105, 78 },{ 167, 108, 78 },{ 167, 110, 80 },{ 176, 119, 90 },{ 185, 133, 109 },{ 200, 149, 128 },{ 207, 160, 144 },{ 215, 168, 152 },{ 219, 172, 156 },{ 230, 184, 169 },{ 241, 198, 182 },{ 243, 201, 185 },{ 238, 196, 180 },{ 229, 186, 169 },{ 217, 170, 154 },{ 204, 156, 136 },{ 194, 146, 123 },{ 184, 134, 109 },{ 173, 117, 90 },{ 159, 98, 69 },{ 153, 88, 58 },{ 147, 86, 57 },{ 142, 84, 60 },{ 127, 75, 53 },{ 110, 62, 39 },{ 106, 64, 40 },{ 103, 65, 42 },{ 127, 89, 68 },{ 150, 114, 90 },{ 166, 134, 111 },{ 170, 135, 113 },{ 174, 142, 119 },{ 169, 142, 123 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 59, 36, 28 },{ 68, 39, 23 },{ 76, 45, 27 },{ 67, 35, 22 },{ 73, 38, 19 },{ 87, 46, 26 },{ 103, 57, 33 },{ 118, 66, 44 },{ 127, 73, 47 },{ 132, 75, 48 },{ 134, 74, 46 },{ 138, 81, 52 },{ 149, 96, 65 },{ 166, 109, 80 },{ 170, 116, 90 },{ 176, 119, 92 },{ 177, 120, 91 },{ 174, 110, 82 },{ 177, 112, 80 },{ 177, 112, 80 },{ 177, 111, 79 },{ 177, 112, 80 },{ 181, 115, 83 },{ 181, 115, 83 },{ 183, 114, 81 },{ 182, 113, 80 },{ 183, 114, 81 },{ 180, 114, 80 },{ 178, 112, 78 },{ 179, 113, 81 },{ 177, 112, 80 },{ 177, 116, 85 },{ 176, 120, 93 },{ 202, 147, 126 },{ 213, 162, 143 },{ 218, 169, 152 },{ 221, 174, 158 },{ 227, 180, 164 },{ 232, 186, 171 },{ 236, 193, 177 },{ 247, 204, 188 },{ 245, 202, 185 },{ 241, 198, 181 },{ 233, 187, 171 },{ 219, 172, 154 },{ 209, 161, 139 },{ 199, 149, 126 },{ 192, 138, 114 },{ 177, 120, 93 },{ 156, 95, 66 },{ 151, 88, 57 },{ 149, 89, 61 },{ 144, 90, 64 },{ 137, 87, 62 },{ 141, 101, 75 },{ 152, 114, 91 },{ 172, 135, 109 },{ 183, 149, 124 },{ 191, 157, 132 },{ 192, 158, 133 },{ 171, 139, 118 },{ 172, 133, 104 },{ 158, 124, 99 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 74, 56, 46 },{ 69, 43, 30 },{ 77, 50, 33 },{ 76, 44, 29 },{ 68, 35, 20 },{ 78, 41, 23 },{ 91, 44, 26 },{ 109, 62, 36 },{ 139, 81, 57 },{ 143, 83, 55 },{ 147, 84, 53 },{ 150, 85, 57 },{ 157, 100, 73 },{ 171, 117, 89 },{ 175, 118, 89 },{ 181, 120, 92 },{ 187, 128, 98 },{ 185, 122, 91 },{ 186, 120, 88 },{ 191, 122, 91 },{ 195, 126, 97 },{ 193, 126, 99 },{ 191, 130, 101 },{ 191, 126, 96 },{ 190, 125, 93 },{ 192, 126, 94 },{ 192, 126, 92 },{ 192, 123, 90 },{ 189, 120, 87 },{ 185, 119, 87 },{ 184, 117, 88 },{ 184, 119, 89 },{ 185, 124, 96 },{ 191, 135, 112 },{ 206, 151, 131 },{ 209, 156, 138 },{ 220, 169, 152 },{ 227, 180, 164 },{ 236, 190, 175 },{ 238, 192, 176 },{ 229, 186, 170 },{ 232, 189, 173 },{ 241, 195, 180 },{ 240, 193, 177 },{ 232, 185, 167 },{ 224, 176, 156 },{ 219, 168, 147 },{ 214, 159, 138 },{ 202, 148, 122 },{ 192, 138, 110 },{ 177, 126, 97 },{ 166, 115, 88 },{ 154, 103, 76 },{ 155, 105, 78 },{ 156, 110, 84 },{ 155, 114, 86 },{ 166, 129, 100 },{ 174, 139, 111 },{ 179, 142, 115 },{ 174, 137, 110 },{ 181, 146, 118 },{ 171, 136, 108 },{ 151, 114, 87 },{ 141, 100, 68 },{ 128, 92, 70 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 68, 41, 30 },{ 80, 48, 33 },{ 92, 59, 42 },{ 76, 43, 24 },{ 83, 47, 31 },{ 89, 52, 33 },{ 105, 63, 41 },{ 139, 85, 59 },{ 159, 99, 71 },{ 163, 102, 71 },{ 163, 100, 67 },{ 169, 106, 75 },{ 178, 117, 88 },{ 177, 116, 87 },{ 179, 116, 85 },{ 183, 120, 89 },{ 187, 124, 91 },{ 187, 122, 90 },{ 190, 124, 92 },{ 196, 130, 98 },{ 202, 133, 104 },{ 198, 129, 100 },{ 195, 128, 99 },{ 194, 125, 94 },{ 191, 124, 95 },{ 189, 124, 94 },{ 189, 123, 91 },{ 188, 119, 86 },{ 186, 117, 84 },{ 188, 122, 90 },{ 191, 126, 98 },{ 190, 126, 98 },{ 195, 134, 106 },{ 200, 140, 114 },{ 203, 145, 121 },{ 207, 151, 128 },{ 216, 164, 143 },{ 230, 181, 164 },{ 241, 195, 179 },{ 246, 203, 186 },{ 247, 204, 188 },{ 243, 200, 184 },{ 243, 197, 181 },{ 237, 190, 172 },{ 228, 180, 160 },{ 219, 168, 147 },{ 217, 162, 141 },{ 214, 160, 136 },{ 209, 155, 129 },{ 203, 149, 123 },{ 200, 148, 124 },{ 194, 142, 118 },{ 178, 127, 100 },{ 161, 114, 86 },{ 164, 120, 91 },{ 172, 131, 101 },{ 167, 128, 99 },{ 164, 125, 96 },{ 177, 138, 109 },{ 170, 131, 102 },{ 159, 122, 93 },{ 162, 125, 96 },{ 142, 105, 76 },{ 119, 82, 56 },{ 109, 73, 47 },{ 102, 74, 52 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 55, 33, 19 },{ 88, 56, 41 },{ 94, 59, 40 },{ 88, 51, 32 },{ 91, 53, 34 },{ 93, 48, 25 },{ 129, 82, 56 },{ 166, 106, 78 },{ 172, 111, 80 },{ 174, 109, 77 },{ 173, 110, 77 },{ 169, 109, 75 },{ 185, 120, 88 },{ 184, 120, 85 },{ 184, 118, 84 },{ 186, 122, 87 },{ 190, 126, 91 },{ 196, 130, 96 },{ 199, 133, 101 },{ 199, 134, 104 },{ 201, 134, 105 },{ 198, 129, 100 },{ 199, 130, 99 },{ 201, 130, 100 },{ 200, 131, 102 },{ 198, 131, 104 },{ 199, 129, 103 },{ 196, 127, 98 },{ 194, 125, 94 },{ 195, 128, 99 },{ 196, 131, 103 },{ 196, 132, 104 },{ 198, 137, 109 },{ 201, 140, 112 },{ 201, 140, 112 },{ 205, 145, 119 },{ 213, 157, 132 },{ 220, 168, 147 },{ 226, 178, 158 },{ 238, 192, 176 },{ 248, 202, 186 },{ 242, 196, 180 },{ 237, 190, 172 },{ 228, 180, 160 },{ 221, 170, 149 },{ 215, 160, 139 },{ 214, 158, 133 },{ 210, 152, 128 },{ 202, 145, 118 },{ 208, 151, 124 },{ 204, 148, 121 },{ 203, 149, 121 },{ 192, 138, 110 },{ 169, 120, 90 },{ 160, 113, 83 },{ 168, 124, 95 },{ 161, 117, 88 },{ 155, 114, 84 },{ 158, 117, 85 },{ 161, 120, 88 },{ 156, 115, 85 },{ 141, 102, 71 },{ 135, 98, 69 },{ 120, 83, 57 },{ 100, 64, 38 },{ 88, 54, 29 },{ 89, 63, 46 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 53, 38, 31 },{ 80, 57, 41 },{ 110, 77, 58 },{ 97, 60, 41 },{ 88, 51, 32 },{ 88, 47, 29 },{ 105, 57, 34 },{ 158, 104, 76 },{ 174, 115, 83 },{ 178, 118, 84 },{ 179, 113, 79 },{ 184, 119, 87 },{ 192, 129, 96 },{ 190, 124, 90 },{ 189, 125, 89 },{ 193, 129, 94 },{ 198, 132, 98 },{ 201, 135, 103 },{ 202, 135, 106 },{ 204, 137, 108 },{ 204, 137, 108 },{ 204, 137, 108 },{ 205, 136, 107 },{ 206, 137, 108 },{ 206, 137, 108 },{ 204, 137, 108 },{ 205, 138, 109 },{ 206, 139, 112 },{ 202, 138, 110 },{ 200, 136, 108 },{ 198, 133, 105 },{ 197, 132, 102 },{ 195, 128, 99 },{ 200, 135, 107 },{ 201, 140, 112 },{ 202, 142, 114 },{ 205, 145, 119 },{ 209, 152, 125 },{ 207, 149, 125 },{ 213, 157, 132 },{ 218, 167, 146 },{ 229, 181, 159 },{ 228, 180, 160 },{ 223, 172, 151 },{ 215, 163, 141 },{ 214, 159, 138 },{ 212, 156, 133 },{ 209, 151, 127 },{ 208, 148, 122 },{ 201, 141, 113 },{ 199, 139, 111 },{ 198, 141, 112 },{ 205, 150, 120 },{ 195, 142, 111 },{ 177, 126, 97 },{ 154, 105, 75 },{ 158, 115, 83 },{ 155, 110, 79 },{ 147, 102, 71 },{ 145, 102, 70 },{ 144, 101, 69 },{ 152, 109, 77 },{ 143, 102, 72 },{ 121, 82, 53 },{ 110, 70, 44 },{ 99, 64, 42 },{ 84, 51, 34 },{ 75, 47, 26 },{ 74, 56, 44 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 58, 36, 23 },{ 104, 71, 52 },{ 109, 71, 50 },{ 92, 56, 34 },{ 86, 48, 29 },{ 87, 44, 27 },{ 133, 83, 60 },{ 175, 113, 88 },{ 178, 115, 82 },{ 180, 116, 81 },{ 184, 118, 83 },{ 190, 127, 94 },{ 193, 130, 97 },{ 190, 124, 90 },{ 194, 130, 95 },{ 197, 133, 98 },{ 199, 134, 102 },{ 205, 140, 110 },{ 205, 140, 112 },{ 207, 140, 111 },{ 207, 138, 107 },{ 206, 137, 106 },{ 206, 139, 110 },{ 206, 139, 113 },{ 207, 136, 108 },{ 207, 138, 109 },{ 208, 139, 110 },{ 207, 137, 111 },{ 203, 138, 110 },{ 204, 137, 110 },{ 201, 131, 105 },{ 199, 130, 101 },{ 195, 129, 97 },{ 198, 133, 103 },{ 199, 138, 110 },{ 200, 140, 112 },{ 195, 138, 111 },{ 198, 138, 110 },{ 198, 138, 110 },{ 203, 144, 114 },{ 209, 152, 125 },{ 212, 156, 131 },{ 211, 155, 132 },{ 210, 154, 129 },{ 209, 153, 128 },{ 206, 150, 125 },{ 203, 145, 121 },{ 203, 143, 117 },{ 202, 143, 113 },{ 197, 138, 108 },{ 198, 138, 110 },{ 195, 139, 112 },{ 201, 147, 121 },{ 198, 144, 116 },{ 179, 130, 100 },{ 156, 109, 79 },{ 150, 106, 77 },{ 151, 106, 77 },{ 139, 94, 65 },{ 134, 91, 59 },{ 129, 85, 56 },{ 129, 84, 55 },{ 134, 90, 63 },{ 125, 83, 58 },{ 99, 63, 37 },{ 88, 51, 32 },{ 86, 54, 33 },{ 71, 42, 26 },{ 61, 37, 24 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 66, 53, 45 },{ 79, 53, 38 },{ 116, 81, 59 },{ 100, 62, 39 },{ 85, 50, 28 },{ 84, 46, 27 },{ 102, 59, 40 },{ 161, 110, 83 },{ 182, 117, 87 },{ 182, 118, 83 },{ 185, 119, 85 },{ 187, 121, 86 },{ 188, 125, 90 },{ 189, 126, 93 },{ 196, 131, 99 },{ 196, 131, 99 },{ 200, 134, 102 },{ 205, 140, 110 },{ 207, 142, 114 },{ 206, 141, 113 },{ 206, 141, 111 },{ 208, 139, 108 },{ 208, 139, 108 },{ 207, 140, 111 },{ 208, 141, 115 },{ 212, 138, 111 },{ 208, 141, 112 },{ 209, 142, 115 },{ 207, 140, 113 },{ 205, 140, 112 },{ 204, 137, 110 },{ 201, 129, 104 },{ 198, 131, 104 },{ 199, 134, 106 },{ 199, 134, 106 },{ 200, 136, 108 },{ 199, 138, 110 },{ 198, 137, 109 },{ 195, 135, 107 },{ 198, 139, 109 },{ 196, 137, 107 },{ 195, 134, 105 },{ 201, 141, 113 },{ 203, 143, 115 },{ 200, 140, 112 },{ 202, 142, 116 },{ 201, 144, 117 },{ 198, 141, 114 },{ 199, 139, 111 },{ 197, 138, 108 },{ 196, 135, 104 },{ 194, 135, 105 },{ 194, 137, 108 },{ 197, 142, 112 },{ 193, 140, 109 },{ 183, 132, 101 },{ 159, 110, 80 },{ 153, 109, 80 },{ 146, 101, 72 },{ 137, 93, 64 },{ 130, 86, 59 },{ 122, 78, 51 },{ 118, 74, 47 },{ 113, 71, 46 },{ 111, 69, 44 },{ 104, 70, 45 },{ 83, 48, 26 },{ 75, 44, 23 },{ 76, 44, 29 },{ 63, 35, 23 },{ 65, 45, 34 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 62, 44, 34 },{ 96, 65, 47 },{ 105, 70, 48 },{ 87, 50, 31 },{ 83, 48, 28 },{ 81, 43, 22 },{ 124, 79, 56 },{ 172, 121, 90 },{ 184, 124, 90 },{ 185, 120, 88 },{ 187, 124, 91 },{ 190, 125, 93 },{ 188, 122, 88 },{ 194, 129, 97 },{ 198, 135, 104 },{ 199, 132, 103 },{ 201, 136, 106 },{ 206, 142, 114 },{ 205, 140, 112 },{ 205, 140, 110 },{ 208, 143, 113 },{ 210, 143, 114 },{ 209, 142, 115 },{ 209, 142, 115 },{ 212, 140, 115 },{ 215, 139, 115 },{ 211, 141, 115 },{ 210, 140, 115 },{ 211, 141, 116 },{ 208, 141, 114 },{ 205, 135, 109 },{ 202, 128, 103 },{ 202, 132, 106 },{ 198, 134, 107 },{ 200, 136, 109 },{ 196, 132, 104 },{ 194, 130, 102 },{ 197, 136, 107 },{ 194, 133, 105 },{ 194, 134, 106 },{ 196, 136, 108 },{ 191, 130, 101 },{ 194, 135, 105 },{ 196, 136, 108 },{ 196, 135, 107 },{ 195, 134, 106 },{ 195, 135, 107 },{ 194, 134, 106 },{ 193, 134, 104 },{ 189, 128, 99 },{ 190, 129, 98 },{ 188, 128, 94 },{ 193, 134, 102 },{ 193, 137, 104 },{ 186, 132, 98 },{ 180, 127, 95 },{ 162, 109, 78 },{ 149, 104, 75 },{ 154, 110, 81 },{ 135, 91, 64 },{ 121, 77, 52 },{ 112, 70, 45 },{ 108, 68, 42 },{ 105, 65, 40 },{ 97, 57, 32 },{ 91, 55, 31 },{ 85, 54, 33 },{ 64, 37, 20 },{ 63, 34, 18 },{ 65, 39, 24 },{ 58, 36, 23 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 54, 35, 21 },{ 93, 61, 40 },{ 100, 65, 43 },{ 95, 58, 40 },{ 90, 53, 35 },{ 89, 48, 28 },{ 138, 90, 67 },{ 177, 120, 91 },{ 183, 122, 91 },{ 186, 125, 94 },{ 192, 133, 103 },{ 193, 130, 99 },{ 194, 125, 94 },{ 199, 134, 102 },{ 201, 138, 107 },{ 205, 138, 111 },{ 206, 145, 116 },{ 212, 148, 121 },{ 212, 145, 118 },{ 213, 146, 117 },{ 215, 150, 120 },{ 215, 149, 123 },{ 214, 148, 124 },{ 216, 146, 121 },{ 215, 139, 115 },{ 212, 137, 114 },{ 213, 143, 118 },{ 212, 140, 115 },{ 212, 140, 115 },{ 208, 144, 117 },{ 205, 139, 115 },{ 203, 133, 108 },{ 203, 133, 107 },{ 201, 137, 112 },{ 200, 134, 108 },{ 197, 133, 106 },{ 195, 131, 104 },{ 194, 133, 105 },{ 195, 134, 106 },{ 192, 132, 104 },{ 194, 134, 106 },{ 191, 132, 102 },{ 188, 128, 100 },{ 189, 129, 101 },{ 193, 132, 104 },{ 189, 128, 99 },{ 191, 130, 101 },{ 190, 129, 100 },{ 187, 126, 95 },{ 189, 130, 100 },{ 187, 126, 95 },{ 186, 125, 94 },{ 192, 135, 105 },{ 189, 134, 103 },{ 188, 133, 102 },{ 177, 124, 92 },{ 162, 112, 79 },{ 146, 99, 69 },{ 144, 99, 70 },{ 134, 93, 65 },{ 113, 71, 46 },{ 101, 59, 35 },{ 93, 56, 30 },{ 89, 52, 26 },{ 88, 51, 25 },{ 77, 40, 21 },{ 76, 43, 24 },{ 68, 42, 25 },{ 47, 28, 13 },{ 51, 29, 15 },{ 49, 26, 18 },{ 66, 49, 42 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 60, 38, 25 },{ 103, 68, 48 },{ 119, 78, 56 },{ 96, 60, 36 },{ 93, 55, 34 },{ 111, 66, 43 },{ 155, 105, 78 },{ 179, 124, 94 },{ 183, 124, 94 },{ 190, 130, 102 },{ 194, 133, 102 },{ 192, 129, 96 },{ 196, 133, 102 },{ 202, 138, 110 },{ 207, 144, 113 },{ 208, 143, 113 },{ 208, 143, 113 },{ 214, 149, 119 },{ 216, 149, 120 },{ 216, 151, 123 },{ 216, 152, 125 },{ 217, 153, 126 },{ 217, 151, 125 },{ 215, 148, 122 },{ 214, 147, 121 },{ 214, 144, 118 },{ 216, 144, 120 },{ 214, 142, 118 },{ 210, 143, 117 },{ 211, 144, 118 },{ 207, 140, 114 },{ 208, 138, 113 },{ 203, 136, 110 },{ 201, 137, 112 },{ 201, 137, 112 },{ 197, 133, 108 },{ 194, 133, 105 },{ 192, 128, 100 },{ 191, 127, 99 },{ 192, 131, 103 },{ 189, 132, 105 },{ 190, 132, 108 },{ 185, 130, 100 },{ 183, 124, 94 },{ 186, 122, 94 },{ 186, 125, 96 },{ 186, 125, 94 },{ 186, 125, 94 },{ 184, 125, 95 },{ 184, 123, 92 },{ 186, 130, 97 },{ 185, 126, 94 },{ 186, 127, 95 },{ 191, 132, 98 },{ 183, 127, 92 },{ 172, 119, 85 },{ 161, 106, 76 },{ 146, 95, 64 },{ 136, 87, 57 },{ 124, 78, 52 },{ 110, 70, 45 },{ 85, 49, 25 },{ 81, 46, 24 },{ 74, 41, 22 },{ 68, 35, 18 },{ 70, 39, 21 },{ 58, 29, 13 },{ 61, 35, 22 },{ 53, 31, 18 },{ 38, 21, 13 },{ 44, 25, 19 },{ 51, 34, 27 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 59, 37, 24 },{ 114, 78, 56 },{ 121, 79, 55 },{ 106, 66, 41 },{ 114, 72, 50 },{ 120, 74, 50 },{ 160, 110, 83 },{ 183, 126, 97 },{ 190, 129, 100 },{ 191, 127, 100 },{ 193, 128, 98 },{ 194, 130, 95 },{ 202, 137, 107 },{ 207, 143, 115 },{ 210, 146, 118 },{ 213, 150, 119 },{ 213, 150, 119 },{ 214, 150, 122 },{ 219, 152, 126 },{ 218, 151, 125 },{ 218, 152, 126 },{ 214, 153, 125 },{ 219, 155, 128 },{ 216, 150, 124 },{ 217, 146, 124 },{ 215, 148, 122 },{ 216, 146, 121 },{ 215, 148, 122 },{ 216, 148, 125 },{ 213, 147, 123 },{ 210, 144, 120 },{ 207, 141, 117 },{ 203, 137, 113 },{ 200, 138, 113 },{ 199, 137, 112 },{ 199, 137, 112 },{ 195, 133, 108 },{ 193, 132, 104 },{ 190, 129, 100 },{ 190, 129, 101 },{ 188, 131, 102 },{ 187, 125, 100 },{ 185, 126, 96 },{ 182, 121, 90 },{ 181, 118, 87 },{ 181, 120, 89 },{ 185, 122, 91 },{ 181, 118, 87 },{ 182, 121, 90 },{ 181, 121, 87 },{ 180, 120, 86 },{ 189, 132, 102 },{ 188, 128, 100 },{ 186, 127, 95 },{ 179, 123, 86 },{ 169, 113, 78 },{ 159, 108, 77 },{ 146, 93, 62 },{ 132, 83, 53 },{ 114, 70, 43 },{ 98, 61, 35 },{ 84, 48, 26 },{ 68, 35, 16 },{ 65, 36, 20 },{ 56, 30, 15 },{ 51, 25, 12 },{ 54, 28, 15 },{ 53, 27, 14 },{ 55, 33, 20 },{ 35, 20, 13 },{ 31, 16, 9 },{ 39, 24, 19 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 61, 37, 25 },{ 109, 73, 51 },{ 124, 82, 58 },{ 116, 74, 50 },{ 112, 70, 46 },{ 121, 74, 48 },{ 162, 111, 84 },{ 188, 131, 104 },{ 189, 128, 99 },{ 187, 126, 95 },{ 193, 130, 99 },{ 198, 133, 103 },{ 205, 140, 110 },{ 212, 147, 119 },{ 214, 149, 119 },{ 218, 153, 125 },{ 219, 152, 126 },{ 219, 155, 128 },{ 219, 153, 127 },{ 221, 153, 130 },{ 219, 153, 129 },{ 216, 154, 129 },{ 220, 156, 131 },{ 218, 152, 128 },{ 217, 149, 126 },{ 215, 149, 125 },{ 217, 151, 127 },{ 217, 153, 128 },{ 218, 152, 128 },{ 213, 149, 124 },{ 212, 148, 123 },{ 209, 147, 122 },{ 206, 144, 119 },{ 205, 143, 118 },{ 201, 137, 112 },{ 197, 133, 108 },{ 194, 133, 105 },{ 193, 132, 104 },{ 189, 128, 100 },{ 189, 128, 99 },{ 186, 127, 97 },{ 185, 120, 92 },{ 184, 120, 92 },{ 179, 118, 89 },{ 179, 118, 87 },{ 177, 116, 85 },{ 177, 114, 83 },{ 176, 111, 79 },{ 180, 116, 81 },{ 179, 118, 87 },{ 178, 115, 80 },{ 187, 124, 91 },{ 191, 132, 102 },{ 183, 124, 92 },{ 178, 120, 83 },{ 168, 112, 77 },{ 158, 105, 74 },{ 145, 92, 61 },{ 127, 78, 48 },{ 110, 64, 40 },{ 90, 52, 29 },{ 74, 41, 22 },{ 57, 30, 13 },{ 47, 23, 11 },{ 41, 21, 12 },{ 37, 19, 9 },{ 41, 21, 10 },{ 47, 25, 14 },{ 49, 27, 16 },{ 44, 29, 22 },{ 28, 17, 11 },{ 45, 34, 28 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 63, 39, 26 },{ 103, 68, 48 },{ 123, 82, 60 },{ 120, 79, 57 },{ 107, 65, 41 },{ 128, 80, 57 },{ 166, 116, 91 },{ 186, 130, 105 },{ 190, 131, 101 },{ 191, 128, 95 },{ 198, 133, 103 },{ 202, 137, 109 },{ 209, 145, 117 },{ 216, 152, 124 },{ 218, 155, 124 },{ 223, 159, 132 },{ 227, 162, 140 },{ 225, 161, 136 },{ 221, 157, 132 },{ 221, 157, 132 },{ 220, 156, 129 },{ 220, 156, 129 },{ 218, 152, 126 },{ 217, 151, 125 },{ 218, 152, 128 },{ 217, 153, 128 },{ 215, 153, 128 },{ 213, 151, 126 },{ 216, 152, 125 },{ 214, 150, 123 },{ 211, 150, 122 },{ 212, 150, 125 },{ 207, 145, 120 },{ 205, 140, 118 },{ 201, 134, 108 },{ 196, 129, 102 },{ 197, 132, 104 },{ 192, 128, 100 },{ 191, 127, 100 },{ 191, 127, 100 },{ 188, 124, 96 },{ 185, 121, 93 },{ 184, 120, 93 },{ 183, 122, 94 },{ 177, 114, 83 },{ 175, 112, 81 },{ 175, 110, 80 },{ 174, 109, 79 },{ 177, 112, 80 },{ 178, 117, 88 },{ 182, 119, 84 },{ 177, 113, 77 },{ 181, 121, 85 },{ 182, 123, 89 },{ 173, 114, 80 },{ 164, 110, 76 },{ 155, 98, 68 },{ 144, 91, 60 },{ 125, 76, 46 },{ 105, 59, 35 },{ 87, 49, 28 },{ 69, 38, 20 },{ 53, 27, 14 },{ 39, 19, 10 },{ 33, 16, 9 },{ 30, 15, 8 },{ 26, 13, 5 },{ 28, 15, 6 },{ 38, 19, 12 },{ 41, 24, 17 },{ 35, 20, 13 },{ 49, 38, 32 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 67, 43, 30 },{ 91, 60, 40 },{ 115, 77, 56 },{ 117, 76, 54 },{ 113, 71, 47 },{ 136, 89, 63 },{ 169, 119, 94 },{ 187, 130, 103 },{ 190, 131, 99 },{ 192, 129, 98 },{ 202, 138, 110 },{ 206, 142, 114 },{ 211, 147, 119 },{ 216, 155, 127 },{ 223, 161, 136 },{ 227, 165, 140 },{ 226, 164, 141 },{ 225, 163, 138 },{ 223, 161, 136 },{ 221, 159, 134 },{ 222, 158, 131 },{ 219, 153, 127 },{ 217, 150, 124 },{ 214, 147, 121 },{ 217, 151, 127 },{ 219, 153, 129 },{ 214, 152, 127 },{ 215, 151, 124 },{ 216, 150, 124 },{ 213, 146, 119 },{ 211, 145, 119 },{ 212, 148, 121 },{ 206, 140, 114 },{ 201, 135, 111 },{ 198, 131, 105 },{ 199, 129, 103 },{ 194, 129, 101 },{ 191, 127, 99 },{ 192, 128, 101 },{ 189, 125, 98 },{ 188, 124, 96 },{ 183, 123, 95 },{ 181, 120, 92 },{ 183, 119, 91 },{ 176, 111, 79 },{ 175, 106, 75 },{ 171, 104, 75 },{ 171, 106, 76 },{ 171, 110, 79 },{ 177, 114, 83 },{ 183, 120, 87 },{ 175, 117, 80 },{ 173, 113, 76 },{ 176, 116, 80 },{ 173, 117, 84 },{ 163, 107, 74 },{ 150, 96, 62 },{ 133, 78, 48 },{ 115, 65, 38 },{ 100, 58, 33 },{ 83, 47, 25 },{ 68, 37, 19 },{ 54, 28, 15 },{ 41, 23, 13 },{ 26, 13, 5 },{ 25, 12, 6 },{ 19, 10, 5 },{ 17, 10, 4 },{ 28, 13, 6 },{ 38, 21, 14 },{ 35, 18, 10 },{ 51, 40, 34 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 54, 30, 17 },{ 84, 53, 33 },{ 119, 81, 58 },{ 131, 91, 66 },{ 126, 81, 58 },{ 138, 91, 65 },{ 171, 121, 94 },{ 188, 131, 102 },{ 191, 130, 99 },{ 193, 132, 101 },{ 203, 142, 113 },{ 207, 146, 118 },{ 214, 153, 125 },{ 221, 160, 132 },{ 226, 164, 139 },{ 228, 166, 143 },{ 229, 167, 144 },{ 227, 169, 145 },{ 225, 163, 140 },{ 223, 159, 134 },{ 221, 157, 132 },{ 218, 154, 127 },{ 219, 153, 129 },{ 220, 154, 130 },{ 220, 152, 131 },{ 217, 151, 127 },{ 214, 148, 124 },{ 216, 150, 124 },{ 216, 149, 123 },{ 213, 143, 117 },{ 209, 139, 113 },{ 209, 142, 116 },{ 207, 140, 114 },{ 202, 137, 109 },{ 200, 133, 106 },{ 200, 133, 107 },{ 194, 130, 103 },{ 193, 129, 102 },{ 193, 129, 101 },{ 189, 125, 97 },{ 186, 123, 92 },{ 183, 120, 89 },{ 184, 120, 92 },{ 179, 115, 87 },{ 174, 110, 82 },{ 173, 106, 77 },{ 175, 108, 79 },{ 176, 111, 79 },{ 172, 107, 75 },{ 173, 109, 74 },{ 180, 121, 89 },{ 179, 124, 93 },{ 177, 121, 88 },{ 173, 113, 79 },{ 172, 113, 81 },{ 165, 111, 77 },{ 154, 101, 67 },{ 134, 79, 49 },{ 114, 64, 37 },{ 96, 54, 30 },{ 81, 46, 26 },{ 68, 36, 21 },{ 51, 25, 12 },{ 43, 25, 15 },{ 31, 17, 8 },{ 22, 11, 5 },{ 18, 10, 7 },{ 16, 8, 5 },{ 24, 11, 5 },{ 35, 20, 13 },{ 34, 20, 11 },{ 52, 41, 37 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 62, 36, 23 },{ 100, 68, 47 },{ 128, 88, 63 },{ 128, 86, 61 },{ 123, 79, 54 },{ 140, 93, 67 },{ 171, 122, 92 },{ 190, 133, 104 },{ 191, 130, 99 },{ 193, 130, 99 },{ 206, 142, 115 },{ 211, 149, 124 },{ 215, 154, 126 },{ 221, 161, 133 },{ 225, 165, 139 },{ 227, 167, 143 },{ 230, 169, 148 },{ 227, 166, 145 },{ 222, 160, 137 },{ 222, 158, 133 },{ 221, 157, 132 },{ 221, 157, 130 },{ 218, 154, 129 },{ 221, 155, 133 },{ 219, 153, 131 },{ 217, 151, 129 },{ 212, 146, 122 },{ 214, 148, 124 },{ 216, 150, 126 },{ 214, 147, 121 },{ 210, 140, 115 },{ 207, 140, 114 },{ 210, 144, 118 },{ 205, 141, 114 },{ 204, 140, 113 },{ 203, 139, 114 },{ 196, 135, 107 },{ 194, 130, 102 },{ 191, 126, 96 },{ 188, 123, 93 },{ 183, 118, 88 },{ 183, 119, 84 },{ 183, 118, 88 },{ 182, 121, 92 },{ 180, 120, 92 },{ 178, 114, 86 },{ 176, 111, 81 },{ 174, 111, 78 },{ 176, 111, 79 },{ 177, 114, 79 },{ 179, 123, 90 },{ 181, 124, 95 },{ 181, 124, 94 },{ 174, 118, 83 },{ 164, 105, 71 },{ 161, 107, 73 },{ 155, 101, 67 },{ 140, 85, 55 },{ 114, 64, 37 },{ 89, 47, 25 },{ 75, 40, 21 },{ 62, 33, 19 },{ 49, 25, 13 },{ 37, 19, 9 },{ 35, 18, 10 },{ 29, 16, 8 },{ 21, 10, 6 },{ 19, 9, 7 },{ 21, 10, 4 },{ 33, 20, 12 },{ 32, 17, 10 },{ 53, 42, 38 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 69, 43, 30 },{ 96, 61, 41 },{ 125, 83, 59 },{ 130, 88, 63 },{ 134, 89, 66 },{ 151, 105, 81 },{ 174, 124, 97 },{ 187, 132, 102 },{ 195, 134, 105 },{ 199, 138, 109 },{ 206, 145, 117 },{ 211, 151, 123 },{ 216, 156, 128 },{ 226, 164, 139 },{ 227, 165, 144 },{ 226, 164, 143 },{ 223, 161, 136 },{ 222, 160, 137 },{ 226, 164, 141 },{ 222, 160, 135 },{ 220, 159, 131 },{ 219, 155, 128 },{ 218, 152, 126 },{ 217, 153, 128 },{ 216, 154, 131 },{ 215, 150, 128 },{ 214, 149, 127 },{ 215, 150, 128 },{ 215, 153, 128 },{ 212, 148, 123 },{ 212, 144, 121 },{ 211, 145, 121 },{ 208, 146, 121 },{ 205, 143, 118 },{ 203, 141, 118 },{ 203, 141, 116 },{ 198, 137, 109 },{ 195, 132, 101 },{ 194, 128, 96 },{ 186, 119, 90 },{ 187, 122, 92 },{ 184, 123, 94 },{ 185, 122, 91 },{ 181, 122, 90 },{ 183, 124, 94 },{ 188, 123, 93 },{ 180, 117, 84 },{ 183, 124, 92 },{ 187, 127, 99 },{ 181, 122, 90 },{ 178, 115, 84 },{ 176, 115, 84 },{ 179, 120, 88 },{ 175, 119, 84 },{ 170, 114, 77 },{ 166, 106, 72 },{ 150, 91, 59 },{ 139, 82, 53 },{ 120, 70, 45 },{ 86, 45, 23 },{ 65, 32, 15 },{ 57, 27, 16 },{ 51, 24, 15 },{ 37, 19, 9 },{ 30, 16, 7 },{ 31, 16, 9 },{ 25, 11, 8 },{ 21, 10, 6 },{ 23, 12, 6 },{ 29, 16, 10 },{ 29, 14, 7 },{ 56, 45, 43 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 59, 32, 21 },{ 86, 53, 36 },{ 118, 80, 57 },{ 133, 93, 67 },{ 136, 92, 65 },{ 150, 103, 77 },{ 175, 125, 98 },{ 186, 129, 99 },{ 195, 134, 105 },{ 200, 143, 114 },{ 204, 145, 115 },{ 213, 152, 124 },{ 217, 157, 131 },{ 224, 164, 138 },{ 223, 161, 136 },{ 222, 158, 133 },{ 225, 161, 136 },{ 226, 165, 144 },{ 228, 168, 144 },{ 225, 163, 138 },{ 222, 158, 133 },{ 218, 154, 127 },{ 215, 154, 126 },{ 218, 158, 132 },{ 217, 156, 135 },{ 216, 150, 128 },{ 210, 144, 118 },{ 214, 148, 122 },{ 215, 153, 128 },{ 211, 147, 122 },{ 209, 143, 117 },{ 207, 143, 116 },{ 206, 144, 119 },{ 205, 141, 116 },{ 202, 136, 110 },{ 200, 136, 108 },{ 197, 136, 107 },{ 198, 133, 105 },{ 196, 129, 102 },{ 188, 123, 95 },{ 191, 126, 96 },{ 186, 125, 96 },{ 185, 126, 96 },{ 183, 120, 89 },{ 181, 118, 87 },{ 184, 123, 92 },{ 182, 121, 90 },{ 185, 121, 93 },{ 185, 126, 96 },{ 188, 129, 97 },{ 182, 123, 89 },{ 174, 115, 81 },{ 172, 114, 77 },{ 175, 117, 80 },{ 171, 115, 80 },{ 164, 108, 73 },{ 152, 93, 61 },{ 141, 78, 47 },{ 121, 70, 43 },{ 100, 59, 37 },{ 74, 39, 20 },{ 55, 27, 13 },{ 42, 23, 9 },{ 37, 20, 10 },{ 29, 14, 9 },{ 24, 13, 9 },{ 21, 10, 6 },{ 20, 11, 6 },{ 28, 14, 11 },{ 28, 13, 8 },{ 27, 17, 8 },{ 56, 48, 46 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 46, 24, 13 },{ 67, 38, 22 },{ 107, 71, 49 },{ 126, 84, 60 },{ 137, 91, 67 },{ 153, 106, 80 },{ 180, 130, 103 },{ 183, 126, 96 },{ 199, 139, 111 },{ 207, 147, 119 },{ 209, 149, 121 },{ 215, 155, 127 },{ 214, 154, 128 },{ 218, 156, 131 },{ 223, 159, 134 },{ 222, 158, 133 },{ 224, 160, 135 },{ 223, 161, 138 },{ 223, 161, 136 },{ 222, 158, 131 },{ 219, 153, 127 },{ 214, 148, 122 },{ 213, 149, 122 },{ 216, 154, 129 },{ 214, 152, 129 },{ 214, 147, 121 },{ 210, 143, 117 },{ 208, 141, 115 },{ 212, 146, 122 },{ 213, 147, 123 },{ 211, 144, 117 },{ 208, 141, 114 },{ 204, 140, 113 },{ 205, 140, 110 },{ 205, 140, 112 },{ 202, 138, 111 },{ 199, 135, 107 },{ 197, 133, 105 },{ 194, 130, 102 },{ 191, 130, 101 },{ 192, 131, 103 },{ 187, 130, 101 },{ 188, 128, 100 },{ 185, 124, 95 },{ 181, 120, 91 },{ 183, 124, 94 },{ 185, 126, 96 },{ 187, 126, 97 },{ 186, 120, 94 },{ 180, 117, 84 },{ 179, 119, 85 },{ 182, 123, 89 },{ 176, 117, 83 },{ 175, 119, 84 },{ 175, 119, 84 },{ 162, 103, 69 },{ 151, 91, 57 },{ 141, 80, 49 },{ 124, 70, 42 },{ 105, 59, 35 },{ 87, 46, 26 },{ 67, 34, 19 },{ 52, 25, 14 },{ 38, 20, 10 },{ 30, 15, 8 },{ 22, 11, 7 },{ 21, 10, 6 },{ 20, 11, 6 },{ 29, 15, 12 },{ 32, 17, 12 },{ 29, 16, 8 },{ 60, 52, 50 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 37, 18, 11 },{ 52, 30, 16 },{ 82, 49, 30 },{ 117, 74, 55 },{ 135, 89, 65 },{ 156, 109, 83 },{ 179, 128, 99 },{ 184, 127, 98 },{ 202, 142, 116 },{ 208, 151, 124 },{ 212, 155, 128 },{ 216, 156, 130 },{ 219, 159, 135 },{ 222, 160, 137 },{ 222, 156, 132 },{ 220, 156, 131 },{ 218, 156, 131 },{ 220, 156, 129 },{ 221, 157, 130 },{ 220, 156, 129 },{ 222, 156, 132 },{ 218, 152, 128 },{ 214, 148, 122 },{ 212, 148, 121 },{ 214, 148, 122 },{ 213, 146, 120 },{ 211, 144, 117 },{ 210, 143, 117 },{ 212, 144, 121 },{ 212, 144, 121 },{ 210, 143, 117 },{ 207, 140, 113 },{ 206, 141, 113 },{ 206, 141, 113 },{ 207, 141, 115 },{ 205, 141, 114 },{ 202, 138, 111 },{ 198, 133, 105 },{ 192, 131, 102 },{ 193, 134, 104 },{ 189, 128, 100 },{ 185, 128, 98 },{ 180, 119, 90 },{ 182, 121, 92 },{ 181, 118, 87 },{ 180, 115, 85 },{ 184, 121, 90 },{ 185, 124, 93 },{ 185, 124, 95 },{ 184, 121, 88 },{ 179, 119, 85 },{ 180, 121, 87 },{ 179, 120, 86 },{ 180, 124, 89 },{ 173, 117, 82 },{ 167, 108, 74 },{ 153, 90, 55 },{ 139, 80, 48 },{ 129, 74, 44 },{ 114, 63, 36 },{ 96, 48, 26 },{ 76, 34, 18 },{ 57, 28, 14 },{ 43, 21, 10 },{ 33, 16, 9 },{ 23, 12, 8 },{ 21, 10, 6 },{ 20, 11, 6 },{ 27, 13, 10 },{ 30, 15, 10 },{ 31, 16, 9 },{ 66, 58, 56 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 42, 25, 18 },{ 43, 25, 13 },{ 81, 50, 32 },{ 115, 74, 52 },{ 131, 87, 60 },{ 158, 113, 84 },{ 179, 128, 101 },{ 189, 129, 101 },{ 201, 141, 115 },{ 209, 152, 125 },{ 214, 157, 130 },{ 218, 161, 134 },{ 220, 160, 136 },{ 220, 155, 133 },{ 220, 154, 130 },{ 224, 158, 134 },{ 224, 162, 137 },{ 224, 160, 133 },{ 225, 161, 134 },{ 222, 158, 133 },{ 221, 155, 133 },{ 221, 155, 133 },{ 218, 152, 128 },{ 213, 147, 121 },{ 215, 150, 122 },{ 216, 150, 124 },{ 210, 144, 118 },{ 209, 143, 117 },{ 210, 144, 120 },{ 210, 144, 118 },{ 211, 145, 119 },{ 207, 143, 116 },{ 206, 142, 115 },{ 208, 142, 116 },{ 205, 139, 113 },{ 202, 138, 110 },{ 203, 137, 111 },{ 200, 134, 108 },{ 197, 133, 106 },{ 193, 134, 104 },{ 187, 126, 97 },{ 181, 122, 90 },{ 178, 117, 86 },{ 175, 114, 83 },{ 177, 114, 83 },{ 179, 114, 84 },{ 180, 117, 86 },{ 182, 125, 95 },{ 185, 130, 99 },{ 186, 130, 97 },{ 184, 128, 95 },{ 185, 129, 94 },{ 183, 124, 90 },{ 186, 128, 91 },{ 179, 121, 84 },{ 171, 113, 76 },{ 159, 99, 63 },{ 138, 82, 49 },{ 132, 75, 45 },{ 128, 73, 43 },{ 113, 62, 35 },{ 87, 42, 23 },{ 63, 30, 15 },{ 49, 25, 13 },{ 34, 15, 8 },{ 29, 16, 10 },{ 23, 12, 6 },{ 23, 12, 8 },{ 27, 14, 8 },{ 31, 14, 7 },{ 38, 21, 13 },{ 69, 61, 59 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 49, 32, 25 },{ 43, 23, 14 },{ 85, 52, 35 },{ 119, 79, 54 },{ 145, 101, 74 },{ 155, 110, 81 },{ 178, 127, 100 },{ 188, 128, 100 },{ 202, 142, 114 },{ 211, 151, 125 },{ 219, 159, 133 },{ 213, 153, 127 },{ 215, 155, 131 },{ 221, 156, 134 },{ 221, 153, 132 },{ 226, 160, 138 },{ 226, 164, 141 },{ 227, 162, 140 },{ 224, 160, 135 },{ 221, 157, 130 },{ 219, 155, 128 },{ 221, 157, 130 },{ 221, 155, 129 },{ 216, 150, 124 },{ 216, 150, 126 },{ 214, 150, 125 },{ 212, 148, 121 },{ 209, 145, 118 },{ 212, 147, 119 },{ 210, 145, 117 },{ 209, 143, 117 },{ 210, 146, 119 },{ 209, 145, 118 },{ 209, 144, 116 },{ 207, 137, 111 },{ 200, 133, 106 },{ 202, 138, 111 },{ 202, 138, 113 },{ 198, 136, 111 },{ 194, 134, 108 },{ 188, 131, 102 },{ 185, 126, 96 },{ 180, 121, 91 },{ 180, 119, 90 },{ 179, 118, 89 },{ 178, 121, 92 },{ 182, 127, 97 },{ 183, 127, 100 },{ 186, 130, 103 },{ 189, 134, 104 },{ 190, 137, 105 },{ 190, 137, 105 },{ 184, 130, 96 },{ 180, 124, 89 },{ 185, 129, 94 },{ 174, 116, 79 },{ 157, 99, 62 },{ 142, 83, 51 },{ 132, 73, 43 },{ 131, 74, 44 },{ 126, 73, 42 },{ 102, 56, 32 },{ 74, 37, 18 },{ 57, 29, 15 },{ 46, 26, 17 },{ 34, 19, 12 },{ 28, 13, 6 },{ 28, 13, 8 },{ 32, 17, 10 },{ 36, 17, 10 },{ 43, 23, 16 },{ 73, 65, 63 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 53, 38, 33 },{ 57, 30, 23 },{ 93, 57, 41 },{ 114, 73, 51 },{ 144, 100, 73 },{ 152, 107, 78 },{ 180, 129, 102 },{ 187, 130, 101 },{ 202, 142, 114 },{ 214, 153, 125 },{ 213, 152, 124 },{ 211, 149, 124 },{ 217, 155, 132 },{ 222, 156, 134 },{ 223, 157, 135 },{ 231, 165, 143 },{ 234, 169, 149 },{ 228, 163, 141 },{ 225, 159, 133 },{ 224, 159, 131 },{ 221, 157, 129 },{ 219, 155, 128 },{ 218, 152, 126 },{ 215, 149, 123 },{ 215, 149, 123 },{ 213, 147, 121 },{ 210, 146, 119 },{ 207, 142, 114 },{ 207, 140, 111 },{ 209, 140, 111 },{ 207, 140, 113 },{ 207, 141, 115 },{ 207, 141, 115 },{ 208, 144, 116 },{ 207, 140, 113 },{ 204, 138, 112 },{ 203, 142, 114 },{ 193, 132, 104 },{ 192, 131, 103 },{ 195, 135, 107 },{ 188, 131, 102 },{ 184, 125, 95 },{ 182, 125, 95 },{ 183, 124, 94 },{ 182, 122, 94 },{ 179, 122, 93 },{ 183, 126, 99 },{ 189, 132, 105 },{ 194, 138, 113 },{ 190, 137, 106 },{ 191, 140, 109 },{ 196, 147, 117 },{ 197, 146, 117 },{ 188, 135, 104 },{ 189, 136, 104 },{ 178, 122, 87 },{ 158, 100, 63 },{ 146, 87, 53 },{ 134, 75, 45 },{ 131, 72, 42 },{ 132, 77, 46 },{ 119, 68, 41 },{ 96, 51, 30 },{ 68, 35, 18 },{ 48, 24, 12 },{ 39, 20, 13 },{ 37, 20, 12 },{ 36, 19, 12 },{ 36, 19, 11 },{ 42, 22, 13 },{ 46, 21, 14 },{ 73, 63, 61 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 63, 52, 50 },{ 55, 30, 23 },{ 75, 43, 28 },{ 91, 54, 35 },{ 123, 81, 57 },{ 147, 100, 72 },{ 174, 123, 94 },{ 188, 131, 101 },{ 197, 140, 113 },{ 211, 149, 124 },{ 212, 151, 123 },{ 213, 149, 124 },{ 219, 157, 134 },{ 224, 159, 137 },{ 225, 160, 138 },{ 232, 167, 145 },{ 231, 169, 148 },{ 226, 162, 137 },{ 226, 160, 134 },{ 227, 161, 135 },{ 225, 161, 136 },{ 221, 157, 132 },{ 221, 155, 129 },{ 218, 153, 125 },{ 215, 151, 123 },{ 213, 148, 120 },{ 208, 144, 117 },{ 207, 142, 114 },{ 206, 137, 108 },{ 210, 139, 111 },{ 211, 144, 117 },{ 208, 142, 116 },{ 207, 142, 114 },{ 206, 145, 117 },{ 203, 139, 112 },{ 199, 138, 110 },{ 199, 138, 110 },{ 192, 128, 101 },{ 193, 132, 104 },{ 197, 138, 108 },{ 190, 129, 100 },{ 187, 126, 97 },{ 185, 126, 96 },{ 180, 123, 93 },{ 178, 119, 89 },{ 179, 120, 90 },{ 183, 124, 94 },{ 184, 127, 98 },{ 189, 136, 105 },{ 195, 142, 110 },{ 197, 146, 115 },{ 199, 152, 122 },{ 202, 153, 123 },{ 200, 151, 121 },{ 193, 142, 111 },{ 183, 130, 96 },{ 167, 113, 77 },{ 152, 96, 61 },{ 138, 79, 49 },{ 136, 77, 47 },{ 134, 77, 47 },{ 124, 67, 38 },{ 112, 62, 37 },{ 90, 52, 33 },{ 60, 32, 20 },{ 37, 19, 9 },{ 34, 17, 9 },{ 41, 22, 15 },{ 40, 21, 14 },{ 44, 22, 11 },{ 52, 25, 14 },{ 78, 64, 61 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 55, 37, 27 },{ 51, 27, 14 },{ 74, 42, 27 },{ 101, 60, 40 },{ 139, 92, 66 },{ 174, 121, 90 },{ 187, 130, 100 },{ 198, 138, 114 },{ 210, 150, 124 },{ 211, 149, 124 },{ 211, 149, 124 },{ 218, 156, 133 },{ 222, 162, 138 },{ 229, 167, 144 },{ 230, 168, 145 },{ 229, 168, 147 },{ 228, 166, 143 },{ 226, 162, 137 },{ 227, 163, 136 },{ 226, 162, 137 },{ 221, 159, 136 },{ 223, 159, 134 },{ 221, 157, 130 },{ 216, 155, 127 },{ 215, 151, 123 },{ 214, 153, 125 },{ 213, 149, 122 },{ 210, 143, 116 },{ 210, 140, 114 },{ 210, 143, 117 },{ 209, 143, 117 },{ 206, 141, 113 },{ 205, 140, 112 },{ 201, 136, 106 },{ 195, 134, 103 },{ 195, 134, 103 },{ 197, 133, 106 },{ 195, 133, 108 },{ 189, 129, 101 },{ 184, 123, 94 },{ 180, 119, 90 },{ 176, 115, 86 },{ 175, 118, 88 },{ 179, 126, 94 },{ 178, 125, 93 },{ 177, 127, 94 },{ 182, 132, 99 },{ 183, 130, 98 },{ 187, 137, 104 },{ 193, 144, 111 },{ 190, 141, 108 },{ 191, 142, 110 },{ 193, 142, 111 },{ 188, 139, 106 },{ 182, 133, 100 },{ 168, 119, 86 },{ 158, 104, 68 },{ 140, 85, 54 },{ 139, 82, 52 },{ 139, 80, 50 },{ 135, 76, 46 },{ 123, 67, 42 },{ 100, 59, 37 },{ 66, 39, 22 },{ 41, 22, 15 },{ 33, 16, 8 },{ 40, 21, 14 },{ 43, 25, 15 },{ 53, 29, 17 },{ 59, 29, 18 },{ 71, 56, 51 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 48, 23, 16 },{ 59, 32, 21 },{ 92, 54, 31 },{ 134, 88, 62 },{ 171, 117, 89 },{ 191, 131, 103 },{ 195, 138, 109 },{ 209, 143, 119 },{ 211, 149, 126 },{ 210, 150, 124 },{ 215, 154, 126 },{ 221, 160, 132 },{ 227, 167, 143 },{ 230, 172, 152 },{ 234, 173, 154 },{ 229, 168, 147 },{ 224, 162, 139 },{ 222, 158, 133 },{ 224, 158, 132 },{ 221, 157, 130 },{ 219, 158, 130 },{ 219, 158, 130 },{ 218, 152, 126 },{ 217, 151, 125 },{ 214, 150, 123 },{ 215, 151, 126 },{ 210, 143, 117 },{ 209, 139, 113 },{ 204, 137, 110 },{ 204, 140, 112 },{ 202, 138, 113 },{ 205, 140, 112 },{ 197, 133, 105 },{ 192, 131, 103 },{ 194, 133, 105 },{ 193, 128, 98 },{ 181, 116, 84 },{ 170, 107, 74 },{ 171, 108, 75 },{ 163, 103, 69 },{ 163, 109, 73 },{ 170, 118, 78 },{ 173, 119, 81 },{ 169, 119, 84 },{ 174, 126, 88 },{ 177, 132, 93 },{ 178, 133, 100 },{ 179, 129, 94 },{ 179, 131, 95 },{ 173, 125, 85 },{ 170, 122, 86 },{ 169, 114, 83 },{ 169, 115, 81 },{ 174, 125, 92 },{ 169, 120, 87 },{ 159, 104, 73 },{ 148, 89, 59 },{ 147, 88, 56 },{ 148, 89, 57 },{ 142, 86, 53 },{ 130, 75, 45 },{ 102, 54, 32 },{ 70, 37, 22 },{ 48, 26, 15 },{ 34, 17, 10 },{ 36, 19, 12 },{ 42, 22, 13 },{ 61, 29, 18 },{ 97, 50, 32 },{ 158, 113, 90 },{ 216, 178, 165 },{ 221, 174, 164 },{ 153, 133, 132 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 61, 41, 34 },{ 54, 26, 15 },{ 88, 51, 32 },{ 131, 85, 61 },{ 168, 117, 90 },{ 186, 132, 106 },{ 195, 138, 111 },{ 204, 143, 114 },{ 205, 143, 118 },{ 209, 147, 122 },{ 213, 149, 122 },{ 222, 158, 131 },{ 226, 166, 140 },{ 225, 169, 146 },{ 226, 170, 147 },{ 226, 166, 142 },{ 223, 161, 136 },{ 221, 160, 132 },{ 219, 158, 130 },{ 214, 154, 126 },{ 216, 155, 127 },{ 214, 150, 123 },{ 214, 153, 125 },{ 211, 147, 120 },{ 212, 147, 119 },{ 214, 147, 121 },{ 209, 139, 114 },{ 209, 139, 113 },{ 203, 138, 110 },{ 201, 138, 107 },{ 201, 140, 112 },{ 198, 139, 109 },{ 189, 128, 99 },{ 188, 125, 94 },{ 185, 122, 91 },{ 173, 108, 78 },{ 163, 94, 63 },{ 153, 84, 55 },{ 149, 84, 56 },{ 140, 79, 48 },{ 134, 78, 43 },{ 133, 74, 40 },{ 132, 76, 43 },{ 137, 83, 49 },{ 155, 99, 64 },{ 164, 110, 74 },{ 164, 111, 79 },{ 160, 104, 71 },{ 162, 112, 77 },{ 164, 110, 74 },{ 160, 104, 69 },{ 164, 108, 75 },{ 156, 102, 66 },{ 152, 102, 69 },{ 156, 106, 71 },{ 151, 101, 66 },{ 147, 92, 61 },{ 150, 94, 61 },{ 158, 99, 65 },{ 153, 97, 64 },{ 144, 89, 58 },{ 117, 69, 46 },{ 77, 44, 25 },{ 50, 26, 14 },{ 42, 22, 13 },{ 36, 22, 13 },{ 39, 19, 12 },{ 67, 30, 12 },{ 162, 95, 68 },{ 229, 155, 128 },{ 221, 147, 122 },{ 213, 134, 104 },{ 211, 121, 95 },{ 179, 128, 111 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 57, 27, 17 },{ 78, 45, 30 },{ 127, 83, 58 },{ 169, 119, 94 },{ 182, 132, 107 },{ 195, 135, 109 },{ 197, 134, 101 },{ 199, 136, 105 },{ 206, 142, 114 },{ 209, 143, 117 },{ 218, 152, 128 },{ 222, 157, 135 },{ 215, 155, 129 },{ 208, 148, 120 },{ 205, 145, 119 },{ 209, 145, 120 },{ 207, 141, 115 },{ 205, 144, 116 },{ 209, 149, 121 },{ 207, 147, 121 },{ 214, 154, 128 },{ 208, 146, 121 },{ 208, 141, 114 },{ 203, 136, 107 },{ 210, 144, 118 },{ 207, 142, 114 },{ 206, 139, 112 },{ 200, 133, 104 },{ 194, 131, 100 },{ 197, 136, 107 },{ 191, 132, 102 },{ 176, 113, 82 },{ 161, 96, 64 },{ 162, 97, 65 },{ 162, 96, 64 },{ 149, 82, 53 },{ 136, 69, 42 },{ 125, 61, 34 },{ 113, 52, 24 },{ 114, 53, 32 },{ 116, 62, 34 },{ 130, 75, 45 },{ 145, 88, 59 },{ 162, 101, 72 },{ 160, 96, 61 },{ 158, 98, 62 },{ 155, 94, 63 },{ 150, 91, 59 },{ 152, 87, 55 },{ 150, 87, 54 },{ 148, 92, 59 },{ 147, 92, 61 },{ 151, 96, 66 },{ 150, 93, 63 },{ 140, 90, 55 },{ 153, 99, 65 },{ 157, 103, 67 },{ 163, 107, 72 },{ 162, 108, 74 },{ 154, 98, 65 },{ 130, 79, 52 },{ 97, 56, 36 },{ 66, 37, 21 },{ 53, 27, 14 },{ 42, 24, 10 },{ 39, 19, 12 },{ 76, 35, 17 },{ 176, 110, 76 },{ 189, 118, 88 },{ 183, 100, 70 },{ 173, 68, 38 },{ 182, 55, 20 },{ 204, 86, 48 },{ 158, 118, 106 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 69, 39, 28 },{ 70, 38, 25 },{ 109, 64, 41 },{ 159, 107, 83 },{ 181, 131, 106 },{ 189, 132, 103 },{ 190, 126, 91 },{ 196, 133, 102 },{ 201, 141, 113 },{ 200, 143, 116 },{ 191, 131, 105 },{ 184, 124, 96 },{ 175, 116, 86 },{ 167, 108, 74 },{ 160, 99, 68 },{ 159, 96, 63 },{ 160, 95, 63 },{ 172, 105, 76 },{ 181, 116, 88 },{ 191, 130, 102 },{ 201, 140, 112 },{ 203, 139, 112 },{ 202, 135, 108 },{ 196, 131, 101 },{ 208, 144, 117 },{ 209, 148, 119 },{ 210, 146, 118 },{ 198, 131, 102 },{ 184, 119, 87 },{ 191, 130, 99 },{ 186, 125, 96 },{ 177, 114, 81 },{ 157, 92, 60 },{ 151, 85, 53 },{ 149, 82, 55 },{ 131, 67, 40 },{ 121, 59, 34 },{ 122, 62, 36 },{ 125, 65, 37 },{ 116, 56, 32 },{ 115, 54, 33 },{ 135, 68, 42 },{ 143, 78, 48 },{ 139, 79, 51 },{ 133, 71, 48 },{ 132, 71, 43 },{ 132, 71, 42 },{ 129, 66, 35 },{ 132, 66, 34 },{ 139, 74, 44 },{ 136, 75, 44 },{ 129, 72, 42 },{ 133, 77, 50 },{ 128, 73, 43 },{ 138, 84, 50 },{ 155, 99, 66 },{ 162, 106, 71 },{ 165, 109, 74 },{ 168, 114, 80 },{ 165, 109, 76 },{ 143, 88, 58 },{ 108, 62, 39 },{ 77, 46, 28 },{ 58, 31, 20 },{ 56, 29, 18 },{ 52, 24, 13 },{ 99, 46, 28 },{ 174, 110, 75 },{ 170, 99, 69 },{ 166, 79, 52 },{ 178, 67, 47 },{ 187, 62, 40 },{ 183, 59, 23 },{ 199, 106, 75 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 65, 43, 32 },{ 57, 27, 16 },{ 98, 55, 36 },{ 152, 101, 74 },{ 182, 130, 106 },{ 181, 130, 101 },{ 185, 131, 97 },{ 193, 138, 108 },{ 190, 134, 109 },{ 186, 132, 106 },{ 174, 121, 90 },{ 159, 105, 69 },{ 147, 87, 53 },{ 146, 80, 48 },{ 146, 77, 46 },{ 145, 81, 46 },{ 144, 80, 45 },{ 150, 84, 52 },{ 160, 91, 62 },{ 170, 103, 76 },{ 184, 120, 92 },{ 188, 127, 98 },{ 192, 133, 101 },{ 195, 134, 103 },{ 210, 149, 121 },{ 213, 152, 124 },{ 212, 148, 120 },{ 200, 135, 105 },{ 185, 119, 85 },{ 183, 118, 86 },{ 181, 120, 89 },{ 169, 106, 73 },{ 157, 92, 62 },{ 137, 71, 45 },{ 124, 60, 35 },{ 121, 61, 37 },{ 124, 66, 42 },{ 129, 71, 47 },{ 113, 56, 36 },{ 100, 45, 25 },{ 112, 50, 29 },{ 109, 48, 29 },{ 87, 35, 21 },{ 74, 34, 24 },{ 66, 34, 19 },{ 78, 36, 22 },{ 103, 47, 30 },{ 109, 51, 27 },{ 101, 45, 18 },{ 103, 45, 21 },{ 116, 55, 27 },{ 119, 58, 29 },{ 118, 63, 32 },{ 121, 66, 36 },{ 137, 83, 49 },{ 156, 100, 67 },{ 160, 104, 69 },{ 168, 109, 75 },{ 172, 113, 81 },{ 167, 108, 76 },{ 159, 102, 72 },{ 125, 78, 52 },{ 96, 55, 37 },{ 65, 37, 25 },{ 59, 29, 19 },{ 63, 26, 10 },{ 130, 68, 43 },{ 166, 95, 63 },{ 157, 82, 53 },{ 170, 83, 55 },{ 178, 80, 53 },{ 186, 85, 63 },{ 181, 62, 38 },{ 194, 80, 44 },{ 172, 122, 99 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 58, 31, 20 },{ 88, 49, 32 },{ 150, 100, 75 },{ 178, 127, 98 },{ 176, 129, 99 },{ 188, 135, 104 },{ 187, 137, 104 },{ 188, 138, 105 },{ 184, 134, 101 },{ 174, 117, 87 },{ 168, 104, 76 },{ 164, 100, 72 },{ 161, 97, 69 },{ 160, 93, 66 },{ 156, 89, 60 },{ 160, 95, 65 },{ 163, 98, 68 },{ 163, 98, 70 },{ 171, 104, 77 },{ 177, 110, 83 },{ 176, 113, 82 },{ 184, 125, 93 },{ 195, 136, 104 },{ 207, 147, 119 },{ 216, 156, 128 },{ 218, 157, 128 },{ 199, 134, 102 },{ 178, 112, 77 },{ 173, 107, 73 },{ 164, 101, 68 },{ 155, 90, 58 },{ 138, 73, 45 },{ 122, 60, 35 },{ 110, 49, 28 },{ 113, 52, 31 },{ 118, 60, 36 },{ 102, 48, 22 },{ 102, 45, 25 },{ 118, 53, 31 },{ 123, 61, 48 },{ 104, 75, 69 },{ 54, 49, 46 },{ 35, 33, 36 },{ 48, 46, 49 },{ 38, 30, 28 },{ 82, 55, 44 },{ 103, 56, 38 },{ 88, 40, 20 },{ 85, 37, 17 },{ 94, 38, 15 },{ 115, 54, 26 },{ 122, 61, 32 },{ 128, 69, 39 },{ 137, 81, 48 },{ 155, 96, 64 },{ 165, 109, 74 },{ 169, 113, 78 },{ 173, 117, 84 },{ 173, 117, 84 },{ 169, 113, 80 },{ 143, 94, 64 },{ 104, 64, 39 },{ 74, 43, 25 },{ 49, 25, 15 },{ 79, 41, 22 },{ 150, 83, 54 },{ 156, 80, 56 },{ 172, 100, 76 },{ 174, 95, 64 },{ 173, 74, 45 },{ 176, 74, 52 },{ 182, 75, 57 },{ 178, 59, 29 },{ 192, 107, 76 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 60, 36, 26 },{ 79, 44, 25 },{ 148, 102, 78 },{ 174, 121, 90 },{ 178, 131, 103 },{ 180, 129, 98 },{ 174, 118, 85 },{ 179, 119, 85 },{ 181, 124, 94 },{ 184, 129, 99 },{ 184, 129, 99 },{ 189, 129, 101 },{ 185, 125, 97 },{ 177, 116, 85 },{ 164, 99, 67 },{ 163, 97, 65 },{ 162, 97, 67 },{ 171, 108, 77 },{ 171, 107, 79 },{ 172, 105, 76 },{ 175, 106, 77 },{ 177, 112, 82 },{ 189, 128, 97 },{ 199, 144, 114 },{ 214, 157, 128 },{ 221, 160, 131 },{ 207, 142, 110 },{ 187, 121, 86 },{ 172, 104, 69 },{ 162, 93, 60 },{ 148, 82, 50 },{ 133, 69, 41 },{ 124, 64, 40 },{ 114, 56, 32 },{ 109, 49, 25 },{ 106, 46, 20 },{ 104, 48, 23 },{ 108, 44, 19 },{ 107, 50, 30 },{ 158, 122, 108 },{ 139, 119, 118 },{ 76, 67, 70 },{ 16, 16, 16 },{ 28, 24, 21 },{ 47, 37, 36 },{ 101, 79, 65 },{ 126, 78, 58 },{ 94, 35, 19 },{ 94, 38, 21 },{ 101, 43, 19 },{ 109, 48, 20 },{ 123, 59, 31 },{ 133, 72, 43 },{ 139, 79, 45 },{ 151, 92, 58 },{ 169, 115, 77 },{ 182, 128, 92 },{ 182, 128, 94 },{ 177, 121, 88 },{ 177, 118, 86 },{ 165, 110, 80 },{ 109, 67, 43 },{ 73, 37, 23 },{ 49, 22, 15 },{ 83, 40, 23 },{ 153, 82, 54 },{ 167, 95, 71 },{ 189, 125, 98 },{ 175, 104, 72 },{ 165, 74, 43 },{ 169, 63, 39 },{ 178, 81, 65 },{ 171, 60, 40 },{ 192, 99, 65 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 77, 55, 44 },{ 78, 45, 26 },{ 139, 93, 69 },{ 159, 105, 71 },{ 162, 113, 83 },{ 171, 117, 83 },{ 177, 118, 88 },{ 188, 126, 101 },{ 194, 132, 109 },{ 192, 128, 101 },{ 184, 119, 89 },{ 181, 115, 83 },{ 176, 111, 83 },{ 167, 96, 66 },{ 158, 87, 57 },{ 153, 79, 50 },{ 151, 80, 52 },{ 151, 87, 59 },{ 166, 109, 80 },{ 174, 117, 88 },{ 172, 111, 83 },{ 174, 109, 79 },{ 180, 119, 88 },{ 194, 134, 106 },{ 215, 154, 126 },{ 227, 164, 133 },{ 213, 150, 117 },{ 191, 127, 89 },{ 174, 106, 71 },{ 165, 95, 61 },{ 149, 84, 52 },{ 136, 77, 47 },{ 130, 73, 46 },{ 124, 64, 40 },{ 123, 61, 36 },{ 119, 55, 30 },{ 112, 48, 23 },{ 105, 38, 22 },{ 124, 67, 48 },{ 166, 127, 110 },{ 141, 114, 105 },{ 73, 58, 51 },{ 43, 35, 32 },{ 51, 36, 33 },{ 81, 52, 44 },{ 133, 74, 56 },{ 140, 75, 47 },{ 138, 72, 46 },{ 126, 62, 35 },{ 116, 55, 26 },{ 115, 56, 26 },{ 117, 52, 22 },{ 136, 69, 40 },{ 151, 92, 58 },{ 164, 105, 71 },{ 169, 113, 76 },{ 175, 116, 82 },{ 181, 122, 92 },{ 178, 119, 87 },{ 174, 114, 80 },{ 167, 112, 81 },{ 127, 81, 55 },{ 62, 33, 19 },{ 48, 20, 9 },{ 94, 46, 24 },{ 153, 81, 56 },{ 179, 115, 88 },{ 190, 123, 97 },{ 180, 109, 81 },{ 164, 82, 42 },{ 165, 61, 36 },{ 170, 73, 56 },{ 164, 63, 45 },{ 183, 101, 63 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 85, 68, 61 },{ 87, 50, 31 },{ 123, 79, 54 },{ 165, 110, 80 },{ 165, 118, 90 },{ 175, 121, 95 },{ 187, 127, 103 },{ 193, 129, 104 },{ 186, 117, 88 },{ 166, 91, 59 },{ 170, 95, 66 },{ 184, 120, 93 },{ 164, 98, 72 },{ 133, 67, 43 },{ 107, 52, 32 },{ 98, 46, 32 },{ 124, 63, 44 },{ 136, 64, 39 },{ 132, 63, 32 },{ 143, 76, 47 },{ 161, 87, 58 },{ 166, 101, 71 },{ 182, 125, 98 },{ 192, 138, 110 },{ 213, 148, 118 },{ 227, 160, 131 },{ 217, 154, 123 },{ 195, 130, 92 },{ 173, 105, 68 },{ 160, 89, 57 },{ 150, 84, 52 },{ 144, 81, 50 },{ 137, 72, 44 },{ 135, 70, 40 },{ 146, 81, 51 },{ 125, 58, 32 },{ 120, 49, 29 },{ 125, 52, 35 },{ 129, 58, 36 },{ 146, 76, 51 },{ 153, 88, 66 },{ 138, 82, 65 },{ 124, 73, 56 },{ 132, 75, 56 },{ 144, 82, 59 },{ 150, 80, 54 },{ 144, 78, 52 },{ 141, 76, 48 },{ 140, 71, 42 },{ 141, 74, 45 },{ 146, 90, 57 },{ 140, 84, 51 },{ 148, 84, 49 },{ 163, 105, 68 },{ 179, 120, 88 },{ 183, 129, 95 },{ 184, 128, 93 },{ 177, 117, 81 },{ 175, 112, 79 },{ 174, 111, 80 },{ 172, 111, 80 },{ 147, 97, 72 },{ 65, 36, 20 },{ 51, 23, 12 },{ 121, 63, 39 },{ 162, 93, 64 },{ 184, 120, 93 },{ 184, 114, 89 },{ 181, 110, 80 },{ 172, 96, 62 },{ 160, 65, 35 },{ 163, 64, 45 },{ 155, 58, 41 },{ 175, 91, 55 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 88, 72, 72 },{ 94, 55, 38 },{ 127, 85, 60 },{ 170, 120, 95 },{ 172, 121, 90 },{ 180, 126, 100 },{ 183, 128, 98 },{ 188, 119, 88 },{ 158, 77, 47 },{ 161, 92, 63 },{ 167, 101, 75 },{ 158, 92, 70 },{ 158, 116, 104 },{ 112, 92, 91 },{ 43, 31, 31 },{ 38, 36, 39 },{ 62, 53, 44 },{ 103, 64, 49 },{ 137, 70, 44 },{ 138, 61, 35 },{ 152, 79, 47 },{ 167, 99, 64 },{ 187, 122, 90 },{ 198, 137, 108 },{ 205, 145, 111 },{ 219, 156, 123 },{ 212, 151, 120 },{ 200, 136, 101 },{ 176, 106, 72 },{ 163, 87, 55 },{ 153, 80, 48 },{ 149, 80, 49 },{ 138, 71, 42 },{ 142, 73, 42 },{ 149, 78, 46 },{ 137, 66, 36 },{ 132, 62, 36 },{ 129, 64, 36 },{ 133, 66, 39 },{ 138, 67, 39 },{ 145, 69, 43 },{ 151, 75, 49 },{ 155, 81, 54 },{ 151, 82, 53 },{ 151, 82, 51 },{ 146, 80, 46 },{ 143, 77, 43 },{ 146, 78, 43 },{ 146, 80, 45 },{ 152, 89, 54 },{ 160, 100, 64 },{ 163, 103, 66 },{ 166, 106, 69 },{ 173, 115, 77 },{ 182, 126, 91 },{ 184, 125, 91 },{ 183, 123, 86 },{ 182, 123, 83 },{ 179, 117, 80 },{ 174, 110, 75 },{ 168, 108, 74 },{ 157, 103, 77 },{ 82, 49, 32 },{ 67, 28, 13 },{ 134, 68, 42 },{ 169, 100, 71 },{ 182, 116, 84 },{ 180, 105, 76 },{ 180, 97, 67 },{ 184, 105, 74 },{ 167, 80, 52 },{ 159, 64, 44 },{ 154, 57, 38 },{ 167, 80, 50 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 160, 107, 93 },{ 157, 92, 72 },{ 121, 73, 53 },{ 133, 87, 63 },{ 167, 118, 88 },{ 177, 131, 98 },{ 181, 121, 93 },{ 182, 119, 86 },{ 174, 98, 64 },{ 143, 64, 33 },{ 154, 83, 53 },{ 143, 73, 48 },{ 181, 142, 125 },{ 184, 164, 155 },{ 147, 136, 140 },{ 54, 50, 51 },{ 17, 18, 20 },{ 48, 43, 37 },{ 106, 76, 65 },{ 153, 101, 79 },{ 134, 68, 44 },{ 160, 84, 58 },{ 187, 117, 81 },{ 197, 132, 100 },{ 200, 135, 103 },{ 203, 138, 106 },{ 214, 149, 117 },{ 209, 144, 112 },{ 195, 129, 97 },{ 179, 113, 78 },{ 167, 94, 61 },{ 153, 78, 47 },{ 146, 72, 43 },{ 141, 72, 43 },{ 139, 74, 42 },{ 138, 73, 41 },{ 129, 64, 34 },{ 129, 63, 37 },{ 131, 61, 35 },{ 135, 61, 36 },{ 136, 64, 39 },{ 142, 71, 41 },{ 152, 79, 47 },{ 156, 83, 50 },{ 158, 92, 58 },{ 151, 85, 53 },{ 151, 76, 45 },{ 149, 76, 43 },{ 150, 85, 47 },{ 151, 89, 52 },{ 157, 93, 57 },{ 166, 107, 67 },{ 172, 114, 74 },{ 168, 108, 71 },{ 175, 117, 77 },{ 185, 129, 92 },{ 188, 132, 95 },{ 175, 112, 71 },{ 176, 107, 66 },{ 179, 111, 74 },{ 182, 118, 83 },{ 172, 108, 70 },{ 172, 109, 78 },{ 124, 76, 56 },{ 106, 58, 38 },{ 141, 70, 42 },{ 156, 69, 41 },{ 173, 94, 61 },{ 179, 95, 67 },{ 176, 90, 55 },{ 181, 98, 68 },{ 179, 98, 69 },{ 157, 65, 44 },{ 152, 56, 34 },{ 167, 77, 51 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 186, 119, 103 },{ 204, 127, 97 },{ 200, 131, 100 },{ 159, 102, 72 },{ 129, 85, 56 },{ 170, 116, 90 },{ 187, 131, 104 },{ 187, 131, 104 },{ 183, 127, 92 },{ 169, 96, 61 },{ 146, 72, 37 },{ 152, 75, 49 },{ 168, 97, 75 },{ 197, 152, 131 },{ 206, 172, 160 },{ 132, 113, 106 },{ 58, 44, 41 },{ 48, 40, 37 },{ 80, 61, 54 },{ 126, 83, 66 },{ 158, 97, 76 },{ 151, 79, 54 },{ 169, 92, 64 },{ 181, 108, 76 },{ 202, 141, 112 },{ 205, 140, 108 },{ 202, 136, 104 },{ 211, 146, 116 },{ 208, 139, 106 },{ 194, 121, 88 },{ 187, 114, 81 },{ 171, 97, 62 },{ 157, 81, 49 },{ 148, 68, 41 },{ 139, 63, 37 },{ 148, 77, 49 },{ 144, 77, 48 },{ 134, 69, 41 },{ 126, 60, 34 },{ 130, 60, 34 },{ 137, 63, 36 },{ 144, 70, 43 },{ 150, 81, 50 },{ 154, 85, 52 },{ 155, 89, 55 },{ 162, 98, 63 },{ 159, 96, 63 },{ 156, 93, 62 },{ 156, 93, 60 },{ 157, 93, 58 },{ 158, 94, 59 },{ 156, 90, 56 },{ 161, 88, 53 },{ 165, 95, 59 },{ 181, 122, 80 },{ 181, 123, 86 },{ 179, 117, 80 },{ 186, 124, 85 },{ 183, 124, 84 },{ 173, 108, 68 },{ 170, 98, 58 },{ 175, 101, 66 },{ 175, 100, 69 },{ 168, 97, 65 },{ 158, 99, 69 },{ 150, 99, 70 },{ 159, 83, 59 },{ 162, 72, 45 },{ 167, 66, 38 },{ 172, 89, 59 },{ 173, 84, 54 },{ 171, 84, 56 },{ 178, 103, 72 },{ 163, 73, 49 },{ 159, 63, 39 },{ 169, 79, 55 } },
		{ { 0, 0, 0 },{ 144, 103, 101 },{ 201, 106, 84 },{ 179, 87, 62 },{ 197, 130, 103 },{ 172, 114, 90 },{ 130, 84, 61 },{ 173, 119, 91 },{ 188, 132, 105 },{ 191, 136, 105 },{ 187, 127, 93 },{ 176, 108, 71 },{ 164, 88, 52 },{ 176, 103, 70 },{ 194, 107, 79 },{ 198, 123, 94 },{ 204, 149, 129 },{ 191, 143, 131 },{ 135, 87, 73 },{ 123, 76, 56 },{ 150, 94, 71 },{ 172, 101, 71 },{ 167, 88, 58 },{ 169, 93, 61 },{ 181, 108, 73 },{ 186, 117, 86 },{ 197, 136, 108 },{ 200, 131, 100 },{ 206, 132, 103 },{ 210, 144, 112 },{ 204, 134, 100 },{ 198, 124, 89 },{ 185, 111, 74 },{ 175, 95, 62 },{ 160, 81, 50 },{ 149, 72, 42 },{ 144, 63, 36 },{ 147, 66, 39 },{ 150, 73, 43 },{ 148, 79, 46 },{ 138, 73, 41 },{ 138, 69, 38 },{ 141, 72, 41 },{ 148, 77, 47 },{ 150, 81, 50 },{ 163, 96, 67 },{ 161, 94, 65 },{ 158, 93, 61 },{ 164, 105, 71 },{ 164, 105, 73 },{ 162, 99, 68 },{ 166, 106, 72 },{ 162, 102, 66 },{ 163, 97, 62 },{ 160, 92, 57 },{ 166, 101, 63 },{ 176, 111, 71 },{ 190, 132, 95 },{ 183, 124, 84 },{ 177, 112, 74 },{ 182, 114, 77 },{ 185, 116, 77 },{ 171, 97, 58 },{ 162, 86, 50 },{ 164, 91, 59 },{ 164, 95, 64 },{ 160, 100, 66 },{ 168, 117, 86 },{ 166, 96, 70 },{ 179, 99, 76 },{ 170, 70, 46 },{ 162, 73, 43 },{ 164, 73, 52 },{ 167, 77, 50 },{ 180, 107, 74 },{ 170, 85, 56 },{ 161, 67, 42 },{ 169, 81, 59 } },
		{ { 0, 0, 0 },{ 192, 125, 109 },{ 179, 61, 35 },{ 135, 23, 9 },{ 176, 94, 83 },{ 179, 114, 92 },{ 137, 91, 65 },{ 177, 126, 97 },{ 194, 137, 108 },{ 203, 143, 117 },{ 193, 133, 99 },{ 176, 106, 70 },{ 177, 111, 77 },{ 188, 125, 90 },{ 197, 132, 102 },{ 210, 148, 125 },{ 204, 142, 119 },{ 195, 127, 104 },{ 185, 112, 79 },{ 175, 99, 63 },{ 177, 98, 65 },{ 173, 94, 64 },{ 171, 97, 68 },{ 171, 100, 70 },{ 177, 102, 70 },{ 180, 115, 85 },{ 197, 130, 101 },{ 203, 129, 100 },{ 204, 133, 103 },{ 207, 136, 104 },{ 204, 128, 96 },{ 198, 122, 88 },{ 187, 115, 77 },{ 180, 101, 68 },{ 164, 88, 54 },{ 151, 75, 43 },{ 148, 67, 40 },{ 148, 67, 40 },{ 150, 73, 43 },{ 153, 84, 51 },{ 157, 93, 58 },{ 156, 91, 59 },{ 150, 85, 55 },{ 148, 81, 54 },{ 149, 84, 56 },{ 153, 96, 66 },{ 161, 105, 72 },{ 163, 102, 71 },{ 162, 103, 69 },{ 163, 105, 68 },{ 163, 104, 70 },{ 165, 100, 68 },{ 169, 100, 67 },{ 171, 106, 74 },{ 176, 113, 80 },{ 178, 115, 80 },{ 179, 115, 77 },{ 182, 117, 79 },{ 185, 123, 86 },{ 183, 119, 81 },{ 183, 111, 71 },{ 179, 105, 66 },{ 177, 104, 69 },{ 160, 90, 56 },{ 164, 94, 58 },{ 159, 96, 63 },{ 160, 97, 62 },{ 174, 123, 92 },{ 178, 119, 89 },{ 172, 90, 66 },{ 173, 86, 58 },{ 160, 64, 39 },{ 152, 60, 35 },{ 163, 73, 46 },{ 172, 102, 68 },{ 169, 88, 58 },{ 166, 76, 50 },{ 167, 85, 64 } },
		{ { 0, 0, 0 },{ 211, 135, 111 },{ 171, 46, 18 },{ 143, 37, 23 },{ 190, 110, 101 },{ 194, 119, 100 },{ 151, 93, 71 },{ 180, 129, 102 },{ 195, 138, 108 },{ 202, 141, 110 },{ 196, 132, 97 },{ 182, 122, 88 },{ 190, 130, 96 },{ 194, 138, 103 },{ 204, 142, 117 },{ 210, 156, 132 },{ 207, 151, 128 },{ 204, 139, 111 },{ 195, 122, 87 },{ 183, 109, 70 },{ 177, 103, 68 },{ 178, 104, 69 },{ 182, 109, 76 },{ 182, 116, 84 },{ 185, 122, 91 },{ 199, 130, 101 },{ 203, 129, 100 },{ 203, 132, 102 },{ 206, 140, 106 },{ 210, 139, 107 },{ 205, 126, 96 },{ 195, 115, 82 },{ 189, 113, 79 },{ 180, 100, 67 },{ 164, 84, 51 },{ 154, 73, 44 },{ 147, 66, 39 },{ 148, 68, 43 },{ 148, 71, 43 },{ 154, 80, 51 },{ 159, 85, 56 },{ 160, 97, 64 },{ 158, 93, 61 },{ 157, 88, 59 },{ 157, 90, 61 },{ 161, 98, 65 },{ 160, 100, 66 },{ 167, 104, 69 },{ 170, 104, 70 },{ 170, 100, 66 },{ 170, 95, 64 },{ 173, 100, 68 },{ 182, 116, 84 },{ 187, 124, 93 },{ 194, 131, 100 },{ 194, 131, 100 },{ 184, 121, 88 },{ 174, 112, 75 },{ 182, 113, 74 },{ 180, 108, 70 },{ 176, 102, 65 },{ 177, 103, 68 },{ 174, 102, 64 },{ 170, 102, 65 },{ 158, 89, 56 },{ 162, 96, 64 },{ 157, 95, 58 },{ 169, 114, 83 },{ 185, 128, 99 },{ 169, 95, 66 },{ 166, 79, 51 },{ 160, 74, 47 },{ 146, 52, 26 },{ 160, 73, 45 },{ 178, 101, 73 },{ 172, 89, 59 },{ 172, 84, 60 },{ 159, 88, 70 } },
		{ { 0, 0, 0 },{ 214, 132, 108 },{ 165, 43, 22 },{ 156, 54, 40 },{ 196, 113, 95 },{ 207, 123, 99 },{ 176, 111, 91 },{ 173, 117, 94 },{ 197, 138, 108 },{ 204, 144, 116 },{ 201, 137, 101 },{ 192, 134, 97 },{ 193, 133, 105 },{ 201, 137, 109 },{ 208, 143, 115 },{ 210, 148, 123 },{ 209, 150, 120 },{ 202, 142, 108 },{ 200, 131, 98 },{ 192, 122, 86 },{ 192, 123, 92 },{ 189, 123, 88 },{ 191, 126, 98 },{ 197, 134, 103 },{ 203, 142, 113 },{ 205, 138, 111 },{ 204, 133, 105 },{ 205, 134, 104 },{ 205, 132, 100 },{ 211, 137, 110 },{ 206, 129, 101 },{ 201, 117, 83 },{ 191, 108, 78 },{ 182, 99, 69 },{ 166, 81, 52 },{ 155, 71, 43 },{ 149, 69, 42 },{ 147, 70, 42 },{ 150, 71, 41 },{ 151, 75, 43 },{ 154, 79, 48 },{ 159, 84, 55 },{ 163, 88, 57 },{ 163, 90, 58 },{ 164, 93, 61 },{ 163, 92, 62 },{ 162, 96, 64 },{ 167, 101, 69 },{ 172, 101, 69 },{ 168, 94, 59 },{ 173, 100, 65 },{ 180, 110, 76 },{ 191, 122, 91 },{ 195, 126, 97 },{ 195, 124, 94 },{ 194, 123, 93 },{ 189, 118, 88 },{ 179, 109, 75 },{ 173, 104, 65 },{ 174, 102, 64 },{ 171, 97, 62 },{ 168, 95, 60 },{ 167, 93, 58 },{ 172, 97, 65 },{ 154, 83, 53 },{ 158, 85, 50 },{ 152, 91, 60 },{ 160, 100, 72 },{ 179, 124, 93 },{ 177, 114, 83 },{ 175, 99, 73 },{ 159, 78, 57 },{ 143, 49, 24 },{ 168, 81, 53 },{ 177, 93, 69 },{ 171, 81, 55 },{ 179, 89, 65 },{ 150, 87, 70 } },
		{ { 0, 0, 0 },{ 212, 127, 96 },{ 164, 40, 16 },{ 155, 55, 40 },{ 193, 99, 74 },{ 212, 126, 103 },{ 209, 144, 124 },{ 184, 119, 91 },{ 194, 135, 103 },{ 206, 147, 115 },{ 204, 145, 111 },{ 195, 135, 101 },{ 195, 131, 96 },{ 196, 131, 99 },{ 204, 141, 110 },{ 205, 144, 115 },{ 207, 146, 118 },{ 210, 151, 121 },{ 209, 150, 120 },{ 207, 146, 117 },{ 203, 140, 107 },{ 195, 135, 101 },{ 204, 144, 116 },{ 209, 149, 121 },{ 213, 147, 121 },{ 205, 138, 111 },{ 204, 133, 101 },{ 205, 131, 102 },{ 201, 131, 97 },{ 206, 131, 99 },{ 203, 128, 99 },{ 199, 116, 82 },{ 190, 109, 79 },{ 181, 98, 68 },{ 164, 80, 52 },{ 151, 74, 44 },{ 150, 76, 49 },{ 146, 72, 47 },{ 152, 76, 52 },{ 153, 78, 47 },{ 153, 79, 44 },{ 153, 76, 46 },{ 159, 80, 49 },{ 162, 87, 55 },{ 164, 91, 58 },{ 167, 91, 59 },{ 165, 89, 57 },{ 170, 97, 64 },{ 174, 99, 67 },{ 177, 104, 72 },{ 180, 109, 77 },{ 185, 114, 82 },{ 191, 118, 86 },{ 193, 118, 87 },{ 191, 116, 84 },{ 189, 116, 84 },{ 186, 115, 83 },{ 184, 115, 82 },{ 177, 104, 69 },{ 169, 96, 61 },{ 172, 99, 64 },{ 171, 98, 63 },{ 164, 91, 56 },{ 162, 92, 58 },{ 154, 83, 53 },{ 152, 78, 49 },{ 154, 91, 60 },{ 156, 101, 70 },{ 173, 116, 87 },{ 180, 126, 102 },{ 168, 112, 85 },{ 155, 81, 56 },{ 157, 77, 50 },{ 177, 100, 72 },{ 172, 88, 64 },{ 171, 81, 55 },{ 182, 96, 73 },{ 135, 87, 77 } },
		{ { 0, 0, 0 },{ 203, 117, 94 },{ 169, 45, 21 },{ 167, 78, 64 },{ 196, 112, 88 },{ 211, 123, 99 },{ 220, 152, 133 },{ 191, 126, 98 },{ 195, 135, 101 },{ 211, 148, 117 },{ 204, 144, 110 },{ 194, 132, 95 },{ 199, 131, 94 },{ 198, 132, 98 },{ 203, 142, 113 },{ 206, 149, 120 },{ 216, 161, 130 },{ 218, 159, 129 },{ 210, 150, 124 },{ 208, 148, 120 },{ 206, 142, 114 },{ 208, 143, 113 },{ 212, 149, 118 },{ 213, 149, 121 },{ 212, 145, 118 },{ 205, 134, 106 },{ 203, 132, 102 },{ 206, 135, 107 },{ 201, 130, 100 },{ 198, 121, 91 },{ 196, 119, 89 },{ 195, 112, 82 },{ 185, 104, 75 },{ 176, 95, 65 },{ 168, 91, 63 },{ 158, 82, 56 },{ 149, 78, 50 },{ 146, 76, 50 },{ 147, 77, 51 },{ 152, 81, 51 },{ 157, 82, 50 },{ 159, 83, 51 },{ 160, 84, 52 },{ 162, 83, 53 },{ 163, 84, 53 },{ 166, 90, 58 },{ 173, 98, 66 },{ 175, 105, 71 },{ 176, 110, 75 },{ 184, 115, 84 },{ 190, 123, 94 },{ 194, 127, 98 },{ 189, 122, 93 },{ 191, 117, 88 },{ 189, 114, 83 },{ 185, 110, 79 },{ 183, 108, 76 },{ 181, 106, 75 },{ 176, 106, 72 },{ 172, 99, 64 },{ 167, 92, 60 },{ 167, 94, 61 },{ 170, 97, 64 },{ 157, 86, 54 },{ 153, 84, 55 },{ 140, 70, 44 },{ 143, 82, 53 },{ 157, 98, 68 },{ 166, 110, 77 },{ 176, 119, 90 },{ 180, 116, 91 },{ 209, 145, 120 },{ 204, 148, 125 },{ 177, 97, 70 },{ 170, 84, 59 },{ 179, 89, 63 },{ 177, 97, 74 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 177, 117, 106 },{ 182, 67, 40 },{ 178, 86, 71 },{ 203, 122, 105 },{ 205, 117, 95 },{ 216, 145, 123 },{ 189, 122, 93 },{ 197, 136, 105 },{ 208, 147, 116 },{ 200, 136, 101 },{ 196, 131, 93 },{ 205, 139, 105 },{ 201, 136, 106 },{ 206, 141, 111 },{ 209, 145, 117 },{ 208, 147, 118 },{ 211, 150, 122 },{ 212, 148, 123 },{ 208, 143, 115 },{ 211, 146, 118 },{ 211, 148, 117 },{ 211, 148, 117 },{ 214, 147, 121 },{ 212, 138, 111 },{ 203, 132, 100 },{ 206, 137, 108 },{ 205, 135, 109 },{ 198, 124, 97 },{ 194, 113, 83 },{ 191, 108, 76 },{ 189, 104, 75 },{ 183, 104, 73 },{ 178, 104, 77 },{ 168, 98, 72 },{ 156, 82, 57 },{ 154, 80, 53 },{ 147, 78, 49 },{ 141, 75, 49 },{ 141, 77, 50 },{ 149, 80, 51 },{ 160, 81, 50 },{ 163, 90, 57 },{ 167, 92, 60 },{ 171, 90, 60 },{ 171, 92, 62 },{ 174, 99, 68 },{ 178, 105, 73 },{ 182, 111, 79 },{ 187, 113, 84 },{ 192, 118, 91 },{ 197, 127, 101 },{ 194, 127, 100 },{ 193, 122, 94 },{ 192, 121, 91 },{ 187, 118, 85 },{ 181, 111, 77 },{ 179, 106, 74 },{ 173, 100, 67 },{ 165, 95, 61 },{ 163, 88, 56 },{ 162, 83, 52 },{ 171, 96, 64 },{ 158, 92, 60 },{ 145, 78, 49 },{ 133, 68, 40 },{ 139, 75, 47 },{ 152, 97, 66 },{ 166, 110, 77 },{ 180, 124, 91 },{ 205, 140, 112 },{ 212, 137, 108 },{ 224, 160, 132 },{ 198, 134, 109 },{ 184, 107, 87 },{ 186, 104, 80 },{ 166, 91, 72 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 138, 110, 109 },{ 198, 97, 69 },{ 173, 70, 53 },{ 201, 114, 104 },{ 209, 123, 100 },{ 206, 130, 104 },{ 187, 113, 84 },{ 203, 138, 110 },{ 208, 145, 112 },{ 199, 135, 100 },{ 204, 138, 104 },{ 205, 138, 109 },{ 205, 138, 109 },{ 205, 134, 104 },{ 206, 135, 105 },{ 215, 145, 119 },{ 213, 141, 116 },{ 215, 146, 117 },{ 213, 144, 111 },{ 213, 142, 112 },{ 211, 142, 113 },{ 214, 144, 118 },{ 216, 144, 120 },{ 213, 144, 115 },{ 208, 142, 108 },{ 206, 139, 112 },{ 199, 132, 105 },{ 195, 121, 92 },{ 193, 112, 82 },{ 192, 105, 75 },{ 191, 106, 77 },{ 187, 108, 77 },{ 182, 112, 86 },{ 176, 110, 84 },{ 162, 87, 64 },{ 149, 75, 50 },{ 146, 76, 50 },{ 140, 73, 47 },{ 137, 73, 48 },{ 141, 70, 48 },{ 142, 68, 43 },{ 146, 71, 42 },{ 160, 84, 52 },{ 168, 93, 61 },{ 174, 101, 69 },{ 178, 101, 73 },{ 180, 103, 73 },{ 183, 108, 76 },{ 186, 109, 79 },{ 188, 111, 81 },{ 192, 117, 88 },{ 193, 119, 90 },{ 193, 116, 88 },{ 191, 111, 84 },{ 187, 110, 80 },{ 187, 108, 78 },{ 181, 110, 78 },{ 175, 104, 72 },{ 168, 97, 65 },{ 162, 89, 57 },{ 154, 79, 48 },{ 154, 79, 48 },{ 158, 87, 57 },{ 139, 78, 47 },{ 131, 71, 43 },{ 145, 86, 56 },{ 157, 98, 66 },{ 161, 105, 70 },{ 171, 117, 81 },{ 198, 131, 105 },{ 201, 124, 94 },{ 216, 142, 115 },{ 233, 171, 150 },{ 226, 168, 148 },{ 207, 142, 124 },{ 149, 92, 85 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 197, 111, 88 },{ 181, 71, 46 },{ 199, 116, 100 },{ 210, 130, 105 },{ 200, 115, 86 },{ 185, 108, 80 },{ 200, 134, 102 },{ 203, 132, 102 },{ 200, 133, 104 },{ 204, 137, 108 },{ 204, 129, 97 },{ 204, 128, 96 },{ 207, 132, 101 },{ 213, 142, 114 },{ 216, 145, 115 },{ 213, 138, 109 },{ 211, 138, 105 },{ 214, 138, 106 },{ 210, 131, 101 },{ 211, 136, 107 },{ 217, 141, 115 },{ 217, 143, 114 },{ 212, 146, 114 },{ 207, 141, 115 },{ 201, 135, 109 },{ 197, 130, 103 },{ 193, 119, 90 },{ 195, 114, 85 },{ 199, 113, 86 },{ 196, 110, 83 },{ 191, 107, 81 },{ 184, 107, 81 },{ 179, 108, 80 },{ 164, 88, 64 },{ 152, 80, 58 },{ 147, 79, 56 },{ 145, 75, 50 },{ 138, 72, 46 },{ 140, 70, 45 },{ 134, 66, 43 },{ 134, 58, 34 },{ 136, 56, 29 },{ 151, 74, 44 },{ 170, 95, 64 },{ 175, 101, 72 },{ 182, 111, 81 },{ 185, 114, 82 },{ 185, 112, 80 },{ 188, 113, 82 },{ 188, 113, 82 },{ 187, 111, 79 },{ 187, 106, 77 },{ 185, 104, 75 },{ 183, 102, 73 },{ 179, 98, 69 },{ 181, 102, 72 },{ 177, 104, 72 },{ 170, 101, 68 },{ 156, 87, 54 },{ 155, 86, 53 },{ 150, 79, 49 },{ 156, 87, 56 },{ 145, 82, 51 },{ 136, 81, 50 },{ 143, 86, 57 },{ 150, 91, 61 },{ 156, 101, 70 },{ 162, 107, 76 },{ 184, 116, 93 },{ 198, 123, 94 },{ 207, 140, 113 },{ 232, 167, 147 },{ 243, 180, 165 },{ 210, 148, 123 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 184, 112, 98 },{ 191, 73, 45 },{ 194, 112, 90 },{ 210, 134, 108 },{ 193, 100, 67 },{ 178, 95, 65 },{ 197, 132, 100 },{ 197, 126, 94 },{ 202, 131, 101 },{ 205, 132, 100 },{ 206, 130, 98 },{ 211, 135, 103 },{ 218, 145, 113 },{ 214, 141, 109 },{ 209, 133, 99 },{ 210, 134, 102 },{ 213, 137, 105 },{ 216, 135, 108 },{ 215, 135, 108 },{ 214, 139, 108 },{ 212, 135, 105 },{ 209, 130, 99 },{ 202, 127, 96 },{ 203, 137, 113 },{ 196, 132, 104 },{ 193, 126, 97 },{ 194, 117, 91 },{ 200, 119, 92 },{ 202, 118, 92 },{ 196, 116, 89 },{ 194, 112, 88 },{ 188, 111, 85 },{ 174, 99, 70 },{ 166, 86, 59 },{ 161, 86, 63 },{ 157, 90, 64 },{ 150, 78, 53 },{ 147, 76, 48 },{ 143, 76, 50 },{ 133, 65, 44 },{ 138, 70, 47 },{ 138, 68, 43 },{ 130, 54, 28 },{ 144, 63, 34 },{ 163, 88, 57 },{ 172, 101, 71 },{ 183, 109, 82 },{ 188, 114, 87 },{ 183, 109, 80 },{ 178, 101, 71 },{ 179, 100, 69 },{ 180, 101, 70 },{ 178, 99, 68 },{ 179, 100, 69 },{ 175, 99, 67 },{ 173, 90, 60 },{ 169, 92, 62 },{ 163, 92, 60 },{ 158, 89, 56 },{ 155, 84, 52 },{ 152, 86, 54 },{ 150, 89, 58 },{ 146, 85, 54 },{ 144, 85, 53 },{ 143, 81, 56 },{ 142, 87, 57 },{ 154, 99, 69 },{ 160, 107, 76 },{ 197, 138, 108 },{ 207, 137, 111 },{ 212, 148, 121 },{ 224, 166, 144 },{ 212, 141, 119 },{ 174, 102, 77 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 160, 109, 105 },{ 198, 82, 57 },{ 196, 100, 78 },{ 209, 131, 108 },{ 188, 89, 58 },{ 173, 80, 47 },{ 196, 127, 96 },{ 194, 125, 94 },{ 203, 130, 98 },{ 205, 130, 99 },{ 214, 139, 110 },{ 215, 140, 111 },{ 212, 137, 106 },{ 212, 133, 102 },{ 215, 135, 102 },{ 218, 137, 108 },{ 217, 140, 110 },{ 215, 138, 110 },{ 216, 139, 111 },{ 207, 132, 100 },{ 203, 123, 90 },{ 196, 115, 85 },{ 195, 119, 93 },{ 197, 127, 101 },{ 189, 120, 89 },{ 189, 118, 88 },{ 205, 127, 104 },{ 214, 134, 111 },{ 203, 119, 93 },{ 197, 121, 95 },{ 196, 120, 96 },{ 200, 133, 107 },{ 182, 111, 83 },{ 165, 88, 60 },{ 158, 81, 55 },{ 154, 82, 57 },{ 156, 80, 54 },{ 154, 76, 53 },{ 153, 78, 55 },{ 140, 70, 45 },{ 142, 74, 51 },{ 153, 89, 64 },{ 139, 72, 46 },{ 125, 49, 23 },{ 141, 60, 33 },{ 165, 84, 57 },{ 171, 94, 68 },{ 182, 102, 79 },{ 183, 106, 80 },{ 178, 97, 70 },{ 174, 90, 62 },{ 174, 93, 64 },{ 172, 91, 62 },{ 173, 88, 59 },{ 171, 90, 60 },{ 168, 89, 58 },{ 163, 90, 58 },{ 156, 85, 55 },{ 162, 89, 57 },{ 157, 88, 55 },{ 153, 88, 58 },{ 148, 87, 56 },{ 148, 92, 59 },{ 139, 80, 50 },{ 138, 81, 54 },{ 146, 87, 57 },{ 152, 95, 65 },{ 160, 107, 76 },{ 191, 132, 102 },{ 213, 147, 125 },{ 210, 145, 125 },{ 204, 133, 111 },{ 178, 90, 66 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 205, 106, 83 },{ 195, 93, 71 },{ 209, 133, 109 },{ 195, 104, 75 },{ 175, 80, 50 },{ 190, 121, 88 },{ 196, 125, 95 },{ 204, 135, 104 },{ 208, 137, 109 },{ 212, 136, 110 },{ 214, 134, 107 },{ 219, 138, 109 },{ 218, 139, 109 },{ 219, 142, 114 },{ 224, 146, 123 },{ 220, 143, 117 },{ 218, 137, 107 },{ 216, 133, 103 },{ 205, 126, 93 },{ 196, 118, 82 },{ 190, 110, 83 },{ 189, 117, 93 },{ 196, 119, 91 },{ 198, 118, 93 },{ 195, 120, 91 },{ 214, 143, 123 },{ 222, 151, 131 },{ 201, 117, 93 },{ 199, 118, 97 },{ 197, 122, 99 },{ 196, 125, 103 },{ 170, 95, 72 },{ 157, 77, 52 },{ 150, 73, 47 },{ 149, 79, 53 },{ 152, 80, 56 },{ 152, 74, 51 },{ 150, 76, 49 },{ 144, 70, 45 },{ 148, 78, 53 },{ 162, 91, 69 },{ 158, 87, 65 },{ 144, 72, 48 },{ 133, 53, 28 },{ 151, 65, 40 },{ 167, 83, 57 },{ 175, 95, 68 },{ 178, 103, 74 },{ 179, 100, 70 },{ 175, 94, 67 },{ 170, 93, 65 },{ 166, 86, 59 },{ 166, 80, 53 },{ 164, 83, 53 },{ 162, 89, 56 },{ 162, 88, 59 },{ 160, 91, 60 },{ 153, 87, 53 },{ 156, 90, 56 },{ 150, 85, 55 },{ 145, 84, 53 },{ 147, 91, 58 },{ 140, 87, 53 },{ 142, 85, 56 },{ 150, 94, 61 },{ 149, 93, 60 },{ 165, 104, 76 },{ 177, 104, 72 },{ 192, 108, 84 },{ 190, 115, 84 },{ 178, 92, 65 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 188, 116, 102 },{ 200, 100, 76 },{ 204, 126, 106 },{ 203, 118, 89 },{ 179, 82, 47 },{ 187, 116, 86 },{ 195, 129, 97 },{ 203, 132, 104 },{ 206, 132, 105 },{ 213, 133, 106 },{ 220, 139, 112 },{ 221, 144, 118 },{ 223, 147, 124 },{ 227, 155, 131 },{ 229, 153, 130 },{ 217, 140, 112 },{ 214, 130, 102 },{ 207, 126, 96 },{ 197, 117, 82 },{ 187, 108, 75 },{ 182, 108, 79 },{ 192, 114, 91 },{ 205, 125, 100 },{ 202, 128, 103 },{ 196, 124, 100 },{ 206, 123, 105 },{ 211, 130, 109 },{ 200, 114, 91 },{ 192, 115, 89 },{ 188, 110, 88 },{ 180, 101, 71 },{ 157, 73, 49 },{ 142, 61, 40 },{ 138, 67, 45 },{ 132, 66, 44 },{ 132, 64, 43 },{ 134, 63, 41 },{ 144, 68, 45 },{ 137, 65, 43 },{ 144, 72, 48 },{ 159, 83, 59 },{ 161, 85, 61 },{ 159, 87, 62 },{ 144, 74, 49 },{ 139, 63, 40 },{ 159, 77, 56 },{ 175, 93, 69 },{ 177, 97, 70 },{ 178, 103, 74 },{ 172, 98, 69 },{ 170, 93, 65 },{ 166, 82, 54 },{ 162, 78, 50 },{ 159, 80, 50 },{ 157, 82, 51 },{ 155, 82, 50 },{ 153, 84, 51 },{ 153, 87, 55 },{ 152, 86, 54 },{ 150, 87, 56 },{ 140, 84, 51 },{ 144, 87, 57 },{ 144, 91, 60 },{ 145, 88, 58 },{ 147, 92, 62 },{ 151, 96, 66 },{ 158, 97, 68 },{ 168, 94, 67 },{ 179, 93, 68 },{ 173, 83, 57 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 144, 106, 103 },{ 208, 109, 88 },{ 202, 108, 83 },{ 208, 131, 105 },{ 197, 118, 87 },{ 186, 120, 88 },{ 195, 129, 97 },{ 200, 129, 101 },{ 205, 128, 100 },{ 216, 136, 109 },{ 220, 143, 117 },{ 224, 149, 126 },{ 231, 159, 137 },{ 235, 160, 139 },{ 224, 142, 118 },{ 213, 132, 105 },{ 206, 125, 96 },{ 197, 116, 86 },{ 188, 108, 75 },{ 182, 107, 78 },{ 186, 119, 92 },{ 201, 123, 100 },{ 207, 132, 111 },{ 205, 134, 112 },{ 199, 128, 106 },{ 195, 113, 91 },{ 195, 117, 94 },{ 197, 119, 96 },{ 182, 106, 80 },{ 172, 92, 65 },{ 162, 84, 61 },{ 145, 67, 45 },{ 132, 57, 38 },{ 103, 44, 30 },{ 70, 26, 15 },{ 70, 23, 15 },{ 85, 27, 15 },{ 105, 40, 22 },{ 122, 54, 35 },{ 143, 72, 52 },{ 151, 76, 53 },{ 159, 83, 60 },{ 161, 86, 63 },{ 158, 86, 62 },{ 147, 71, 47 },{ 150, 70, 47 },{ 164, 87, 61 },{ 174, 92, 68 },{ 180, 98, 74 },{ 174, 97, 71 },{ 166, 89, 61 },{ 162, 83, 53 },{ 156, 77, 47 },{ 158, 78, 51 },{ 154, 80, 51 },{ 153, 79, 50 },{ 144, 75, 44 },{ 149, 84, 52 },{ 148, 83, 53 },{ 148, 85, 54 },{ 141, 84, 54 },{ 148, 91, 61 },{ 146, 91, 60 },{ 140, 87, 55 },{ 143, 89, 55 },{ 150, 93, 63 },{ 148, 92, 67 },{ 149, 77, 53 },{ 149, 68, 47 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 198, 121, 105 },{ 197, 91, 65 },{ 205, 123, 102 },{ 205, 141, 114 },{ 185, 120, 90 },{ 192, 125, 96 },{ 203, 128, 99 },{ 209, 128, 101 },{ 215, 135, 108 },{ 221, 145, 121 },{ 230, 159, 137 },{ 238, 172, 150 },{ 230, 155, 134 },{ 214, 134, 107 },{ 206, 125, 98 },{ 201, 122, 91 },{ 188, 112, 76 },{ 180, 101, 68 },{ 194, 124, 99 },{ 206, 144, 123 },{ 199, 127, 105 },{ 194, 116, 94 },{ 169, 87, 65 },{ 149, 71, 49 },{ 165, 90, 71 },{ 181, 113, 92 },{ 188, 118, 93 },{ 174, 92, 70 },{ 153, 68, 47 },{ 145, 66, 49 },{ 135, 62, 47 },{ 103, 44, 30 },{ 92, 36, 23 },{ 106, 42, 32 },{ 109, 39, 29 },{ 109, 43, 29 },{ 108, 47, 29 },{ 133, 62, 44 },{ 145, 72, 53 },{ 144, 69, 48 },{ 147, 69, 47 },{ 152, 75, 55 },{ 151, 79, 57 },{ 154, 82, 60 },{ 147, 75, 51 },{ 155, 79, 55 },{ 167, 87, 64 },{ 172, 88, 64 },{ 174, 94, 69 },{ 168, 92, 66 },{ 158, 83, 54 },{ 156, 81, 52 },{ 153, 79, 50 },{ 147, 76, 46 },{ 147, 78, 49 },{ 141, 74, 45 },{ 142, 79, 48 },{ 147, 84, 53 },{ 146, 85, 56 },{ 139, 84, 53 },{ 148, 93, 62 },{ 153, 100, 66 },{ 140, 86, 58 },{ 142, 82, 54 },{ 143, 82, 54 },{ 145, 85, 57 },{ 130, 69, 41 },{ 159, 109, 86 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 160, 113, 105 },{ 210, 114, 89 },{ 190, 102, 80 },{ 203, 136, 109 },{ 186, 122, 94 },{ 191, 122, 93 },{ 201, 124, 96 },{ 209, 128, 101 },{ 215, 135, 110 },{ 223, 147, 124 },{ 233, 162, 142 },{ 236, 170, 148 },{ 225, 150, 127 },{ 210, 129, 99 },{ 206, 122, 94 },{ 198, 115, 85 },{ 189, 106, 74 },{ 190, 109, 82 },{ 205, 134, 114 },{ 206, 141, 121 },{ 201, 129, 107 },{ 193, 115, 93 },{ 165, 78, 58 },{ 155, 68, 51 },{ 160, 82, 69 },{ 161, 90, 72 },{ 171, 99, 75 },{ 158, 77, 58 },{ 131, 55, 41 },{ 132, 60, 46 },{ 116, 50, 34 },{ 115, 49, 33 },{ 131, 64, 47 },{ 129, 62, 45 },{ 124, 55, 39 },{ 127, 58, 43 },{ 135, 68, 51 },{ 141, 70, 52 },{ 143, 72, 52 },{ 146, 74, 52 },{ 147, 72, 51 },{ 146, 71, 50 },{ 150, 77, 58 },{ 152, 84, 61 },{ 155, 87, 64 },{ 153, 87, 61 },{ 156, 86, 61 },{ 166, 94, 69 },{ 168, 96, 71 },{ 169, 95, 70 },{ 163, 86, 60 },{ 156, 81, 52 },{ 152, 81, 51 },{ 146, 77, 48 },{ 138, 71, 42 },{ 139, 74, 44 },{ 141, 80, 51 },{ 143, 82, 53 },{ 137, 78, 48 },{ 136, 83, 51 },{ 148, 93, 62 },{ 153, 97, 62 },{ 142, 83, 53 },{ 139, 79, 53 },{ 136, 79, 50 },{ 140, 79, 48 },{ 137, 77, 51 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 202, 124, 104 },{ 199, 113, 88 },{ 205, 134, 104 },{ 187, 120, 93 },{ 187, 120, 93 },{ 200, 123, 97 },{ 208, 128, 101 },{ 215, 135, 110 },{ 222, 144, 122 },{ 226, 155, 135 },{ 229, 158, 136 },{ 218, 144, 119 },{ 208, 124, 96 },{ 199, 118, 88 },{ 193, 110, 78 },{ 186, 99, 71 },{ 193, 115, 92 },{ 207, 139, 118 },{ 206, 138, 117 },{ 210, 133, 113 },{ 201, 128, 109 },{ 193, 125, 106 },{ 192, 120, 105 },{ 190, 125, 107 },{ 186, 115, 97 },{ 180, 104, 88 },{ 161, 79, 65 },{ 136, 57, 42 },{ 150, 76, 63 },{ 158, 89, 73 },{ 152, 81, 61 },{ 151, 76, 57 },{ 142, 74, 55 },{ 128, 63, 45 },{ 130, 63, 46 },{ 138, 70, 51 },{ 141, 74, 57 },{ 143, 76, 57 },{ 144, 76, 55 },{ 146, 78, 57 },{ 145, 74, 54 },{ 149, 76, 57 },{ 151, 80, 58 },{ 153, 86, 60 },{ 157, 93, 68 },{ 152, 86, 60 },{ 151, 87, 60 },{ 158, 94, 67 },{ 165, 98, 71 },{ 166, 92, 65 },{ 161, 87, 58 },{ 149, 82, 53 },{ 146, 79, 50 },{ 140, 75, 47 },{ 134, 70, 42 },{ 137, 78, 48 },{ 139, 82, 52 },{ 131, 74, 44 },{ 129, 78, 47 },{ 140, 87, 56 },{ 148, 94, 60 },{ 138, 85, 54 },{ 130, 74, 47 },{ 135, 78, 49 },{ 136, 77, 45 },{ 134, 78, 53 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 165, 106, 98 },{ 202, 122, 97 },{ 213, 138, 109 },{ 192, 125, 96 },{ 185, 118, 91 },{ 201, 125, 99 },{ 206, 131, 102 },{ 214, 134, 109 },{ 219, 141, 119 },{ 223, 151, 129 },{ 226, 154, 130 },{ 215, 139, 115 },{ 206, 126, 99 },{ 196, 112, 84 },{ 183, 100, 70 },{ 179, 102, 74 },{ 193, 126, 100 },{ 200, 134, 110 },{ 201, 129, 107 },{ 202, 129, 110 },{ 197, 126, 108 },{ 193, 130, 113 },{ 194, 124, 112 },{ 194, 128, 112 },{ 198, 131, 115 },{ 192, 123, 108 },{ 176, 105, 87 },{ 164, 94, 69 },{ 169, 98, 80 },{ 185, 120, 102 },{ 176, 109, 90 },{ 170, 95, 76 },{ 158, 81, 61 },{ 143, 72, 50 },{ 134, 67, 48 },{ 142, 75, 56 },{ 144, 77, 58 },{ 143, 76, 57 },{ 146, 80, 58 },{ 142, 77, 55 },{ 143, 76, 57 },{ 143, 75, 56 },{ 146, 80, 58 },{ 147, 83, 58 },{ 151, 91, 67 },{ 156, 92, 67 },{ 151, 87, 60 },{ 148, 87, 59 },{ 159, 95, 67 },{ 160, 91, 62 },{ 156, 85, 57 },{ 150, 81, 52 },{ 144, 79, 49 },{ 140, 75, 47 },{ 133, 74, 44 },{ 132, 75, 45 },{ 135, 80, 50 },{ 132, 79, 48 },{ 133, 82, 51 },{ 132, 81, 50 },{ 140, 89, 62 },{ 135, 85, 60 },{ 127, 73, 47 },{ 131, 71, 45 },{ 136, 76, 48 },{ 128, 70, 46 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 201, 123, 101 },{ 216, 135, 114 },{ 190, 123, 94 },{ 182, 117, 87 },{ 195, 124, 96 },{ 200, 124, 98 },{ 209, 132, 106 },{ 215, 139, 116 },{ 219, 147, 125 },{ 219, 147, 123 },{ 212, 136, 112 },{ 201, 121, 94 },{ 187, 103, 77 },{ 173, 96, 70 },{ 180, 110, 85 },{ 196, 129, 110 },{ 199, 131, 110 },{ 201, 126, 105 },{ 198, 127, 105 },{ 195, 128, 111 },{ 194, 127, 110 },{ 198, 129, 113 },{ 205, 138, 122 },{ 201, 134, 118 },{ 185, 116, 100 },{ 176, 103, 86 },{ 165, 97, 78 },{ 169, 98, 80 },{ 181, 116, 98 },{ 181, 110, 92 },{ 175, 98, 80 },{ 170, 97, 78 },{ 161, 88, 71 },{ 150, 74, 60 },{ 144, 77, 60 },{ 143, 77, 55 },{ 149, 81, 60 },{ 148, 82, 60 },{ 145, 80, 58 },{ 138, 76, 55 },{ 138, 73, 55 },{ 144, 81, 63 },{ 144, 83, 62 },{ 152, 90, 69 },{ 161, 95, 73 },{ 156, 90, 66 },{ 140, 78, 53 },{ 148, 87, 59 },{ 156, 91, 63 },{ 156, 89, 60 },{ 149, 84, 54 },{ 141, 78, 47 },{ 137, 72, 44 },{ 130, 73, 43 },{ 128, 73, 43 },{ 132, 79, 48 },{ 126, 77, 47 },{ 130, 79, 50 },{ 134, 83, 54 },{ 136, 85, 56 },{ 129, 80, 50 },{ 128, 75, 44 },{ 134, 74, 46 },{ 134, 73, 52 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 206, 125, 104 },{ 218, 141, 121 },{ 198, 126, 101 },{ 179, 116, 85 },{ 191, 124, 95 },{ 200, 123, 97 },{ 205, 128, 102 },{ 213, 139, 114 },{ 218, 146, 124 },{ 213, 142, 120 },{ 205, 129, 106 },{ 192, 115, 89 },{ 178, 102, 78 },{ 172, 105, 78 },{ 182, 115, 89 },{ 191, 123, 104 },{ 195, 133, 112 },{ 191, 126, 104 },{ 197, 130, 111 },{ 195, 127, 108 },{ 195, 128, 109 },{ 193, 132, 114 },{ 203, 139, 127 },{ 202, 133, 118 },{ 182, 107, 88 },{ 175, 97, 77 },{ 166, 98, 77 },{ 165, 103, 82 },{ 184, 117, 98 },{ 187, 111, 95 },{ 177, 102, 83 },{ 174, 106, 85 },{ 166, 98, 77 },{ 154, 83, 63 },{ 149, 83, 61 },{ 142, 77, 57 },{ 148, 81, 62 },{ 147, 80, 61 },{ 143, 78, 58 },{ 135, 72, 54 },{ 132, 69, 51 },{ 135, 73, 52 },{ 142, 81, 60 },{ 147, 86, 65 },{ 153, 88, 66 },{ 156, 91, 69 },{ 144, 82, 57 },{ 136, 75, 47 },{ 150, 85, 57 },{ 158, 91, 62 },{ 152, 87, 57 },{ 144, 81, 50 },{ 138, 71, 44 },{ 124, 69, 38 },{ 130, 75, 45 },{ 128, 74, 46 },{ 122, 75, 45 },{ 130, 79, 50 },{ 131, 82, 52 },{ 138, 83, 53 },{ 124, 77, 49 },{ 125, 74, 47 },{ 130, 70, 42 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 170, 106, 94 },{ 223, 146, 120 },{ 213, 146, 127 },{ 180, 117, 86 },{ 189, 122, 95 },{ 197, 123, 94 },{ 201, 129, 104 },{ 208, 136, 111 },{ 213, 141, 116 },{ 211, 136, 115 },{ 199, 125, 98 },{ 182, 110, 85 },{ 167, 100, 74 },{ 176, 111, 83 },{ 180, 120, 96 },{ 184, 126, 102 },{ 189, 131, 109 },{ 188, 126, 103 },{ 196, 131, 111 },{ 193, 130, 113 },{ 199, 130, 114 },{ 198, 139, 123 },{ 206, 144, 131 },{ 195, 127, 108 },{ 185, 100, 79 },{ 177, 97, 74 },{ 168, 101, 75 },{ 173, 108, 88 },{ 179, 118, 97 },{ 186, 118, 97 },{ 177, 103, 78 },{ 175, 104, 86 },{ 167, 99, 80 },{ 160, 89, 67 },{ 148, 83, 65 },{ 139, 77, 56 },{ 147, 76, 54 },{ 139, 73, 51 },{ 135, 70, 52 },{ 130, 65, 47 },{ 122, 59, 41 },{ 125, 62, 44 },{ 130, 72, 48 },{ 139, 79, 55 },{ 148, 87, 59 },{ 153, 89, 64 },{ 138, 80, 56 },{ 130, 73, 46 },{ 146, 82, 54 },{ 159, 95, 67 },{ 154, 89, 59 },{ 147, 82, 50 },{ 140, 73, 46 },{ 126, 70, 45 },{ 122, 75, 49 },{ 121, 72, 42 },{ 119, 72, 44 },{ 123, 73, 48 },{ 129, 78, 51 },{ 131, 80, 51 },{ 124, 74, 49 },{ 126, 72, 46 },{ 127, 71, 44 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 160, 104, 91 },{ 194, 120, 95 },{ 179, 115, 87 },{ 185, 122, 91 },{ 189, 122, 93 },{ 197, 123, 98 },{ 205, 133, 108 },{ 210, 140, 114 },{ 208, 136, 114 },{ 192, 120, 95 },{ 174, 103, 75 },{ 171, 107, 82 },{ 179, 117, 92 },{ 179, 123, 100 },{ 193, 137, 112 },{ 187, 129, 105 },{ 188, 123, 101 },{ 192, 129, 112 },{ 191, 132, 116 },{ 197, 138, 124 },{ 198, 147, 130 },{ 198, 140, 126 },{ 192, 120, 105 },{ 184, 106, 84 },{ 172, 100, 75 },{ 177, 112, 90 },{ 182, 114, 91 },{ 184, 118, 96 },{ 187, 119, 98 },{ 183, 108, 87 },{ 177, 102, 83 },{ 165, 90, 69 },{ 163, 87, 63 },{ 151, 80, 60 },{ 141, 70, 50 },{ 138, 67, 45 },{ 133, 67, 43 },{ 130, 64, 42 },{ 123, 58, 38 },{ 115, 57, 37 },{ 114, 57, 37 },{ 118, 60, 38 },{ 131, 73, 49 },{ 146, 81, 53 },{ 147, 77, 52 },{ 139, 75, 50 },{ 134, 77, 48 },{ 144, 87, 57 },{ 154, 93, 64 },{ 152, 87, 59 },{ 140, 81, 51 },{ 134, 70, 43 },{ 123, 67, 42 },{ 123, 73, 46 },{ 127, 76, 47 },{ 122, 75, 47 },{ 120, 74, 48 },{ 123, 76, 48 },{ 124, 74, 47 },{ 117, 70, 44 },{ 124, 73, 46 },{ 123, 69, 43 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 160, 108, 87 },{ 181, 121, 87 },{ 181, 120, 91 },{ 193, 123, 97 },{ 200, 130, 104 },{ 204, 135, 106 },{ 199, 129, 104 },{ 180, 110, 84 },{ 168, 101, 72 },{ 181, 121, 93 },{ 177, 120, 93 },{ 183, 133, 108 },{ 187, 137, 112 },{ 186, 129, 109 },{ 182, 117, 95 },{ 185, 124, 103 },{ 191, 129, 104 },{ 195, 128, 112 },{ 191, 129, 114 },{ 195, 130, 112 },{ 192, 121, 99 },{ 190, 112, 89 },{ 184, 108, 84 },{ 185, 110, 89 },{ 185, 113, 89 },{ 188, 113, 92 },{ 184, 108, 82 },{ 187, 102, 81 },{ 176, 88, 66 },{ 163, 78, 58 },{ 152, 70, 49 },{ 141, 65, 42 },{ 135, 62, 47 },{ 130, 62, 43 },{ 124, 56, 35 },{ 122, 55, 36 },{ 117, 54, 37 },{ 104, 47, 30 },{ 92, 39, 21 },{ 95, 42, 26 },{ 114, 56, 36 },{ 143, 79, 54 },{ 145, 79, 53 },{ 145, 79, 55 },{ 136, 75, 47 },{ 140, 79, 51 },{ 154, 88, 62 },{ 143, 79, 51 },{ 134, 74, 48 },{ 127, 67, 41 },{ 121, 67, 41 },{ 122, 75, 47 },{ 132, 83, 53 },{ 124, 77, 49 },{ 115, 71, 46 },{ 119, 72, 46 },{ 120, 70, 43 },{ 117, 70, 44 },{ 124, 73, 46 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 181, 122, 92 },{ 178, 121, 91 },{ 186, 127, 97 },{ 195, 129, 103 },{ 199, 130, 101 },{ 191, 124, 95 },{ 178, 112, 80 },{ 174, 115, 85 },{ 183, 126, 99 },{ 182, 125, 98 },{ 184, 133, 106 },{ 185, 131, 107 },{ 177, 116, 95 },{ 179, 112, 86 },{ 181, 114, 87 },{ 187, 117, 91 },{ 190, 118, 94 },{ 186, 115, 95 },{ 189, 114, 95 },{ 188, 106, 85 },{ 181, 93, 73 },{ 181, 88, 71 },{ 175, 79, 63 },{ 179, 88, 70 },{ 175, 78, 62 },{ 164, 62, 48 },{ 154, 56, 45 },{ 140, 48, 33 },{ 124, 41, 25 },{ 112, 39, 24 },{ 116, 48, 29 },{ 103, 40, 22 },{ 97, 40, 23 },{ 95, 42, 26 },{ 73, 27, 12 },{ 67, 23, 14 },{ 78, 23, 16 },{ 85, 25, 15 },{ 93, 33, 23 },{ 101, 42, 24 },{ 128, 66, 43 },{ 147, 83, 56 },{ 149, 85, 58 },{ 141, 80, 52 },{ 150, 89, 61 },{ 156, 90, 64 },{ 141, 76, 48 },{ 136, 76, 52 },{ 126, 68, 44 },{ 122, 68, 42 },{ 122, 75, 47 },{ 129, 82, 54 },{ 120, 73, 47 },{ 109, 65, 40 },{ 121, 71, 48 },{ 118, 68, 43 },{ 116, 66, 41 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 172, 116, 89 },{ 179, 122, 92 },{ 182, 127, 96 },{ 194, 133, 105 },{ 198, 131, 104 },{ 193, 130, 99 },{ 182, 122, 88 },{ 180, 125, 94 },{ 185, 125, 101 },{ 184, 122, 97 },{ 186, 130, 103 },{ 176, 119, 90 },{ 171, 111, 83 },{ 175, 110, 82 },{ 179, 109, 83 },{ 178, 100, 78 },{ 178, 96, 72 },{ 172, 85, 65 },{ 164, 73, 55 },{ 158, 65, 47 },{ 158, 67, 48 },{ 144, 57, 38 },{ 146, 57, 41 },{ 154, 68, 51 },{ 143, 62, 43 },{ 130, 57, 38 },{ 127, 71, 44 },{ 128, 75, 43 },{ 121, 68, 34 },{ 116, 75, 53 },{ 143, 108, 86 },{ 107, 66, 44 },{ 90, 44, 28 },{ 103, 46, 35 },{ 86, 28, 17 },{ 88, 21, 15 },{ 109, 29, 22 },{ 132, 53, 36 },{ 135, 63, 41 },{ 121, 52, 36 },{ 126, 60, 38 },{ 147, 80, 54 },{ 150, 86, 58 },{ 151, 91, 63 },{ 162, 101, 73 },{ 153, 92, 64 },{ 142, 81, 53 },{ 133, 77, 50 },{ 123, 72, 45 },{ 123, 69, 43 },{ 121, 71, 46 },{ 123, 73, 50 },{ 111, 65, 42 },{ 103, 61, 37 },{ 118, 72, 48 },{ 111, 65, 41 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 169, 119, 94 },{ 178, 122, 89 },{ 180, 123, 93 },{ 188, 131, 104 },{ 194, 134, 106 },{ 195, 135, 107 },{ 187, 130, 100 },{ 184, 129, 98 },{ 184, 123, 95 },{ 177, 112, 82 },{ 172, 112, 78 },{ 156, 91, 61 },{ 137, 73, 48 },{ 135, 67, 46 },{ 129, 50, 35 },{ 84, 30, 20 },{ 74, 32, 20 },{ 125, 74, 57 },{ 160, 104, 77 },{ 140, 95, 64 },{ 179, 146, 115 },{ 161, 126, 96 },{ 150, 114, 82 },{ 173, 136, 109 },{ 173, 137, 105 },{ 164, 125, 92 },{ 152, 113, 72 },{ 159, 114, 72 },{ 152, 103, 63 },{ 113, 65, 42 },{ 128, 75, 59 },{ 133, 60, 43 },{ 126, 44, 32 },{ 136, 44, 33 },{ 131, 41, 30 },{ 126, 44, 30 },{ 131, 53, 33 },{ 143, 65, 43 },{ 143, 65, 42 },{ 136, 59, 39 },{ 126, 58, 35 },{ 141, 74, 47 },{ 158, 93, 65 },{ 163, 103, 75 },{ 165, 103, 78 },{ 154, 94, 68 },{ 141, 81, 57 },{ 132, 78, 50 },{ 120, 70, 43 },{ 118, 68, 43 },{ 116, 68, 45 },{ 112, 65, 45 },{ 103, 58, 37 },{ 102, 60, 36 },{ 120, 73, 47 },{ 109, 64, 41 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 167, 120, 100 },{ 180, 124, 91 },{ 174, 121, 89 },{ 181, 124, 95 },{ 191, 134, 105 },{ 196, 139, 110 },{ 197, 140, 113 },{ 189, 134, 103 },{ 184, 124, 96 },{ 173, 110, 75 },{ 161, 95, 60 },{ 149, 75, 46 },{ 137, 62, 43 },{ 158, 75, 61 },{ 155, 61, 51 },{ 123, 41, 30 },{ 94, 34, 24 },{ 130, 60, 48 },{ 168, 86, 65 },{ 146, 70, 47 },{ 158, 101, 82 },{ 160, 109, 90 },{ 144, 96, 73 },{ 177, 129, 106 },{ 177, 130, 100 },{ 167, 116, 87 },{ 151, 86, 58 },{ 152, 74, 51 },{ 152, 67, 47 },{ 143, 54, 40 },{ 149, 51, 42 },{ 156, 58, 47 },{ 144, 50, 38 },{ 136, 48, 36 },{ 132, 49, 35 },{ 133, 54, 37 },{ 137, 62, 41 },{ 137, 62, 39 },{ 144, 68, 44 },{ 138, 66, 42 },{ 123, 57, 33 },{ 135, 68, 42 },{ 158, 91, 64 },{ 164, 103, 75 },{ 165, 104, 76 },{ 155, 91, 64 },{ 138, 76, 53 },{ 131, 75, 50 },{ 118, 68, 45 },{ 114, 68, 42 },{ 116, 70, 46 },{ 108, 66, 44 },{ 97, 56, 34 },{ 108, 64, 39 },{ 119, 69, 44 },{ 106, 61, 38 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 147, 99, 85 },{ 175, 120, 89 },{ 172, 123, 91 },{ 181, 122, 92 },{ 190, 131, 101 },{ 197, 140, 111 },{ 203, 143, 119 },{ 194, 137, 108 },{ 187, 131, 106 },{ 177, 118, 88 },{ 168, 103, 71 },{ 159, 90, 57 },{ 175, 105, 79 },{ 196, 119, 93 },{ 193, 105, 85 },{ 192, 88, 75 },{ 194, 92, 78 },{ 191, 87, 76 },{ 192, 92, 76 },{ 190, 80, 65 },{ 177, 69, 57 },{ 175, 72, 63 },{ 171, 73, 60 },{ 170, 77, 62 },{ 172, 75, 59 },{ 176, 80, 66 },{ 177, 75, 63 },{ 176, 70, 57 },{ 173, 72, 62 },{ 172, 74, 61 },{ 165, 77, 65 },{ 154, 65, 51 },{ 146, 57, 41 },{ 140, 59, 40 },{ 135, 54, 35 },{ 137, 56, 35 },{ 135, 63, 39 },{ 141, 70, 48 },{ 142, 72, 47 },{ 137, 65, 43 },{ 121, 56, 36 },{ 126, 66, 42 },{ 149, 83, 57 },{ 162, 95, 69 },{ 167, 101, 75 },{ 155, 89, 63 },{ 136, 80, 53 },{ 127, 72, 51 },{ 118, 66, 45 },{ 113, 67, 41 },{ 113, 66, 40 },{ 107, 62, 41 },{ 100, 59, 37 },{ 117, 71, 47 },{ 116, 66, 41 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 167, 112, 82 },{ 176, 125, 96 },{ 176, 127, 95 },{ 182, 127, 97 },{ 197, 137, 111 },{ 201, 144, 117 },{ 197, 141, 114 },{ 191, 134, 105 },{ 178, 118, 84 },{ 170, 106, 71 },{ 167, 102, 72 },{ 182, 116, 90 },{ 199, 132, 106 },{ 197, 125, 101 },{ 192, 110, 89 },{ 190, 101, 83 },{ 191, 95, 81 },{ 197, 105, 92 },{ 199, 111, 97 },{ 197, 101, 87 },{ 199, 97, 83 },{ 193, 90, 81 },{ 188, 85, 68 },{ 187, 81, 68 },{ 189, 87, 75 },{ 187, 90, 74 },{ 185, 88, 72 },{ 186, 97, 83 },{ 183, 106, 88 },{ 175, 94, 77 },{ 158, 76, 54 },{ 151, 69, 48 },{ 142, 61, 42 },{ 136, 60, 37 },{ 135, 63, 38 },{ 138, 66, 41 },{ 146, 72, 47 },{ 143, 68, 45 },{ 138, 66, 44 },{ 129, 64, 42 },{ 132, 72, 48 },{ 140, 80, 56 },{ 155, 89, 65 },{ 160, 94, 72 },{ 147, 85, 62 },{ 132, 74, 50 },{ 123, 71, 47 },{ 111, 63, 41 },{ 106, 60, 36 },{ 114, 64, 37 },{ 104, 59, 38 },{ 101, 58, 41 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 142, 105, 87 },{ 177, 123, 89 },{ 176, 122, 96 },{ 179, 125, 99 },{ 186, 133, 102 },{ 199, 142, 123 },{ 201, 143, 119 },{ 193, 133, 107 },{ 183, 122, 91 },{ 177, 114, 81 },{ 175, 111, 83 },{ 182, 116, 90 },{ 200, 133, 107 },{ 203, 131, 107 },{ 197, 120, 100 },{ 193, 107, 84 },{ 194, 106, 84 },{ 199, 118, 101 },{ 209, 137, 122 },{ 210, 134, 120 },{ 201, 113, 99 },{ 193, 105, 83 },{ 190, 99, 80 },{ 185, 98, 79 },{ 179, 92, 73 },{ 175, 90, 69 },{ 175, 88, 68 },{ 171, 89, 67 },{ 162, 87, 64 },{ 154, 76, 54 },{ 147, 69, 49 },{ 143, 66, 46 },{ 136, 61, 40 },{ 133, 61, 39 },{ 138, 66, 42 },{ 145, 70, 47 },{ 143, 68, 45 },{ 143, 68, 45 },{ 138, 71, 45 },{ 136, 74, 49 },{ 133, 75, 51 },{ 134, 78, 53 },{ 144, 82, 57 },{ 150, 86, 61 },{ 137, 77, 53 },{ 127, 73, 49 },{ 117, 67, 42 },{ 108, 60, 38 },{ 107, 61, 37 },{ 105, 63, 39 },{ 96, 54, 32 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 175, 116, 86 },{ 173, 118, 88 },{ 173, 119, 91 },{ 182, 128, 104 },{ 192, 140, 119 },{ 198, 140, 118 },{ 194, 132, 109 },{ 182, 121, 92 },{ 180, 119, 90 },{ 180, 119, 91 },{ 189, 123, 99 },{ 201, 131, 106 },{ 205, 133, 109 },{ 196, 124, 100 },{ 195, 114, 93 },{ 197, 117, 94 },{ 195, 119, 96 },{ 196, 125, 107 },{ 191, 119, 97 },{ 180, 100, 75 },{ 174, 98, 72 },{ 166, 90, 67 },{ 162, 86, 63 },{ 154, 78, 55 },{ 156, 81, 58 },{ 158, 86, 62 },{ 158, 83, 60 },{ 148, 78, 53 },{ 142, 71, 49 },{ 137, 64, 45 },{ 136, 65, 45 },{ 129, 61, 40 },{ 132, 64, 41 },{ 142, 70, 48 },{ 145, 70, 47 },{ 142, 67, 44 },{ 142, 70, 46 },{ 139, 72, 46 },{ 134, 77, 50 },{ 140, 80, 56 },{ 136, 80, 57 },{ 134, 74, 50 },{ 142, 78, 53 },{ 133, 73, 49 },{ 116, 64, 42 },{ 110, 63, 37 },{ 102, 57, 36 },{ 103, 58, 35 },{ 102, 61, 39 },{ 100, 58, 34 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 146, 104, 82 },{ 177, 118, 86 },{ 172, 118, 92 },{ 177, 122, 101 },{ 187, 135, 113 },{ 193, 137, 114 },{ 191, 131, 107 },{ 182, 121, 93 },{ 172, 115, 86 },{ 176, 118, 94 },{ 197, 131, 109 },{ 200, 130, 105 },{ 204, 132, 110 },{ 195, 128, 102 },{ 193, 123, 98 },{ 193, 118, 97 },{ 191, 118, 99 },{ 187, 115, 93 },{ 183, 105, 83 },{ 179, 104, 83 },{ 173, 102, 84 },{ 170, 103, 84 },{ 162, 94, 75 },{ 167, 90, 72 },{ 165, 93, 71 },{ 161, 95, 71 },{ 168, 98, 73 },{ 157, 86, 64 },{ 151, 79, 57 },{ 140, 69, 49 },{ 138, 67, 45 },{ 136, 68, 45 },{ 136, 68, 45 },{ 141, 70, 48 },{ 141, 69, 47 },{ 144, 72, 48 },{ 147, 77, 51 },{ 143, 77, 53 },{ 135, 79, 54 },{ 141, 79, 58 },{ 128, 71, 52 },{ 133, 77, 54 },{ 134, 72, 47 },{ 124, 66, 42 },{ 110, 59, 38 },{ 101, 56, 33 },{ 103, 58, 35 },{ 103, 58, 35 },{ 100, 58, 34 },{ 108, 62, 38 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 161, 111, 86 },{ 176, 120, 93 },{ 169, 117, 93 },{ 182, 130, 106 },{ 193, 137, 114 },{ 191, 131, 107 },{ 183, 123, 95 },{ 176, 120, 93 },{ 180, 124, 101 },{ 194, 129, 107 },{ 199, 131, 108 },{ 199, 131, 108 },{ 202, 131, 109 },{ 194, 122, 100 },{ 191, 118, 99 },{ 184, 118, 96 },{ 187, 121, 95 },{ 191, 122, 93 },{ 184, 114, 88 },{ 179, 109, 84 },{ 181, 109, 87 },{ 176, 108, 85 },{ 176, 99, 79 },{ 179, 101, 78 },{ 173, 103, 78 },{ 168, 102, 80 },{ 161, 95, 73 },{ 153, 80, 61 },{ 146, 74, 52 },{ 142, 70, 48 },{ 145, 73, 49 },{ 140, 70, 45 },{ 139, 68, 46 },{ 141, 70, 48 },{ 142, 71, 49 },{ 143, 76, 50 },{ 141, 79, 56 },{ 131, 77, 51 },{ 132, 71, 52 },{ 122, 67, 47 },{ 121, 69, 47 },{ 121, 65, 40 },{ 116, 62, 38 },{ 104, 57, 37 },{ 98, 57, 39 },{ 103, 57, 33 },{ 103, 58, 35 },{ 102, 57, 34 },{ 123, 73, 48 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 170, 115, 85 },{ 171, 115, 88 },{ 172, 122, 99 },{ 187, 131, 108 },{ 186, 126, 100 },{ 178, 118, 90 },{ 181, 125, 100 },{ 183, 128, 107 },{ 186, 125, 104 },{ 193, 129, 104 },{ 196, 131, 109 },{ 196, 126, 101 },{ 198, 123, 104 },{ 195, 124, 104 },{ 193, 126, 107 },{ 196, 131, 113 },{ 196, 130, 108 },{ 194, 124, 99 },{ 188, 115, 96 },{ 183, 112, 90 },{ 185, 120, 98 },{ 181, 109, 87 },{ 175, 99, 75 },{ 174, 100, 75 },{ 168, 96, 74 },{ 157, 84, 65 },{ 151, 75, 52 },{ 148, 73, 52 },{ 145, 70, 49 },{ 146, 71, 50 },{ 147, 75, 53 },{ 145, 74, 52 },{ 145, 77, 56 },{ 139, 73, 51 },{ 138, 73, 51 },{ 132, 71, 50 },{ 127, 71, 48 },{ 127, 66, 45 },{ 122, 67, 46 },{ 116, 64, 42 },{ 114, 62, 38 },{ 110, 60, 37 },{ 101, 54, 34 },{ 101, 56, 37 },{ 107, 61, 38 },{ 97, 54, 35 },{ 112, 64, 41 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 133, 100, 83 },{ 166, 113, 82 },{ 166, 114, 90 },{ 174, 120, 96 },{ 178, 120, 96 },{ 177, 120, 93 },{ 181, 125, 102 },{ 182, 127, 107 },{ 188, 127, 106 },{ 185, 120, 98 },{ 189, 124, 104 },{ 194, 128, 106 },{ 195, 125, 100 },{ 194, 128, 106 },{ 199, 132, 115 },{ 201, 135, 113 },{ 195, 129, 105 },{ 188, 121, 95 },{ 185, 115, 90 },{ 177, 106, 84 },{ 174, 106, 83 },{ 167, 95, 71 },{ 160, 90, 65 },{ 160, 89, 67 },{ 157, 82, 63 },{ 151, 76, 57 },{ 151, 76, 57 },{ 151, 75, 52 },{ 150, 75, 54 },{ 144, 72, 50 },{ 150, 79, 57 },{ 149, 81, 58 },{ 148, 82, 58 },{ 137, 75, 52 },{ 133, 75, 53 },{ 128, 70, 50 },{ 122, 65, 45 },{ 125, 67, 47 },{ 115, 63, 41 },{ 114, 62, 41 },{ 106, 58, 36 },{ 103, 56, 36 },{ 94, 49, 28 },{ 103, 55, 35 },{ 103, 60, 41 },{ 97, 54, 35 },{ 125, 73, 49 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 138, 97, 79 },{ 162, 106, 79 },{ 167, 117, 94 },{ 175, 121, 95 },{ 176, 120, 93 },{ 178, 122, 99 },{ 181, 124, 105 },{ 183, 122, 101 },{ 186, 120, 98 },{ 186, 119, 100 },{ 187, 122, 102 },{ 193, 125, 102 },{ 198, 128, 103 },{ 197, 131, 109 },{ 195, 128, 109 },{ 188, 120, 99 },{ 185, 121, 96 },{ 188, 117, 95 },{ 180, 109, 89 },{ 175, 107, 84 },{ 177, 105, 81 },{ 175, 105, 80 },{ 171, 104, 78 },{ 169, 97, 73 },{ 164, 89, 66 },{ 157, 79, 59 },{ 154, 79, 56 },{ 150, 78, 56 },{ 142, 74, 51 },{ 145, 77, 54 },{ 151, 84, 58 },{ 152, 86, 60 },{ 137, 75, 52 },{ 127, 73, 49 },{ 122, 70, 49 },{ 115, 63, 42 },{ 113, 61, 40 },{ 103, 55, 33 },{ 99, 51, 31 },{ 93, 52, 32 },{ 91, 50, 32 },{ 94, 56, 35 },{ 105, 58, 38 },{ 98, 55, 36 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 141, 96, 73 },{ 163, 109, 81 },{ 166, 112, 86 },{ 172, 118, 92 },{ 179, 127, 103 },{ 179, 122, 102 },{ 182, 125, 105 },{ 187, 122, 100 },{ 190, 123, 104 },{ 188, 122, 100 },{ 195, 127, 106 },{ 195, 127, 104 },{ 193, 127, 103 },{ 193, 122, 102 },{ 189, 121, 102 },{ 196, 134, 111 },{ 199, 133, 109 },{ 195, 129, 105 },{ 189, 123, 97 },{ 187, 122, 94 },{ 187, 120, 94 },{ 185, 117, 94 },{ 183, 117, 93 },{ 178, 112, 86 },{ 162, 90, 66 },{ 157, 81, 58 },{ 150, 80, 54 },{ 144, 77, 51 },{ 143, 76, 50 },{ 146, 81, 59 },{ 144, 79, 59 },{ 133, 72, 51 },{ 125, 73, 49 },{ 121, 71, 48 },{ 115, 63, 41 },{ 110, 62, 40 },{ 99, 52, 32 },{ 93, 48, 29 },{ 88, 47, 29 },{ 91, 53, 34 },{ 98, 57, 37 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 132, 95, 77 },{ 163, 103, 77 },{ 167, 112, 91 },{ 176, 120, 95 },{ 171, 121, 96 },{ 180, 124, 101 },{ 184, 128, 105 },{ 184, 125, 107 },{ 188, 127, 108 },{ 191, 129, 108 },{ 192, 127, 105 },{ 192, 128, 103 },{ 194, 126, 103 },{ 200, 135, 115 },{ 201, 140, 121 },{ 200, 133, 116 },{ 189, 122, 96 },{ 188, 118, 93 },{ 185, 118, 91 },{ 180, 113, 86 },{ 177, 111, 85 },{ 181, 117, 92 },{ 178, 114, 87 },{ 165, 98, 71 },{ 160, 86, 57 },{ 145, 78, 51 },{ 148, 83, 61 },{ 132, 71, 50 },{ 133, 76, 56 },{ 129, 74, 53 },{ 127, 72, 51 },{ 124, 72, 50 },{ 119, 67, 45 },{ 110, 62, 40 },{ 103, 58, 37 },{ 94, 53, 33 },{ 87, 52, 33 },{ 87, 52, 33 },{ 98, 57, 39 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 123, 85, 74 },{ 160, 100, 72 },{ 166, 108, 84 },{ 169, 114, 93 },{ 180, 123, 103 },{ 177, 126, 107 },{ 184, 125, 107 },{ 186, 127, 111 },{ 191, 130, 112 },{ 193, 130, 112 },{ 191, 126, 104 },{ 195, 128, 109 },{ 201, 140, 121 },{ 197, 145, 124 },{ 196, 135, 117 },{ 190, 123, 97 },{ 187, 122, 94 },{ 185, 124, 95 },{ 180, 119, 91 },{ 181, 117, 90 },{ 177, 113, 88 },{ 170, 109, 81 },{ 165, 101, 74 },{ 158, 94, 67 },{ 152, 87, 65 },{ 147, 85, 64 },{ 136, 78, 58 },{ 134, 77, 57 },{ 125, 73, 51 },{ 120, 70, 47 },{ 118, 67, 46 },{ 111, 63, 43 },{ 101, 54, 34 },{ 91, 53, 32 },{ 83, 52, 34 },{ 83, 50, 35 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 146, 94, 72 },{ 162, 105, 76 },{ 169, 112, 92 },{ 168, 116, 95 },{ 173, 122, 101 },{ 187, 131, 114 },{ 190, 133, 116 },{ 190, 131, 115 },{ 197, 138, 120 },{ 203, 142, 124 },{ 202, 143, 125 },{ 194, 137, 117 },{ 187, 125, 102 },{ 186, 122, 95 },{ 176, 115, 87 },{ 177, 122, 92 },{ 173, 118, 88 },{ 171, 110, 82 },{ 168, 104, 77 },{ 164, 103, 75 },{ 161, 100, 72 },{ 155, 95, 69 },{ 153, 93, 67 },{ 143, 81, 56 },{ 146, 84, 61 },{ 131, 73, 53 },{ 119, 67, 46 },{ 112, 65, 45 },{ 105, 63, 41 },{ 100, 55, 36 },{ 91, 53, 34 },{ 83, 50, 35 },{ 83, 50, 35 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 131, 83, 69 },{ 156, 98, 76 },{ 160, 101, 83 },{ 169, 110, 92 },{ 179, 124, 104 },{ 187, 130, 113 },{ 188, 130, 116 },{ 195, 144, 127 },{ 199, 146, 128 },{ 199, 142, 122 },{ 195, 135, 111 },{ 189, 125, 98 },{ 179, 118, 90 },{ 172, 111, 83 },{ 172, 115, 88 },{ 166, 111, 81 },{ 168, 108, 80 },{ 173, 112, 83 },{ 165, 106, 76 },{ 162, 101, 72 },{ 153, 96, 67 },{ 146, 91, 61 },{ 138, 81, 54 },{ 138, 80, 58 },{ 124, 69, 49 },{ 112, 61, 42 },{ 105, 58, 40 },{ 95, 54, 34 },{ 89, 54, 34 },{ 82, 48, 36 },{ 88, 50, 37 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 141, 89, 68 },{ 152, 101, 80 },{ 167, 112, 92 },{ 183, 126, 109 },{ 187, 130, 113 },{ 182, 131, 112 },{ 186, 134, 112 },{ 188, 130, 108 },{ 185, 128, 101 },{ 179, 122, 93 },{ 176, 119, 90 },{ 175, 113, 88 },{ 170, 108, 83 },{ 162, 105, 76 },{ 171, 114, 85 },{ 172, 113, 83 },{ 164, 103, 74 },{ 154, 90, 62 },{ 147, 87, 63 },{ 133, 79, 55 },{ 127, 72, 51 },{ 121, 66, 45 },{ 111, 63, 41 },{ 105, 58, 38 },{ 104, 57, 39 },{ 94, 55, 38 },{ 84, 52, 37 },{ 90, 55, 36 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 168, 116, 95 },{ 177, 122, 102 },{ 175, 123, 101 },{ 175, 121, 97 },{ 179, 119, 95 },{ 174, 117, 90 },{ 172, 117, 87 },{ 175, 119, 92 },{ 167, 107, 81 },{ 155, 95, 71 },{ 155, 98, 71 },{ 164, 107, 78 },{ 161, 104, 75 },{ 160, 100, 72 },{ 147, 85, 60 },{ 136, 78, 58 },{ 124, 69, 49 },{ 114, 63, 44 },{ 108, 61, 43 },{ 97, 56, 38 },{ 93, 56, 38 },{ 93, 55, 42 },{ 88, 52, 40 },{ 92, 55, 39 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 165, 113, 92 },{ 171, 121, 98 },{ 171, 119, 95 },{ 168, 112, 87 },{ 163, 107, 82 },{ 159, 103, 76 },{ 157, 97, 71 },{ 155, 93, 70 },{ 150, 92, 70 },{ 156, 102, 78 },{ 152, 96, 71 },{ 147, 91, 64 },{ 140, 86, 60 },{ 131, 79, 57 },{ 116, 71, 52 },{ 109, 66, 49 },{ 99, 60, 43 },{ 93, 60, 45 },{ 84, 55, 41 },{ 87, 55, 42 },{ 91, 55, 41 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } },
		{ { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 156, 108, 85 },{ 155, 103, 82 },{ 149, 97, 73 },{ 144, 96, 73 },{ 147, 93, 69 },{ 145, 93, 71 },{ 138, 87, 66 },{ 132, 81, 62 },{ 124, 76, 56 },{ 113, 68, 49 },{ 97, 60, 44 },{ 94, 60, 48 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } }
	};
	for (int i = 0; i < 120; i++)
	{
		for (int j = 0; j < 170; j++)
		{
			float red = (1.0 / 255.0) * colorsRGB[i][j][0];
			float green = (1.0 / 255.0) * colorsRGB[i][j][1];;
			float blue = (1.0 / 255.0) * colorsRGB[i][j][2];;
			if (red != 0.0 && green != 0.0 && blue != 0.0)
			{
				changeColor(red, green, blue);
				Point pt(center.getX() + j - 24, center.getY() + i - 24);
				rotate(pt, center, rotation);
				drawDot(Point(pt.getX(), pt.getY()));
			}
		}
	}
	changeColor(1, 1, 1);
}
