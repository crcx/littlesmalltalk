/** \file
 * \brief Processing Counter
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_counter.h,v 1.1 2005/04/02 22:07:00 scuri Exp $
 */

#ifndef __IM_COUNTER_H
#define __IM_COUNTER_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup counter Counter 
 * \par
 * Used to notify the application that a step in the loading, saving or processing operation has been performed.
 * \par
 * See \ref im_counter.h
 * \ingroup util */

/** Counter callback, informs the progress of the operation to the client. \n
 * Text contains a constant string that is NULL during normal counting, a title in the begining of a sequence 
 * and a message in the begining of a count.
 * Counter id identifies diferrent counters. \n
 * Progress in a count reports a value from 0 to 1000. If -1 indicates the start of a sequence of operations, 1001 ends the sequence. \n
 * If returns 0 the client should abort the operation. \n
 * If the counter is aborted, the callback will be called one last time at 1001.
 * \ingroup counter */
typedef int (*imCounterCallback)(int counter, void* user_data, const char* text, int progress);

/** Changes the counter callback. Returns old callback. \n
 * User data is changed only if not NULL.
 * \ingroup counter */
imCounterCallback imCounterSetCallback(void* user_data, imCounterCallback counter_func);

/** Begins a new count, or a partial-count in a sequence. \n
 * Calls the callback with "-1" and text=title, if it is at the top level. \n     
 * This is to be used by the operations. Returns a counter Id.
 * \ingroup counter */
int imCounterBegin(const char* title);

/** Ends a count, or a partial-count in a sequence. \n
 * Calls the callback with "1001", text=null, and releases the counter if it is at top level count. \n
 * \ingroup counter */
void imCounterEnd(int counter);

/** Increments a count. Must set the total first. \n
 * Calls the callback, text=message if it is the first increment for the count. \n
 * Returns 0 if the callback aborted, 1 if returns normally.
 * \ingroup counter */
int imCounterInc(int counter);

/** Sets the total increments of a count.
 * \ingroup counter */
void imCounterTotal(int counter, int total, const char* message);


#if defined(__cplusplus)
}
#endif

#endif
