#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iup/iup.h>
#include <cd/cd.h>
#include <cd/cdiup.h>
#include "picat.h"

static cdCanvas *cdcanvas = NULL;

typedef enum  {Circle, Line, Rect, Triangle, Polygon}TagType;

struct Lines{
  int x1;
  int x2;
  int y1;
  int y2;
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

struct Vertex{
  int x;
  int y;
  struct Vertex* next;
};

struct Polygons{
  int fill;
  struct Vertex* vertices; 
};

struct Drawing {
  TagType type;
  void* value;
  int color;
  struct Drawing* next;
};

static struct Drawing* drawings = NULL;
static struct Drawing* last = NULL;
static char canvasSize[100];
static int curColor = 1;

void setColor(int color){
  switch(color){
    case 1:
      cdCanvasForeground(cdcanvas, CD_RED);
      break;
    case 2:
      cdCanvasForeground(cdcanvas, CD_DARK_RED);
      break;
    case 3:
      cdCanvasForeground(cdcanvas, CD_YELLOW);
      break;
    case 4:
      cdCanvasForeground(cdcanvas, CD_GREEN);
      break;
    case 5:
      cdCanvasForeground(cdcanvas, CD_BLUE);
      break;
    case 6:
      cdCanvasForeground(cdcanvas, CD_MAGENTA);
      break;
    case 7:
      cdCanvasForeground(cdcanvas, CD_GRAY);
      break;
    case 8:
      cdCanvasForeground(cdcanvas, CD_BLACK);
      break;
    case 9:
      cdCanvasForeground(cdcanvas, CD_WHITE);
      break;
  }
}


void insertEnd(void* nextNode, TagType type){
  //create new node and assign data
  struct Drawing* draw = (struct Drawing*) malloc(sizeof(struct Drawing));
  if( draw == NULL){
    printf("Error: Out of Memory");
    exit(1);
  }
  draw->type = type;
  draw->value = nextNode;
  draw->color = -1;
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

void insertE(void* nextNode, TagType type, int color){
  //create new node and assign data
  struct Drawing* draw = (struct Drawing*) malloc(sizeof(struct Drawing));
  if( draw == NULL){
    printf("Error: Out of Memory");
    exit(1);
  }
  draw->type = type;
  draw->value = nextNode;
  draw->color = color;
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
  struct Vertex* cur;
  struct Vertex* nextVer;
  struct Polygons* poly;
  while(node != NULL){
    next = node->next;
    if(node->type == 4){
      poly = node->value;
      cur = poly->vertices;
      while(cur!= NULL){
        nextVer = cur->next;
	free(cur);
        cur = nextVer;
      }
    }
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
  
  int *height = malloc(sizeof(int));
  cdCanvasGetSize(cdcanvas, NULL, height, NULL, NULL);
  struct Drawing* node = drawings;
  curColor = 1;
  while(node != NULL){
    if(node->color != curColor){
      setColor(node->color);
      curColor = node->color;
    } 
    switch(node->type){
      case 1: ;
        struct Lines *line = node->value;
        //printf(" line x1 y1 x2 y2 %d %d %d %d %li\n",line->x1,line->y1, line->x2, line->y2, *height); 
        cdCanvasLine(cdcanvas, line->x1, *height - line->y1, line->x2, *height - line->y2);
        break;
      case 0: ;
        struct Circles *circle = node->value;
        if(circle->fill == 0){
          cdCanvasArc(cdcanvas, circle->x1, *height - circle->y1, circle->x2,circle->y2, circle->angle1, circle->angle2);
        }else
          cdCanvasSector(cdcanvas, circle->x1, *height - circle->y1, circle->x2, circle->y2, circle->angle1,circle->angle2);
        break;
      case 2: ;
        struct Rects *rect = node->value;
        if(rect->fill == 0){
          cdCanvasRect(cdcanvas, rect->x1, rect->x2, *height - rect->y1, *height - rect->y2);
        } else {
          cdCanvasBox(cdcanvas, rect->x1, rect->x2, *height - rect->y1, *height - rect->y2);
        }
        break;
      case 3: ;
        struct Triangles *tri = node->value;
        cdCanvasBegin(cdcanvas, CD_CLOSED_LINES);
        cdCanvasVertex(cdcanvas, tri->x1, *height - tri->y1);
        cdCanvasVertex(cdcanvas, tri->x2, *height - tri->y2);
        cdCanvasVertex(cdcanvas, tri->x3, *height - tri->y3);
        cdCanvasEnd(cdcanvas);
        break;
      case 4: ;
	struct Polygons* poly = node->value;
        struct Vertex* vertex = poly->vertices;
        cdCanvasBegin(cdcanvas, CD_CLOSED_LINES);
        while(vertex != NULL){
          //printf("x y for vertex %d %d\n", vertex->x, vertex->y);
	  cdCanvasVertex(cdcanvas, vertex->x,*height - vertex->y);
          vertex = vertex->next;
        }
        cdCanvasEnd(cdcanvas);
	break;
    }
    node = node->next;  
  }

  free(height);
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

int getColorValue(char* color){
  if( strcmp(color, "red") == 0){
    return 1;
  }
  if(strcmp(color, "dark red") == 0){
    return 2;
  }
  if(strcmp(color, "yellow") == 0){
    return 3;
  }
  if(strcmp(color, "green") == 0){
    return 4;
  }
  if(strcmp(color, "blue") == 0){
    return 5;
  }
  if(strcmp(color, "pink") == 0){
    return 6;
  }
  if(strcmp(color, "grey") == 0|| strcmp(color, "gray") == 0){
    return 7;
  }
  if(strcmp(color, "black") == 0){
    return 8;
  }
  if(strcmp(color, "white") == 0){
    return 9;
  }
}

int assignRect(int x1, int x2, int y1, int y2, int color, int fill, TagType type){
  //Puts the variables into array holding shapes we want to draw
  if(type == 2){
    //Rectangle
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
      if( color != curColor && color != -1){
        curColor = color;
      } else {
        color = -1;
      }
      insertE(rect, type, color);
  }
}

int assignLine(int x1, int x2, int y1, int y2, TagType type, int color){
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
      if( color != curColor && color != -1){
        curColor = color;
      } else {
        color = -1;
      }
      insertE(line, type, color);
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

void assignStar(int x, int y,int w,int h,int n, int centerX,int centerY, int angle,int diameter, int fill){
  struct Polygons* polygon;  
  struct Vertex* vertex;
  struct Vertex* next;
  polygon = (struct Polygons*) malloc(sizeof(struct Polygons));
  polygon->fill = getFill(fill);
  int arc = 360 / n;
  int alpha = 90 - arc + angle;
  if(alpha<0) alpha += 360;
  int angle1 = alpha + arc / 2;
  int angle2 = alpha + arc;
  for(int i = 0; i < n; i++){
    angle1 = (angle1 >=360 ? angle1-360 : angle1);
    angle2 = (angle2 >=360 ? angle2-360 : angle2);
    double rad1 = ((double)angle1/180.0)*M_PI;
    double rad2 = ((double)angle2/180.0)*M_PI;
    int sx = (int)(centerX-(diameter/2)*cos(rad1));
    int sy = (int)(centerY-(diameter/2)*sin(rad1));
    int lx = (int)(centerX-(h/2)*cos(rad2));
    int ly = (int)(centerY-(h/2)*sin(rad2));
    //printf("Small vertex %d %d\nLarge %d %d", sx, sy, lx, ly);
    vertex = (struct Vertex*) malloc(sizeof(struct Vertex));
    if( i == 0){
      polygon->vertices = vertex;
    } else {
      next->next = vertex;
    }
    next = (struct Vertex*) malloc(sizeof(struct Vertex));
    if(vertex == NULL|| next == NULL){
      printf("Error: Out of Memory\n");
      exit(1);
    }
    vertex->x = sx;
    vertex->y = sy;
    vertex->next = next;
    next->x = lx;
    next->y = ly;
    angle1 += arc;
    angle2 += arc;
    next->next = NULL;
  }
  insertEnd(polygon, Polygon);
}

int assignTri(int x1, int y1, int x2, int y2, int x3, int y3, int fill, TagType type, int color){
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
    if( color != curColor && color != -1){
      curColor = color;
    } else {
      color = -1;
    }
    insertE(tri, type, color);
  }
}

void deletePoly(struct Polygons* poly){

}

int assignPoly(int n, TERM x,TERM y, int fill){
  struct Polygons* poly;
  struct Vertex* vertex;
  struct Vertex* last;
  TERM curX;
  TERM curY;
  TERM remX;
  TERM remY;
  int valX;
  int valY;
  poly = (struct Polygons*) malloc(sizeof(struct Polygons));
  if(poly == NULL){
    printf("Error: Out of Memory\n");
    exit(1);
  }
  poly->fill = getFill(fill);
  curX = x;
  curY = y;
  for(int i = 0; i < n; i++){
    vertex = (struct Vertex*)malloc(sizeof(struct Vertex));
    vertex->next = NULL;
    curX = picat_get_car(curX);
    curY = picat_get_car(curY);
    remX = picat_get_cdr(curX);
    remY = picat_get_cdr(curY);
    if(picat_is_nil(curX) || picat_is_nil(curY)||!picat_is_integer(curX) || !picat_is_integer(curY)){
      deletePoly(poly);
      free(vertex);
      return 0;
    } 
    valX = (int) picat_get_integer(curX);
    valY = (int) picat_get_integer(curY);
    vertex->x = valX;
    vertex->y = valY;
    if( i == 0){
      poly->vertices = vertex;
    } else {
      last->next = vertex;
    }
    last = vertex;
    curX = remX;
    curY = remY;
  }
  insertEnd(poly, Polygon);
}



c_arc(){
  TERM x1 	= picat_get_call_arg(1,8);
  TERM y1 	= picat_get_call_arg(2,8);
  TERM x2 	= picat_get_call_arg(3,8);
  TERM y2 	= picat_get_call_arg(4,8);
  TERM color    = picat_get_call_arg(5,8);
  TERM angle1 	= picat_get_call_arg(6,8);
  TERM angle2	= picat_get_call_arg(7,8);
  TERM fill 	= picat_get_call_arg(8,8);
  int ix1 = 0;
  int iy1 = 0;
  int ix2 = 0;
  int iy2 = 0;
  long cAngle1 = 0;
  long cAngle2 = 0;
  char* buffer;
  if(!picat_is_integer(x1) || !picat_is_integer(y1) || !picat_is_integer(x2) || !picat_is_integer(y2) || !picat_is_integer(angle1) || !picat_is_integer(angle2) || !picat_is_integer(fill)){
    return PICAT_FALSE;
  }
  if(!picat_is_atom(color)){
    return PICAT_FALSE;
  }

  ix1 = (int) picat_get_integer(x1);
  ix2 = (int) picat_get_integer(x2);
  iy1 = (int) picat_get_integer(y1);
  iy2 = (int) picat_get_integer(y2);
  cAngle1 = picat_get_integer(angle1);
  cAngle2 = (long) picat_get_integer(angle2);
  long cFill = picat_get_integer(fill);
  buffer = picat_get_atom_name(color);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  assignArc(ix1 + ix2/2, ix2, iy1 - iy2/2, iy2, cAngle1, cAngle2, (int)cFill, Circle); 
  return PICAT_TRUE;
}

c_triangle(){
  TERM x1 	= picat_get_call_arg(1,8);
  TERM y1 	= picat_get_call_arg(2,8);
  TERM x2 	= picat_get_call_arg(3,8);
  TERM y2 	= picat_get_call_arg(4,8);
  TERM x3 	= picat_get_call_arg(5,8);
  TERM y3	= picat_get_call_arg(6,8);
  TERM color	= picat_get_call_arg(7,8);
  TERM fill 	= picat_get_call_arg(8,8);
  int ix1 = 0;
  int ix2 = 0;
  int ix3 = 0;
  int iy1 = 0;
  int iy2 = 0;
  int iy3 = 0;
  char* col;
  if(!picat_is_integer(x1) || !picat_is_integer(y1)|| !picat_is_integer(x2) || !picat_is_integer(y2)||!picat_is_integer(x3) ||!picat_is_integer(y3)||!picat_is_integer(fill)){
    printf("Error: Expected interger value\n");
    return PICAT_FALSE;
  }
  if(!picat_is_atom(color)){
    printf("Error: Expected atom\n");
    return PICAT_FALSE;
  }
  ix1 = (int) picat_get_integer(x1);
  ix2 = (int) picat_get_integer(x2);
  ix3 = (int) picat_get_integer(x3);
  iy1 = (int) picat_get_integer(y1);
  iy2 = (int) picat_get_integer(y2);
  iy3 = (int) picat_get_integer(y3);
  col = picat_get_atom_name(color);
  long cFill = picat_get_integer(fill);
  if(exception == NULL){
    printf("Error: Exception encountered while converting terms\n");
    return PICAT_FALSE;
  }
  int iColor = getColorValue(col);
  assignTri(ix1, iy1, ix2, iy2, ix3, iy3, (int)cFill, Triangle, iColor);
  return PICAT_TRUE;
}



c_oval(){
  TERM x1 = picat_get_call_arg(1,5);
  TERM y1 = picat_get_call_arg(2,5);
  TERM x2 = picat_get_call_arg(3,5);
  TERM y2 = picat_get_call_arg(4,5);
  TERM fill = picat_get_call_arg(5,5);
  int ix1 = 0;
  int ix2 = 0;
  int iy1 = 0;
  int iy2 = 0;
  if(!picat_is_integer(x1)||!picat_is_integer(y1)||!picat_is_integer(x2)||!picat_is_integer(y2) || !picat_is_integer(fill)){
    return PICAT_FALSE;
  }
  ix1 = (int) picat_get_integer(x1);
  ix2 = (int) picat_get_float(x2);
  iy1 = (int) picat_get_float(y1);
  iy2 = (int) picat_get_integer(y2);
  long cFill = picat_get_integer(fill);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  assignArc(ix1 + ix2/2, ix2, iy1 + iy2/2, iy2, 0, 360, (int) cFill, Circle);
  return PICAT_TRUE;
}



c_circle(){
  TERM x1 = picat_get_call_arg(1,4);
  TERM y1 = picat_get_call_arg(2,4);
  TERM x2 = picat_get_call_arg(3,4);
  TERM fill = picat_get_call_arg(4,4);
  int cx = 0;
  int cy = 0;
  int cx2= 0;
  if (!picat_is_integer(x1)||!picat_is_integer(y1) ||!picat_is_integer(x2)|| !picat_is_integer(fill)){
    return PICAT_FALSE;   
  }
  cx  = (int) picat_get_integer(x1);
  cy  = (int) picat_get_integer(y1);
  cx2 = (int) picat_get_integer(x2);
  long cFill = picat_get_integer(fill);
  if(exception == NULL){
    return PICAT_FALSE;
  }
  assignArc((int)cx+ cx2/2,(int) cx2,(int) cy + cx2/2, (int)cx2, 0, 360, (int) cFill, Circle); 
  return PICAT_TRUE;
}

c_line(){
  TERM x1 = picat_get_call_arg(1,5);
  TERM y1 = picat_get_call_arg(2,5);
  TERM x2 = picat_get_call_arg(3,5);
  TERM y2 = picat_get_call_arg(4,5);
  TERM color = picat_get_call_arg(5,5);
  int ix1 = 0;
  int iy1 = 0;
  int ix2 = 0;
  int iy2 = 0;
  char* buffer;
  int iColor;
  if(!picat_is_integer(x1) ||!picat_is_integer(y1) ||!picat_is_integer(x2) ||!picat_is_integer(y2)){
    return PICAT_FALSE;
  }
  if(!picat_is_atom(color)){
    return PICAT_FALSE;
  }
  
  ix1 = (int) picat_get_integer(x1);
  iy1 = (int) picat_get_integer(y1);
  ix2 = (int) picat_get_integer(x2);
  iy2 = (int) picat_get_integer(y2);
  buffer = picat_get_atom_name(color);
  iColor = getColorValue(buffer);
  assignLine(ix1,ix2,iy1, iy2, Line, iColor); 
  return PICAT_TRUE;
}

c_rectangle(){
  TERM xMin = picat_get_call_arg(1,6);
  TERM yMin = picat_get_call_arg(2,6);
  TERM xMax = picat_get_call_arg(3,6);
  TERM yMax = picat_get_call_arg(4,6);
  TERM color = picat_get_call_arg(5,6);
  TERM fill = picat_get_call_arg(6,6);
  int xLow = 0;
  int xHigh = 0;
  int yLow = 0;
  int yHigh = 0;
  char* buffer;
  if(!picat_is_integer(xMin) ||!picat_is_integer(yMin) ||!picat_is_integer(xMax) ||!picat_is_integer(yMax)||!picat_is_integer(fill)){
    printf("Error: Expected an integer value\n");
    return PICAT_FALSE;
  }
  if(!picat_is_atom(color)){
    printf("Error: Expected a string value\n");
    return PICAT_FALSE;
  }
  xLow  = (int) picat_get_integer(xMin);
  xHigh = (int) picat_get_integer(xMax);
  yLow  = (int) picat_get_integer(yMin);
  yHigh = (int) picat_get_integer(yMax);
  long cFill = picat_get_integer(fill);
  buffer = picat_get_atom_name(color);
  if(exception == NULL){
    printf("Error: Exception encountered while converting terms\n");
    return PICAT_FALSE;
  }
  int iColor = getColorValue(buffer);
  assignRect(xLow, xHigh + xLow, yLow, yLow + yHigh, iColor, (int)cFill, 2);
  return PICAT_TRUE;
}


c_star(){
  TERM x = 		picat_get_call_arg(1,10);
  TERM y = 		picat_get_call_arg(2,10);
  TERM w = 		picat_get_call_arg(3,10);
  TERM h = 		picat_get_call_arg(4,10);
  TERM n = 		picat_get_call_arg(5,10);
  TERM centerX = 	picat_get_call_arg(6,10);
  TERM centerY = 	picat_get_call_arg(7,10);
  TERM angle = 		picat_get_call_arg(8,10);
  TERM diameter = 	picat_get_call_arg(9,10);
  TERM fill = 		picat_get_call_arg(10,10);

  if(!picat_is_integer(x) || !picat_is_integer(y) || !picat_is_integer(w) || !picat_is_integer(h) || !picat_is_integer(n) || !picat_is_integer(centerX) ||!picat_is_integer(centerY) || !picat_is_integer(angle) || !picat_is_integer(diameter) || !picat_is_integer(fill))
    return PICAT_FALSE;
  long cx = picat_get_integer(x);
  long cy = picat_get_integer(y);
  long cw = picat_get_integer(w);
  long ch = picat_get_integer(h);
  long cn = picat_get_integer(n);
  long cCenterX = picat_get_integer(centerX);
  long cCenterY = picat_get_integer(centerY);
  long cAngle = picat_get_integer(angle);
  long cDiameter = picat_get_integer(diameter);
  long cFill = picat_get_integer(fill);

  if(exception == NULL){
    return PICAT_FALSE;
  }

  assignStar(cx,cy,cw,ch,cn,cCenterX, cCenterY,cAngle,cDiameter, cFill);
  return PICAT_TRUE;
}


c_polygon(){
  TERM n = picat_get_call_arg(1,4);
  TERM x = picat_get_call_arg(2,4);
  TERM y = picat_get_call_arg(3,4);
  TERM fill = picat_get_call_arg(4,4);
  if(!picat_is_integer(n) || !picat_is_integer(fill)){
    return PICAT_FALSE;
  }
  if(!picat_is_list(x) ||!picat_is_list(y)){
    return PICAT_FALSE;
  }
  int cn = (int) picat_get_integer(n);
  int cFill = (int) picat_get_integer(fill);
  assignPoly(cn, x, y, cFill);
  return PICAT_TRUE;
}

c_canvas()
{
  TERM x = picat_get_call_arg(1,2);
  TERM y = picat_get_call_arg(2,2);

  if(!picat_is_integer(x) || !picat_is_integer(y)){
    return PICAT_FALSE;
  }
  long cX =  picat_get_integer(x) / 1.75;
  long cY =  picat_get_integer(y) / 2.12;

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
  //int *width = malloc(sizeof(int));
  //int *height = malloc(sizeof(int));
  //cdCanvasGetSize(cdcanvas, width, height, NULL, NULL);
  //printf("height %d %d", *width, *height);   
  IupShowXY( dlg, IUP_CENTER, IUP_CENTER );
  IupMainLoop();
  IupClose();  
  return PICAT_TRUE;
}

