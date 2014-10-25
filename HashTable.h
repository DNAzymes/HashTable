/* ========================================================================= *
 * HashTable
 *
 * HashTables are associative containers that store elements as key-value
 * pairs.
 * ========================================================================= */

#ifndef _HASHTABLE_H_
# define _HASHTABLE_H_

#include <stddef.h>
#include <stdbool.h>

/* Opaque Structure */
typedef struct HashTable_t HashTable;

/* ------------------------------------------------------------------------- *
 * Create a new empty HashTable object of initial capacity $capacity$.
 *
 * A hash table is based on a hash function that maps an input key
 * to a positive integer value. As the hash function strongly depends on the
 * nature of input data, the user has to provide a compatible hash function
 * ($hashFunction$).
 *
 * Moreover, in order to compare keys, the user has to provide a second
 * function ($compareFunction$) that compares the content of two keys. In
 * the interface defined below, two keys $a$ and $b$ are said to be equivalent
 * (i.e., equal) if $compareFunction(a, b)$ == 0.
 *
 * The HashTable object must later be deleted by calling freeHashTable().
 *
 * PARAMETERS
 * capacity         Initial container size.
 * hashFunction     Pointer to a hash function. The hash function returns the
 *                  hash value of a key $key$ as a positive integer.
 * compareFunction  Pointer to the comparison function. This function returns
 *                  an integer less than, equal to, or greater than zero if $a$
 *                  is found, respectively, to be less than, to match, or be
 *                  greater than $b$.
 *
 * RETURN
 * HashTable    A new pointer to a HashTable object.
 * NULL         If an error has occured. The function does not produces any
 *              error message.
 *
 * EXAMPLE

   // Comparison function for keys of type char*
   int compareString(const void* a, const void* b)
   {
     return strcmp((char*)a, (char*)b);
   }

   // Naive hash function keys of type char*
   size_t naiveHashString(const void* key)
   {
     const char* str = (char*)key;
     if (strlen(str))
       return str[0];
     return 0;
   }

   HashTable* myHashTable = createHashTable(10, naiveHashString, compareString);
   size_t myHashValue = naiveHashString("Hello World !");
   int isEqual = compareString("Hello", "World");
 * ------------------------------------------------------------------------- */
HashTable* createHashTable(size_t capacity,
                           size_t (*hashFunction)(const void* key),
                           int (*compareFunction)(const void* a, const void* b));

/* ------------------------------------------------------------------------- *
 * Free the allocated memory of the HashTable object $hashTable$ but not its
 * key-value elements.
 *
 * PARAMETERS
 * hashTable       An HashMap object
 * ------------------------------------------------------------------------- */
void freeHashTable(HashTable* hashTable);

/* ------------------------------------------------------------------------- *
 * Insert a new key-value pair into the HashTable object $hashTable$.
 * Because keys in a HashTable are unique, an insertion overrides the
 * the value associated to a key if an equivalent key is already contained in
 * in the HashTable $hashTable$. In such case, the function returns the old
 * value.
 *
 * Note that the function copies adresses of keys and values, but does not
 * clone their contents.
 *
 * PARAMETERS
 * hashTable       An HashMap object
 * key             An key value
 * value           The value associated to $key$
 *
 * RETURN
 * old value       Pointer to the value previously associated to $key$
 * NULL            Otherwise. NULL may also be returned on error. The function
 *                 does not produces any error message.
 * ------------------------------------------------------------------------- */
void* insertElement(HashTable* hashTable, void* key, void* value);

/* ------------------------------------------------------------------------- *
 * Remove the key-value pair with (equivalent) key $key$ from the HashTable
 * $hashTable$.
 *
 * This function doesn't call free() on the element.
 *
 * PARAMETERS
 * hashTable       An HashMap object
 * key             An key value
 *
 * RETURN
 * old value       Pointer to value previously associated to $key$
 * NULL            Otherwise. NULL may also be returned on error. The function
 *                 does not produces any error message.
 * ------------------------------------------------------------------------- */
void* removeElement(HashTable* hashTable, void* key);

/* ------------------------------------------------------------------------- *
 * Return the number of elements in the HashTable $hashTable$.
 *
 * PARAMETERS
 * hashTable       An HashMap object
 *
 * RETURN
 * size_t          The number of elements
 * (size_t)-1      If error
 * ------------------------------------------------------------------------- */
size_t getNumElements(const HashTable* hashTable);

/* ------------------------------------------------------------------------- *
 * Check whether a key-value pair with (equivalent) key $key$ exists in
 * the HashTable $hashTable$.
 *
 * PARAMETERS
 * hashTable       An HashMap object
 *
 * RETURN
 * true            If the key exists
 * false           Otherwise
 * ------------------------------------------------------------------------- */
bool hasKey(const HashTable* hashTable, const void* key);

#endif // !_HASHTABLE_H_
