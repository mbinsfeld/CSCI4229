/*  Matt Binsfeld
 *  Homework 5
 *
 *  Draw a scene with several houses and lighting
 *
 *  Key bindings:
 *  'p' to switch modes (projections)
 *  'x' to toggle axes
 *  '0' snaps angles to 0,0
 *  arrows to rotate the world
 *  PgUp/PgDn zooms in/out
 *  +/- changes field of view of perspective
 *  F1 toggles smooth/flat shading
 *  F2 toggles local viewer mode
 *  F3 toggles light distance (1/5)
 *  F8 change ball increment
 *  F9 invert bottom normal
 *  'l' toggles lighting
 *  a/A decrease/increase ambient light
 *  d/D decrease/increase diffuse light
 *  s/S decrease/increase specular light
 *  e/E decrease/increase emitted light
 *  n/N decrease/increase shininess
 *  m  toggles light movement
 *  [] lower/rise light
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "CSCIx229.h"
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


int axes=0;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=10;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=5.0;  //  Size of world
// Light values
int one       =   1;  // Unit value
int distance  =   10;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
void ball(double x,double y,double z,double r);


/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}


/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}



/*
 *  Set projection
 */
/*static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}*/




/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(1,0,0);
   glNormal3f( 0, 0, 1);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3f(0,0,1);
   glNormal3f( 0, 0, -1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3f(1,1,0);
   glNormal3f( +1, 0, 0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3f(0,1,0);
   glNormal3f(-1, 0, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3f(0,1,1);
   glNormal3f(0, +1, 0);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(1,0,1);
   glNormal3f(0, -1, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

//draw a house
static void house(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{

   // Set specular color to white
  float white[] = {1,1,1,1};
  float black[] = {0,0,0,1};
  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   // Start with cube to make house base. (Used to investigate how to use multiple 

   cube(x,y,z,dx,dy,dz,th);

   // Next, add roof and chimney. This bit is all done without using "cube" or other built-in functions
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);


   //  Add roof to cube to make house
   glBegin(GL_QUADS);
   // Roof part 1
   glColor3f(0,1,1);
   glNormal3f(+1,+1,0);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+2,0);
   glVertex3f(-1,+2,0);
   // Roof part 2
   glNormal3f(-1,-1,0);
   glVertex3f(+1,+2,0);
   glVertex3f(-1,+2,0);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   // Roof ends (triangular bits)
   glEnd();
   glBegin(GL_TRIANGLES);
   glColor3f(0,1,0);
   glNormal3f(+1, 0, 0);
   glVertex3f(1,2,0);
   glVertex3f(1,1,-1);
   glVertex3f(1,1,1);
   glColor3f(1,1,0);
   glNormal3f(-1, 0, 0);
   glVertex3f(-1,2,0);
   glVertex3f(-1,1,-1);
   glVertex3f(-1,1,1); 
   glEnd();

   // Chimney
   glBegin(GL_QUADS);
   glVertex3f(.5,3,.5);
   glVertex3f(0,3,.5);
   glVertex3f(0,3,0);
   glVertex3f(.5,3,0); //end chimney top
   glColor3f(1,1,0);
   glNormal3f( -1, 0,   0);
   glVertex3f(.5,3,.5);
   glVertex3f(.5,1,.5);
   glVertex3f(0,1,.5);
   glVertex3f(0,3,.5); //end side 1

   glColor3f(1,0,0);
   glNormal3f( +1, 0,   0);
   glVertex3f(.5,3,.5);
   glVertex3f(.5,1,.5);
   glVertex3f(.5,1,0);
   glVertex3f(.5,3,0); //end side 2
   glColor3f(0,0,1);
   glNormal3f( 0, 0,+1);
   glVertex3f(0,3,0);
   glVertex3f(0,1,0);
   glVertex3f(0,1,.5);
   glVertex3f(0,3,.5); //end side 3
   glColor3f(0,1,0);
   glNormal3f(0,0,-1);
   glVertex3f(0,3,0);
   glVertex3f(0,1,0);
   glVertex3f(.5,1,0);
   glVertex3f(.5,3,0);

   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   //  Light switch
   if (light)
   {
     //  Translate intensity to color vectors
     float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
     float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
     float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
     //  Light position
     float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
     //  Draw light position as ball (still no lighting here)
     glColor3f(1,1,1);
     ball(Position[0],Position[1],Position[2] , 0.1);
     //  OpenGL should normalize normal vectors
     glEnable(GL_NORMALIZE);
     //  Enable lighting
     glEnable(GL_LIGHTING);
     //  Location of viewer for specular calculations
     glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
     //  glColor sets ambient and diffuse color materials
     glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
     glEnable(GL_COLOR_MATERIAL);
     //  Enable light 0
     glEnable(GL_LIGHT0);
     //  Set ambient, diffuse, specular components and position of light 0
     glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
     glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
     glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
     glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
     glDisable(GL_LIGHTING);
   
   house(-1,0,1,0.3,0.3,0.3,0);
   house(-1,0,-1,0.3,0.3,0.3,90);
   house(1,0,-1,0.3,0.3,0.3,180);
   house(1,0,1,0.3,0.3,0.3,270);

   house(-3,0,3,1,0.3,0.3,0);
   house(-3,.5,-3,0.3,1,0.3,0);
   house(3,0,-3,0.3,0.3,1,180);
   house(3,0,3,1,0.3,1,270);

   house(0,0,3,.3,.5,.5,0);
   house(0,0,-3,1.2,.5,.5,0);
   house(3,0,0,.5,.5,.3,270);


   // Draw ground on which houses sit
   glBegin(GL_QUADS);
   glColor3f(0,.5,0);
   glVertex3f(-5,-.5,-5);
   glVertex3f(-5,-.5,5);
   glVertex3f(5,-.5,5);
   glVertex3f(5,-.5,-5);
   glEnd();

   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/2000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th -= 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th += 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Smooth color model
   else if (key == GLUT_KEY_F1)
      smooth = 1-smooth;
   //  Local Viewer
   else if (key == GLUT_KEY_F2)
      local = 1-local;
   else if (key == GLUT_KEY_F3)
      distance = (distance==1) ? 10 : 1;
   //  Toggle ball increment
   else if (key == GLUT_KEY_F8)
      inc = (inc==10)?3:10;
   //  Flip sign
   else if (key == GLUT_KEY_F9)
      one = -one;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(mode?fov:0,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P')
      mode = 1-mode;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   //  Move light
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   //  Ambient level
   else if (ch=='a' && ambient>0)
      ambient -= 5;
   else if (ch=='A' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='d' && diffuse>0)
      diffuse -= 5;
   else if (ch=='D' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='s' && specular>0)
      specular -= 5;
   else if (ch=='S' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
   //  Translate shininess power to value (-1 => 0)
   shinyvec[0] = shininess<0 ? 0 : pow(2.0,shininess);

   //  Reproject
   Project(mode?fov:0,asp,dim);

   //  Animate if requested
   glutIdleFunc(move?idle:NULL);

   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode?fov:0,asp,dim);

}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Matthew Binsfeld");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(move?idle:NULL);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}