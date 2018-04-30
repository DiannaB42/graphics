#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iup/iup.h>
#include <cd/cd.h>
#include <cd/cdiup.h>
#include "picat.h"

static cdCanvas *cdcanvas = NULL;

typedef enum  {Circle, Line, Rect, Triangle, Component}TagType;

struct Lines{
  int x1;
  int x2;
  int y1;
  int y2;
};

struct LineCom{
  int color;
  int width;
  int style;
};

struct Rects{
  int x1;
  int x2;
  int y1;
  int y2;
  int fill;
};

struct Triangles{
  int x1;
  int x2;
  int x3;
  int y1;
  int y2;
  int y3;
};

struct Circles{
  int x1;
  int x2;
  int y1;
  int y2;
  long angle1;
  long angle2;
  int fill;
};

struct Drawing {
  TagType type;
  void* value;
  struct Drawing* next;
};

static struct Drawing* drawings = NULL;
static struct Drawing* last = NULL;
static char canvasSize[100];


void insertEnd(void* nextNode, TagType type){
  //create new node and assign data
  struct Drawing* draw = (struct Drawing*) malloc(sizeof(struct Drawing));
  if( draw == NULL){
    printf("Error: Out of Memory");
    exit(1);
  }
  draw->type = type;
  draw->value = nextNode;
  draw->next = NULL;

  if(drawings == NULL){
    //empty list
    drawings = draw;
    last = draw;
  } else {
    if(last != NULL){ 
      last->next = draw;
      last = draw;
    }
  }
} 

void clearList(){
  struct Drawing* node = drawings; 
  struct Drawing* next;
  while(node != NULL){
    next = node->next;
    free(node->value);
    free(node);
    node = next;
  }
  drawings = NULL;
  last = NULL;
}

int redraw_cb( Ihandle *self, float x, float y )
{
  cdCanvasActivate(cdcanvas);
  cdCanvasClear(cdcanvas);
  cdCanvasForeground(cdcanvas, CD_RED);

  struct Drawing* node = drawings;
  while(node != NULL){
    switch(node->type){
      case 1: ;
        struct Lines *line = node->value;  
        cdCanvasLine(cdcanvas, line->x1, line->y1, line->x2, line->y2);
        break;
      case 0: ;
        struct Circles *circle = node->value;
        if(circle->fill == 0)
          cdCanvasArc(cdcanvas, circle->x1, circle->y1, circle->x2, circle->y2, circle->angle1, circle->angle2);
        else
          cdCanvasSector(cdcanvas, circle->x1, circle->y1, circle->x2, circle->y2, circle->angle1,circle->angle2);
        break;
      case 2: ;
        struct Rects *rect = node->value;
        if(rect->fill == 0){
          cdCanvasRect(cdcanvas, rect->x1, rect->x2, rect->y1, rect->y2);
        } else {
          cdCanvasBox(cdcanvas, rect->x1, rect->x2, rect->y1, rect->y2);
        }
        break;
      case 3: ;
        struct Triangles *tri = node->value;
        cdCanvasBegin(cdcanvas, CD_CLOSED_LINES);
        cdCanvasVertex(cdcanvas, tri->x1, tri->y1);
        cdCanvasVertex(cdcanvas, tri->x2, tri->y2);
        cdCanvasVertex(cdcanvas, tri->x3, tri->y3);
        cdCanvasEnd(cdcanvas);
    }
    node = node->next;  
  }
  //clearList();
  return IUP_DEFAULT;
}

c_clearCanvas(){
  clearList();
  return PICAT_TRUE;
}


int getFill(int fill){
  if( fill > 0)
    return 1;
  else
    return 0;
}

int assignShape(int x1, int x2, int y1, int y2, int fill, TagType type){
  //Puts the variables into array holding shapes we want to draw
  switch (type){
    case 2: ;//Rectangle
      struct Rects *rect;
      rect = (struct Rects*) malloc(sizeof(struct Rects));
      if(rect == NULL){
        printf("Error: Out of Memory\n");
        exit(1);
      }
      rect->x1 = x1;
      rect->x2 = x2;
      rect->y1 = y1;
      rect->y2 = y2;
      rect->fill = getFill(fill);
      insertEnd(rect, type);
      break;
  }
}

int assignLine(int x1, int x2, int y1, int y2, TagType type){
    if(type == 1){
      struct Lines *line;
      line = (struct Lines*) malloc(sizeof(struct Lines));
      if(line == NULL){
        printf("Error: Out of Memory\n");
        exit(1);
      }
      line->x1 = x1;
      line->x2 = x2;
      line->y1 = y1;
      line->y2 = y2;
      insertEnd(line, type);
    } 
}

int assignArc(int x1, int x2, int y1, int y2, long angle1, long angle2, int fill, TagType type){
  if( type == 0){
    struct Circles *circle;
    circle = (struct Circles*) malloc(sizeof(struct Circles));
    if(circle == NULL){
      printf("Error: Out of Memory\n");
      exit(1);
    }
    circle->x1 = x1;
    circle->x2 = x2;
    circle->y1 = y1;
    circle->y2 = y2;
    circle->angle1 = angle1;
    circle->angle2 = angle2;
    circle->fill = getFill(fill);
    insertEnd(circle, type); 
  }
}

int assignTri(int x1, int y1, int x2, int y2, int x3, int y3, int fill, TagType type){
  if(type == 3){
    struct Triangles* tri;
    tri = (struct Triangles*) malloc(sizeof(struct Triangles));
    if(tri == NULL){
      printf("Error: Out of Memory\n");
      exit(1);
    }
    tri->x1 = x1;
    tri->y1 = y1;
    tri->x2 = x2;
    tri->y2 = y2;
    tri->x3 = x3;
    tri->y3 = y3;
    insertEnd(tri, type);
  }
}

c_arc(){
  TERM x1 	= picat_get_call_arg(1,7);
  TERM y1 	= picat_get_call_arg(2,7);
  TERM x2 	= picat_get_call_arg(3,7);
  TERM y2 	= picat_get_call_arg(4,7);
  TERM angle1 	= picat_get_call_arg(5,7);
  TERM angle2	= picat_get_call_arg(6,7);
  TERM fill 	= picat_get_call_arg(7,7);
  if(!picat_is_integer(x1) || !picat_is_integer(x2) || !picat_is_integer(y1) || !picat_is_integer(y2) || !picat_is_integer(angle1) || !picat_is_integer(angle2) || !picat_is_integer(fill)){
    return PICAT_FALSE;
  }
  long cX1 = picat_get_integer(x1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cX2 = picat_get_integer(x2);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cY1 = picat_get_integer(y1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cY2 = picat_get_integer(y2);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cAngle1 = picat_get_integer(angle1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cAngle2 = picat_get_integer(angle2);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cFill = picat_get_integer(fill);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  assignArc((int)cX1+cX2/2,(int) cX2,(int) cY1-cY2/2, (int)cY2, cAngle1, cAngle2, (int)cFill, Circle); 
  return PICAT_TRUE;
}

c_triangle(){
  TERM x1 	= picat_get_call_arg(1,7);
  TERM y1 	= picat_get_call_arg(2,7);
  TERM x2 	= picat_get_call_arg(3,7);
  TERM y2 	= picat_get_call_arg(4,7);
  TERM x3 	= picat_get_call_arg(5,7);
  TERM y3	= picat_get_call_arg(6,7);
  TERM fill 	= picat_get_call_arg(7,7);
  if(!picat_is_integer(x1) || !picat_is_integer(x2) || !picat_is_integer(y1) || !picat_is_integer(y2) || !picat_is_integer(x3) || !picat_is_integer(y3) || !picat_is_integer(fill)){
    return PICAT_FALSE;
  }
  long cX1 = picat_get_integer(x1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cX2 = picat_get_integer(x2);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cX3 = picat_get_integer(x3);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cY1 = picat_get_integer(y1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cY2 = picat_get_integer(y2);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cY3 = picat_get_integer(y3);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cFill = picat_get_integer(fill);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  assignTri((int)cX1,(int) cY1,(int) cX2, (int)cY2, (int) cX3,(int) cY3, (int)cFill, Triangle); 
  return PICAT_TRUE;
}

c_oval(){
  TERM x1 = picat_get_call_arg(1,5);
  TERM y1 = picat_get_call_arg(2,5);
  TERM x2 = picat_get_call_arg(3,5);
  TERM y2 = picat_get_call_arg(4,5);
  TERM fill = picat_get_call_arg(5,5);
  if(!picat_is_integer(x1) || !picat_is_integer(x2) || !picat_is_integer(y1) || !picat_is_integer(y2) || !picat_is_integer(fill)){
    return PICAT_FALSE;
  }
  long cX1 = picat_get_integer(x1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cX2 = picat_get_integer(x2);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cY1 = picat_get_integer(y1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cY2 = picat_get_integer(y2);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cFill = picat_get_integer(fill);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  assignArc((int)cX1 + cX2/2,(int) cX2,(int) cY1 - cY2/2, (int)cY2, 0, 360, (int)cFill, Circle); 
  return PICAT_TRUE;
}

c_circle(){
  TERM x1 = picat_get_call_arg(1,4);
  TERM y1 = picat_get_call_arg(2,4);
  TERM x2 = picat_get_call_arg(3,4);
  TERM fill = picat_get_call_arg(4,4);
  if(!picat_is_integer(x1) || !picat_is_integer(x2) || !picat_is_integer(y1) || !picat_is_integer(fill)){
    return PICAT_FALSE;
  }
  long cX1 = picat_get_integer(x1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cX2 = picat_get_integer(x2);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cY1 = picat_get_integer(y1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cFill = picat_get_integer(fill);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  assignArc((int)cX1+ cX2/2,(int) cX2,(int) cY1 - cX2/2, (int)cX2, (int)cFill, 0, 360, Circle); 
  return PICAT_TRUE;
}

c_line(){
  TERM x1 = picat_get_call_arg(1,4);
  TERM y1 = picat_get_call_arg(2,4);
  TERM x2 = picat_get_call_arg(3,4);
  TERM y2 = picat_get_call_arg(4,4);
  if(!picat_is_integer(x1) || !picat_is_integer(x2) || !picat_is_integer(y1) || !picat_is_integer(y2)){
    return PICAT_FALSE;
  }
  long cX1 = picat_get_integer(x1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cX2 = picat_get_integer(x2);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cY1 = picat_get_integer(y1);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cY2 = picat_get_integer(y2);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  assignLine((int)cX1,(int) cX2,(int) cY1, (int)cY2, Line); 
  return PICAT_TRUE;
}

c_rectangle(){
  TERM xMin = picat_get_call_arg(1,5);
  TERM yMin = picat_get_call_arg(2,5);
  TERM xMax = picat_get_call_arg(3,5);
  TERM yMax = picat_get_call_arg(4,5);
  TERM fill = picat_get_call_arg(5,5);
  if(!picat_is_integer(xMin) || !picat_is_integer(xMax) || !picat_is_integer(yMin) ||!picat_is_integer(yMax) || !picat_is_integer(fill)){
    return PICAT_FALSE;
  }
  long cXMin = picat_get_integer(xMin);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cXMax = picat_get_integer(xMax);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cYMin = picat_get_integer(yMin);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cYMax = picat_get_integer(yMax);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  long cFill = picat_get_integer(fill);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  assignShape((int)cXMin,(int) cXMax+ cXMin,(int) cYMin, (int)cYMax+ cYMin, (int)cFill, Rect); 
  return PICAT_TRUE;
}

c_star(){
  TERM n = picat_get_call_arg(1,6);
  TERM x = picat_get_call_arg(2,6);
  TERM y = picat_get_call_arg(3,6);
  TERM angle = picat_get_call_arg(4,6);
  TERM diameter = picat_get_call_arg(5,6);
  TERM fill = picat_get_call_arg(6,6);

}


c_canvas()
{
  TERM x = picat_get_call_arg(1,2);
  TERM y = picat_get_call_arg(2,2);


  if(!picat_is_integer(x) || !picat_is_integer(y)){
    return PICAT_FALSE;
  }
  long cX = picat_get_integer(x);
  long cY = picat_get_integer(y);

  snprintf(canvasSize, 100, "%lix%li", cX, cY);
  Ihandle *dlg, *cnvs;
  IupOpen(NULL, NULL);

  cnvs = IupCanvas( NULL );
  IupSetCallback(cnvs, "ACTION",( Icallback )redraw_cb );
  IupSetAttribute(cnvs, "SIZE", canvasSize);

  dlg = IupDialog( IupVbox( cnvs, NULL ) );
  IupSetAttribute(dlg, "TITLE", "IupCanvas + Canvas Draw" );
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupMap( dlg );

  //cdInitContextPlus();
  //cdUseContextPlus(1);

  cdcanvas = cdCreateCanvas( CD_IUP, cnvs );
  cdCanvasSetAttribute(cdcanvas, "ANTIALIAS", "0");
  
  IupShowXY( dlg, IUP_CENTER, IUP_CENTER );
  IupMainLoop();
  IupClose();  
  return PICAT_TRUE;
}

