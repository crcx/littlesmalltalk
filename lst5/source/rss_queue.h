/* $Id: $
 *
 * LittleSmalltalk system
 * Originally written by Timothy Budd.
 * Timothy Budd allowed me (Danny Reinhold) to replace
 * his original copyright notice and to push a
 * LittleSmalltalk project.
 *
 * The new LittleSmalltalk distribution is licensed
 * as follows:
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions: 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software. 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE. 
 *
 * Author of the original version: Timothy Budd
 * Maintainer after version 4: Danny Reinhold
 *
 * For more detailed information visit our web site:
 * http://www.littlesmalltalk.org
 */


#ifndef RSS_QUEUE_H
#define RSS_QUEUE_H


#ifdef __cplusplus
extern "C"
{
#endif


struct RSS_Queue_Struct;
typedef struct RSS_Queue_Struct *RSS_Queue;


RSS_Queue rss_queueCreate(int initialSize);
void      rss_queueRelease(RSS_Queue queue);

/* returns the current number of elements in the
 * queue
 */
int       rss_queueSize(RSS_Queue queue);


/* the client is responsible for allocation/deallocation
 * of the element's memory
 */
void     *rss_queueGet(RSS_Queue queue);
int       rss_queuePut(RSS_Queue queue, void *element);


/* this function copies the specified string element.
 * the user has to free it after calling rss_queueGet()...
 */
int       rss_queuePutString(RSS_Queue queue, const char *element);


#ifdef __cplusplus
}
#endif


#endif


