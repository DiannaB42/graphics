# graphics

##Hello World Program

```import cg.					1

go =>						2

   Hello = new_text_box(),			3
	
   Hello.text = ‘hello world’,			4
	
   show(Hello).					5```

First we need to import the constraint-based solver which is done with line 1. new_text_box() on line 3 creates a new text object which is assigned to the variable Hello. Initially no text is assigned. The words are set to the string(atom?) in  line 4 by using the text aspect of the text box. To display the object, you must call show() function with the single object you wish to display or multiple objects to be displayed in a list. Objects at the beginning of the list will be drawn first so they may be covered by later objects in the list.
 

The graphical library has the following primitives:
Arc
Circle
Label
Line
Oval
Polygon
Rectangle
Square
Star
Text box
Triangle


Components and Attributes
All objects have some quantity of attributes that describe how they look. All attributes have some type for example X, Y, Width and Height are all integer values(is there a size limitation?) while the fill attribute of circle, rectangle, and triangle only accepts 0 or 1. If a specific attribute’s value is not specified by the program, default values provided by cg will be selected and used.

Attributes can be accessed by using the following notation. An attribute attr of an object O is accessed by O.attr. Attributes may be assigned values by a constraint as in the Hello.text #= ‘Hello World’ of the first example.

All the primitives are based around a shared component which determines some of the basic location attributes of the object. A component has the following parts:

x - the horizontal starting position measured in pixels from the left hand corner
y - the vertical starting position measured in pixels from the top of the canvas
width - the width of the object, so the end of the component is X+Width pixels from the left
height - the height of the object. The end of the component will be at Y+Height pixels below the top of the canvas
color - the color of the object, currently the supported colors are 
dark red, red, orange, yellow, green, blue,purple black, white, silver, gray, and pink.

These attributes of the component are called the base attributes and are stored explicitly. Each object also has several derived attributes which are calculated from the base attributes, for example centerX. Each object has a centerX and when that attribute is accessed the system replaces the attribute with a variable and add a constraint to calculate the variable from the base attributes.

eg 
O.centerX #< 50

Is internally changed in the following way 
O.centerX #= V,
V #= O.x + O.width//2

The different objects may also have additional attributes, such as text having a font attribute which sets the font for that object. Both component attributes and the specific ones for each object can be accessed with dot notation. 

The color attribute can be assigned by:
Hello.color #= red

The default color for objects is black.


## Arc
A portion of a circle
A new arc object is created with

`O = new_arc()`
### Attributes

`fill` - Filled if `1` and unfilled if `0`. Default value of `1`

`startAngle` - the starting angle of the arc in degrees. Accepts integer values between `0` and `360`. The angles lie on a coordinate system with an origin of the center of the arc

`arcAngle` - how far the arc continues in degrees. Accepts integer values between `0` and `360`

An unfilled arc with a `startAngle` of `0` and `arcAngle` of `360` will be a hollow circle.

A filled arc with a `startAngle` of `180` and an `arcAngle` of `360` will be the solid bottom half of a circle

 
## Circle

A graphical object of type circle is created with
`O = new_circle()`

#### Attributes

`fill` - A solid filled-in circle if `1` and a hollow circle if `0`. Default of `1`

Creating a circle also creates an additional constraint that the width and height must be equal

`O.width #= O.height`

If this is not desired there is also the Oval object.

  
  

## Label

Created with

O = new_label()

Must also set the text of the label before showing it.

#### Attributes

`text`- the words to be written. Can either be `O.text #= hello` or `O.text #= ‘hello world’` (test if this works without space in the word)

`font` - the font of the text. Accepted values are `courier`, `helvetica`, `times`, or `system`. Defaults to `system`

`fontSize` - the size of the font. Accepts integer values between `1` and `100`. Defaults to size `12`.

`fontStyle` - the style of the font. Accepts values `bold`, `italic` or `plain`. Defaults to `plain`

 
Label has an internal calculation of the size of the text based on the font, `fontSize` and `fontStyle` attributes.

  

## Line

Created with

`O = new_line()`

#### Attributes

`x1` - the x-coordinate of the starting point. Non-negative integer values accepted

`x2` - the x-coordinate of the ending point. Non-negative integer values accepted

`y1` - the y-coordinate of the starting point. Non-negative integer values accepted

`y2` - the y-coordinate of the starting point. Non-negative integer values accepted

Enforces additional constraints that the `x` of the component must be less than or equal to the values for `x1` and `x2` and the y of the component must be less than or equal to the values of `y1` and `y2`.

Line also has attributes point1 and point2

## Oval

Created with

`O = new_oval()`

#### Attributes

`fill` - A solid filled-in circle if `1` and a hollow circle if `0`. Default of `1`

  

## Polygon

Created with

`O = new_polygon(N)`

where `N` is the number of points in the polygon.

#### Attributes

`fill` - a filled polygon if `1`, a hollow polygon if `0`

`xs` - the x-coordinates of the points in the polygon. Must be in a list of length N

`ys` - the y-coordinates of the points in the polygon. Must be in a list of length N
## Rectangle
A new rectangle object is created with

`O = new_rectangle()`

#### Attributes
`fill` - a solid rectangle if `1`, a hollow rectangle if `0`. Default value of `1`

##Star
A new star object is created with 
`O = new_star(N)`
 Where `N` is the number of points on the star.

####Attributes
`

## Square
Created with

`O = new_square()`

 #### Attributes
`fill` - a solid square if `1`, a hollow square if `0`. Default value of `1`

## Triangle
Created with
`O = new_triangle()`
Attributes
`fill` - a solid triangle if `1`, a hollow triangle if 0. Defaults to `1`

Triangle has additional attributes of point1, point2, and point3, the three points of a triangle.

Example program 
`import cg.
Main =>
	Tri = new_triangle(),
	Sq = new_square(),
	Tri.point1 #= Sq.leftTopPoint,
	Tri.point2 #= Sq.rightTopPoint,
	Tri.point3 #= Sq.leftBottomPoint,

Objects that have been created are shown with a call to show. Show must be sent a single object or a list of objects ordered in the order in which they will be drawn. Show can be called multiple times, each time it will open a window with the objects that have been drawn since the start of the program. To clear objects from the window, you must call clear which will clear the canvas of all objects previously drawn.

It is not necessary to show all the objects that have been created. For example in grid, you may need to create extra components to handle blank spaces in between objects. If you do not wish these displays simply do not add them to the list that is sent to show.

Eg

`Show =[Show1,Show2,Show3,Show4,Show5],
Extra = [Extra1,Extra2,Extra3,Extra4],
foreach (I in Show)
	I = new_rectangle()
end,
foreach (J in Extra)
	J = new_circle()
end,
grid([[Show1, Extra1, Show2],
	[Extra2, Show3, Extra3],
	[Show4, Extra4, Show5]]),
show(Show).`

In this example, only Show1-6 will be drawn and the pattern will look like (Make figure of program) if no other constraints are given.


