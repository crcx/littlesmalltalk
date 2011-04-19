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

#include <stdlib.h>
#include <string.h>

#include "rss_queue.h"

static int rss_queueResize(RSS_Queue queue, int newSize);

struct RSS_Queue_Struct
{
  int    maxSize;
  int    size;
  int    firstIndex;
  int    lastIndex;
  void **queue;
};


RSS_Queue rss_queueCreate(int initialSize)
{
  RSS_Queue q;

  q = (RSS_Queue)malloc(sizeof(struct RSS_Queue_Struct));
  if(!q)
    return 0;

  q->maxSize    = 0;
  q->size       = 0;
  q->firstIndex = 0;
  q->lastIndex  = 0;
  q->queue      = 0;

  if(!rss_queueResize(q, initialSize ? initialSize : 100))
  {
    free(q);
    q = 0;
  }

  return q;
}


void rss_queueRelease(RSS_Queue queue)
{
  if(!queue)
    return;

  if(queue->queue)
    free(queue->queue);

  free(queue);
}


static int rss_queueResize(RSS_Queue queue, int newSize)
{
  void **newQueue;
  int    i;

  if(!queue)
    return 0;

  newQueue = (void **)malloc(newSize * sizeof(void *));
  if(!newQueue)
    return 0;

  if(queue->queue)
  {
    for(i = 0; i < queue->size; i++)
      newQueue[i] = queue->queue[(queue->firstIndex + i) % queue->maxSize];

    free(queue->queue);
  }

  queue->queue      = newQueue;
  queue->firstIndex = 0;
  queue->lastIndex  = queue->size;
  queue->maxSize    = newSize;

  return 1;
}



int rss_queueSize(RSS_Queue queue)
{
  if(!queue)
    return -1;

  return queue->size;
}


void *rss_queueGet(RSS_Queue queue)
{
  void *element;

  if(!queue)
    return 0;

  if(queue->size <= 0)
    return 0;

  element = queue->queue[queue->firstIndex];
  queue->firstIndex++;
  if(queue->firstIndex >= queue->maxSize)
    queue->firstIndex -= queue->maxSize;

  queue->size--;

  return element;
}


int rss_queuePut(RSS_Queue queue, void *element)
{
  if(!queue)
    return 0;

  if(queue->size >= queue->maxSize)
  {
    if(!rss_queueResize(queue, queue->maxSize + (queue->maxSize / 2) + 1))
      return 0;
  }

  queue->queue[queue->lastIndex] = element;
  queue->lastIndex++;
  if(queue->lastIndex >= queue->maxSize)
    queue->lastIndex -= queue->maxSize;

  queue->size++;

  return 1;
}


int rss_queuePutString(RSS_Queue queue, const char *element)
{
  return rss_queuePut(queue, element ? ((char*)strdup(element)) : ((char *)0));
}
