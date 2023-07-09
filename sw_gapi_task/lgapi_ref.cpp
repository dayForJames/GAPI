#include "lgapi.h"
#include <memory>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

void ExitProgram();

struct BatchRenderGL : public IRender
{
  ~BatchRenderGL() override { ExitProgram(); }
  
  unsigned int AddImage(Image2D a_img) override;

  void BeginRenderPass(Image2D fb) override;
  void Draw(PipelineStateObject a_state, Geom a_geom) override;
  void EndRenderPass(Image2D fb) override;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

Display              *dpy;
Window               root, win;
GLint                att[]   = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo          *vi;
GLXContext           glc;
Colormap             cmap;
XSetWindowAttributes swa;
XWindowAttributes	   wa;
XEvent			         xev;

void CreateWindow(int WIN_WIDTH_INITIAL, int WIN_HEIGHT_INITIAL)
{
  if((dpy = XOpenDisplay(NULL)) == NULL)
  {
    printf("\n\tcannot connect to x server\n\n");
    exit(0);
  }

  root = DefaultRootWindow(dpy);

  if((vi = glXChooseVisual(dpy, 0, att)) == NULL)
  {
    printf("\n\tno matching visual\n\n");
    exit(0);
  }

  if((cmap = XCreateColormap(dpy, root, vi->visual, AllocNone)) == 0)
  {
    printf("\n\tcannot create colormap\n\n");
    exit(0);
  }

  swa.event_mask = KeyPressMask;
  swa.colormap 	= cmap;
  win = XCreateWindow(dpy, root, 0, 0, WIN_WIDTH_INITIAL, WIN_HEIGHT_INITIAL, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
  XStoreName(dpy, win, "OpenGL Animation");
  XMapWindow(dpy, win);
}

void SetupGL()
{
  char		font_string[128];
  XFontStruct	*font_struct;

  glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);

  if(glc == NULL)
  {
    printf("\n\tcannot create gl context\n\n");
    exit(0);
  }

  glXMakeCurrent(dpy, win, glc);
  glClearColor(0.00, 0.00, 0.40, 1.00);
}

void ExitProgram()
{
  glXMakeCurrent(dpy, None, NULL);
  glXDestroyContext(dpy, glc);
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
  exit(0);
}

extern uint32_t WIN_WIDTH ;
extern uint32_t WIN_HEIGHT;

std::shared_ptr<IRender> MakeReferenceImpl() 
{ 
  CreateWindow(WIN_WIDTH,WIN_HEIGHT);
  SetupGL();

  XGetWindowAttributes(dpy, win, &wa);
  glViewport(0, 0, wa.width, wa.height);

  return std::make_shared<BatchRenderGL>(); 
};

static void transposeMatrix(const float in_matrix[16], float out_matrix[16])
{
  for(int i=0;i<4;i++)
    for(int j=0;j<4;j++)
      out_matrix[i*4+j] = in_matrix[j*4+i];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BatchRenderGL::BeginRenderPass(Image2D fb)
{
  glViewport(0, 0, fb.width, fb.height);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

  // usually we don't have to actually clear framebuffer if we are not going to immediately use it 
  // In this implementation we don't, because OpenGL render inside it's owk framebuffer. 
  // So, we should not do anything with 'fb'
  // In you software implementation you will likely to clear fb here unless you don't plan to use some specific internal framebuffer format and copy final image to fb at the end 

  // so, you could save input fb and later just directly draw to it! 

  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glEnable (GL_DEPTH_TEST);
}

unsigned int BatchRenderGL::AddImage(Image2D a_img)
{
  GLuint texture = (GLuint)(-1);
  glGenTextures(1, &texture);					// Create The Texture

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, a_img.width, a_img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, a_img.data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return (unsigned int)texture;
}

void BatchRenderGL::Draw(PipelineStateObject a_state, Geom a_geom)
{
  float tempMat[16];  
  glMatrixMode(GL_PROJECTION);
  transposeMatrix(a_state.projMatrix, tempMat);      // GL assume col-major matrices (which is usually better), while in our API thet are row-major
  glLoadMatrixf(tempMat);

  glMatrixMode(GL_MODELVIEW);
  transposeMatrix(a_state.worldViewMatrix, tempMat); // GL assume col-major matrices (which is usually better), while in our API thet are row-major
  glLoadMatrixf(tempMat);
  
  if(a_state.mode == MODE_TEXURE_3D)
  {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, a_state.imgId);
  }
  else
  {
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  glColorPointer (4, GL_FLOAT, 0,  a_geom.vcol4f);
  glVertexPointer(4, GL_FLOAT, 0,  a_geom.vpos4f);  
  glTexCoordPointer(2, GL_FLOAT, 0, a_geom.vtex2f);

  glDrawElements (GL_TRIANGLES, a_geom.primsNum*3, GL_UNSIGNED_INT, a_geom.indices);
}

void BatchRenderGL::EndRenderPass(Image2D fb)
{
  glFlush();
  glReadPixels(0, 0, fb.width, fb.height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)fb.data);
}