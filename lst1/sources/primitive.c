/*************************************************************************
 * Little Smalltalk, Version 1
 *
 * The source code for the Little Smalltalk system may be freely copied
 * provided that the source of all files is acknowledged and that this
 * condition is copied with each file.
 *
 * The Little Smalltalk system is distributed without responsibility for
 * the performance of the program and without any guarantee of maintenance.
 *
 * All questions concerning Little Smalltalk should be addressed to:
 *
 *         Professor Tim Budd
 *         Department of Computer Science
 *         Oregon State University
 *         Corvallis, Oregon
 *         97331
 *         USA
 *
 * This copy of Little Smalltalk has been updated by Charles R. Childers,
 * http://charleschilders.com
 *************************************************************************
 * Primitive manager
 * timothy a. budd, 10/84
 *
 * hashcode code written by Robert McConeghy
 *         (who also wrote classes Dictionary, et al).
 *************************************************************************/

# ifdef CURSES
# include <curses.h>
# endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include "lst.h"

extern int errno;
extern int prntcmd;
extern double modf();
extern long time();
extern object *lookup_class();
extern object *fnd_class();
extern object *fnd_super();
extern process *runningProcess;
extern int responds_to(), generality();
extern class  *mk_class();
extern object *o_object, *o_true, *o_false, *o_nil, *o_number, *o_magnitude;

object *primitive(primnumber, numargs, args)
int primnumber, numargs;
object **args;
{	object *resultobj;
	object *leftarg, *rightarg;
	int    leftint, rightint, i, j;
	double leftfloat, rightfloat;

// FIXME
//	time_t   clock;

	char   *leftp, *rightp, *errp;
	class  *aClass;
	bytearray *byarray;
	struct file_struct *phil;
	int    opnumber = primnumber % 10;
	char   strbuffer[300], tempname[100];

	errno = 0;
	/* first do argument type checking */
	switch(i = (primnumber / 10)) {
		case 0: /* misc operations */
			if (opnumber <= 5 && numargs != 1) goto argcerror;
			leftarg = args[0];
			break;

		case 1: /* integer operations */
		case 2: 
			if (numargs != 2) goto argcerror;
			rightarg = args[1];
			if (! is_integer(rightarg)) goto argterror;
			rightint = int_value(rightarg);
		case 3: 
			if (i == 3 && opnumber && numargs != 1) 
				goto argcerror;
			leftarg = args[0];
			if (! is_integer(leftarg)) goto argterror;
			leftint = int_value(leftarg);
			break;

		case 4: /* character operations */
			if (numargs != 2) goto argcerror;
			rightarg = args[1];
			if (! is_character(rightarg)) goto argterror;
			rightint = int_value(rightarg);
		case 5: 
			if (i == 5 && numargs != 1) goto argcerror;
			leftarg = args[0];
			if (! is_character(leftarg)) goto argterror;
			leftint = int_value(leftarg);
			break;

		case 6: /* floating point operations */
			if (numargs != 2) goto argcerror;
			rightarg = args[1];
			if (! is_float(rightarg)) goto argterror;
			rightfloat = float_value(rightarg);
		case 7: 
			if (i == 7 && numargs != 1) goto argcerror;
		case 8:
			if (i == 8 && opnumber < 8 && numargs != 1) 
				goto argcerror;
			leftarg = args[0];
			if (! is_float(leftarg)) goto argterror;
			leftfloat = float_value(leftarg);
			break;

		case 9: /* symbol operations */
			leftarg = args[0];
			if (! is_symbol(leftarg)) goto argterror;
			leftp = symbol_value(leftarg);
			break;

		case 10: /* string operations */
			if (numargs < 1) goto argcerror;
			leftarg = args[0];
			if (! is_string(leftarg)) goto argterror;
			leftp = string_value(leftarg);
			if (opnumber && opnumber <= 3) {
				if (numargs != 2) goto argcerror;
				rightarg = args[1];
				if (! is_string(rightarg)) goto argterror;
				rightp = string_value(rightarg);
				}
			else if ((opnumber >= 4) && (opnumber <= 6)) {
				if (numargs < 2) goto argcerror;
				if (! is_integer(args[1])) goto argterror;
				i = int_value(args[1])-1;
				if ((i < 0) || (i >= strlen(leftp)))
					goto indexerror;
				}
			else if ((opnumber >= 7) && (numargs != 1))
				goto argcerror;
			break;

		case 11: /* misc operations */
			if ((opnumber == 1) || (opnumber == 2)) {
				if (is_bltin(args[0])) goto argterror;
				if (numargs < 2) goto argcerror;
				if (! is_integer(args[1])) goto argterror;
				i = int_value(args[1]);
				if (i < 1 || i > args[0]->size)
					goto indexerror;
				}
			else if ((opnumber >= 4) && (opnumber <= 6)) {
				if (numargs != 1) goto argcerror;
				if (! is_integer(args[0])) goto argterror;
				i = int_value(args[0]);
				if (i < 0) goto indexerror;
				}
			else if (opnumber >= 7) {
				if (numargs < 1) goto argcerror;
				if (! is_bytearray(args[0])) goto argterror;
				byarray = (bytearray *) args[0];
				if (opnumber >= 8) {
					if (numargs < 2) goto argcerror;
					if (! is_integer(args[1]))
						goto argterror;
					i = int_value(args[1]) - 1;
					if (i < 0 || i >= byarray->a_bsize)
						goto indexerror;
					}
				}
			break;

		case 12: /* string i/o operations */
			if (opnumber < 6) {
				if (numargs < 1) goto argcerror;
				leftarg = args[0];
				if (! is_string(leftarg)) goto argterror;
				leftp = string_value(leftarg);
				}
			break;

		case 13: /* operations on file */
			if (numargs < 1) goto argcerror;
			if (! is_file(args[0])) goto argterror;
			phil = (struct file_struct *) args[0];
			if (opnumber && (phil->fp == (FILE *) NULL)) {
				errp = "file must be open for operation";
				goto return_error;
				}
			break;

		case 15: /* operations on classes */
			if (opnumber < 3 && numargs != 1) goto argcerror;
			if (! is_class(args[0])) goto argterror;
			aClass = (class *) args[0];
			break;

# ifdef PLOT3
		case 17: /* plot(3) interface */
			if (opnumber && opnumber <= 3) {
				if (numargs != 2) goto argcerror;
				if ((! is_integer(args[0])) || 
					(! is_integer(args[1])))
					goto argterror;
				leftint = int_value(args[0]);
				rightint = int_value(args[1]);
				}
			else if ((opnumber == 6) || (opnumber == 7)) {
				if (numargs != 4) goto argcerror;
				for (i = 0; i < 4; i++)
					if (! is_integer(args[i]))
						goto argterror;
				leftint = int_value(args[0]);
				rightint = int_value(args[1]);
				i = int_value(args[2]);
				j = int_value(args[3]);
				}
			else if (opnumber >= 8) {
				if (numargs != 1) goto argcerror;
				if (! is_string(args[0])) goto argterror;
				leftp = string_value(args[0]);
				}
			break;
# endif
		}


	/* now do operation */
	switch(primnumber) {

		case 1:		/* class of object */
			resultobj = fnd_class(args[0]);
			if (resultobj) goto return_obj;
			else goto return_nil;

		case 2:		/* get super_object */
			resultobj = fnd_super(args[0]);
			if (resultobj) goto return_obj;
			else goto return_nil;

		case 3:		/* see if class responds to new */
			leftint = 0;
			if (! is_class(args[0])) goto return_boolean;
			leftint = responds_to("new", (class *) args[0]);
			goto return_boolean;

		case 4:		/* compute size of object */
			leftint = args[0]->size;
			goto return_integer;

		case 5:		/* return hashnum of object */
			if (is_integer(leftarg))
				leftint = int_value(leftarg);
			else if (is_character(leftarg))
				leftint = int_value(leftarg);
			else if (is_symbol(leftarg))
				leftint = *((int*)symbol_value(leftarg));
			else if (is_string(leftarg)) {
				leftp = string_value(leftarg);
				leftint = 0;
				for(i = 0; *leftp != 0; leftp++){
					leftint += *leftp;
					i++;
					if(i > 5)
					   break;
					}
				}
			else /* for all other objects return address */
				leftint = *((int*) &leftarg);
			if (leftint < 0)
				leftint = -leftint;
			goto return_integer;

		case 6:		/* built in object type testing */
			if (numargs != 2) goto argcerror;
			leftint = 0;
			if (is_bltin(args[0]) == is_bltin(args[1]))
				if (is_bltin(args[0]))
					leftint = (args[0]->size == args[1]->size);
				else leftint = (args[0]->class == args[1]->class);
			goto return_boolean;

		case 7:		/* object equality testing */
			if (numargs != 2) goto argcerror;
			leftint = (args[0] == args[1]);
			goto return_boolean;

		case 8:		/* toggle debugging flag */
			if (numargs == 0) {
				debug = 1 - debug;
				goto return_nil;
				}
			if (numargs != 2) goto argcerror;
			if (! is_integer(args[0])) goto argterror;
			if (! is_integer(args[1])) goto argterror;
			leftint = int_value(args[0]);
			rightint = int_value(args[1]);
			switch(leftint) {
				case 1: prntcmd = rightint; break;
				case 2: debug = rightint; break;
				}
			goto return_nil;

		case 9:		/* numerical generality comparison */
			if (numargs != 2) goto argcerror;
			leftint = 
				(generality(args[0]) > generality(args[1]));
			goto return_boolean;

		case 10:	/* integer addition */
			leftint += rightint;
			goto return_integer;

		case 11:	/* integer subtraction */
			leftint -= rightint;
			goto return_integer;

		case 12: case 42:
			leftint = (leftint < rightint);
			goto return_boolean;

		case 13: case 43:
			leftint = (leftint > rightint);
			goto return_boolean;

		case 14: case 44:
			leftint = (leftint <= rightint);
			goto return_boolean;

		case 15: case 45:
			leftint = (leftint >= rightint);
			goto return_boolean;

		case 16: case 46:
			leftint = (leftint == rightint);
			goto return_boolean;

		case 17: case 47:
			leftint = (leftint != rightint);
			goto return_boolean;

		case 18:
			leftint *= rightint;
			goto return_integer;

		case 19:	/* // integer */
			if (rightint == 0) goto numerror;
			i  = leftint / rightint;
			if ((leftint < 0) && (leftint % rightint))
				i -= 1;
			leftint = i;
			goto return_integer;

		case 20:	/* gcd of two integers */
			if (leftint == 0 || rightint == 0) goto numerror;
			if (leftint < 0) leftint = -leftint;
			if (rightint < 0) rightint = -rightint;
			if (leftint > rightint) 
				{i = leftint; leftint = rightint; rightint = i;}
			while (i = rightint % leftint)
				{rightint = leftint; leftint = i;}
			goto return_integer;
			
		case 21:	/* bitAt: */
			leftint = (leftint & (1 << rightint)) ? 1 : 0;
			goto return_integer;

		case 22:	/* logical bit-or */
			leftint |= rightint;
			goto return_integer;

		case 23:	/* logical bit-and */
			leftint &= rightint;
			goto return_integer;

		case 24:	/* logical bit-exclusive or */
			leftint ^= rightint;
			goto return_integer;

		case 25:	/* bit shift */
			if (rightint < 0)
				leftint >>= - rightint;
			else
				leftint <<= rightint;
			goto return_integer;

		case 26:	/* integer radix */
			if (rightint < 2 || rightint > 36) goto numerror;
			prnt_radix(leftint, rightint, strbuffer);
			goto return_string;

		case 28:
			if (rightint == 0) goto numerror;
			leftint /= rightint;
			goto return_integer;

		case 29:
			if (rightint == 0) goto numerror;
			leftint %= rightint;
			goto return_integer;

		case 30:	/* doPrimitive:withArguments: */
			if (numargs != 2) goto argcerror;
			resultobj = primitive(leftint, args[1]->size, 
				&args[1]->inst_var[0]);
			goto return_obj;

		case 32:	/* convert random int into random float */
			leftfloat = ((double) ((leftint/10) % 1000)) / 1000.0;
			goto return_float;

		case 33:	/* bit inverse */
			leftint ^= -1;
			goto return_integer;

		case 34:	/* highBit */
			rightint = leftint;
			for (leftint = 32; leftint >= 0; leftint--)
				if (rightint & (1 << leftint))
					goto return_integer;
			goto return_nil;

		case 35:	/* random number */
			srand(leftint);
			leftint = rand();
			goto return_integer;

		case 36:	/* convert integer to character */
			goto return_character;

		case 37:	/* convert integer to string */
			sprintf(strbuffer,"%d", leftint);
			goto return_string;

		case 38:	/* factorial */
			if (leftint < 0) goto numerror;
			if (leftint < FACTMAX) {
				for (i = 1; leftint; leftint--)
					i *= leftint;
				leftint = i;
				goto return_integer;
				}
# ifndef GAMMA
			/* gamma not supported, use float multiply */
			leftfloat = 1.0;
			if (leftint < 30) {
				for (i = 1; leftint; leftint--)
					leftfloat *= leftint;
				}
			goto return_float;
# endif
# ifdef GAMMA
			/* compute gamma */
			leftfloat = (double) (leftint + 1);
			sassign(leftarg, new_float(leftfloat));
			resultobj = primitive(GAMMAFUN, 1, &leftarg);
			obj_dec(leftarg);
			goto return_obj;
# endif

		case 39:	/* convert integer to float */
			leftfloat = (double) leftint;
			goto return_float;

		case 50:	/* digitValue */
			if (isdigit(leftint))
				leftint -= '0';
			else if (isupper(leftint)) {
				leftint -= 'A';
				leftint += 10;
				}
			else goto return_nil;
			goto return_integer;

		case 51:
			if (isupper(leftint)) leftint += 'a' - 'A';
			leftint = (leftint == 'a') || (leftint == 'e') ||
				  (leftint == 'i') || (leftint == 'o') ||
				  (leftint == 'u');
			goto return_boolean;

		case 52:
			leftint = isalpha(leftint);
			goto return_boolean;

		case 53:
			leftint = islower(leftint);
			goto return_boolean;

		case 54:
			leftint = isupper(leftint);
			goto return_boolean;

		case 55:
			leftint = isspace(leftint);
			goto return_boolean;

		case 56:
			leftint = isalnum(leftint);
			goto return_boolean;

		case 57:
			if (isupper(leftint)) leftint += 'a' - 'A';
			else if (islower(leftint)) leftint += 'A' - 'a';
			goto return_character;

		case 58:	/* convert character to string */
			sprintf(strbuffer,"%c", leftint);
			goto return_string;

		case 59:	/* convert character to integer */
			goto return_integer;

		case 60:	/* floating point addition */
			leftfloat += rightfloat;
			goto return_float;

		case 61:	/* floating point subtraction */
			leftfloat -= rightfloat;
			goto return_float;

		case 62:
			leftint = (leftfloat < rightfloat);
			goto return_boolean;

		case 63:
			leftint = (leftfloat > rightfloat);
			goto return_boolean;

		case 64:
			leftint = (leftfloat <= rightfloat);
			goto return_boolean;

		case 65:
			leftint = (leftfloat >= rightfloat);
			goto return_boolean;

		case 66:
			leftint = (leftfloat == rightfloat);
			goto return_boolean;

		case 67:
			leftint = (leftfloat != rightfloat);
			goto return_boolean;

		case 68:
			leftfloat *= rightfloat;
			goto return_float;

		case 69:
			if (rightfloat == 0) goto numerror;
			leftfloat /= rightfloat;
			goto return_float;

		case 70:
			leftfloat = log(leftfloat);
			goto float_check;

		case 71:
			if (leftfloat < 0) goto numerror;
			leftfloat = sqrt(leftfloat);
			goto float_check;

		case 72:
			leftint = (int) floor(leftfloat);
			goto return_integer;

		case 73:	/* ceiling */
			leftint = (int) ceil(leftfloat);
			goto return_integer;

		case 75:	/* integer part */
			leftfloat = modf(leftfloat, &rightfloat);
			leftint = (int) rightfloat;
			goto return_integer;

		case 76:	/* fractional part */
			leftfloat = modf(leftfloat, &rightfloat);
			goto return_float;

		case 77:	/* gamma function */
# ifdef GAMMA
			leftfloat = gamma(leftfloat);
			if (leftfloat > 88.0) goto numerror;
			leftfloat = exp(leftfloat);
			goto float_check;
# endif
# ifndef GAMMA
			errp = "gamma function";
			goto not_implemented;
# endif

		case 78:
			sprintf(strbuffer,"%g", leftfloat);
			goto return_string;

		case 79:
			leftfloat = exp(leftfloat);
			goto return_float;

		case 80:	/* normalize radian value */
# define TWOPI (double) 6.2831853072
			rightfloat = 
			floor(((leftfloat < 0) ? -leftfloat:leftfloat) / TWOPI);
			if (leftfloat < 0)
				leftfloat += (1 + rightfloat) * TWOPI;
			else
				leftfloat -= rightfloat * TWOPI;
			goto return_float;

		case 81:
			leftfloat = sin(leftfloat);
			goto float_check;

		case 82:
			leftfloat = cos(leftfloat);
			goto float_check;

		case 84:
			leftfloat = asin(leftfloat);
			goto float_check;

		case 85:
			leftfloat = acos(leftfloat);
			goto float_check;

		case 86:
			leftfloat = atan(leftfloat);
			goto float_check;

		case 88:
			if (numargs != 2) goto argcerror;
			if (! is_float(args[1])) goto argterror;
			leftfloat = pow(leftfloat, float_value(args[1]));
			goto float_check;

		case 89:	/* floating point radix */
			if (numargs != 2) goto argcerror;
			if (! is_integer(args[1])) goto argterror;
			i = int_value(args[1]); /* base */
			if (i < 2 || i > 36) goto numerror;
			fprnt_radix(leftfloat, i, strbuffer);
			goto return_string;

		case 91:	/* symbol comparison */
			if (numargs != 2) goto argcerror;
			if (! is_symbol(args[1])) goto argterror;
			leftint = (leftp == symbol_value(args[1]));
			goto return_boolean;

		case 92:	/* symbol printString */
			sprintf(strbuffer, "#%s", leftp);
			goto return_string;

		case 93:	/* symbol asString */
			sprintf(strbuffer, "%s", leftp);
			goto return_string;

		case 94:	/* symbol print ( with tabs) */
			if (numargs == 2) {
				if (! is_integer(args[1])) goto argterror;
				for (i = int_value(args[1]); i >= 0; i--)
					putchar('\t');
			}
			printf("%s\n", leftp);
# ifdef FLUSHREQ
			fflush(stdout);
# endif
			goto return_nil;

		case 96:
			goto return_nil;

		case 97:	/* make a new class (generated by parser)*/
			if (numargs != 8) goto argcerror;
			if (! is_symbol(args[1])) goto argterror;
			if (! is_symbol(args[2])) goto argterror;
			if (! is_integer(args[6])) goto argterror;
			if (! is_integer(args[7])) goto argterror;
			resultobj = (object *) mk_class(leftp, args);
			goto return_obj;

		case 98:	/* install class in dictionary */
			if (numargs != 2) goto argcerror;
			if (! is_class(args[1])) goto argterror;
			enter_class(leftp, args[1]);
			goto return_nil;

		case 99:	/* find a class in class dictionary */
			if (numargs != 1) goto argcerror;
			resultobj = lookup_class(leftp);
			if (resultobj == (object *) 0) {
				sprintf(strbuffer,"cannot find class %s",
				leftp);
				sassign(resultobj, new_str(strbuffer));
				primitive(ERRPRINT, 1, &resultobj);
				obj_dec(resultobj);
				resultobj = lookup_class("Object");
				if (! resultobj) cant_happen(7);
				}
			goto return_obj;

		case 100:	/* string length */
			leftint = strlen(leftp);
			goto return_integer;

		case 101: 	/* string compare, case dependent */
			leftint = strcmp(leftp, rightp);
			goto return_integer;

		case 102:	/* string compare, case independent */
			leftint = 1;
			while (*leftp || *rightp) {
				i = *leftp++;
				j = *rightp++;
				if (i >= 'A' && i <= 'Z')
					i = i - 'A' + 'a';
				if (j >= 'A' && j <= 'Z')
					j = j - 'A' + 'a';
				if (i != j) {leftint = 0; break;}
				}
			goto return_boolean;

		case 103: 	/* string catenation */
			for (i = leftint = 0; i < numargs; i++) {
				if (! is_string(args[i])) goto argterror;
				leftint += strlen(string_value(args[i]));
				}
			errp = (char *) o_alloc((unsigned) (1 + leftint));
			*errp = '\0';
			for (i = 0; i < numargs; i++)
				strcat(errp, string_value(args[i]));
			resultobj = (object *) new_istr(errp);
			goto return_obj;

		case 104:	/* string at: */
			if (numargs != 2) goto argcerror;
			leftint = leftp[i];
			goto return_character;

		case 105:	/* string at: put: */
			if (numargs != 3) goto argcerror;
			if (! is_character(args[2])) goto argterror;
			leftp[i] = int_value(args[2]);
			goto return_nil;

		case 106:	/* copyFrom: length: */
			if (numargs != 3) goto argcerror;
			if (! is_integer(args[2])) goto argterror;
			j = int_value(args[2]);
			if (j < 0) goto indexerror;
			for (rightp = strbuffer; j; j--, i++)
				*rightp++ = leftp[i];
			*rightp = '\0';
			goto return_string;

		case 107:	/* string copy */
			resultobj = new_str(leftp);
			goto return_obj;

		case 108:	/* string asSymbol */
			resultobj = new_sym(leftp);
			goto return_obj;

		case 109:	/* string printString */
			sprintf(strbuffer,"\'%s\'", leftp);
			goto return_string;

		case 110:	/* new untyped object */
			if (numargs != 1) goto argcerror;
			if (! is_integer(args[0])) goto argterror;
			leftint = int_value(args[0]);
			if (leftint < 0) goto numerror;
			resultobj = new_obj((class *) 0, leftint, 1);
			goto return_obj;

		case 111:	/* object at: */
			if (numargs != 2) goto argcerror;
			resultobj = args[0]->inst_var[ i - 1 ];
			goto return_obj;

		case 112:	/* object at:put: */
			if (numargs != 3) goto argcerror;
			assign(args[0]->inst_var[i - 1], args[2]);
			goto return_nil;

		case 113:	/*  object grow */
			leftarg = args[0];
			rightarg = args[1];
			if (is_bltin(leftarg)) goto argterror;
			resultobj = new_obj(leftarg->class,
				leftarg->size+1, 0);
			if (leftarg->super_obj)
				sassign(resultobj->super_obj,
					leftarg->super_obj);
			for (i = 0; i < leftarg->size; i++)
				sassign(resultobj->inst_var[i], leftarg->inst_var[i]);
			sassign(resultobj->inst_var[i], rightarg);
			goto return_obj;


		case 114:	/* new array */
			resultobj = new_array(i, 1);
			goto return_obj;

		case 115:	/* new string */
			for (j = 0; j < i; j++)
				strbuffer[j] = ' ';
			strbuffer[j] = '\0';
			goto return_string;

		case 116:	/* bytearray new */
			/* initialize with random garbage */
			resultobj = new_bytearray(strbuffer, i);
			goto return_obj;

		case 117:	/* bytearray size */
			if (numargs != 1) goto argcerror;
			leftint = byarray->a_bsize;
			goto return_integer;

		case 118:	/* bytearray at: */
			if (numargs != 2) goto argcerror;
			leftint = uctoi(byarray->a_bytes[i]);
			goto return_integer;

		case 119:	/* bytearray at:put: */
			if (numargs != 3) goto argcerror;
			if (! int_value(args[2])) goto argterror;
			byarray->a_bytes[i] = itouc(int_value(args[2]));
			goto return_nil;

		case 120:	/* print, no return */
			printf("%s", leftp);
# ifdef FLUSHREQ
			fflush(stdout);
# endif
			goto return_nil;

		case 121:	/* print, with return */
			printf("%s\n", leftp);
# ifdef FLUSHREQ
			fflush(stdout);
# endif
			goto return_nil;

		case 122:	/* format for error printing */
			aClass = (class *) fnd_class(args[1]);
			sprintf(strbuffer,"%s: %s",
				symbol_value(aClass->class_name), leftp);
			leftp = strbuffer;

		case 123:	/* print on error output */
			fprintf(stderr,"%s\n", leftp);
# ifdef FLUSHREQ
			fflush(stderr);
# endif
			goto return_nil;

		case 125:	/* unix system call */
# ifndef NOSYSTEM
			leftint = system(leftp);
			goto return_integer;
# endif
# ifdef NOSYSTEM
			errp = "system()";
			goto not_implemented;
# endif

		case 126:	/* printAt: */
# ifndef CURSES
			errp = "curses graphics package not available";
			goto return_error;
# endif
# ifdef CURSES
			if (numargs != 3) goto argcerror;
			if ((! is_string(args[0])) ||
				(! is_integer(args[1])) ||
				(! is_integer(args[2])) ) goto argterror;
			move(int_value(args[1]), int_value(args[2]));
			addstr(string_value(args[0]));
			refresh();
			move(0, LINES-1);
			goto return_nil;
# endif

		case 127:	/* block return */
			errp = "block return without surrounding context";
			goto return_error;

		case 128: /* reference count error */
			if (numargs != 1) goto argcerror;
			sprintf(strbuffer,"object %p reference count %d",
				args[0], args[0]->ref_count);
			errp = strbuffer;
			goto return_error;

		case 129: /* does not respond error */
			if (numargs != 2) goto argcerror;
			if (! is_symbol(args[1])) goto argterror;
			fprintf(stderr,"respond error: %s\n",
			symbol_value(args[1]));
			aClass = (class *) fnd_class(args[0]);
			if (! is_class(aClass)) goto argterror;
			sprintf(strbuffer,"%s: does not respond to %s",
				symbol_value(aClass->class_name), 
				symbol_value(args[1]));
			errp = strbuffer;
			goto return_error;

		case 130:	/* file open */
			if (numargs != 3) goto argcerror;
			if (! is_string(args[1])) goto argterror;
			if (! is_string(args[2])) goto argterror;
			file_open(phil, 
				string_value(args[1]), string_value(args[2]));
			goto return_nil;

		case 131:	/* file read */
			if (numargs != 1) goto argcerror;
			resultobj = file_read(phil);
			goto return_obj;

		case 132:	/* file write */
			if (numargs != 2) goto argcerror;
			file_write(phil, args[1]);
			goto return_nil;

		case 133:	/* set file mode */
			if (numargs != 2) goto argcerror;
			if (! is_integer(args[1])) goto argterror;
			phil->file_mode = int_value(args[1]);
			goto return_nil;

		case 134:	/* compute file size */
			fseek(phil->fp, (long) 0, 2);
			leftint = (int) ftell(phil->fp);
			goto return_integer;

		case 135:	/* set file position */
			if (numargs != 2) goto argcerror;
			if (! is_integer(args[1])) goto argterror;
			leftint = fseek(phil->fp, (long) int_value(args[1]), 0);
			goto return_integer;

		case 136:	/* find current position */
			if (numargs != 1) goto argcerror;
			leftint = (int) ftell(phil->fp);
			goto return_integer;

		case 140:
			errp = "block execute should be trapped by interp";
			goto return_error;

		case 141:	/* newProcess (withArguments:) */
			if (numargs < 1) goto argcerror;
			if (! is_block(args[0])) goto argterror;
			if (numargs == 1)
				resultobj = (object *)
					block_execute((interpreter *) 0, 
					(block *) args[0], 0, args);
			else if (numargs == 2)
				resultobj = (object *) 
					block_execute((interpreter *) 0, 
					(block *) args[0], args[1]->size,
					&(args[1]->inst_var[0]));
			else goto argcerror;
			if (((object *) 0) == resultobj) goto return_nil;
			resultobj = (object *) cr_process(resultobj);
			goto return_obj;

		case 142:	/* terminate a process */
			if (numargs != 1) goto argcerror;
			if (! is_process(args[0])) goto argterror;
			terminate_process( (process *) args[0]);
			goto return_nil;

		case 143:	/* perform:withArguments: */
			errp = "perform should be trapped by interpreter";
			goto return_error;

		case 145:	/* set the state of a process */
			if (numargs != 2) goto argcerror;
			if (! is_process(args[0])) goto argterror;
			if (! is_integer(args[1])) goto argterror;
			leftint = int_value(args[1]);
			switch (leftint) {
				case 0:	leftint = READY;
					break;
				case 1:	leftint = SUSPENDED;
					break;
				case 2:	leftint = BLOCKED;
					break;
				case 3:	leftint = UNBLOCKED;
					break;
				default:  errp = "invalid state for process";
					  goto return_error;

				}
			set_state((process *) args[0], leftint);
			goto return_integer;

		case 146:	/* return the state of a process */
			if (numargs != 1) goto argcerror;
			if (! is_process(args[0])) goto argterror;
			leftint = set_state((process *) args[0], CUR_STATE);
			goto return_integer;

		case 148:	/* begin atomic action */
			if (numargs != 0) goto argcerror;
			atomcnt++;
			goto return_nil;

		case 149:	/* end atomic action */
			if (numargs != 0) goto argcerror;
			if (atomcnt == 0) {
				errp = "end atomic attempted while not in atomic action";
				goto return_error;
				}
			atomcnt--;
			goto return_nil;

		case 150:	/* class edit */
			leftp = symbol_value(aClass->file_name);
			if (! writeable(leftp)) {
				gettemp(tempname);
				sprintf(strbuffer,"cp %s %s", leftp, tempname);
# ifndef NOSYSTEM
				system(strbuffer);
# endif
				leftp = tempname;
				}
			if (! lexedit(leftp)) lexinclude(leftp);
			goto return_nil;

		case 151: 	/* superclass of a class */
			if (! aClass->super_class)
				goto return_nil;
			resultobj = (object *) aClass->super_class;
			if (! is_symbol(resultobj)) goto return_nil;
			resultobj = lookup_class(symbol_value(resultobj));
			if (! resultobj) goto return_nil;
			goto return_obj;

		case 152: /* class name */
			resultobj = aClass->class_name;
			leftp = symbol_value(resultobj);
			resultobj = new_str(leftp);
			goto return_obj;

		case 153: /* new */
			if (numargs != 2) goto argcerror;
			if (args[1] == o_nil)
				resultobj = new_inst(aClass);
			else
				resultobj = new_sinst(aClass, args[1]);
			goto return_obj;

		case 154:	/* print message names list */
			prnt_messages(aClass);
			goto return_nil;

		case 155: 	/* respondsTo: aMessage  */
			if (numargs != 2) goto argcerror;
			if (! is_symbol(args[1])) goto argterror;
			leftint = responds_to(symbol_value(args[1]), aClass);
			goto return_boolean;

		case 156:	/* class view */
# ifndef NOSYSTEM
			leftp = symbol_value(aClass->file_name);
			gettemp(tempname);
			sprintf(strbuffer,"cp %s %s", leftp, tempname);
			system(strbuffer);
			leftp = tempname;
			lexedit(leftp);
			goto return_nil;
# endif
# ifdef NOSYSTEM
			errp = "cannot view classes on this system";
			goto return_error;
# endif

		case 157:	/* class list */
			class_list(aClass, 0);
			goto return_nil;


		case 158:	/* variables */
			resultobj = aClass->c_inst_vars;
			goto return_obj;

		case 160:	/* current time */
// FIXME
//			time(&clock);
//                        ctime_r(&clock,strbuffer);
			goto return_string;

		case 161:	/* time, measure in seconds */
			leftint = (int) time((long *) 0);
			goto return_integer;

		case 162:	/* clear screen */
# ifdef CURSES
			clear();
			move(0,0);
			refresh();
# endif
# ifdef PLOT3
			erase();
# endif
			goto return_nil;

		case 163:	/* getString */
			gets(strbuffer);
			goto return_string;

		case 164:	/* string asInteger */
			if (! is_string(args[0])) goto argterror;
			leftint = atoi(string_value(args[0]));
			goto return_integer;

		case 165:	/* string asFloat */
			if (! is_string(args[0])) goto argterror;
			leftfloat = atof(string_value(args[0]));
			goto return_float;

# ifdef PLOT3

/**************************
	warning - the calls on the plot(3) routines are very device
	specific, and will probably require changes to work on any one
	particular new device
**********************************/
		case 170:	/* clear */
			erase();
			goto return_nil;

		case 171:	/* move(x,y) */
			move(leftint, rightint);
			goto return_nil;

		case 172:	/* cont(x,y) (draw line) */
			cont(leftint, rightint);
			goto return_nil;

		case 173:	/* point(x,y) (draw point) */
			point(leftint, rightint);
			goto return_nil;

		case 174:	/* circle(x, y, r) */
			if (numargs != 3) goto argcerror;
			for (i = 0; i < 3; i++)
				if (! is_integer(args[i])) 
					goto argterror;
			circle(int_value(args[0]), int_value(args[1]),
				int_value(args[2]));
			goto return_nil;

		case 175:	/* arg(x, y, x0, y0, x1, y1) */
			if (numargs != 6) goto argcerror;
			for (i = 0; i < 6; i++)
				if (! is_integer(args[i])) goto argterror;
			arc(int_value(args[0]), int_value(args[1]),
				int_value(args[2]), int_value(args[3]),
				int_value(args[4]), int_value(args[5]));
			goto return_nil;

		case 176:	/* space */
			space(leftint, rightint, i, j);
			goto return_nil;

		case 177:	/* line */
			line(leftint, rightint, i, j);
			goto return_nil;

		case 178:	/* label */
			label(leftp);
			goto return_nil;

		case 179:	/* linemod */
			linemod(leftp);
			goto return_nil;
# endif

		default: fprintf(stderr,"Primitive number %d not implemented\n",
						primnumber);
			goto return_nil;
	}

/* return different types of objects */

return_obj:

	return(resultobj);

return_nil:

	return(o_nil);

return_integer:

	return(new_int(leftint));

return_character:

	return(new_char(leftint));

return_boolean:

	return(leftint ? o_true : o_false);

float_check:

	if (errno == ERANGE || errno == EDOM) goto numerror;

return_float:

	return(new_float(leftfloat));

return_string:

	return(new_str(strbuffer));

/* error conditions */

not_implemented:
	sprintf(strbuffer,"%s not implemented yet", errp);
	errp = strbuffer;
	goto return_error;

argcerror:
	sprintf(strbuffer,"%d is wrong number of arguments for primitive %d",
		numargs, primnumber);
	errp = strbuffer;
	goto return_error;

argterror:
	sprintf(strbuffer,"argument type not correct for primitive %d",
		primnumber);
	errp = strbuffer;
	goto return_error;

numerror:
	errp = "numerical error in primitive"; 
	goto return_error;

indexerror:
	errp = "primitive index error";
	goto return_error;

return_error:
	sassign(resultobj, new_str(errp));
	primitive(ERRPRINT, 1, &resultobj);
	obj_dec(resultobj);
	goto return_nil;
}

 prnt_radix(n, r, buffer)
int n, r;
char buffer[];
{  char *p, *q, buffer2[60];
   int i, s;

   if (n < 0) {n = - n; s = 1;}
   else s = 0;
   p = buffer2; *p++ = '\0';
   if (n == 0) *p++ = '0';
   while (n) {
      i = n % r;
      *p++ = i + ((i < 10) ?  '0' : ('A' - 10));
      n = n / r;
      }
   sprintf(buffer,"%dr", r);
   for (q = buffer; *q; q++);
   if (s) *q++ = '-';
   for (*p = '0' ; *p ; ) *q++ = *--p;
   *q = '\0';
}

int fprnt_radix(f, n, buffer)
double f;
int n;
char buffer[];
{	int sign, exp, i, j;
	char *p, *q, tempbuffer[60];
	double ip;

	if (f < 0) {
		sign = 1;
		f = - f;
		}
	else sign = 0;
	exp = 0;
	if (f != 0) {
		exp = (int) floor(log(f) / log((double) n));
		if (exp < -4 || 4 < exp) {
			f *= pow((double) n, (double) - exp);
			}
		else exp = 0;
		}
	f = modf(f, &ip);
	if (sign) ip = - ip;
	prnt_radix((int) ip, n, buffer);
	for (p = buffer; *p; p++) ;
	if (f != 0) {
		*p++ = '.';
		for (j = 0; (f != 0) && (j < 6); j++){
			i = (int) (f *= n);
			*p++ = (i < 10) ? '0' + i : 'A' + (i-10) ;
			f -= i;
			}
		}
	if (exp) {
		*p++ = 'e';
		sprintf(tempbuffer,"%d", exp);
		for (q = tempbuffer; *q; )
			*p++ = *q++;
		}
	*p = '\0';
	return 0;
}

/* generalit - numerical generality */
 int generality(aNumber)
object *aNumber;
{	int i;

	if (is_integer(aNumber)) i = 1;
	else if (is_float(aNumber)) i = 2;
	else i = 3;
	return(i);
}

/* cant_happen - report that an impossible condition has occured */
cant_happen(n) int n;
{   char *s;

# ifdef SMALLDATA
	s = "what a pain!";
# endif
# ifndef SMALLDATA
    switch(n) {
       case 1:  s = "out of memory allocation space"; break;
       case 2:  s = "array size less than zero"; break;
       case 3:  s = "block return from call should not occur"; break;
       case 4:  s = "attempt to make instance of non class"; break;
       case 5:  s = "case error in new integer or string"; break;
       case 6:  s = "decrement on unknown built in object"; break;
       case 7:  s = "cannot find class Object"; break;
       case 8:  s = "primitive free of object of wrong type"; break;
       case 9:  s = "internal interpreter error"; break;
       case 11: s = "block execute on non-block"; break;
       case 12: s = "out of symbol space"; break;
       case 14: s = "out of standard bytecode space"; break;
       case 15: s = "system deadlocked - all processes blocked"; break;
       case 16: s = "attempt to free symbol"; break;
       case 17: s = "invalid process state passed to set_state"; break;
       case 18: s = "internal buffer overflow"; break;
       case 20: s = "can't open prelude file"; break;
       case 22: s = "system file open error"; break;
       case 23: s = "fastsave error"; break;
       default: s = "unknown, but impossible nonetheless, condition"; break;
       }
# endif
   fprintf(stderr,"Can't happen number %d: %s\n", n, s);
   exit(1);
}

/* writeable - see if a file can be written to */
int writeable(name)
char *name;
{	char buffer[150];

	sprintf(buffer,"test -w %s", name);
# ifdef NOSYSTEM
	return(0);
# endif
# ifndef NOSYSTEM
	return(! system(buffer));
# endif
}
