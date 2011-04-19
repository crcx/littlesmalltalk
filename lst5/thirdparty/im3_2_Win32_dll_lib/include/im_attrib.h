/** \file
 * \brief Attributes Table.
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_attrib.h,v 1.2 2006/11/21 11:56:16 scuri Exp $
 */

#ifndef __IM_ATTRIB_H_
#define __IM_ATTRIB_H_

#include "im_attrib_flat.h"

/** \brief Attributes Table.
 *
 * \par
 * All the attributes have a name, a type, a count and the data.\n
 * Names are usually strings with less that 30 chars.
 * \par
 * Attributes are stored in a hash table for fast access. \n
 * We use the hash function described in "The Pratice of Programming" of Kernighan & Pike.
 * \ingroup util */
class imAttribTable
{
  imAttribTablePrivate* ptable; 
public:

  /** Creates an empty table.
   * If size is zero the default size of 101 is used. Size must be a prime number. 
   * Other common values are 67, 599 and 1499.*/ 
  imAttribTable(int hash_size)
    { ptable = imAttribTableCreate(hash_size); }

  /** Destroys the table and all the attributes. */
  ~imAttribTable()
    { imAttribTableDestroy(ptable); ptable = 0; }

  /** Returns the number of elements in the table. */
  int Count() const
    { return imAttribTableCount(ptable); }

  /** Removes all the attributes in the table */
  void RemoveAll()
    { imAttribTableRemoveAll(ptable); }

  /** Copies the contents of the given table into this table. */
  void CopyFrom(const imAttribTable& table)
    { imAttribTableCopyFrom(ptable, table.ptable); }

  /** Inserts an attribute into the table. \n 
   * Data is duplicated if not NULL, else data is initialized with zeros.
   * See also \ref imDataType. */
  void Set(const char* name, int data_type, int count, const void* data)
    { imAttribTableSet(ptable, name, data_type, count, data); }

  /** Removes an attribute from the table given its name. */
  void UnSet(const char *name)
    { imAttribTableUnSet(ptable, name); }

  /** Finds an attribute in the table.
   * Returns the attribute if found, NULL otherwise. 
   * See also \ref imDataType. */
  const void* Get(const char *name, int *data_type = 0, int *count = 0) const
    { return imAttribTableGet(ptable, name, data_type, count); }

  /** For each attribute calls the user callback. If the callback returns 0 the function returns. */
  void ForEach(void* user_data, imAttribTableCallback attrib_func) const
    { imAttribTableForEach(ptable, user_data, attrib_func); }
};

/** \brief Attributes Table.
 *
 * \par
 * Same as \ref imAttribTable, but uses an array of fixed size.
 * \ingroup util */
class imAttribArray
{
  imAttribTablePrivate* ptable; 
public:

  /** Creates an empty array. */ 
  imAttribArray(int count)
    { ptable = imAttribArrayCreate(count); }

  /** Destroys the array and all the attributes. */
  ~imAttribArray()
    { imAttribTableDestroy(ptable); ptable = 0; }

  /** Returns the number of elements in the array. */
  int Count() const
    { return imAttribTableCount(ptable); }

  /** Removes all the attributes in the array */
  void RemoveAll()
    { imAttribTableRemoveAll(ptable); }

  /** Copies the contents of the given table into this table. */
  void CopyFrom(const imAttribArray& table)
    { imAttribArrayCopyFrom(ptable, table.ptable); }

  /** Inserts an attribute into the array. \n 
   * Data is duplicated if not NULL, else data is initialized with zeros.
   * See also \ref imDataType. */
  void Set(int index, const char* name, int data_type, int count, const void* data)
    { imAttribArraySet(ptable, index, name, data_type, count, data); }

  /** Finds an attribute in the array.
   * Returns the attribute if found, NULL otherwise.
   * See also \ref imDataType. */
  const void* Get(int index, char *name = 0, int *data_type = 0, int *count = 0) const
    { return imAttribArrayGet(ptable, index, name, data_type, count); }

  /** For each attribute calls the user callback. If the callback returns 0 the function returns. */
  void ForEach(void* user_data, imAttribTableCallback attrib_func) const
    { imAttribTableForEach(ptable, user_data, attrib_func); }
};

#endif
