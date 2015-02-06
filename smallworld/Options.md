# SmallWorld

A rewrite / alternative to Little Smalltalk, with a VM in Java rather than
C/C++.

## Tim Budd

The original implementation is from 2004, and I have been informed (2015) that
it no longer works on the current version of Java due to ArrayIndexOutOfBound
as a result of some changes to Object.hashCode(). So it's preserved here for
historial purposes, but there are newer options.

## Russell Allen 

Russell updated the original in 2007 to add a traditional workspace, class
browser, and other useful things. In February 2015 he also updated it to work
with current Java implementations.
 
* https://github.com/russellallen/SmallWorld

## Eric Scharff

Another option was done by Eric Scharff. He has made some signficant changes
to serialization, decoupled the GUI, and reformatted and refactored the code.

His apparently also has some classes that Russell's implementation is missing,
so some things like turtle grahics are working in this one.

This requires gradle to build.

* https://github.com/ericscharff/SmallWorld

## Davide Della Casa

Many thanks to Davide for bringing these issues and implementations to my
attention. 
