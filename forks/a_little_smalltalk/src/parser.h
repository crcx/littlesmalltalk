/*
 * File:   parser.h
 * Author: oni
 *
 * Created on 31. August 2012, 21:05
 */

#ifndef PARSER_H
#define	PARSER_H

void setInstanceVariables(object aClass);

boolean parse(object method, const char *text, boolean savetext);

#endif	/* PARSER_H */

