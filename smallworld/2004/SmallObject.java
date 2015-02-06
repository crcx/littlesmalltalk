/*
   Little Smalltalk Interpreter written in Java
   Written by Tim Budd, budd@acm.org

   Version 0.8 (November 2002)
*/

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.JTextComponent;

class SmallObject implements Serializable {
   public SmallObject objClass;
   public SmallObject [ ] data;

   public SmallObject () { objClass = null; data = null; }

   public SmallObject (SmallObject cl, int size) 
      { objClass = cl; data = new SmallObject [size]; }

   public SmallObject copy(SmallObject cl) { return this; }
}

class SmallInt extends SmallObject {
   public int value;

   public SmallInt (SmallObject IntegerClass, int v) 
      { super (IntegerClass, 0); value = v; }

   public String toString () { return "SmallInt: "+ value; }
}

class SmallByteArray extends SmallObject {
   public byte [ ] values;

   public SmallByteArray (SmallObject cl, int size) 
      { super(cl, 0); values = new byte[size]; }

   public SmallByteArray (SmallObject cl, String text) {
      super(cl, 0);
      int size = text.length();
      values = new byte[size];
      for (int i = 0; i < size; i++)
         values[i] = (byte) text.charAt(i);
   }

   public String toString () {
      // we assume its a string, tho not always true...
      return new String(values); 
   }

   public SmallObject copy (SmallObject cl) {
      SmallByteArray newObj = new SmallByteArray(cl, values.length);
      for (int i = 0; i < values.length; i++) {
         newObj.values[i] = values[i];
      }
      return newObj;
   } 
}

class SmallJavaObject extends SmallObject {
   public SmallJavaObject(SmallObject cls, Object v) 
      { super(cls, 0); value = v; }

   public Object value;
}

class SmallException extends Exception {
   SmallException (String gripe, SmallObject c) 
      { super(gripe); context = c;}
   public SmallObject context;
}

class Sema {
   public synchronized SmallObject get() { if (! hasBeenSet) try {
      wait(); 
     } catch(Exception e) { System.out.println("Sema got exception " + e); }
   return value; }
   public synchronized void set(SmallObject v) 
      { value = v; hasBeenSet = true; notifyAll(); }
   private SmallObject value;
   private boolean hasBeenSet = false;
}

class SmallInterpreter implements Serializable {

      // global constants
   public SmallObject nilObject;
   public SmallObject trueObject;
   public SmallObject falseObject;
   public SmallInt [ ] smallInts;
   public SmallObject ArrayClass;
   public SmallObject BlockClass;
   public SmallObject ContextClass;
   public SmallObject IntegerClass;

      // create a new small integer
   SmallInt newInteger (int val) {
      if ((val >= 0) && (val < 10))
         return smallInts[val];
      else
         return new SmallInt (IntegerClass, val);
   }

   private SmallObject methodLookup (SmallObject receiver, 
      SmallByteArray messageSelector, SmallObject context, 
      SmallObject arguments) 
               throws SmallException {
      String name = messageSelector.toString();
      SmallObject cls;
      for (cls = receiver; cls != nilObject; cls = cls.data[1]) {
         SmallObject dict = cls.data[2]; // dictionary in class
         for (int i = 0; i < dict.data.length; i++) {
            SmallObject aMethod = dict.data[i];
            if (name.equals(aMethod.data[0].toString())) {
               return aMethod;
            }
         }
      }
      // try once to handle method in Smalltalk before giving up
      if (name.equals("error:"))
         throw new SmallException("Unrecognized message selector: " + 
	    messageSelector, context);
      SmallObject[] newArgs = new SmallObject[2];
      newArgs[0] = arguments.data[0]; // same receiver
      newArgs[1] = new SmallByteArray(messageSelector.objClass,
         "Unrecognized message selector: " + name);
      arguments.data = newArgs;
      return methodLookup(receiver, 
        new SmallByteArray(messageSelector.objClass, "error:"),
        context, arguments);
   }

   SmallObject buildContext 
      (SmallObject oldContext, SmallObject arguments, SmallObject method) {
      SmallObject context = new SmallObject(ContextClass, 7);
      context.data[0] = method;
      context.data[1] = arguments;
         // allocate temporaries
      int max = ((SmallInt) (method.data[4])).value;
      if (max > 0) {
         context.data[2] = new SmallObject(ArrayClass, max);
         while (max > 0) // iniailize to nil
            context.data[2].data[--max] = nilObject;
      }
         // allocate stack
      max = ((SmallInt) (method.data[3])).value;
      context.data[3] = new SmallObject(ArrayClass, max);
      context.data[4] = smallInts[0]; // byte pointer
      context.data[5] = smallInts[0]; // stacktop
      context.data[6] = oldContext;
      return context;
      }

      // execution method
   SmallObject execute (SmallObject context,
     final Thread myThread, final Thread parentThread) throws SmallException {
      SmallObject [ ] selectorCache = new SmallObject[197];
      SmallObject [ ] classCache = new SmallObject[197];
      SmallObject [ ] methodCache = new SmallObject[197];
      int lookup = 0;
      int cached = 0;

      SmallObject [ ] contextData = context.data;

outerLoop:
      while (true) { 

      SmallObject method = contextData[0]; // method in context
      byte [ ] code = ((SmallByteArray) method.data[1]).values; // code pointer
      int bytePointer = ((SmallInt) contextData[4]).value; 
      SmallObject [ ] stack = contextData[3].data;
      int stackTop = ((SmallInt) contextData[5]).value;
      SmallObject returnedValue = null;
      SmallObject temp;
      SmallObject [ ] tempa;

         // everything else can be null for now
      SmallObject [ ] temporaries = null;
      SmallObject [ ] instanceVariables = null;
      SmallObject arguments = null;
      SmallObject [ ] literals = null;

innerLoop:
      while (true) { 
         int high = code[bytePointer++];
         int low = high & 0x0F;
         high = (high >>= 4) & 0x0F;
         if (high == 0) {
            high = low;
               // convert to positive int
            low = (int) code[bytePointer++] & 0x0FF;
            }

switch (high) {

   case 1: // PushInstance 
      if (arguments == null)
         arguments = contextData[1];
      if (instanceVariables == null)
         instanceVariables = arguments.data[0].data;
      stack[stackTop++] = instanceVariables[low];
      break;

   case 2: // PushArgument
      if (arguments == null)
         arguments = contextData[1];
      stack[stackTop++] = arguments.data[low];
      break;

   case 3: // PushTemporary
      if (temporaries == null)
         temporaries = contextData[2].data;
      stack[stackTop++] = temporaries[low];
      break;

   case 4: // PushLiteral
      if (literals == null)
         literals = method.data[2].data;
      stack[stackTop++] = literals[low];
      break;

   case 5: // PushConstant
      switch(low) {
         case 0: case 1: case 2: case 3: case 4:
         case 5: case 6: case 7: case 8: case 9:
            stack[stackTop++] = smallInts[low]; break;
         case 10: 
            stack[stackTop++] = nilObject; break;
         case 11:
            stack[stackTop++] = trueObject; break;
         case 12:
            stack[stackTop++] = falseObject; break;
         default:
            throw new SmallException("Unknown constant " + low, context);
      }
      break;

   case 12: // PushBlock
         // low is argument location
         // next byte is goto value
      high = (int) code[bytePointer++] & 0x0FF;
      returnedValue = new SmallObject(BlockClass, 10);
      tempa = returnedValue.data;
      tempa[0] = contextData[0]; // share method
      tempa[1] = contextData[1]; // share arguments
      tempa[2] = contextData[2]; // share temporaries
      tempa[3] = contextData[3]; // stack (later replaced)
      tempa[4] = newInteger(bytePointer); // current byte pointer 
      tempa[5] = smallInts[0]; // stacktop
      tempa[6] = contextData[6]; // previous context
      tempa[7] = newInteger(low); // argument location
      tempa[8] = context; // creating context
      tempa[9] = newInteger(bytePointer); // current byte pointer
      stack[stackTop++] = returnedValue;
      bytePointer = high;
      break;

   case 14: // PushClassVariable 
      if (arguments == null)
         arguments = contextData[1];
      if (instanceVariables == null)
         instanceVariables = arguments.data[0].data;
      stack[stackTop++] = arguments.data[0].objClass.data[low+5];
      break;

   case 6: // AssignInstance
      if (arguments == null)
         arguments = contextData[1];
      if (instanceVariables == null)
         instanceVariables = arguments.data[0].data;
         // leave result on stack
      instanceVariables[low] = stack[stackTop-1];
      break;

   case 7: // AssignTemporary
      if (temporaries == null)
         temporaries = contextData[2].data;
      temporaries[low] = stack[stackTop-1];
      break;

   case 8: // MarkArguments
      SmallObject newArguments = new SmallObject(ArrayClass, low);
      tempa = newArguments.data; // direct access to array
      while (low > 0)
         tempa[--low] = stack[--stackTop];
      stack[stackTop++] = newArguments;
      break;

   case 9: // SendMessage
         // save old context
      arguments = stack[--stackTop];
         // expand newInteger in line
      //contextData[5] = newInteger(stackTop);
      contextData[5] = (stackTop < 10)?smallInts[stackTop]:new SmallInt(IntegerClass,stackTop);
      //contextData[4] = newInteger(bytePointer);
      contextData[4] = (bytePointer < 10)?smallInts[bytePointer]:new SmallInt(IntegerClass,bytePointer);
         // now build new context
      if (literals == null)
         literals = method.data[2].data;
      returnedValue = literals[low]; // message selector
//System.out.println("Sending " + returnedValue);
//System.out.println("Arguments " + arguments);
//System.out.println("Arguments receiver " + arguments.data[0]);
//System.out.println("Arguments class " + arguments.data[0].objClass);
      high = (arguments.data[0].objClass.hashCode() + returnedValue.hashCode()) 
         % 197;
      if ((selectorCache[high] != null) && 
         (selectorCache[high] == returnedValue) &&
         (classCache[high] == arguments.data[0].objClass)) {
            method = methodCache[high];
            cached++;
            }
      else {
         method = methodLookup(arguments.data[0].objClass, (SmallByteArray) literals[low], context, arguments);
         lookup++;
         selectorCache[high] = returnedValue;
         classCache[high] = arguments.data[0].objClass;
         methodCache[high] = method;
         }
      context = buildContext(context, arguments, method);
      contextData = context.data;
         // load information from context
      continue outerLoop;

   case 10: // SendUnary
      if (low == 0) { // isNil
         SmallObject arg = stack[--stackTop];
         stack[stackTop++] = (arg == nilObject)?trueObject:falseObject;
         }
      else if (low == 1) { // notNil
         SmallObject arg = stack[--stackTop];
         stack[stackTop++] = (arg != nilObject)?trueObject:falseObject;
         }
      else
         throw new SmallException("Illegal SendUnary " + low, context);
      break;

   case 11:{// SendBinary
      if ((stack[stackTop-1] instanceof SmallInt) &&
         (stack[stackTop-2] instanceof SmallInt)) {
         int j = ((SmallInt) stack[--stackTop]).value;
         int i = ((SmallInt) stack[--stackTop]).value;
         boolean done = true;
         switch (low) {
            case 0: // <
               returnedValue = (i < j)?trueObject:falseObject;
               break;
            case 1: // <=
               returnedValue = (i <= j)?trueObject:falseObject;
               break;
            case 2: // +
               long li = i + (long) j;
               if (li != (i+j))
                  done = false; // overflow
               returnedValue = newInteger(i+j);
               break;
            }
         if (done) { 
            stack[stackTop++] = returnedValue;
            break;
         } else stackTop += 2; // overflow, send message
      }
         // non optimized binary message
      arguments = new SmallObject(ArrayClass, 2);
      arguments.data[1] = stack[--stackTop];
      arguments.data[0] = stack[--stackTop];
      contextData[5] = newInteger(stackTop);
      contextData[4] = newInteger(bytePointer);
      SmallByteArray msg = null; 
      switch(low) {
         case 0: msg = new SmallByteArray(null,"<"); break;
         case 1: msg = new SmallByteArray(null,"<="); break;
         case 2: msg = new SmallByteArray(null,"+"); break;
      }
      method = methodLookup(arguments.data[0].objClass, msg, context, arguments);
      context = buildContext(context, arguments, method);
      contextData = context.data;
      continue outerLoop;
      }

   case 13: // Do Primitive, low is arg count, next byte is number
      high = (int) code[bytePointer++] & 0x0FF;
      switch (high) {

         case 1: // object identity
            returnedValue = stack[--stackTop];
            if (returnedValue == stack[--stackTop])
               returnedValue = trueObject;
            else returnedValue = falseObject;
            break;

         case 2: // object class
            returnedValue = stack[--stackTop].objClass;
            break;

         case 4: // object size
            returnedValue = stack[--stackTop];
            if (returnedValue instanceof SmallByteArray)
               low = ((SmallByteArray) returnedValue).values.length;
            else
               low = returnedValue.data.length;
            returnedValue = newInteger(low);
            break;

         case 5: // object at put
            low = ((SmallInt) stack[--stackTop]).value;
            returnedValue = stack[--stackTop];
            returnedValue.data[low - 1] = stack[--stackTop];
            break;
            
         case 6: // new context execute
            returnedValue = execute(stack[--stackTop], myThread, parentThread);
            break;

         case 7: // new object allocation
            low = ((SmallInt) stack[--stackTop]).value;
            returnedValue = new SmallObject(stack[--stackTop], low);
            while (low > 0)
               returnedValue.data[--low] = nilObject;
            break;

         case 8: { // block invocation
            returnedValue = stack[--stackTop]; // the block
            high = ((SmallInt) returnedValue.data[7]).value; // arg location
            low -= 2;
            if (low >= 0) {
               temporaries = returnedValue.data[2].data;
               while (low >= 0) {
                  temporaries[high + low--] = stack[--stackTop];
                  }
               }
            contextData[5] = newInteger(stackTop);
            contextData[4] = newInteger(bytePointer);
            SmallObject newContext = new SmallObject(ContextClass, 10);
	    for (int i = 0; i < 10; i++)
	       newContext.data[i] = returnedValue.data[i];
            newContext.data[6] = contextData[6];
            newContext.data[5] = smallInts[0]; // stack top
            newContext.data[4] = returnedValue.data[9]; // starting addr
	    low = newContext.data[3].data.length; //stack size
	    newContext.data[3] = new SmallObject(ArrayClass, low); // new stack
            context = newContext;
            contextData = context.data;
            continue outerLoop;
	    }

         case 9: // read a char from input
            try {
               returnedValue = newInteger(System.in.read());
            } catch(IOException e) 
               { returnedValue = nilObject; }
            break;

         case 10:{ // small integer addition need to handle ovflow
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
	    long lhigh = ((long) high) + (long) low;
            high += low;
	    if (lhigh == high) returnedValue = newInteger(high);
	    else returnedValue = nilObject;
            } break;

         case 11: // small integer quotient 
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
            high /= low;
            returnedValue = newInteger(high);
            break;

         case 12: // small integer remainder 
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
            high %= low;
            returnedValue = newInteger(high);
            break;

         case 14: // small int equality
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
            returnedValue = (low == high)?trueObject:falseObject;
            break;

         case 15:{ // small integer multiplication 
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
	    long lhigh = ((long) high) * (long) low;
            high *= low;
	    if (lhigh == high) returnedValue = newInteger(high);
	    else returnedValue = nilObject;
            } break;

         case 16:{ // small integer subtraction 
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
	    long lhigh = ((long) high) - (long) low;
            high -= low;
	    if (lhigh == high) returnedValue = newInteger(high);
	    else returnedValue = nilObject;
            } break;

         case 17: // small integer as string
            low = ((SmallInt) stack[--stackTop]).value;
            returnedValue = new SmallByteArray(stack[--stackTop], String.valueOf(low));
            break;

         case 18: // debugg -- dummy for now
            returnedValue = stack[--stackTop];
            System.out.println("Debug " + returnedValue + " class " + returnedValue.objClass.data[0]);
            break;

         case 19: // block fork
            returnedValue = stack[--stackTop];
            new ActionThread(returnedValue, myThread).start();
            break;

         case 20: // byte array allocation
            low = ((SmallInt) stack[--stackTop]).value;
            returnedValue = new SmallByteArray(stack[--stackTop], low);
            break;
            
         case 21: // string at
            low = ((SmallInt) stack[--stackTop]).value;
            returnedValue = stack[--stackTop];
            SmallByteArray baa = (SmallByteArray) returnedValue;
            low = (int) baa.values[low-1] & 0x0FF;
            returnedValue = newInteger(low);
            break;
            
         case 22: // string at put
            low = ((SmallInt) stack[--stackTop]).value;
            SmallByteArray ba = (SmallByteArray) stack[--stackTop];
            high = ((SmallInt) stack[--stackTop]).value;
            ba.values[low-1] = (byte) high;
            returnedValue = ba;
            break;
            
         case 23: // string copy
            returnedValue = stack[--stackTop];
            returnedValue = stack[--stackTop].copy(returnedValue);
            break;

         case 24:{ // string append
            SmallByteArray a = (SmallByteArray) stack[--stackTop];
            SmallByteArray b = (SmallByteArray) stack[--stackTop];
            low = a.values.length + b.values.length;
            SmallByteArray n = new SmallByteArray(a.objClass, low);
            high = 0;
            for (int i = 0; i < a.values.length; i++)
               n.values[high++] = a.values[i];
            for (int i = 0; i < b.values.length; i++)
               n.values[high++] = b.values[i];
            returnedValue = n;
            } break;

         case 26: { // string compare
            SmallByteArray a = (SmallByteArray) stack[--stackTop];
            SmallByteArray b = (SmallByteArray) stack[--stackTop];
            low = a.values.length;
            high = b.values.length;
            int s = (low < high)?low:high;
            int r = 0;
            for (int i = 0; i < s; i++) 
               if (a.values[i] < b.values[i]) 
                  { r = 1; break; }
               else if (b.values[i] < a.values[i])
                  { r = -1; break; }
            if (r == 0) 
               if (low < high) r = 1;
               else if (low > high) r = -1;
            returnedValue = newInteger(r);
            } break;

         case 29: { // image save
            SmallByteArray a = (SmallByteArray) stack[--stackTop];
            String name = a.toString();
            try {
            ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(name));
            //oos.writeObject(this);
            // write one by one to avoid serialization
            oos.writeObject(nilObject);
            oos.writeObject(trueObject);
            oos.writeObject(falseObject);
            oos.writeObject(smallInts);
            oos.writeObject(ArrayClass);
            oos.writeObject(BlockClass);
            oos.writeObject(ContextClass);
            oos.writeObject(IntegerClass);
            } catch (Exception e) {
            throw new SmallException("got I/O Exception " + e, context);
            }
            returnedValue = a;
            } break;

         case 30: // array at
            low = ((SmallInt) stack[--stackTop]).value;
            returnedValue = stack[--stackTop];
            returnedValue = returnedValue.data[low-1];
            break;

         case 31: {// array with:  (add new item)
            SmallObject oldar = stack[--stackTop];
            low = oldar.data.length;
            returnedValue = new SmallObject(oldar.objClass, low+1);
            for (int i = 0; i < low; i++)
               returnedValue.data[i] = oldar.data[i];
            returnedValue.data[low] = stack[--stackTop];
            } break;

         case 32:{ // object add: increase object size
            returnedValue = stack[--stackTop];
            low = returnedValue.data.length;
            SmallObject na[] = new SmallObject[low+1];
            for (int i = 0; i < low; i++) 
               na[i] = returnedValue.data[i];
            na[low] = stack[--stackTop];
            returnedValue.data = na;
            } break;

         case 33: {// Sleep for a bit
            low = ((SmallInt) stack[--stackTop]).value;
            try { Thread.sleep(low); } catch (Exception a) { }
            } break;

         case 34: { // thread kill
            if (parentThread != null) parentThread.stop();
            if (myThread != null) myThread.stop();
            System.out.println("is there life after death?");
            } break;

	 case 35: // return current context
	    returnedValue = context;
	    break;

         case 36:  // fast array creation
            returnedValue = new SmallObject(ArrayClass, low);
            for (int i = low-1; i >= 0; i--)
               returnedValue.data[i] = stack[--stackTop];
            break;

         case 41: {// open file for output
            try {
            FileOutputStream of = new FileOutputStream(stack[--stackTop].toString());
            PrintStream ps = new PrintStream(of);
            returnedValue = new SmallJavaObject(stack[--stackTop], ps);
            } catch (IOException e) {
               throw new SmallException("I/O exception " + e, context);
            } } break;

         case 42: {// open file for input
            try {
            FileInputStream of = new FileInputStream(stack[--stackTop].toString());
            DataInput ps = new DataInputStream(of);
            returnedValue = new SmallJavaObject(stack[--stackTop], ps);
            } catch (IOException e) {
               throw new SmallException("I/O exception " + e, context);
            } } break;

         case 43: {// write a string
            try {
            PrintStream ps = (PrintStream) 
               ((SmallJavaObject) stack[--stackTop]).value;
            ps.print(stack[--stackTop]);
            } catch (Exception e) {
               throw new SmallException("I/O exception " + e, context);
            } } break;

         case 44: { // read a string
            try {
            DataInput di = (DataInput)
               ((SmallJavaObject) stack[--stackTop]).value;
            String line = di.readLine();
            if (line == null) {
               --stackTop; returnedValue = nilObject;
               }
            else 
               returnedValue = new SmallByteArray(stack[--stackTop], line);
            } catch (EOFException e) {
               returnedValue = nilObject;
            } catch (IOException f) {
               throw new SmallException("I/O exception " + f, context);
            } } break;

         case 50:  // integer into float
            low = ((SmallInt) stack[--stackTop]).value;
            returnedValue = new SmallJavaObject(stack[--stackTop], new Double((double) low));
            break;

         case 51:{ // addition of float
            double a = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            double b = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            returnedValue = new SmallJavaObject(stack[--stackTop], new Double(a+b));
            } break;

         case 52:{ // subtraction of float
            double a = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            double b = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            returnedValue = new SmallJavaObject(stack[--stackTop], new Double(a-b));
            } break;

         case 53:{ // multiplication of float
            double a = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            double b = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            returnedValue = new SmallJavaObject(stack[--stackTop], new Double(a*b));
            } break;

         case 54:{ // division of float
            double a = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            double b = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            returnedValue = new SmallJavaObject(stack[--stackTop], new Double(a/b));
            } break;

         case 55:{ // less than test of float
            double a = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            double b = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            returnedValue = (a<b)?trueObject:falseObject;
            } break;

         case 56:{ // equality test of float
            double a = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            double b = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
            returnedValue = (a==b)?trueObject:falseObject;
            } break;

	 case 57:{ // float to int
            double a = ((Double)((SmallJavaObject) stack[--stackTop]).value).doubleValue();
	    returnedValue = newInteger((int) a);
	    } break;

	 case 58: // random float
	    returnedValue = new SmallJavaObject(stack[--stackTop], new Double(Math.random()));
            break;

         case 59: // print of float
            returnedValue = (SmallJavaObject) stack[--stackTop];
            returnedValue = new SmallByteArray(stack[--stackTop],
            String.valueOf(((Double) ((SmallJavaObject) returnedValue).value).doubleValue()));
            break;
         
         case 60:{ // make window
            JDialog jd = new JDialog();
            jd.setVisible(false);
            returnedValue = new SmallJavaObject(stack[--stackTop], jd);
            } break;

         case 61:{ // show/hide text window
            returnedValue = stack[--stackTop];
            SmallJavaObject jo = (SmallJavaObject) stack[--stackTop];
            if (returnedValue == trueObject)
               ((JDialog) jo.value).setVisible(true);
            else
               ((JDialog) jo.value).setVisible(false);
            } break;

         case 62:{ // set content pane
            SmallJavaObject tc = (SmallJavaObject) stack[--stackTop];
            returnedValue = stack[--stackTop];
            SmallJavaObject jd = (SmallJavaObject) returnedValue;
            ((JDialog) jd.value).getContentPane().add((Component) tc.value);
            } break;

         case 63: // set size
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
            returnedValue = stack[--stackTop];
            {
            SmallJavaObject wo = (SmallJavaObject) returnedValue;
            ((JDialog) wo.value).setSize(low, high);
            } break;

         case 64:{ // add menu to window
            SmallJavaObject menu = (SmallJavaObject) stack[--stackTop];
            returnedValue = stack[--stackTop];
            SmallJavaObject jo = (SmallJavaObject) returnedValue;
            JDialog jd = (JDialog) jo.value;
            if (jd.getJMenuBar() == null) jd.setJMenuBar(new JMenuBar());
            jd.getJMenuBar().add((JMenu) menu.value);
            } break;

         case 65:{ // set title
            SmallObject title = stack[--stackTop];
            returnedValue = stack[--stackTop];
            SmallJavaObject jd = (SmallJavaObject) returnedValue;
            ((JDialog) jd.value).setTitle(title.toString());
            } break;

         case 66: { // repaint window
            returnedValue = stack[--stackTop];
            SmallJavaObject jd = (SmallJavaObject) returnedValue;
            ((JDialog) jd.value).repaint();
            } break;

         case 70:{ // new label panel
            JLabel jl = new JLabel(stack[--stackTop].toString());
            returnedValue = new SmallJavaObject(stack[--stackTop], new JScrollPane(jl));
            } break;
            
         case 71:{ // new button
            final SmallObject action = stack[--stackTop];
            final JButton jb = new JButton(stack[--stackTop].toString());
            returnedValue = new SmallJavaObject(stack[--stackTop], jb);
            jb.addActionListener(new ActionListener() {
               public void actionPerformed(ActionEvent e) {
                  new ActionThread(action, myThread).start();
            }});
            } break;

         case 72: // new text line
            returnedValue = new SmallJavaObject(stack[--stackTop], new JTextField());
            break;

         case 73: // new text area
            returnedValue = new SmallJavaObject(stack[--stackTop], new JScrollPane(new JTextArea()));
            break;

         case 74:{ // new grid panel
            SmallObject data = stack[--stackTop];
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
            JPanel jp = new JPanel();
            jp.setLayout(new GridLayout(low, high));
            for (int i = 0; i < data.data.length; i++)
            jp.add((Component)((SmallJavaObject)data.data[i]).value);
            returnedValue = new SmallJavaObject(stack[--stackTop], jp);
            }
            break;

         case 75:{ // new list panel
            final SmallObject action = stack[--stackTop];
            SmallObject data = stack[--stackTop];
            returnedValue = stack[--stackTop];
            final JList jl = new JList(data.data); 
            jl.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
            returnedValue = new SmallJavaObject(returnedValue, new JScrollPane(jl));
            jl.addListSelectionListener(
            new ListSelectionListener() {
            public void valueChanged(ListSelectionEvent e) {
            if ((!e.getValueIsAdjusting()) && (jl.getSelectedIndex() >= 0)) {
               new ActionThread(action, myThread, jl.getSelectedIndex()+1).start();
               //new ActionThread(action, myThread).start();
            }}});
            } break;

         case 76:{ // new border panel
            JPanel jp = new JPanel();
            jp.setLayout(new BorderLayout());
            returnedValue = stack[--stackTop];
            if (returnedValue != nilObject)
            jp.add("Center", (Component) ((SmallJavaObject) returnedValue).value);
            returnedValue = stack[--stackTop];
            if (returnedValue != nilObject)
            jp.add("West", (Component) ((SmallJavaObject) returnedValue).value);
            returnedValue = stack[--stackTop];
            if (returnedValue != nilObject)
            jp.add("East", (Component) ((SmallJavaObject) returnedValue).value);
            returnedValue = stack[--stackTop];
            if (returnedValue != nilObject)
            jp.add("South", (Component) ((SmallJavaObject) returnedValue).value);
            returnedValue = stack[--stackTop];
            if (returnedValue != nilObject)
            jp.add("North", (Component) ((SmallJavaObject) returnedValue).value);
            returnedValue = new SmallJavaObject(stack[--stackTop], jp);
            } break;

         case 77:{ // set image on label
            SmallJavaObject img = (SmallJavaObject) stack[--stackTop];
            SmallJavaObject lab = (SmallJavaObject) stack[--stackTop];
            Object jo = lab.value;
            if (jo instanceof JScrollPane) jo = ((JScrollPane) jo).getViewport().getView();
            if (jo instanceof JLabel) {
            JLabel jlb = (JLabel) jo;
            jlb.setIcon(new ImageIcon((Image) img.value));
            jlb.setHorizontalAlignment(SwingConstants.LEFT);
            jlb.setVerticalAlignment(SwingConstants.TOP);
            jlb.repaint();
            }} break;

         case 79: {// repaint
            returnedValue = stack[--stackTop];
            SmallJavaObject jo = (SmallJavaObject) returnedValue;
            ((JComponent) jo.value).repaint();
            } break;

         case 80:{ // content of text area
            SmallJavaObject jt = (SmallJavaObject) stack[--stackTop];
            returnedValue = stack[--stackTop]; // class
            Object jo = jt.value;
            if (jo instanceof JScrollPane) jo = ((JScrollPane) jo).getViewport().getView();
            if (jo instanceof JTextComponent)

            returnedValue = new SmallByteArray(returnedValue,((JTextComponent) jo).getText());
            else returnedValue = new SmallByteArray(returnedValue, "");
            } break;

         case 81: {// content of selected text area
            SmallJavaObject jt = (SmallJavaObject) stack[--stackTop];
            returnedValue = stack[--stackTop]; // class
            Object jo = jt.value;
            if (jo instanceof JScrollPane) jo = ((JScrollPane) jo).getViewport().getView();
            if (jo instanceof JTextComponent)

            returnedValue = new SmallByteArray(returnedValue,((JTextComponent) jo).getSelectedText());
            else returnedValue = new SmallByteArray(returnedValue, "");
            } break;

         case 82:{ // set text area
            returnedValue = stack[--stackTop];// text
            SmallJavaObject jt = (SmallJavaObject) stack[--stackTop];
            Object jo = jt.value;
            if (jo instanceof JScrollPane) jo = ((JScrollPane) jo).getViewport().getView();
            if (jo instanceof JTextComponent)
            ((JTextComponent) jo).setText(returnedValue.toString());
            } break;

         case 83:{ // get selected index
            SmallJavaObject jo = (SmallJavaObject) stack[--stackTop];
            Object jl = jo.value;
            if (jl instanceof JScrollPane) jl = ((JScrollPane) jl).getViewport().getView();
            if (jl instanceof JList)
               returnedValue = newInteger(((JList) jl).getSelectedIndex()+1);
	    else if (jl instanceof JScrollBar)
	       returnedValue = newInteger(((JScrollBar) jl).getValue());
            else returnedValue = newInteger(0);
            } break;

         case 84:{ // set list data
            SmallObject data = stack[--stackTop];
            returnedValue = stack[--stackTop];
            SmallJavaObject jo = (SmallJavaObject) returnedValue;
            Object jl = jo.value;
            if (jl instanceof JScrollPane) jl = ((JScrollPane) jl).getViewport().getView();
            if (jl instanceof JList) {
               ((JList) jl).setListData(data.data);
               ((JList) jl).repaint();
            }} break;

         case 85: { // new slider
	    final SmallObject action = stack[--stackTop];
	    int max = ((SmallInt) stack[--stackTop]).value+10; //why?
	    int min = ((SmallInt) stack[--stackTop]).value;
	    SmallObject orient = stack[--stackTop];
	    final JScrollBar bar = new JScrollBar(
	   ((orient==trueObject)?JScrollBar.VERTICAL:JScrollBar.HORIZONTAL),
		min, 10, min, max);
	    returnedValue = new SmallJavaObject(stack[--stackTop],bar);
	    if (action != nilObject)
	    bar.addAdjustmentListener(new AdjustmentListener() {
	       public void adjustmentValueChanged(AdjustmentEvent ae) {
	          new ActionThread(action, myThread, ae.getValue()).start();
	    }});
	    } break;

         case 86: { // onMouseDown b
            final SmallObject action = stack[--stackTop];
            SmallJavaObject pan = (SmallJavaObject) stack[--stackTop];
            Object jo = pan.value;
            if (jo instanceof JScrollPane) jo = (JComponent) ((JScrollPane) jo).getViewport().getView();
            final JComponent jpan = (JComponent) jo;
            jpan.addMouseListener(new MouseAdapter() {
               public void mousePressed(MouseEvent e) {
                  new ActionThread(action, myThread, e.getX(), e.getY()).start();
            }});
            } break;

         case 87:{ // onMouseUp b
            final SmallObject action = stack[--stackTop];
            SmallJavaObject pan = (SmallJavaObject) stack[--stackTop];
            Object jo = pan.value;
            if (jo instanceof JScrollPane) jo = (JComponent) ((JScrollPane) jo).getViewport().getView();
            final JComponent jpan = (JComponent) jo;
            jpan.addMouseListener(new MouseAdapter() {
               public void mouseReleased(MouseEvent e) {
                  new ActionThread(action, myThread, e.getX(), e.getY()).start();
            }});
            } break;

         case 88:{ // onMouseMove b
            final SmallObject action = stack[--stackTop];
            SmallJavaObject pan = (SmallJavaObject) stack[--stackTop];
            Object jo = pan.value;
            if (jo instanceof JScrollPane) jo = (JComponent) ((JScrollPane) jo).getViewport().getView();
            final JComponent jpan = (JComponent) jo;
            jpan.addMouseMotionListener(new MouseMotionAdapter() {
	       public void mouseDragged(MouseEvent e) 
	       { new ActionThread(action, myThread, e.getX(), e.getY()).start(); }
               public void mouseMoved(MouseEvent e) {
               new ActionThread(action, myThread, e.getX(), e.getY()).start();
            }});
            } break;

         case 90:{ // new menu 
            SmallObject title = stack[--stackTop]; // text
            returnedValue = stack[--stackTop]; // class
            JMenu menu = new JMenu(title.toString());
            returnedValue = new SmallJavaObject(returnedValue, menu);
            } break;

         case 91:{ // new menu item
            final SmallObject action = stack[--stackTop];
            final SmallObject text = stack[--stackTop];
            returnedValue = stack[--stackTop];
            SmallJavaObject mo = (SmallJavaObject) returnedValue;
            JMenu menu = (JMenu) mo.value;
            JMenuItem ji = new JMenuItem(text.toString());
            ji.addActionListener(
               new ActionListener() {
                  public void actionPerformed(ActionEvent e) {
                     new ActionThread(action, myThread).start();
            }});
            menu.add(ji);
            } break;

         case 100: // new semaphore
            returnedValue = new SmallJavaObject(stack[--stackTop], new Sema());
            break;

         case 101:{ // semaphore wait
            SmallJavaObject jo = (SmallJavaObject) stack[--stackTop];
            returnedValue = ((Sema) jo.value).get();
            } break;

         case 102:{ // semaphore set
            returnedValue = stack[--stackTop];
            SmallJavaObject jo = (SmallJavaObject) stack[--stackTop];
            ((Sema) jo.value).set(returnedValue);
            } break;

         case 110:{ // new image
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
            Image img = new BufferedImage(low, high,
               BufferedImage.TYPE_INT_RGB);
            Graphics g = img.getGraphics();
            g.setColor(Color.white);
            g.fillRect(0, 0, low, high);
            returnedValue = new SmallJavaObject(stack[--stackTop], img);
            } break;

         case 111:{ // new image from file
            SmallByteArray title = (SmallByteArray) stack[--stackTop];
            returnedValue = stack[--stackTop];
	    Image img = null;
            try {
            img = Toolkit.getDefaultToolkit().getImage(title.toString());
System.out.println("got image " + img);
            //MediaTracker mt = new MediaTracker(null);
            //mt.addImage(img, 0);
            //mt.waitForID(0);
            } catch(Exception e) { 
System.out.println("image read got exception " + e);
	    }
            low = img.getWidth(null);
            high = img.getHeight(null);
            BufferedImage bi = new BufferedImage(low, high,BufferedImage.TYPE_INT_RGB);
            Graphics g = bi.createGraphics();
            g.drawImage(img, 0, 0, null);
            returnedValue = new SmallJavaObject(returnedValue, bi);
            } break;

	 case 113:{ // draw image
	    SmallJavaObject img2 = (SmallJavaObject) stack[--stackTop];
	    low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
            SmallJavaObject img = (SmallJavaObject) stack[--stackTop];
            Graphics g = ((Image) img.value).getGraphics();
	    g.drawImage((Image) img2.value, low, high, null);
	    } break;

         case 114:{ // draw text
            SmallByteArray text = (SmallByteArray) stack[--stackTop];
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
            int c = ((SmallInt) stack[--stackTop]).value;
            SmallJavaObject img = (SmallJavaObject) stack[--stackTop];
            Graphics g = ((Image) img.value).getGraphics();
            g.setColor(new Color(c));
            g.drawString(text.toString(), low, high);
            } break;

         case 115:{ // draw/fill circle
            int s = ((SmallInt) stack[--stackTop]).value;
            int h = ((SmallInt) stack[--stackTop]).value;
            int w = ((SmallInt) stack[--stackTop]).value;
            low = ((SmallInt) stack[--stackTop]).value;
            high = ((SmallInt) stack[--stackTop]).value;
            int c = ((SmallInt) stack[--stackTop]).value;
            SmallJavaObject img = (SmallJavaObject) stack[--stackTop];
            Graphics g = ((Image) img.value).getGraphics();
            g.setColor(new Color(c));
            switch(s) {
              case 1: g.drawOval(low, high, h, w); break;
              case 2: g.fillOval(low, high, h, w); break;
              case 3: g.drawRect(low, high, h, w); break;
              case 4: g.fillRect(low, high, h, w); break;
              case 5: g.drawLine(low, high, h, w); break;
            } } break;

         default:
            throw new SmallException("Unknown Primitive " + high, context);
         }
      stack[stackTop++] = returnedValue;
      break;

   case 15: // Do Special 
      switch(low) {
         case 1: // self return
            if (arguments == null)
               arguments = contextData[1];
            returnedValue = arguments.data[0];
            context = contextData[6]; // previous context
            break innerLoop;

         case 2: // stack return
            returnedValue = stack[--stackTop];
            context = contextData[6]; // previous context
            break innerLoop;

         case 3: // block return
            returnedValue = stack[--stackTop];
            context = contextData[8]; // creating context in block
            context = context.data[6]; // previous context
            break innerLoop;

         case 4: // duplicate
            returnedValue = stack[stackTop-1];
            stack[stackTop++] = returnedValue;
            break;

         case 5: // pop top
            stackTop--;
            break;

         case 6: // branch
            low = (int) code[bytePointer++] & 0x0FF;
            bytePointer = low;
            break;

         case 7: // branch if true
            low = (int) code[bytePointer++] & 0x0FF;
            returnedValue = stack[--stackTop];
            if (returnedValue == trueObject)
               bytePointer = low;
            break;

         case 8: // branch if false
            low = (int) code[bytePointer++] & 0x0FF;
            returnedValue = stack[--stackTop];
            if (returnedValue == falseObject)
               bytePointer = low;
            break;

         case 11: // send to super
            low = (int) code[bytePointer++] & 0x0FF; 
               // message selector
               // save old context
            arguments = stack[--stackTop];
            contextData[5] = newInteger(stackTop);
            contextData[4] = newInteger(bytePointer);
               // now build new context
            if (literals == null)
               literals = method.data[2].data;
            if (method == null)
               method = context.data[0];
            method = method.data[5]; // class in method
            method = method.data[1]; // parent in class
            method = methodLookup(method, (SmallByteArray) literals[low], context, arguments);
            context = buildContext(context, arguments, method);
            contextData = context.data;
               // load information from context
            continue outerLoop;

         default: // throw exception
            throw new SmallException("Unrecogized DoSpecial " + low, context);
         }
      break;

   default:   // throw exception
      throw new SmallException("Unrecogized opCode " + low, context);
      }
      } // end of inner loop
   
   if ((context == null) || (context == nilObject)) {
//System.out.println("lookups " + lookup + " cached " + cached);
      return returnedValue;
      }
   contextData = context.data;
   stack = contextData[3].data;
   stackTop = ((SmallInt) contextData[5]).value;
   stack[stackTop++] = returnedValue;
   contextData[5] = newInteger(stackTop);
   } 
} // end of outer loop

private class ActionThread extends Thread {
   public ActionThread(SmallObject block, Thread myT) {
      myThread = myT;
      action = new SmallObject(ContextClass, 10);
      for (int i = 0; i < 10; i++)
         action.data[i] = block.data[i];
   }

   public ActionThread(SmallObject block, Thread myT, int v1) {
      myThread = myT;
      action = new SmallObject(ContextClass, 10);
      for (int i = 0; i < 10; i++)
         action.data[i] = block.data[i];
      int argLoc = ((SmallInt) action.data[7]).value;
      action.data[2].data[argLoc] = newInteger(v1);
   }

   public ActionThread(SmallObject block, Thread myT, int v1, int v2) {
      myThread = myT;
      action = new SmallObject(ContextClass, 10);
      for (int i = 0; i < 10; i++)
         action.data[i] = block.data[i];
      int argLoc = ((SmallInt) action.data[7]).value;
      action.data[2].data[argLoc] = newInteger(v1);
      action.data[2].data[argLoc+1] = newInteger(v2);
   }

   private SmallObject action;
   private Thread myThread;

   public void run() {
      int stksize = action.data[3].data.length;
      action.data[3] = new SmallObject(ArrayClass, stksize); // new stack
      action.data[4] = action.data[9]; // byte pointer
      action.data[5] = newInteger(0); // stack top
      action.data[6] = nilObject;
      try {
         execute(action, this, myThread);
      } catch (Exception e) {
         System.out.println("caught exception " + e);
      }
   }
}

}
