#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "PNGLoader.h"
#include "TGALoader.h"

int main(int argc, char **argv) {
  Display              *display;
  Window               window;
  GLXContext           context;
  Window               root;
  GLint                desiredAttributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
  Colormap             colorMap;
  XSetWindowAttributes setWindowAttributes;
  XVisualInfo          *visualInfo;
  XWindowAttributes    windowAttributes;
  XEvent               event;
  GLuint               texture;
  int                  filenameLength;
  
  if (argc <= 1) {
    printf("Please supply an image as an argument.\n");
    return 0;
  }
  
  display = XOpenDisplay(NULL);
  
  if (display == NULL) {
    printf("\n\tcannot connect to X server\n\n");
    exit(1);
  }
  
  root = DefaultRootWindow(display);
  
  visualInfo = glXChooseVisual(display, 0, desiredAttributes);
  
  if (visualInfo == NULL) {
    printf("\n\tno appropriate visual found\n\n");
    exit(1);
  }
  
  colorMap = XCreateColormap(display, root, visualInfo->visual, AllocNone);
  
  setWindowAttributes.colormap = colorMap;
  setWindowAttributes.event_mask = ExposureMask | KeyPressMask;
  
  window = XCreateWindow(display, root, 0, 0, 640, 640, 0, visualInfo->depth,
			 InputOutput, visualInfo->visual, CWColormap | CWEventMask,
			 &setWindowAttributes);
  
  XMapWindow(display, window);
  
  XStoreName(display, window, "IMGLoader");
  
  context = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
  glXMakeCurrent(display, window, context);
  
  XGetWindowAttributes(display, window, &windowAttributes);
  glViewport(0, 0, windowAttributes.width, windowAttributes.height);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 1, 1, 0, 1, -1);
  glMatrixMode(GL_MODELVIEW);
  
  glEnable(GL_DEPTH_TEST);
  
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glColor3f(1.0, 1.0, 1.0);
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  
  texture = 0;
  filenameLength = strlen(argv[1]);
  if (strcasecmp(&argv[1][filenameLength-4], ".tga") == 0) {
    texture = loadTGA(argv[1]);
  }
  else if (strcasecmp(&argv[1][filenameLength-4], ".png") == 0) {
    texture = loadPNG(argv[1]);
  }
  else {
    printf("Could not determine image type of '%s'\n", argv[1]);
  }
  
  while (1) {
    XNextEvent(display, &event);
    
    if (event.type == Expose) {
      XGetWindowAttributes(display, window, &windowAttributes);
      glViewport(0, 0, windowAttributes.width, windowAttributes.height);
      
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);		glVertex2f(0.0, 0.0);
	glTexCoord2f(0.0, 1.0);		glVertex2f(0.0, 1.0);
	glTexCoord2f(1.0, 1.0);		glVertex2f(1.0, 1.0);
	glTexCoord2f(1.0, 0.0);		glVertex2f(1.0, 0.0);
      glEnd();
      
      glXSwapBuffers(display, window);
    }
    else if (event.type == KeyPress) {
      glXMakeCurrent(display, None, NULL);
      glXDestroyContext(display, context);
      XDestroyWindow(display, window);
      XCloseDisplay(display);
      exit(0);
    }
  }
  
  return 0;
}