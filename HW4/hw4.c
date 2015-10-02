/*  Matt Binsfeld
 *  Homework 3
 *
 *  Draw a scene with several houses
 *
 *  Key bindings:
 *  m          Toggle between perspective and orthogonal
 *  +/-        Changes field of view for perspective
 *  arrows     Change view angle
 *  w/s  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

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
 *  Set projection
 */
static void Project()
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
}




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
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3f(0,0,1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3f(1,1,0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3f(0,1,0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3f(0,1,1);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(1,0,1);
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
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+2,0);
   glVertex3f(-1,+2,0);
   // Roof part 2
   glVertex3f(+1,+2,0);
   glVertex3f(-1,+2,0);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   // Roof ends (triangular bits)
   glEnd();
   glBegin(GL_TRIANGLES);
   glColor3f(0,1,0);
   glVertex3f(1,2,0);
   glVertex3f(1,1,-1);
   glVertex3f(1,1,1);
   glColor3f(1,1,0);
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
   glVertex3f(.5,3,.5);
   glVertex3f(.5,1,.5);
   glVertex3f(0,1,.5);
   glVertex3f(0,3,.5); //end side 1
   glColor3f(1,0,0);
   glVertex3f(.5,3,.5);
   glVertex3f(.5,1,.5);
   glVertex3f(.5,1,0);
   glVertex3f(.5,3,0); //end side 2
   glColor3f(0,0,1);
   glVertex3f(0,3,0);
   glVertex3f(0,1,0);
   glVertex3f(0,1,.5);
   glVertex3f(0,3,.5); //end side 3
   glColor3f(0,1,0);
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

   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
  //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   Project();
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
   else if (ch == '0'){
      th = 0;
      ph = 5;
   }
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   else if (ch == 'm' || ch == 'M')
     mode = (mode + 1) % 3;//3 possible modes now
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;

   //  PageUp key - increase dim
   else if (ch == 's')
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (ch == 'w' && dim>1)
      dim -= 0.1;

   Project();
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
   Project();
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
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
