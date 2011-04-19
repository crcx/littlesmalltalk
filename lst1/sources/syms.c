#include "headers/object.h"
#include "headers/symbol.h"
char x_str[] = {041, 0,   /* ! */
046, 0,   /* & */
050, 0,   /* ( */
051, 0,   /* ) */
052, 0,   /* * */
053, 0,   /* + */
054, 0,   /* , */
055, 0,   /* - */
057, 0,   /* / */
057, 057, 0,   /* // */
074, 0,   /* < */
074, 075, 0,   /* <= */
075, 0,   /* = */
075, 075, 0,   /* == */
076, 0,   /* > */
076, 075, 0,   /* >= */
0100, 0,   /* @ */
0101, 0162, 0162, 0141, 0171, 0,   /* Array */
0101, 0162, 0162, 0141, 0171, 0145, 0144, 0103, 0157, 0154, 0154, 0145, 0143, 0164, 0151, 0157, 0156, 0,   /* ArrayedCollection */
0102, 0114, 0117, 0103, 0113, 0105, 0104, 0,   /* BLOCKED */
0102, 0141, 0147, 0,   /* Bag */
0102, 0154, 0157, 0143, 0153, 0,   /* Block */
0102, 0157, 0157, 0154, 0145, 0141, 0156, 0,   /* Boolean */
0102, 0171, 0164, 0145, 0101, 0162, 0162, 0141, 0171, 0,   /* ByteArray */
0103, 0150, 0141, 0162, 0,   /* Char */
0103, 0154, 0141, 0163, 0163, 0,   /* Class */
0103, 0157, 0154, 0154, 0145, 0143, 0164, 0151, 0157, 0156, 0,   /* Collection */
0103, 0157, 0155, 0160, 0154, 0145, 0170, 0,   /* Complex */
0104, 0151, 0143, 0164, 0151, 0157, 0156, 0141, 0162, 0171, 0,   /* Dictionary */
0106, 0141, 0154, 0163, 0145, 0,   /* False */
0106, 0151, 0154, 0145, 0,   /* File */
0106, 0154, 0157, 0141, 0164, 0,   /* Float */
0111, 0156, 0164, 0145, 0147, 0145, 0162, 0,   /* Integer */
0111, 0156, 0164, 0145, 0162, 0160, 0162, 0145, 0164, 0145, 0162, 0,   /* Interpreter */
0111, 0156, 0164, 0145, 0162, 0166, 0141, 0154, 0,   /* Interval */
0113, 0145, 0171, 0145, 0144, 0103, 0157, 0154, 0154, 0145, 0143, 0164, 0151, 0157, 0156, 0,   /* KeyedCollection */
0114, 0151, 0163, 0164, 0,   /* List */
0114, 0151, 0164, 0164, 0154, 0145, 040, 0123, 0155, 0141, 0154, 0154, 0164, 0141, 0154, 0153, 0,   /* Little Smalltalk */
0115, 0141, 0147, 0156, 0151, 0164, 0165, 0144, 0145, 0,   /* Magnitude */
0115, 0141, 0151, 0156, 0,   /* Main */
0116, 0165, 0155, 0142, 0145, 0162, 0,   /* Number */
0117, 0142, 0152, 0145, 0143, 0164, 0,   /* Object */
0117, 0162, 0144, 0145, 0162, 0145, 0144, 0103, 0157, 0154, 0154, 0145, 0143, 0164, 0151, 0157, 0156, 0,   /* OrderedCollection */
0120, 0157, 0151, 0156, 0164, 0,   /* Point */
0120, 0162, 0157, 0143, 0145, 0163, 0163, 0,   /* Process */
0122, 0105, 0101, 0104, 0131, 0,   /* READY */
0122, 0141, 0144, 0151, 0141, 0156, 0,   /* Radian */
0122, 0141, 0156, 0144, 0157, 0155, 0,   /* Random */
0123, 0125, 0123, 0120, 0105, 0116, 0104, 0105, 0104, 0,   /* SUSPENDED */
0123, 0145, 0155, 0141, 0160, 0150, 0157, 0162, 0145, 0,   /* Semaphore */
0123, 0145, 0161, 0165, 0145, 0156, 0143, 0145, 0141, 0142, 0154, 0145, 0103, 0157, 0154, 0154, 0145, 0143, 0164, 0151, 0157, 0156, 0,   /* SequenceableCollection */
0123, 0145, 0164, 0,   /* Set */
0123, 0155, 0141, 0154, 0154, 0164, 0141, 0154, 0153, 0,   /* Smalltalk */
0123, 0164, 0162, 0151, 0156, 0147, 0,   /* String */
0123, 0171, 0155, 0142, 0157, 0154, 0,   /* Symbol */
0124, 0105, 0122, 0115, 0111, 0116, 0101, 0124, 0105, 0104, 0,   /* TERMINATED */
0124, 0162, 0165, 0145, 0,   /* True */
0125, 0156, 0144, 0145, 0146, 0151, 0156, 0145, 0144, 0117, 0142, 0152, 0145, 0143, 0164, 0,   /* UndefinedObject */
0133, 0,   /* [ */
0134, 0134, 0,   /* \\ */
0134, 0134, 0134, 0134, 0,   /* \\\\ */
0135, 0,   /* ] */
0136, 0,   /* ^ */
0141, 0142, 0163, 0,   /* abs */
0141, 0144, 0144, 072, 0,   /* add: */
0141, 0144, 0144, 072, 0141, 0146, 0164, 0145, 0162, 072, 0,   /* add:after: */
0141, 0144, 0144, 072, 0142, 0145, 0146, 0157, 0162, 0145, 072, 0,   /* add:before: */
0141, 0144, 0144, 072, 0167, 0151, 0164, 0150, 0117, 0143, 0143, 0165, 0162, 0162, 0145, 0156, 0143, 0145, 0163, 072, 0,   /* add:withOccurrences: */
0141, 0144, 0144, 0101, 0154, 0154, 072, 0,   /* addAll: */
0141, 0144, 0144, 0101, 0154, 0154, 0106, 0151, 0162, 0163, 0164, 072, 0,   /* addAllFirst: */
0141, 0144, 0144, 0101, 0154, 0154, 0114, 0141, 0163, 0164, 072, 0,   /* addAllLast: */
0141, 0144, 0144, 0106, 0151, 0162, 0163, 0164, 072, 0,   /* addFirst: */
0141, 0144, 0144, 0114, 0141, 0163, 0164, 072, 0,   /* addLast: */
0141, 0146, 0164, 0145, 0162, 072, 0,   /* after: */
0141, 0154, 0154, 0115, 0141, 0163, 0153, 072, 0,   /* allMask: */
0141, 0156, 0144, 072, 0,   /* and: */
0141, 0156, 0171, 0115, 0141, 0163, 0153, 072, 0,   /* anyMask: */
0141, 0162, 0143, 0103, 0157, 0163, 0,   /* arcCos */
0141, 0162, 0143, 0123, 0151, 0156, 0,   /* arcSin */
0141, 0162, 0143, 0124, 0141, 0156, 0,   /* arcTan */
0141, 0162, 0147, 0145, 0162, 0162, 0157, 0162, 0,   /* argerror */
0141, 0163, 0101, 0162, 0162, 0141, 0171, 0,   /* asArray */
0141, 0163, 0102, 0141, 0147, 0,   /* asBag */
0141, 0163, 0103, 0150, 0141, 0162, 0141, 0143, 0164, 0145, 0162, 0,   /* asCharacter */
0141, 0163, 0104, 0151, 0143, 0164, 0151, 0157, 0156, 0141, 0162, 0171, 0,   /* asDictionary */
0141, 0163, 0106, 0154, 0157, 0141, 0164, 0,   /* asFloat */
0141, 0163, 0106, 0162, 0141, 0143, 0164, 0151, 0157, 0156, 0,   /* asFraction */
0141, 0163, 0111, 0156, 0164, 0145, 0147, 0145, 0162, 0,   /* asInteger */
0141, 0163, 0114, 0151, 0163, 0164, 0,   /* asList */
0141, 0163, 0114, 0157, 0167, 0145, 0162, 0143, 0141, 0163, 0145, 0,   /* asLowercase */
0141, 0163, 0117, 0162, 0144, 0145, 0162, 0145, 0144, 0103, 0157, 0154, 0154, 0145, 0143, 0164, 0151, 0157, 0156, 0,   /* asOrderedCollection */
0141, 0163, 0123, 0145, 0164, 0,   /* asSet */
0141, 0163, 0123, 0164, 0162, 0151, 0156, 0147, 0,   /* asString */
0141, 0163, 0123, 0171, 0155, 0142, 0157, 0154, 0,   /* asSymbol */
0141, 0163, 0125, 0160, 0160, 0145, 0162, 0143, 0141, 0163, 0145, 0,   /* asUppercase */
0141, 0163, 0143, 0151, 0151, 0126, 0141, 0154, 0165, 0145, 0,   /* asciiValue */
0141, 0164, 072, 0,   /* at: */
0141, 0164, 072, 0151, 0146, 0101, 0142, 0163, 0145, 0156, 0164, 072, 0,   /* at:ifAbsent: */
0141, 0164, 072, 0160, 0165, 0164, 072, 0,   /* at:put: */
0141, 0164, 0101, 0154, 0154, 072, 0160, 0165, 0164, 072, 0,   /* atAll:put: */
0141, 0164, 0101, 0154, 0154, 0120, 0165, 0164, 072, 0,   /* atAllPut: */
0142, 0145, 0146, 0157, 0162, 0145, 072, 0,   /* before: */
0142, 0145, 0164, 0167, 0145, 0145, 0156, 072, 0141, 0156, 0144, 072, 0,   /* between:and: */
0142, 0151, 0156, 0141, 0162, 0171, 0104, 0157, 072, 0,   /* binaryDo: */
0142, 0151, 0164, 0101, 0156, 0144, 072, 0,   /* bitAnd: */
0142, 0151, 0164, 0101, 0164, 072, 0,   /* bitAt: */
0142, 0151, 0164, 0111, 0156, 0166, 0145, 0162, 0164, 0,   /* bitInvert */
0142, 0151, 0164, 0117, 0162, 072, 0,   /* bitOr: */
0142, 0151, 0164, 0123, 0150, 0151, 0146, 0164, 072, 0,   /* bitShift: */
0142, 0151, 0164, 0130, 0157, 0162, 072, 0,   /* bitXor: */
0142, 0154, 0157, 0143, 0153, 0,   /* block */
0142, 0154, 0157, 0143, 0153, 0145, 0144, 0120, 0162, 0157, 0143, 0145, 0163, 0163, 0121, 0165, 0145, 0165, 0145, 0,   /* blockedProcessQueue */
0143, 0145, 0151, 0154, 0151, 0156, 0147, 0,   /* ceiling */
0143, 0150, 0145, 0143, 0153, 0102, 0165, 0143, 0153, 0145, 0164, 072, 0,   /* checkBucket: */
0143, 0154, 0141, 0163, 0163, 0,   /* class */
0143, 0154, 0145, 0141, 0156, 0125, 0160, 0,   /* cleanUp */
0143, 0157, 0145, 0162, 0143, 0145, 072, 0,   /* coerce: */
0143, 0157, 0154, 0154, 0145, 0143, 0164, 072, 0,   /* collect: */
0143, 0157, 0155, 0155, 0141, 0156, 0144, 0163, 072, 0,   /* commands: */
0143, 0157, 0155, 0160, 0141, 0162, 0145, 0105, 0162, 0162, 0157, 0162, 0,   /* compareError */
0143, 0157, 0160, 0171, 0,   /* copy */
0143, 0157, 0160, 0171, 0101, 0162, 0147, 0165, 0155, 0145, 0156, 0164, 0163, 072, 0,   /* copyArguments: */
0143, 0157, 0160, 0171, 0101, 0162, 0147, 0165, 0155, 0145, 0156, 0164, 0163, 072, 0164, 0157, 072, 0,   /* copyArguments:to: */
0143, 0157, 0160, 0171, 0106, 0162, 0157, 0155, 072, 0,   /* copyFrom: */
0143, 0157, 0160, 0171, 0106, 0162, 0157, 0155, 072, 0154, 0145, 0156, 0147, 0164, 0150, 072, 0,   /* copyFrom:length: */
0143, 0157, 0160, 0171, 0106, 0162, 0157, 0155, 072, 0164, 0157, 072, 0,   /* copyFrom:to: */
0143, 0157, 0160, 0171, 0127, 0151, 0164, 0150, 072, 0,   /* copyWith: */
0143, 0157, 0160, 0171, 0127, 0151, 0164, 0150, 0157, 0165, 0164, 072, 0,   /* copyWithout: */
0143, 0157, 0163, 0,   /* cos */
0143, 0157, 0165, 0156, 0164, 0,   /* count */
0143, 0165, 0162, 0162, 0101, 0163, 0163, 0157, 0143, 0,   /* currAssoc */
0143, 0165, 0162, 0162, 0102, 0165, 0143, 0153, 0145, 0164, 0,   /* currBucket */
0143, 0165, 0162, 0162, 0145, 0156, 0164, 0,   /* current */
0143, 0165, 0162, 0162, 0145, 0156, 0164, 0102, 0165, 0143, 0153, 0145, 0164, 0,   /* currentBucket */
0143, 0165, 0162, 0162, 0145, 0156, 0164, 0113, 0145, 0171, 0,   /* currentKey */
0143, 0165, 0162, 0162, 0145, 0156, 0164, 0114, 0151, 0163, 0164, 0,   /* currentList */
0144, 0141, 0164, 0145, 0,   /* date */
0144, 0145, 0142, 0165, 0147, 072, 0,   /* debug: */
0144, 0145, 0145, 0160, 0103, 0157, 0160, 0171, 0,   /* deepCopy */
0144, 0145, 0145, 0160, 0103, 0157, 0160, 0171, 072, 0,   /* deepCopy: */
0144, 0145, 0164, 0145, 0143, 0164, 072, 0,   /* detect: */
0144, 0145, 0164, 0145, 0143, 0164, 072, 0151, 0146, 0101, 0142, 0163, 0145, 0156, 0164, 072, 0,   /* detect:ifAbsent: */
0144, 0145, 0164, 0145, 0143, 0164, 072, 0151, 0146, 0116, 0157, 0156, 0145, 072, 0,   /* detect:ifNone: */
0144, 0151, 0143, 0164, 0,   /* dict */
0144, 0151, 0143, 0164, 0151, 0157, 0156, 0141, 0162, 0171, 0,   /* dictionary */
0144, 0151, 0147, 0151, 0164, 0126, 0141, 0154, 0165, 0145, 0,   /* digitValue */
0144, 0151, 0147, 0151, 0164, 0126, 0141, 0154, 0165, 0145, 072, 0,   /* digitValue: */
0144, 0151, 0163, 0160, 0154, 0141, 0171, 0,   /* display */
0144, 0151, 0163, 0160, 0154, 0141, 0171, 0101, 0163, 0163, 0151, 0147, 0156, 0,   /* displayAssign */
0144, 0151, 0163, 0164, 072, 0,   /* dist: */
0144, 0157, 072, 0,   /* do: */
0144, 0157, 0120, 0162, 0151, 0155, 0151, 0164, 0151, 0166, 0145, 072, 0,   /* doPrimitive: */
0144, 0157, 0120, 0162, 0151, 0155, 0151, 0164, 0151, 0166, 0145, 072, 0167, 0151, 0164, 0150, 0101, 0162, 0147, 0165, 0155, 0145, 0156, 0164, 0163, 072, 0,   /* doPrimitive:withArguments: */
0145, 0144, 0151, 0164, 0,   /* edit */
0145, 0161, 0165, 0141, 0154, 0163, 072, 0163, 0164, 0141, 0162, 0164, 0151, 0156, 0147, 0101, 0164, 072, 0,   /* equals:startingAt: */
0145, 0161, 0166, 072, 0,   /* eqv: */
0145, 0162, 0162, 0157, 0162, 072, 0,   /* error: */
0145, 0166, 0145, 0156, 0,   /* even */
0145, 0170, 0143, 0145, 0163, 0163, 0123, 0151, 0147, 0156, 0141, 0154, 0163, 0,   /* excessSignals */
0145, 0170, 0145, 0143, 0165, 0164, 0145, 0127, 0151, 0164, 0150, 072, 0,   /* executeWith: */
0145, 0170, 0160, 0,   /* exp */
0146, 0141, 0143, 0164, 0157, 0162, 0151, 0141, 0154, 0,   /* factorial */
0146, 0151, 0156, 0144, 0101, 0163, 0163, 0157, 0143, 0151, 0141, 0164, 0151, 0157, 0156, 072, 0151, 0156, 0114, 0151, 0163, 0164, 072, 0,   /* findAssociation:inList: */
0146, 0151, 0156, 0144, 0106, 0151, 0162, 0163, 0164, 072, 0,   /* findFirst: */
0146, 0151, 0156, 0144, 0106, 0151, 0162, 0163, 0164, 072, 0151, 0146, 0101, 0142, 0163, 0145, 0156, 0164, 072, 0,   /* findFirst:ifAbsent: */
0146, 0151, 0156, 0144, 0114, 0141, 0163, 0164, 0,   /* findLast */
0146, 0151, 0156, 0144, 0114, 0141, 0163, 0164, 072, 0,   /* findLast: */
0146, 0151, 0156, 0144, 0114, 0141, 0163, 0164, 072, 0151, 0146, 0101, 0142, 0163, 0145, 0156, 0164, 072, 0,   /* findLast:ifAbsent: */
0146, 0151, 0162, 0163, 0164, 0,   /* first */
0146, 0151, 0162, 0163, 0164, 0113, 0145, 0171, 0,   /* firstKey */
0146, 0154, 0157, 0157, 0162, 0,   /* floor */
0146, 0154, 0157, 0157, 0162, 0114, 0157, 0147, 072, 0,   /* floorLog: */
0146, 0157, 0162, 0153, 0,   /* fork */
0146, 0157, 0162, 0153, 0127, 0151, 0164, 0150, 072, 0,   /* forkWith: */
0146, 0162, 0141, 0143, 0164, 0151, 0157, 0156, 0120, 0141, 0162, 0164, 0,   /* fractionPart */
0146, 0162, 0145, 0145, 072, 0,   /* free: */
0146, 0162, 0157, 0155, 072, 0,   /* from: */
0146, 0162, 0157, 0155, 072, 0164, 0157, 072, 0,   /* from:to: */
0146, 0162, 0157, 0155, 072, 0164, 0157, 072, 0142, 0171, 072, 0,   /* from:to:by: */
0147, 0141, 0155, 0155, 0141, 0,   /* gamma */
0147, 0143, 0144, 072, 0,   /* gcd: */
0147, 0145, 0164, 0114, 0151, 0163, 0164, 072, 0,   /* getList: */
0147, 0162, 0151, 0144, 072, 0,   /* grid: */
0150, 0141, 0163, 0150, 0116, 0165, 0155, 0142, 0145, 0162, 072, 0,   /* hashNumber: */
0150, 0141, 0163, 0150, 0124, 0141, 0142, 0,   /* hashTab */
0150, 0141, 0163, 0150, 0124, 0141, 0142, 0154, 0145, 0,   /* hashTable */
0150, 0151, 0147, 0150, 0102, 0151, 0164, 0,   /* highBit */
0151, 0,   /* i */
0151, 0146, 0106, 0141, 0154, 0163, 0145, 072, 0,   /* ifFalse: */
0151, 0146, 0106, 0141, 0154, 0163, 0145, 072, 0151, 0146, 0124, 0162, 0165, 0145, 072, 0,   /* ifFalse:ifTrue: */
0151, 0146, 0124, 0162, 0165, 0145, 072, 0,   /* ifTrue: */
0151, 0146, 0124, 0162, 0165, 0145, 072, 0151, 0146, 0106, 0141, 0154, 0163, 0145, 072, 0,   /* ifTrue:ifFalse: */
0151, 0156, 0122, 0141, 0156, 0147, 0145, 072, 0,   /* inRange: */
0151, 0156, 0143, 0154, 0165, 0144, 0145, 0163, 072, 0,   /* includes: */
0151, 0156, 0143, 0154, 0165, 0144, 0145, 0163, 0113, 0145, 0171, 072, 0,   /* includesKey: */
0151, 0156, 0144, 0145, 0170, 0117, 0146, 072, 0,   /* indexOf: */
0151, 0156, 0144, 0145, 0170, 0117, 0146, 072, 0151, 0146, 0101, 0142, 0163, 0145, 0156, 0164, 072, 0,   /* indexOf:ifAbsent: */
0151, 0156, 0144, 0145, 0170, 0117, 0146, 0123, 0165, 0142, 0103, 0157, 0154, 0154, 0145, 0143, 0164, 0151, 0157, 0156, 072, 0163, 0164, 0141, 0162, 0164, 0151, 0156, 0147, 0101, 0164, 072, 0,   /* indexOfSubCollection:startingAt: */
0151, 0156, 0144, 0145, 0170, 0117, 0146, 0123, 0165, 0142, 0103, 0157, 0154, 0154, 0145, 0143, 0164, 0151, 0157, 0156, 072, 0163, 0164, 0141, 0162, 0164, 0151, 0156, 0147, 0101, 0164, 072, 0151, 0146, 0101, 0142, 0163, 0145, 0156, 0164, 072, 0,   /* indexOfSubCollection:startingAt:ifAbsent: */
0151, 0156, 0151, 0164, 072, 0,   /* init: */
0151, 0156, 0151, 0164, 072, 0163, 0165, 0160, 0145, 0162, 072, 0,   /* init:super: */
0151, 0156, 0151, 0164, 072, 0163, 0165, 0160, 0145, 0162, 072, 0156, 0165, 0155, 0126, 0141, 0162, 0163, 072, 0,   /* init:super:numVars: */
0151, 0156, 0152, 0145, 0143, 0164, 072, 0151, 0156, 0164, 0157, 072, 0,   /* inject:into: */
0151, 0156, 0164, 0145, 0147, 0145, 0162, 0120, 0141, 0162, 0164, 0,   /* integerPart */
0151, 0163, 0101, 0154, 0160, 0150, 0141, 0116, 0165, 0155, 0145, 0162, 0151, 0143, 0,   /* isAlphaNumeric */
0151, 0163, 0104, 0151, 0147, 0151, 0164, 0,   /* isDigit */
0151, 0163, 0105, 0155, 0160, 0164, 0171, 0,   /* isEmpty */
0151, 0163, 0113, 0151, 0156, 0144, 0117, 0146, 072, 0,   /* isKindOf: */
0151, 0163, 0114, 0145, 0164, 0164, 0145, 0162, 0,   /* isLetter */
0151, 0163, 0114, 0157, 0167, 0145, 0162, 0143, 0141, 0163, 0145, 0,   /* isLowercase */
0151, 0163, 0115, 0145, 0155, 0142, 0145, 0162, 0117, 0146, 072, 0,   /* isMemberOf: */
0151, 0163, 0116, 0151, 0154, 0,   /* isNil */
0151, 0163, 0123, 0145, 0160, 0141, 0162, 0141, 0164, 0157, 0162, 0,   /* isSeparator */
0151, 0163, 0125, 0160, 0160, 0145, 0162, 0143, 0141, 0163, 0145, 0,   /* isUppercase */
0151, 0163, 0126, 0157, 0167, 0145, 0154, 0,   /* isVowel */
0153, 0145, 0171, 0163, 0,   /* keys */
0153, 0145, 0171, 0163, 0104, 0157, 072, 0,   /* keysDo: */
0153, 0145, 0171, 0163, 0123, 0145, 0154, 0145, 0143, 0164, 072, 0,   /* keysSelect: */
0154, 0141, 0163, 0164, 0,   /* last */
0154, 0141, 0163, 0164, 0113, 0145, 0171, 0,   /* lastKey */
0154, 0143, 0155, 072, 0,   /* lcm: */
0154, 0151, 0163, 0164, 0,   /* list */
0154, 0156, 0,   /* ln */
0154, 0157, 0147, 072, 0,   /* log: */
0154, 0157, 0167, 0145, 0162, 0,   /* lower */
0155, 0141, 0151, 0156, 0,   /* main */
0155, 0141, 0170, 072, 0,   /* max: */
0155, 0141, 0170, 0103, 0157, 0156, 0164, 0145, 0170, 0164, 072, 0,   /* maxContext: */
0155, 0141, 0170, 0164, 0171, 0160, 0145, 072, 0,   /* maxtype: */
0155, 0145, 0164, 0150, 0157, 0144, 0163, 072, 0,   /* methods: */
0155, 0151, 0156, 072, 0,   /* min: */
0155, 0157, 0144, 0145, 0103, 0150, 0141, 0162, 0141, 0143, 0164, 0145, 0162, 0,   /* modeCharacter */
0155, 0157, 0144, 0145, 0111, 0156, 0164, 0145, 0147, 0145, 0162, 0,   /* modeInteger */
0155, 0157, 0144, 0145, 0123, 0164, 0162, 0151, 0156, 0147, 0,   /* modeString */
0156, 0141, 0155, 0145, 072, 0,   /* name: */
0156, 0145, 0147, 0141, 0164, 0145, 0144, 0,   /* negated */
0156, 0145, 0147, 0141, 0164, 0151, 0166, 0145, 0,   /* negative */
0156, 0145, 0167, 0,   /* new */
0156, 0145, 0167, 072, 0,   /* new: */
0156, 0145, 0167, 0120, 0162, 0157, 0143, 0145, 0163, 0163, 0,   /* newProcess */
0156, 0145, 0167, 0120, 0162, 0157, 0143, 0145, 0163, 0163, 0127, 0151, 0164, 0150, 072, 0,   /* newProcessWith: */
0156, 0145, 0170, 0164, 0,   /* next */
0156, 0145, 0170, 0164, 072, 0,   /* next: */
0156, 0157, 0104, 0151, 0163, 0160, 0154, 0141, 0171, 0,   /* noDisplay */
0156, 0157, 0115, 0141, 0163, 0153, 072, 0,   /* noMask: */
0156, 0157, 0164, 0,   /* not */
0156, 0157, 0164, 0116, 0151, 0154, 0,   /* notNil */
0156, 0157, 0164, 0150, 0151, 0156, 0147, 0,   /* nothing */
0157, 0143, 0143, 0165, 0162, 0162, 0145, 0156, 0143, 0145, 0163, 0117, 0146, 072, 0,   /* occurrencesOf: */
0157, 0144, 0144, 0,   /* odd */
0157, 0160, 0105, 0162, 0162, 0157, 0162, 0,   /* opError */
0157, 0160, 0145, 0156, 072, 0,   /* open: */
0157, 0160, 0145, 0156, 072, 0146, 0157, 0162, 072, 0,   /* open:for: */
0157, 0162, 072, 0,   /* or: */
0160, 0145, 0162, 0146, 0157, 0162, 0155, 072, 0,   /* perform: */
0160, 0145, 0162, 0146, 0157, 0162, 0155, 072, 0167, 0151, 0164, 0150, 0101, 0162, 0147, 0165, 0155, 0145, 0156, 0164, 0163, 072, 0,   /* perform:withArguments: */
0160, 0151, 0,   /* pi */
0160, 0157, 0163, 0151, 0164, 0151, 0166, 0145, 0,   /* positive */
0160, 0162, 0151, 0156, 0164, 0,   /* print */
0160, 0162, 0151, 0156, 0164, 0123, 0164, 0162, 0151, 0156, 0147, 0,   /* printString */
0160, 0165, 0164, 072, 0,   /* put: */
0161, 0165, 0157, 072, 0,   /* quo: */
0162, 0141, 0144, 0151, 0141, 0156, 0163, 0,   /* radians */
0162, 0141, 0144, 0151, 0170, 072, 0,   /* radix: */
0162, 0141, 0151, 0163, 0145, 0144, 0124, 0157, 072, 0,   /* raisedTo: */
0162, 0141, 0151, 0163, 0145, 0144, 0124, 0157, 0111, 0156, 0164, 0145, 0147, 0145, 0162, 072, 0,   /* raisedToInteger: */
0162, 0141, 0156, 0144, 0111, 0156, 0164, 0145, 0147, 0145, 0162, 072, 0,   /* randInteger: */
0162, 0141, 0156, 0144, 0157, 0155, 0151, 0172, 0145, 0,   /* randomize */
0162, 0145, 0141, 0144, 0,   /* read */
0162, 0145, 0143, 0151, 0160, 0162, 0157, 0143, 0141, 0154, 0,   /* reciprocal */
0162, 0145, 0152, 0145, 0143, 0164, 072, 0,   /* reject: */
0162, 0145, 0155, 072, 0,   /* rem: */
0162, 0145, 0155, 0157, 0166, 0145, 072, 0,   /* remove: */
0162, 0145, 0155, 0157, 0166, 0145, 072, 0151, 0146, 0101, 0142, 0163, 0145, 0156, 0164, 072, 0,   /* remove:ifAbsent: */
0162, 0145, 0155, 0157, 0166, 0145, 0101, 0154, 0154, 072, 0,   /* removeAll: */
0162, 0145, 0155, 0157, 0166, 0145, 0105, 0162, 0162, 0157, 0162, 0,   /* removeError */
0162, 0145, 0155, 0157, 0166, 0145, 0106, 0151, 0162, 0163, 0164, 0,   /* removeFirst */
0162, 0145, 0155, 0157, 0166, 0145, 0113, 0145, 0171, 072, 0,   /* removeKey: */
0162, 0145, 0155, 0157, 0166, 0145, 0113, 0145, 0171, 072, 0151, 0146, 0101, 0142, 0163, 0145, 0156, 0164, 072, 0,   /* removeKey:ifAbsent: */
0162, 0145, 0155, 0157, 0166, 0145, 0114, 0141, 0163, 0164, 0,   /* removeLast */
0162, 0145, 0155, 0157, 0166, 0145, 0144, 0,   /* removed */
0162, 0145, 0160, 0154, 0141, 0143, 0145, 0106, 0162, 0157, 0155, 072, 0164, 0157, 072, 0167, 0151, 0164, 0150, 072, 0,   /* replaceFrom:to:with: */
0162, 0145, 0160, 0154, 0141, 0143, 0145, 0106, 0162, 0157, 0155, 072, 0164, 0157, 072, 0167, 0151, 0164, 0150, 072, 0163, 0164, 0141, 0162, 0164, 0151, 0156, 0147, 0101, 0164, 072, 0,   /* replaceFrom:to:with:startingAt: */
0162, 0145, 0163, 0160, 0157, 0156, 0144, 0163, 0124, 0157, 0,   /* respondsTo */
0162, 0145, 0163, 0160, 0157, 0156, 0144, 0163, 0124, 0157, 072, 0,   /* respondsTo: */
0162, 0145, 0163, 0165, 0155, 0145, 0,   /* resume */
0162, 0145, 0166, 0145, 0162, 0163, 0145, 0104, 0157, 072, 0,   /* reverseDo: */
0162, 0145, 0166, 0145, 0162, 0163, 0145, 0144, 0,   /* reversed */
0162, 0157, 0165, 0156, 0144, 0124, 0157, 072, 0,   /* roundTo: */
0162, 0157, 0165, 0156, 0144, 0145, 0144, 0,   /* rounded */
0163, 0141, 0155, 0145, 0101, 0163, 072, 0,   /* sameAs: */
0163, 0145, 0145, 0144, 0,   /* seed */
0163, 0145, 0154, 0145, 0143, 0164, 072, 0,   /* select: */
0163, 0145, 0164, 0103, 0165, 0162, 0162, 0145, 0156, 0164, 0114, 0157, 0143, 0141, 0164, 0151, 0157, 0156, 072, 0,   /* setCurrentLocation: */
0163, 0150, 072, 0,   /* sh: */
0163, 0150, 0141, 0154, 0154, 0157, 0167, 0103, 0157, 0160, 0171, 0,   /* shallowCopy */
0163, 0150, 0141, 0154, 0154, 0157, 0167, 0103, 0157, 0160, 0171, 072, 0,   /* shallowCopy: */
0163, 0151, 0147, 0156, 0,   /* sign */
0163, 0151, 0147, 0156, 0141, 0154, 0,   /* signal */
0163, 0151, 0156, 0,   /* sin */
0163, 0151, 0172, 0145, 0,   /* size */
0163, 0155, 0141, 0154, 0154, 0164, 0141, 0154, 0153, 0,   /* smalltalk */
0163, 0157, 0162, 0164, 0,   /* sort */
0163, 0157, 0162, 0164, 072, 0,   /* sort: */
0163, 0161, 0162, 0164, 0,   /* sqrt */
0163, 0161, 0165, 0141, 0162, 0145, 0144, 0,   /* squared */
0163, 0164, 0141, 0164, 0145, 0,   /* state */
0163, 0164, 0145, 0160, 0,   /* step */
0163, 0164, 0162, 0151, 0143, 0164, 0154, 0171, 0120, 0157, 0163, 0151, 0164, 0151, 0166, 0145, 0,   /* strictlyPositive */
0163, 0165, 0160, 0145, 0162, 0103, 0154, 0141, 0163, 0163, 0,   /* superClass */
0163, 0165, 0160, 0145, 0162, 0103, 0154, 0141, 0163, 0163, 072, 0,   /* superClass: */
0163, 0165, 0163, 0160, 0145, 0156, 0144, 0,   /* suspend */
0164, 0141, 0156, 0,   /* tan */
0164, 0145, 0155, 0160, 0,   /* temp */
0164, 0145, 0162, 0155, 0105, 0162, 0162, 072, 0,   /* termErr: */
0164, 0145, 0162, 0155, 0151, 0156, 0141, 0164, 0145, 0,   /* terminate */
0164, 0151, 0155, 0145, 072, 0,   /* time: */
0164, 0151, 0155, 0145, 0163, 0122, 0145, 0160, 0145, 0141, 0164, 072, 0,   /* timesRepeat: */
0164, 0157, 072, 0,   /* to: */
0164, 0157, 072, 0142, 0171, 072, 0,   /* to:by: */
0164, 0162, 0141, 0156, 0163, 0160, 0157, 0163, 0145, 0,   /* transpose */
0164, 0162, 0165, 0156, 0143, 0141, 0164, 0145, 0124, 0157, 072, 0,   /* truncateTo: */
0164, 0162, 0165, 0156, 0143, 0141, 0164, 0145, 0144, 0,   /* truncated */
0164, 0162, 0165, 0156, 0143, 0141, 0164, 0145, 0144, 0107, 0162, 0151, 0144, 072, 0,   /* truncatedGrid: */
0165, 0156, 0142, 0154, 0157, 0143, 0153, 0,   /* unblock */
0165, 0160, 0160, 0145, 0162, 0,   /* upper */
0166, 0141, 0154, 0165, 0145, 0,   /* value */
0166, 0141, 0154, 0165, 0145, 072, 0,   /* value: */
0166, 0141, 0154, 0165, 0145, 072, 0166, 0141, 0154, 0165, 0145, 072, 0,   /* value:value: */
0166, 0141, 0154, 0165, 0145, 072, 0166, 0141, 0154, 0165, 0145, 072, 0166, 0141, 0154, 0165, 0145, 072, 0,   /* value:value:value: */
0166, 0141, 0154, 0165, 0145, 072, 0166, 0141, 0154, 0165, 0145, 072, 0166, 0141, 0154, 0165, 0145, 072, 0166, 0141, 0154, 0165, 0145, 072, 0,   /* value:value:value:value: */
0166, 0141, 0154, 0165, 0145, 072, 0166, 0141, 0154, 0165, 0145, 072, 0166, 0141, 0154, 0165, 0145, 072, 0166, 0141, 0154, 0165, 0145, 072, 0166, 0141, 0154, 0165, 0145, 072, 0,   /* value:value:value:value:value: */
0166, 0141, 0154, 0165, 0145, 0163, 0,   /* values */
0166, 0141, 0162, 0151, 0141, 0142, 0154, 0145, 0163, 0,   /* variables */
0166, 0141, 0162, 0151, 0141, 0142, 0154, 0145, 0163, 072, 0,   /* variables: */
0166, 0151, 0145, 0167, 0,   /* view */
0167, 0141, 0151, 0164, 0,   /* wait */
0167, 0150, 0151, 0154, 0145, 0106, 0141, 0154, 0163, 0145, 072, 0,   /* whileFalse: */
0167, 0150, 0151, 0154, 0145, 0124, 0162, 0165, 0145, 072, 0,   /* whileTrue: */
0167, 0151, 0164, 0150, 072, 0144, 0157, 072, 0,   /* with:do: */
0167, 0151, 0164, 0150, 0101, 0162, 0147, 0165, 0155, 0145, 0156, 0164, 0163, 072, 0,   /* withArguments: */
0167, 0162, 0151, 0164, 0145, 072, 0,   /* write: */
0170, 0,   /* x */
0170, 072, 0,   /* x: */
0170, 0157, 0162, 072, 0,   /* xor: */
0170, 0166, 0141, 0154, 0165, 0145, 0,   /* xvalue */
0171, 0,   /* y */
0171, 072, 0,   /* y: */
0171, 0151, 0145, 0154, 0144, 0,   /* yield */
0171, 0166, 0141, 0154, 0165, 0145, 0,   /* yvalue */
0174, 0,   /* | */
0176, 0,   /* ~ */
0176, 075, 0,   /* ~= */
0176, 0176, 0,   /* ~~ */
0 };
int x_cmax = 3253;
static symbol x_sytab[] = {
{1, SYMBOLSIZE, &x_str[0]}, /* ! */
{1, SYMBOLSIZE, &x_str[2]}, /* & */
{1, SYMBOLSIZE, &x_str[4]}, /* ( */
{1, SYMBOLSIZE, &x_str[6]}, /* ) */
{1, SYMBOLSIZE, &x_str[8]}, /* * */
{1, SYMBOLSIZE, &x_str[10]}, /* + */
{1, SYMBOLSIZE, &x_str[12]}, /* , */
{1, SYMBOLSIZE, &x_str[14]}, /* - */
{1, SYMBOLSIZE, &x_str[16]}, /* / */
{1, SYMBOLSIZE, &x_str[18]}, /* // */
{1, SYMBOLSIZE, &x_str[21]}, /* < */
{1, SYMBOLSIZE, &x_str[23]}, /* <= */
{1, SYMBOLSIZE, &x_str[26]}, /* = */
{1, SYMBOLSIZE, &x_str[28]}, /* == */
{1, SYMBOLSIZE, &x_str[31]}, /* > */
{1, SYMBOLSIZE, &x_str[33]}, /* >= */
{1, SYMBOLSIZE, &x_str[36]}, /* @ */
{1, SYMBOLSIZE, &x_str[38]}, /* Array */
{1, SYMBOLSIZE, &x_str[44]}, /* ArrayedCollection */
{1, SYMBOLSIZE, &x_str[62]}, /* BLOCKED */
{1, SYMBOLSIZE, &x_str[70]}, /* Bag */
{1, SYMBOLSIZE, &x_str[74]}, /* Block */
{1, SYMBOLSIZE, &x_str[80]}, /* Boolean */
{1, SYMBOLSIZE, &x_str[88]}, /* ByteArray */
{1, SYMBOLSIZE, &x_str[98]}, /* Char */
{1, SYMBOLSIZE, &x_str[103]}, /* Class */
{1, SYMBOLSIZE, &x_str[109]}, /* Collection */
{1, SYMBOLSIZE, &x_str[120]}, /* Complex */
{1, SYMBOLSIZE, &x_str[128]}, /* Dictionary */
{1, SYMBOLSIZE, &x_str[139]}, /* False */
{1, SYMBOLSIZE, &x_str[145]}, /* File */
{1, SYMBOLSIZE, &x_str[150]}, /* Float */
{1, SYMBOLSIZE, &x_str[156]}, /* Integer */
{1, SYMBOLSIZE, &x_str[164]}, /* Interpreter */
{1, SYMBOLSIZE, &x_str[176]}, /* Interval */
{1, SYMBOLSIZE, &x_str[185]}, /* KeyedCollection */
{1, SYMBOLSIZE, &x_str[201]}, /* List */
{1, SYMBOLSIZE, &x_str[206]}, /* Little Smalltalk */
{1, SYMBOLSIZE, &x_str[223]}, /* Magnitude */
{1, SYMBOLSIZE, &x_str[233]}, /* Main */
{1, SYMBOLSIZE, &x_str[238]}, /* Number */
{1, SYMBOLSIZE, &x_str[245]}, /* Object */
{1, SYMBOLSIZE, &x_str[252]}, /* OrderedCollection */
{1, SYMBOLSIZE, &x_str[270]}, /* Point */
{1, SYMBOLSIZE, &x_str[276]}, /* Process */
{1, SYMBOLSIZE, &x_str[284]}, /* READY */
{1, SYMBOLSIZE, &x_str[290]}, /* Radian */
{1, SYMBOLSIZE, &x_str[297]}, /* Random */
{1, SYMBOLSIZE, &x_str[304]}, /* SUSPENDED */
{1, SYMBOLSIZE, &x_str[314]}, /* Semaphore */
{1, SYMBOLSIZE, &x_str[324]}, /* SequenceableCollection */
{1, SYMBOLSIZE, &x_str[347]}, /* Set */
{1, SYMBOLSIZE, &x_str[351]}, /* Smalltalk */
{1, SYMBOLSIZE, &x_str[361]}, /* String */
{1, SYMBOLSIZE, &x_str[368]}, /* Symbol */
{1, SYMBOLSIZE, &x_str[375]}, /* TERMINATED */
{1, SYMBOLSIZE, &x_str[386]}, /* True */
{1, SYMBOLSIZE, &x_str[391]}, /* UndefinedObject */
{1, SYMBOLSIZE, &x_str[407]}, /* [ */
{1, SYMBOLSIZE, &x_str[409]}, /* \\ */
{1, SYMBOLSIZE, &x_str[412]}, /* \\\\ */
{1, SYMBOLSIZE, &x_str[417]}, /* ] */
{1, SYMBOLSIZE, &x_str[419]}, /* ^ */
{1, SYMBOLSIZE, &x_str[421]}, /* abs */
{1, SYMBOLSIZE, &x_str[425]}, /* add: */
{1, SYMBOLSIZE, &x_str[430]}, /* add:after: */
{1, SYMBOLSIZE, &x_str[441]}, /* add:before: */
{1, SYMBOLSIZE, &x_str[453]}, /* add:withOccurrences: */
{1, SYMBOLSIZE, &x_str[474]}, /* addAll: */
{1, SYMBOLSIZE, &x_str[482]}, /* addAllFirst: */
{1, SYMBOLSIZE, &x_str[495]}, /* addAllLast: */
{1, SYMBOLSIZE, &x_str[507]}, /* addFirst: */
{1, SYMBOLSIZE, &x_str[517]}, /* addLast: */
{1, SYMBOLSIZE, &x_str[526]}, /* after: */
{1, SYMBOLSIZE, &x_str[533]}, /* allMask: */
{1, SYMBOLSIZE, &x_str[542]}, /* and: */
{1, SYMBOLSIZE, &x_str[547]}, /* anyMask: */
{1, SYMBOLSIZE, &x_str[556]}, /* arcCos */
{1, SYMBOLSIZE, &x_str[563]}, /* arcSin */
{1, SYMBOLSIZE, &x_str[570]}, /* arcTan */
{1, SYMBOLSIZE, &x_str[577]}, /* argerror */
{1, SYMBOLSIZE, &x_str[586]}, /* asArray */
{1, SYMBOLSIZE, &x_str[594]}, /* asBag */
{1, SYMBOLSIZE, &x_str[600]}, /* asCharacter */
{1, SYMBOLSIZE, &x_str[612]}, /* asDictionary */
{1, SYMBOLSIZE, &x_str[625]}, /* asFloat */
{1, SYMBOLSIZE, &x_str[633]}, /* asFraction */
{1, SYMBOLSIZE, &x_str[644]}, /* asInteger */
{1, SYMBOLSIZE, &x_str[654]}, /* asList */
{1, SYMBOLSIZE, &x_str[661]}, /* asLowercase */
{1, SYMBOLSIZE, &x_str[673]}, /* asOrderedCollection */
{1, SYMBOLSIZE, &x_str[693]}, /* asSet */
{1, SYMBOLSIZE, &x_str[699]}, /* asString */
{1, SYMBOLSIZE, &x_str[708]}, /* asSymbol */
{1, SYMBOLSIZE, &x_str[717]}, /* asUppercase */
{1, SYMBOLSIZE, &x_str[729]}, /* asciiValue */
{1, SYMBOLSIZE, &x_str[740]}, /* at: */
{1, SYMBOLSIZE, &x_str[744]}, /* at:ifAbsent: */
{1, SYMBOLSIZE, &x_str[757]}, /* at:put: */
{1, SYMBOLSIZE, &x_str[765]}, /* atAll:put: */
{1, SYMBOLSIZE, &x_str[776]}, /* atAllPut: */
{1, SYMBOLSIZE, &x_str[786]}, /* before: */
{1, SYMBOLSIZE, &x_str[794]}, /* between:and: */
{1, SYMBOLSIZE, &x_str[807]}, /* binaryDo: */
{1, SYMBOLSIZE, &x_str[817]}, /* bitAnd: */
{1, SYMBOLSIZE, &x_str[825]}, /* bitAt: */
{1, SYMBOLSIZE, &x_str[832]}, /* bitInvert */
{1, SYMBOLSIZE, &x_str[842]}, /* bitOr: */
{1, SYMBOLSIZE, &x_str[849]}, /* bitShift: */
{1, SYMBOLSIZE, &x_str[859]}, /* bitXor: */
{1, SYMBOLSIZE, &x_str[867]}, /* block */
{1, SYMBOLSIZE, &x_str[873]}, /* blockedProcessQueue */
{1, SYMBOLSIZE, &x_str[893]}, /* ceiling */
{1, SYMBOLSIZE, &x_str[901]}, /* checkBucket: */
{1, SYMBOLSIZE, &x_str[914]}, /* class */
{1, SYMBOLSIZE, &x_str[920]}, /* cleanUp */
{1, SYMBOLSIZE, &x_str[928]}, /* coerce: */
{1, SYMBOLSIZE, &x_str[936]}, /* collect: */
{1, SYMBOLSIZE, &x_str[945]}, /* commands: */
{1, SYMBOLSIZE, &x_str[955]}, /* compareError */
{1, SYMBOLSIZE, &x_str[968]}, /* copy */
{1, SYMBOLSIZE, &x_str[973]}, /* copyArguments: */
{1, SYMBOLSIZE, &x_str[988]}, /* copyArguments:to: */
{1, SYMBOLSIZE, &x_str[1006]}, /* copyFrom: */
{1, SYMBOLSIZE, &x_str[1016]}, /* copyFrom:length: */
{1, SYMBOLSIZE, &x_str[1033]}, /* copyFrom:to: */
{1, SYMBOLSIZE, &x_str[1046]}, /* copyWith: */
{1, SYMBOLSIZE, &x_str[1056]}, /* copyWithout: */
{1, SYMBOLSIZE, &x_str[1069]}, /* cos */
{1, SYMBOLSIZE, &x_str[1073]}, /* count */
{1, SYMBOLSIZE, &x_str[1079]}, /* currAssoc */
{1, SYMBOLSIZE, &x_str[1089]}, /* currBucket */
{1, SYMBOLSIZE, &x_str[1100]}, /* current */
{1, SYMBOLSIZE, &x_str[1108]}, /* currentBucket */
{1, SYMBOLSIZE, &x_str[1122]}, /* currentKey */
{1, SYMBOLSIZE, &x_str[1133]}, /* currentList */
{1, SYMBOLSIZE, &x_str[1145]}, /* date */
{1, SYMBOLSIZE, &x_str[1150]}, /* debug: */
{1, SYMBOLSIZE, &x_str[1157]}, /* deepCopy */
{1, SYMBOLSIZE, &x_str[1166]}, /* deepCopy: */
{1, SYMBOLSIZE, &x_str[1176]}, /* detect: */
{1, SYMBOLSIZE, &x_str[1184]}, /* detect:ifAbsent: */
{1, SYMBOLSIZE, &x_str[1201]}, /* detect:ifNone: */
{1, SYMBOLSIZE, &x_str[1216]}, /* dict */
{1, SYMBOLSIZE, &x_str[1221]}, /* dictionary */
{1, SYMBOLSIZE, &x_str[1232]}, /* digitValue */
{1, SYMBOLSIZE, &x_str[1243]}, /* digitValue: */
{1, SYMBOLSIZE, &x_str[1255]}, /* display */
{1, SYMBOLSIZE, &x_str[1263]}, /* displayAssign */
{1, SYMBOLSIZE, &x_str[1277]}, /* dist: */
{1, SYMBOLSIZE, &x_str[1283]}, /* do: */
{1, SYMBOLSIZE, &x_str[1287]}, /* doPrimitive: */
{1, SYMBOLSIZE, &x_str[1300]}, /* doPrimitive:withArguments: */
{1, SYMBOLSIZE, &x_str[1327]}, /* edit */
{1, SYMBOLSIZE, &x_str[1332]}, /* equals:startingAt: */
{1, SYMBOLSIZE, &x_str[1351]}, /* eqv: */
{1, SYMBOLSIZE, &x_str[1356]}, /* error: */
{1, SYMBOLSIZE, &x_str[1363]}, /* even */
{1, SYMBOLSIZE, &x_str[1368]}, /* excessSignals */
{1, SYMBOLSIZE, &x_str[1382]}, /* executeWith: */
{1, SYMBOLSIZE, &x_str[1395]}, /* exp */
{1, SYMBOLSIZE, &x_str[1399]}, /* factorial */
{1, SYMBOLSIZE, &x_str[1409]}, /* findAssociation:inList: */
{1, SYMBOLSIZE, &x_str[1433]}, /* findFirst: */
{1, SYMBOLSIZE, &x_str[1444]}, /* findFirst:ifAbsent: */
{1, SYMBOLSIZE, &x_str[1464]}, /* findLast */
{1, SYMBOLSIZE, &x_str[1473]}, /* findLast: */
{1, SYMBOLSIZE, &x_str[1483]}, /* findLast:ifAbsent: */
{1, SYMBOLSIZE, &x_str[1502]}, /* first */
{1, SYMBOLSIZE, &x_str[1508]}, /* firstKey */
{1, SYMBOLSIZE, &x_str[1517]}, /* floor */
{1, SYMBOLSIZE, &x_str[1523]}, /* floorLog: */
{1, SYMBOLSIZE, &x_str[1533]}, /* fork */
{1, SYMBOLSIZE, &x_str[1538]}, /* forkWith: */
{1, SYMBOLSIZE, &x_str[1548]}, /* fractionPart */
{1, SYMBOLSIZE, &x_str[1561]}, /* free: */
{1, SYMBOLSIZE, &x_str[1567]}, /* from: */
{1, SYMBOLSIZE, &x_str[1573]}, /* from:to: */
{1, SYMBOLSIZE, &x_str[1582]}, /* from:to:by: */
{1, SYMBOLSIZE, &x_str[1594]}, /* gamma */
{1, SYMBOLSIZE, &x_str[1600]}, /* gcd: */
{1, SYMBOLSIZE, &x_str[1605]}, /* getList: */
{1, SYMBOLSIZE, &x_str[1614]}, /* grid: */
{1, SYMBOLSIZE, &x_str[1620]}, /* hashNumber: */
{1, SYMBOLSIZE, &x_str[1632]}, /* hashTab */
{1, SYMBOLSIZE, &x_str[1640]}, /* hashTable */
{1, SYMBOLSIZE, &x_str[1650]}, /* highBit */
{1, SYMBOLSIZE, &x_str[1658]}, /* i */
{1, SYMBOLSIZE, &x_str[1660]}, /* ifFalse: */
{1, SYMBOLSIZE, &x_str[1669]}, /* ifFalse:ifTrue: */
{1, SYMBOLSIZE, &x_str[1685]}, /* ifTrue: */
{1, SYMBOLSIZE, &x_str[1693]}, /* ifTrue:ifFalse: */
{1, SYMBOLSIZE, &x_str[1709]}, /* inRange: */
{1, SYMBOLSIZE, &x_str[1718]}, /* includes: */
{1, SYMBOLSIZE, &x_str[1728]}, /* includesKey: */
{1, SYMBOLSIZE, &x_str[1741]}, /* indexOf: */
{1, SYMBOLSIZE, &x_str[1750]}, /* indexOf:ifAbsent: */
{1, SYMBOLSIZE, &x_str[1768]}, /* indexOfSubCollection:startingAt: */
{1, SYMBOLSIZE, &x_str[1801]}, /* indexOfSubCollection:startingAt:ifAbsent: */
{1, SYMBOLSIZE, &x_str[1843]}, /* init: */
{1, SYMBOLSIZE, &x_str[1849]}, /* init:super: */
{1, SYMBOLSIZE, &x_str[1861]}, /* init:super:numVars: */
{1, SYMBOLSIZE, &x_str[1881]}, /* inject:into: */
{1, SYMBOLSIZE, &x_str[1894]}, /* integerPart */
{1, SYMBOLSIZE, &x_str[1906]}, /* isAlphaNumeric */
{1, SYMBOLSIZE, &x_str[1921]}, /* isDigit */
{1, SYMBOLSIZE, &x_str[1929]}, /* isEmpty */
{1, SYMBOLSIZE, &x_str[1937]}, /* isKindOf: */
{1, SYMBOLSIZE, &x_str[1947]}, /* isLetter */
{1, SYMBOLSIZE, &x_str[1956]}, /* isLowercase */
{1, SYMBOLSIZE, &x_str[1968]}, /* isMemberOf: */
{1, SYMBOLSIZE, &x_str[1980]}, /* isNil */
{1, SYMBOLSIZE, &x_str[1986]}, /* isSeparator */
{1, SYMBOLSIZE, &x_str[1998]}, /* isUppercase */
{1, SYMBOLSIZE, &x_str[2010]}, /* isVowel */
{1, SYMBOLSIZE, &x_str[2018]}, /* keys */
{1, SYMBOLSIZE, &x_str[2023]}, /* keysDo: */
{1, SYMBOLSIZE, &x_str[2031]}, /* keysSelect: */
{1, SYMBOLSIZE, &x_str[2043]}, /* last */
{1, SYMBOLSIZE, &x_str[2048]}, /* lastKey */
{1, SYMBOLSIZE, &x_str[2056]}, /* lcm: */
{1, SYMBOLSIZE, &x_str[2061]}, /* list */
{1, SYMBOLSIZE, &x_str[2066]}, /* ln */
{1, SYMBOLSIZE, &x_str[2069]}, /* log: */
{1, SYMBOLSIZE, &x_str[2074]}, /* lower */
{1, SYMBOLSIZE, &x_str[2080]}, /* main */
{1, SYMBOLSIZE, &x_str[2085]}, /* max: */
{1, SYMBOLSIZE, &x_str[2090]}, /* maxContext: */
{1, SYMBOLSIZE, &x_str[2102]}, /* maxtype: */
{1, SYMBOLSIZE, &x_str[2111]}, /* methods: */
{1, SYMBOLSIZE, &x_str[2120]}, /* min: */
{1, SYMBOLSIZE, &x_str[2125]}, /* modeCharacter */
{1, SYMBOLSIZE, &x_str[2139]}, /* modeInteger */
{1, SYMBOLSIZE, &x_str[2151]}, /* modeString */
{1, SYMBOLSIZE, &x_str[2162]}, /* name: */
{1, SYMBOLSIZE, &x_str[2168]}, /* negated */
{1, SYMBOLSIZE, &x_str[2176]}, /* negative */
{1, SYMBOLSIZE, &x_str[2185]}, /* new */
{1, SYMBOLSIZE, &x_str[2189]}, /* new: */
{1, SYMBOLSIZE, &x_str[2194]}, /* newProcess */
{1, SYMBOLSIZE, &x_str[2205]}, /* newProcessWith: */
{1, SYMBOLSIZE, &x_str[2221]}, /* next */
{1, SYMBOLSIZE, &x_str[2226]}, /* next: */
{1, SYMBOLSIZE, &x_str[2232]}, /* noDisplay */
{1, SYMBOLSIZE, &x_str[2242]}, /* noMask: */
{1, SYMBOLSIZE, &x_str[2250]}, /* not */
{1, SYMBOLSIZE, &x_str[2254]}, /* notNil */
{1, SYMBOLSIZE, &x_str[2261]}, /* nothing */
{1, SYMBOLSIZE, &x_str[2269]}, /* occurrencesOf: */
{1, SYMBOLSIZE, &x_str[2284]}, /* odd */
{1, SYMBOLSIZE, &x_str[2288]}, /* opError */
{1, SYMBOLSIZE, &x_str[2296]}, /* open: */
{1, SYMBOLSIZE, &x_str[2302]}, /* open:for: */
{1, SYMBOLSIZE, &x_str[2312]}, /* or: */
{1, SYMBOLSIZE, &x_str[2316]}, /* perform: */
{1, SYMBOLSIZE, &x_str[2325]}, /* perform:withArguments: */
{1, SYMBOLSIZE, &x_str[2348]}, /* pi */
{1, SYMBOLSIZE, &x_str[2351]}, /* positive */
{1, SYMBOLSIZE, &x_str[2360]}, /* print */
{1, SYMBOLSIZE, &x_str[2366]}, /* printString */
{1, SYMBOLSIZE, &x_str[2378]}, /* put: */
{1, SYMBOLSIZE, &x_str[2383]}, /* quo: */
{1, SYMBOLSIZE, &x_str[2388]}, /* radians */
{1, SYMBOLSIZE, &x_str[2396]}, /* radix: */
{1, SYMBOLSIZE, &x_str[2403]}, /* raisedTo: */
{1, SYMBOLSIZE, &x_str[2413]}, /* raisedToInteger: */
{1, SYMBOLSIZE, &x_str[2430]}, /* randInteger: */
{1, SYMBOLSIZE, &x_str[2443]}, /* randomize */
{1, SYMBOLSIZE, &x_str[2453]}, /* read */
{1, SYMBOLSIZE, &x_str[2458]}, /* reciprocal */
{1, SYMBOLSIZE, &x_str[2469]}, /* reject: */
{1, SYMBOLSIZE, &x_str[2477]}, /* rem: */
{1, SYMBOLSIZE, &x_str[2482]}, /* remove: */
{1, SYMBOLSIZE, &x_str[2490]}, /* remove:ifAbsent: */
{1, SYMBOLSIZE, &x_str[2507]}, /* removeAll: */
{1, SYMBOLSIZE, &x_str[2518]}, /* removeError */
{1, SYMBOLSIZE, &x_str[2530]}, /* removeFirst */
{1, SYMBOLSIZE, &x_str[2542]}, /* removeKey: */
{1, SYMBOLSIZE, &x_str[2553]}, /* removeKey:ifAbsent: */
{1, SYMBOLSIZE, &x_str[2573]}, /* removeLast */
{1, SYMBOLSIZE, &x_str[2584]}, /* removed */
{1, SYMBOLSIZE, &x_str[2592]}, /* replaceFrom:to:with: */
{1, SYMBOLSIZE, &x_str[2613]}, /* replaceFrom:to:with:startingAt: */
{1, SYMBOLSIZE, &x_str[2645]}, /* respondsTo */
{1, SYMBOLSIZE, &x_str[2656]}, /* respondsTo: */
{1, SYMBOLSIZE, &x_str[2668]}, /* resume */
{1, SYMBOLSIZE, &x_str[2675]}, /* reverseDo: */
{1, SYMBOLSIZE, &x_str[2686]}, /* reversed */
{1, SYMBOLSIZE, &x_str[2695]}, /* roundTo: */
{1, SYMBOLSIZE, &x_str[2704]}, /* rounded */
{1, SYMBOLSIZE, &x_str[2712]}, /* sameAs: */
{1, SYMBOLSIZE, &x_str[2720]}, /* seed */
{1, SYMBOLSIZE, &x_str[2725]}, /* select: */
{1, SYMBOLSIZE, &x_str[2733]}, /* setCurrentLocation: */
{1, SYMBOLSIZE, &x_str[2753]}, /* sh: */
{1, SYMBOLSIZE, &x_str[2757]}, /* shallowCopy */
{1, SYMBOLSIZE, &x_str[2769]}, /* shallowCopy: */
{1, SYMBOLSIZE, &x_str[2782]}, /* sign */
{1, SYMBOLSIZE, &x_str[2787]}, /* signal */
{1, SYMBOLSIZE, &x_str[2794]}, /* sin */
{1, SYMBOLSIZE, &x_str[2798]}, /* size */
{1, SYMBOLSIZE, &x_str[2803]}, /* smalltalk */
{1, SYMBOLSIZE, &x_str[2813]}, /* sort */
{1, SYMBOLSIZE, &x_str[2818]}, /* sort: */
{1, SYMBOLSIZE, &x_str[2824]}, /* sqrt */
{1, SYMBOLSIZE, &x_str[2829]}, /* squared */
{1, SYMBOLSIZE, &x_str[2837]}, /* state */
{1, SYMBOLSIZE, &x_str[2843]}, /* step */
{1, SYMBOLSIZE, &x_str[2848]}, /* strictlyPositive */
{1, SYMBOLSIZE, &x_str[2865]}, /* superClass */
{1, SYMBOLSIZE, &x_str[2876]}, /* superClass: */
{1, SYMBOLSIZE, &x_str[2888]}, /* suspend */
{1, SYMBOLSIZE, &x_str[2896]}, /* tan */
{1, SYMBOLSIZE, &x_str[2900]}, /* temp */
{1, SYMBOLSIZE, &x_str[2905]}, /* termErr: */
{1, SYMBOLSIZE, &x_str[2914]}, /* terminate */
{1, SYMBOLSIZE, &x_str[2924]}, /* time: */
{1, SYMBOLSIZE, &x_str[2930]}, /* timesRepeat: */
{1, SYMBOLSIZE, &x_str[2943]}, /* to: */
{1, SYMBOLSIZE, &x_str[2947]}, /* to:by: */
{1, SYMBOLSIZE, &x_str[2954]}, /* transpose */
{1, SYMBOLSIZE, &x_str[2964]}, /* truncateTo: */
{1, SYMBOLSIZE, &x_str[2976]}, /* truncated */
{1, SYMBOLSIZE, &x_str[2986]}, /* truncatedGrid: */
{1, SYMBOLSIZE, &x_str[3001]}, /* unblock */
{1, SYMBOLSIZE, &x_str[3009]}, /* upper */
{1, SYMBOLSIZE, &x_str[3015]}, /* value */
{1, SYMBOLSIZE, &x_str[3021]}, /* value: */
{1, SYMBOLSIZE, &x_str[3028]}, /* value:value: */
{1, SYMBOLSIZE, &x_str[3041]}, /* value:value:value: */
{1, SYMBOLSIZE, &x_str[3060]}, /* value:value:value:value: */
{1, SYMBOLSIZE, &x_str[3085]}, /* value:value:value:value:value: */
{1, SYMBOLSIZE, &x_str[3116]}, /* values */
{1, SYMBOLSIZE, &x_str[3123]}, /* variables */
{1, SYMBOLSIZE, &x_str[3133]}, /* variables: */
{1, SYMBOLSIZE, &x_str[3144]}, /* view */
{1, SYMBOLSIZE, &x_str[3149]}, /* wait */
{1, SYMBOLSIZE, &x_str[3154]}, /* whileFalse: */
{1, SYMBOLSIZE, &x_str[3166]}, /* whileTrue: */
{1, SYMBOLSIZE, &x_str[3177]}, /* with:do: */
{1, SYMBOLSIZE, &x_str[3186]}, /* withArguments: */
{1, SYMBOLSIZE, &x_str[3201]}, /* write: */
{1, SYMBOLSIZE, &x_str[3208]}, /* x */
{1, SYMBOLSIZE, &x_str[3210]}, /* x: */
{1, SYMBOLSIZE, &x_str[3213]}, /* xor: */
{1, SYMBOLSIZE, &x_str[3218]}, /* xvalue */
{1, SYMBOLSIZE, &x_str[3225]}, /* y */
{1, SYMBOLSIZE, &x_str[3227]}, /* y: */
{1, SYMBOLSIZE, &x_str[3230]}, /* yield */
{1, SYMBOLSIZE, &x_str[3236]}, /* yvalue */
{1, SYMBOLSIZE, &x_str[3243]}, /* | */
{1, SYMBOLSIZE, &x_str[3245]}, /* ~ */
{1, SYMBOLSIZE, &x_str[3247]}, /* ~= */
{1, SYMBOLSIZE, &x_str[3250]}, /* ~~ */
0};
symbol *x_tab[SYMTABMAX] = {
&x_sytab[0], /* ! */
&x_sytab[1], /* & */
&x_sytab[2], /* ( */
&x_sytab[3], /* ) */
&x_sytab[4], /* * */
&x_sytab[5], /* + */
&x_sytab[6], /* , */
&x_sytab[7], /* - */
&x_sytab[8], /* / */
&x_sytab[9], /* // */
&x_sytab[10], /* < */
&x_sytab[11], /* <= */
&x_sytab[12], /* = */
&x_sytab[13], /* == */
&x_sytab[14], /* > */
&x_sytab[15], /* >= */
&x_sytab[16], /* @ */
&x_sytab[17], /* Array */
&x_sytab[18], /* ArrayedCollection */
&x_sytab[19], /* BLOCKED */
&x_sytab[20], /* Bag */
&x_sytab[21], /* Block */
&x_sytab[22], /* Boolean */
&x_sytab[23], /* ByteArray */
&x_sytab[24], /* Char */
&x_sytab[25], /* Class */
&x_sytab[26], /* Collection */
&x_sytab[27], /* Complex */
&x_sytab[28], /* Dictionary */
&x_sytab[29], /* False */
&x_sytab[30], /* File */
&x_sytab[31], /* Float */
&x_sytab[32], /* Integer */
&x_sytab[33], /* Interpreter */
&x_sytab[34], /* Interval */
&x_sytab[35], /* KeyedCollection */
&x_sytab[36], /* List */
&x_sytab[37], /* Little Smalltalk */
&x_sytab[38], /* Magnitude */
&x_sytab[39], /* Main */
&x_sytab[40], /* Number */
&x_sytab[41], /* Object */
&x_sytab[42], /* OrderedCollection */
&x_sytab[43], /* Point */
&x_sytab[44], /* Process */
&x_sytab[45], /* READY */
&x_sytab[46], /* Radian */
&x_sytab[47], /* Random */
&x_sytab[48], /* SUSPENDED */
&x_sytab[49], /* Semaphore */
&x_sytab[50], /* SequenceableCollection */
&x_sytab[51], /* Set */
&x_sytab[52], /* Smalltalk */
&x_sytab[53], /* String */
&x_sytab[54], /* Symbol */
&x_sytab[55], /* TERMINATED */
&x_sytab[56], /* True */
&x_sytab[57], /* UndefinedObject */
&x_sytab[58], /* [ */
&x_sytab[59], /* \\ */
&x_sytab[60], /* \\\\ */
&x_sytab[61], /* ] */
&x_sytab[62], /* ^ */
&x_sytab[63], /* abs */
&x_sytab[64], /* add: */
&x_sytab[65], /* add:after: */
&x_sytab[66], /* add:before: */
&x_sytab[67], /* add:withOccurrences: */
&x_sytab[68], /* addAll: */
&x_sytab[69], /* addAllFirst: */
&x_sytab[70], /* addAllLast: */
&x_sytab[71], /* addFirst: */
&x_sytab[72], /* addLast: */
&x_sytab[73], /* after: */
&x_sytab[74], /* allMask: */
&x_sytab[75], /* and: */
&x_sytab[76], /* anyMask: */
&x_sytab[77], /* arcCos */
&x_sytab[78], /* arcSin */
&x_sytab[79], /* arcTan */
&x_sytab[80], /* argerror */
&x_sytab[81], /* asArray */
&x_sytab[82], /* asBag */
&x_sytab[83], /* asCharacter */
&x_sytab[84], /* asDictionary */
&x_sytab[85], /* asFloat */
&x_sytab[86], /* asFraction */
&x_sytab[87], /* asInteger */
&x_sytab[88], /* asList */
&x_sytab[89], /* asLowercase */
&x_sytab[90], /* asOrderedCollection */
&x_sytab[91], /* asSet */
&x_sytab[92], /* asString */
&x_sytab[93], /* asSymbol */
&x_sytab[94], /* asUppercase */
&x_sytab[95], /* asciiValue */
&x_sytab[96], /* at: */
&x_sytab[97], /* at:ifAbsent: */
&x_sytab[98], /* at:put: */
&x_sytab[99], /* atAll:put: */
&x_sytab[100], /* atAllPut: */
&x_sytab[101], /* before: */
&x_sytab[102], /* between:and: */
&x_sytab[103], /* binaryDo: */
&x_sytab[104], /* bitAnd: */
&x_sytab[105], /* bitAt: */
&x_sytab[106], /* bitInvert */
&x_sytab[107], /* bitOr: */
&x_sytab[108], /* bitShift: */
&x_sytab[109], /* bitXor: */
&x_sytab[110], /* block */
&x_sytab[111], /* blockedProcessQueue */
&x_sytab[112], /* ceiling */
&x_sytab[113], /* checkBucket: */
&x_sytab[114], /* class */
&x_sytab[115], /* cleanUp */
&x_sytab[116], /* coerce: */
&x_sytab[117], /* collect: */
&x_sytab[118], /* commands: */
&x_sytab[119], /* compareError */
&x_sytab[120], /* copy */
&x_sytab[121], /* copyArguments: */
&x_sytab[122], /* copyArguments:to: */
&x_sytab[123], /* copyFrom: */
&x_sytab[124], /* copyFrom:length: */
&x_sytab[125], /* copyFrom:to: */
&x_sytab[126], /* copyWith: */
&x_sytab[127], /* copyWithout: */
&x_sytab[128], /* cos */
&x_sytab[129], /* count */
&x_sytab[130], /* currAssoc */
&x_sytab[131], /* currBucket */
&x_sytab[132], /* current */
&x_sytab[133], /* currentBucket */
&x_sytab[134], /* currentKey */
&x_sytab[135], /* currentList */
&x_sytab[136], /* date */
&x_sytab[137], /* debug: */
&x_sytab[138], /* deepCopy */
&x_sytab[139], /* deepCopy: */
&x_sytab[140], /* detect: */
&x_sytab[141], /* detect:ifAbsent: */
&x_sytab[142], /* detect:ifNone: */
&x_sytab[143], /* dict */
&x_sytab[144], /* dictionary */
&x_sytab[145], /* digitValue */
&x_sytab[146], /* digitValue: */
&x_sytab[147], /* display */
&x_sytab[148], /* displayAssign */
&x_sytab[149], /* dist: */
&x_sytab[150], /* do: */
&x_sytab[151], /* doPrimitive: */
&x_sytab[152], /* doPrimitive:withArguments: */
&x_sytab[153], /* edit */
&x_sytab[154], /* equals:startingAt: */
&x_sytab[155], /* eqv: */
&x_sytab[156], /* error: */
&x_sytab[157], /* even */
&x_sytab[158], /* excessSignals */
&x_sytab[159], /* executeWith: */
&x_sytab[160], /* exp */
&x_sytab[161], /* factorial */
&x_sytab[162], /* findAssociation:inList: */
&x_sytab[163], /* findFirst: */
&x_sytab[164], /* findFirst:ifAbsent: */
&x_sytab[165], /* findLast */
&x_sytab[166], /* findLast: */
&x_sytab[167], /* findLast:ifAbsent: */
&x_sytab[168], /* first */
&x_sytab[169], /* firstKey */
&x_sytab[170], /* floor */
&x_sytab[171], /* floorLog: */
&x_sytab[172], /* fork */
&x_sytab[173], /* forkWith: */
&x_sytab[174], /* fractionPart */
&x_sytab[175], /* free: */
&x_sytab[176], /* from: */
&x_sytab[177], /* from:to: */
&x_sytab[178], /* from:to:by: */
&x_sytab[179], /* gamma */
&x_sytab[180], /* gcd: */
&x_sytab[181], /* getList: */
&x_sytab[182], /* grid: */
&x_sytab[183], /* hashNumber: */
&x_sytab[184], /* hashTab */
&x_sytab[185], /* hashTable */
&x_sytab[186], /* highBit */
&x_sytab[187], /* i */
&x_sytab[188], /* ifFalse: */
&x_sytab[189], /* ifFalse:ifTrue: */
&x_sytab[190], /* ifTrue: */
&x_sytab[191], /* ifTrue:ifFalse: */
&x_sytab[192], /* inRange: */
&x_sytab[193], /* includes: */
&x_sytab[194], /* includesKey: */
&x_sytab[195], /* indexOf: */
&x_sytab[196], /* indexOf:ifAbsent: */
&x_sytab[197], /* indexOfSubCollection:startingAt: */
&x_sytab[198], /* indexOfSubCollection:startingAt:ifAbsent: */
&x_sytab[199], /* init: */
&x_sytab[200], /* init:super: */
&x_sytab[201], /* init:super:numVars: */
&x_sytab[202], /* inject:into: */
&x_sytab[203], /* integerPart */
&x_sytab[204], /* isAlphaNumeric */
&x_sytab[205], /* isDigit */
&x_sytab[206], /* isEmpty */
&x_sytab[207], /* isKindOf: */
&x_sytab[208], /* isLetter */
&x_sytab[209], /* isLowercase */
&x_sytab[210], /* isMemberOf: */
&x_sytab[211], /* isNil */
&x_sytab[212], /* isSeparator */
&x_sytab[213], /* isUppercase */
&x_sytab[214], /* isVowel */
&x_sytab[215], /* keys */
&x_sytab[216], /* keysDo: */
&x_sytab[217], /* keysSelect: */
&x_sytab[218], /* last */
&x_sytab[219], /* lastKey */
&x_sytab[220], /* lcm: */
&x_sytab[221], /* list */
&x_sytab[222], /* ln */
&x_sytab[223], /* log: */
&x_sytab[224], /* lower */
&x_sytab[225], /* main */
&x_sytab[226], /* max: */
&x_sytab[227], /* maxContext: */
&x_sytab[228], /* maxtype: */
&x_sytab[229], /* methods: */
&x_sytab[230], /* min: */
&x_sytab[231], /* modeCharacter */
&x_sytab[232], /* modeInteger */
&x_sytab[233], /* modeString */
&x_sytab[234], /* name: */
&x_sytab[235], /* negated */
&x_sytab[236], /* negative */
&x_sytab[237], /* new */
&x_sytab[238], /* new: */
&x_sytab[239], /* newProcess */
&x_sytab[240], /* newProcessWith: */
&x_sytab[241], /* next */
&x_sytab[242], /* next: */
&x_sytab[243], /* noDisplay */
&x_sytab[244], /* noMask: */
&x_sytab[245], /* not */
&x_sytab[246], /* notNil */
&x_sytab[247], /* nothing */
&x_sytab[248], /* occurrencesOf: */
&x_sytab[249], /* odd */
&x_sytab[250], /* opError */
&x_sytab[251], /* open: */
&x_sytab[252], /* open:for: */
&x_sytab[253], /* or: */
&x_sytab[254], /* perform: */
&x_sytab[255], /* perform:withArguments: */
&x_sytab[256], /* pi */
&x_sytab[257], /* positive */
&x_sytab[258], /* print */
&x_sytab[259], /* printString */
&x_sytab[260], /* put: */
&x_sytab[261], /* quo: */
&x_sytab[262], /* radians */
&x_sytab[263], /* radix: */
&x_sytab[264], /* raisedTo: */
&x_sytab[265], /* raisedToInteger: */
&x_sytab[266], /* randInteger: */
&x_sytab[267], /* randomize */
&x_sytab[268], /* read */
&x_sytab[269], /* reciprocal */
&x_sytab[270], /* reject: */
&x_sytab[271], /* rem: */
&x_sytab[272], /* remove: */
&x_sytab[273], /* remove:ifAbsent: */
&x_sytab[274], /* removeAll: */
&x_sytab[275], /* removeError */
&x_sytab[276], /* removeFirst */
&x_sytab[277], /* removeKey: */
&x_sytab[278], /* removeKey:ifAbsent: */
&x_sytab[279], /* removeLast */
&x_sytab[280], /* removed */
&x_sytab[281], /* replaceFrom:to:with: */
&x_sytab[282], /* replaceFrom:to:with:startingAt: */
&x_sytab[283], /* respondsTo */
&x_sytab[284], /* respondsTo: */
&x_sytab[285], /* resume */
&x_sytab[286], /* reverseDo: */
&x_sytab[287], /* reversed */
&x_sytab[288], /* roundTo: */
&x_sytab[289], /* rounded */
&x_sytab[290], /* sameAs: */
&x_sytab[291], /* seed */
&x_sytab[292], /* select: */
&x_sytab[293], /* setCurrentLocation: */
&x_sytab[294], /* sh: */
&x_sytab[295], /* shallowCopy */
&x_sytab[296], /* shallowCopy: */
&x_sytab[297], /* sign */
&x_sytab[298], /* signal */
&x_sytab[299], /* sin */
&x_sytab[300], /* size */
&x_sytab[301], /* smalltalk */
&x_sytab[302], /* sort */
&x_sytab[303], /* sort: */
&x_sytab[304], /* sqrt */
&x_sytab[305], /* squared */
&x_sytab[306], /* state */
&x_sytab[307], /* step */
&x_sytab[308], /* strictlyPositive */
&x_sytab[309], /* superClass */
&x_sytab[310], /* superClass: */
&x_sytab[311], /* suspend */
&x_sytab[312], /* tan */
&x_sytab[313], /* temp */
&x_sytab[314], /* termErr: */
&x_sytab[315], /* terminate */
&x_sytab[316], /* time: */
&x_sytab[317], /* timesRepeat: */
&x_sytab[318], /* to: */
&x_sytab[319], /* to:by: */
&x_sytab[320], /* transpose */
&x_sytab[321], /* truncateTo: */
&x_sytab[322], /* truncated */
&x_sytab[323], /* truncatedGrid: */
&x_sytab[324], /* unblock */
&x_sytab[325], /* upper */
&x_sytab[326], /* value */
&x_sytab[327], /* value: */
&x_sytab[328], /* value:value: */
&x_sytab[329], /* value:value:value: */
&x_sytab[330], /* value:value:value:value: */
&x_sytab[331], /* value:value:value:value:value: */
&x_sytab[332], /* values */
&x_sytab[333], /* variables */
&x_sytab[334], /* variables: */
&x_sytab[335], /* view */
&x_sytab[336], /* wait */
&x_sytab[337], /* whileFalse: */
&x_sytab[338], /* whileTrue: */
&x_sytab[339], /* with:do: */
&x_sytab[340], /* withArguments: */
&x_sytab[341], /* write: */
&x_sytab[342], /* x */
&x_sytab[343], /* x: */
&x_sytab[344], /* xor: */
&x_sytab[345], /* xvalue */
&x_sytab[346], /* y */
&x_sytab[347], /* y: */
&x_sytab[348], /* yield */
&x_sytab[349], /* yvalue */
&x_sytab[350], /* | */
&x_sytab[351], /* ~ */
&x_sytab[352], /* ~= */
&x_sytab[353], /* ~~ */
0};
int x_tmax = 353;
