Class Radian :Magnitude
| value |
[
        new: x
                value <- <NormalizeRadian (x asFloat) >

|	< arg
		^ value < arg asFloat

|	= arg
		^ value = arg asFloat

|       sin
                ^ <Sin value>

|       cos
                ^ <Cos value>

|       tan
                ^ <Sin value> / <Cos value>

|       asFloat
                ^ value

|       printString
                ^ value asString , ' radians'
]
