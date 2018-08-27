This graphical library for picat allows drawing of objects and uses a constraint solver to find a location that fits into the constraints provided. It is possible to set exact locations and sizes for the objects or to rely on the constraint solver to place the objects around each other using constraints such as `#=`, `#!=`, `#<`, `#>`, `#<=` and `#=>`.

It supports the object types of Arc, Circle, Label, Line, Oval, Polygon, Rectangle, Square, Star, Text box, Triangle. In general the call to create a new object is similar to `new_label()`, except for Polygon and Star which require an integer number to specify the number of points. For example `new_star(5)` creates a five sided star.


All objects have some quantity of attributes that describe their location and shape. All attributes have specific values type they accept. For example `x`, `y`, `width` and `height` all that non-negative integer values. If an attribute’s value is not specified by the program, default values will be selected and used.

Attributes can be accessed by using dot notation. An attribute attr of an object `O` is accessed by `O.attr`.

All the primitives are based around a shared component which determines some of the basic location attributes of the object. A component has the following parts:

`x` - the horizontal starting position measured in pixels from the left hand corner
`y` - the vertical starting position measured in pixels from the top of the canvas
`width` - the width of the object, so the end of the component is X+Width pixels from the left
`height` - the height of the object. The end of the component will be at Y+Height pixels below the top of the canvas
`color` - the color of the object, currently the supported colors are 
dark red, red, orange, salmon, yellow, gold, lime, green, cyan, blue, purple, pink, white, silver, gray and black

Each object type also has some unique attributes. More information about the unique types can be found in the documentation.

##Installation

