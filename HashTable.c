/* ========================================================================= *
 * HashTable (April 2012)
 *
 * HashTable.c is the implementation of HashTable.h
 * ========================================================================= */

#include <stdlib.h>
#include <float.h>
#include "HashTable.h"


/* Structure */

typedef struct Element_t Element;

struct Element_t
{
   void* key;
   void* value;
   Element* next;
};

typedef struct ElementsArray_t
{
   Element* listElements;
} ElementsArray;

struct HashTable_t
{
   size_t capacity;
   ElementsArray* elementsArray;
   size_t nbElements;
   size_t (*hashFunction)(const void* key);
   int (*compareFunction)(const void* a, const void* b);
};

/* Constant */

static const float LOAD_FACTOR = 0.7;

/* Local Function Declaration */

/* ------------------------------------------------------------------------- *
 * Associate a $key$ and an index of the hash table.
 *
 * PARAMETER
 * key      The key to hash.
 *
 * RETURN
 * hash         The index of the table corresponding to the $key$.
 * ------------------------------------------------------------------------- */
static size_t hashKey(const HashTable* hashTable, size_t key);

/* ------------------------------------------------------------------------- *
 * Doubles the size of $HashTable$ to adjust the load factor.
 *
 * PARAMETER
 * hashTale      The $hashTale$ to resize.
 *
 * RETURN
 * 0           If successful.
 * != 0        If an error occurred.
 * ------------------------------------------------------------------------- */
static int reHashTable(HashTable *hashTable);

/* Public Function */

HashTable* createHashTable(size_t capacity,
                          size_t (*hashFunction)(const void* key),
                          int (*compareFunction)(const void* a, const void* b))
{
   if (hashFunction == NULL || compareFunction == NULL)
      return NULL;

   // Create the HashTable
   HashTable* newHashTable = (HashTable*)malloc(sizeof(HashTable));
   if (newHashTable == NULL)
      return NULL;

   // Create the array that will contain the linked list of elements
   ElementsArray* newElementsArray = NULL;
   newElementsArray = (ElementsArray*)calloc(capacity, sizeof(ElementsArray));
   if(newElementsArray == NULL)
   {
      free(newHashTable);
      return NULL;
   }

   // Assigns the values
   newHashTable->capacity = capacity;
   newHashTable->elementsArray = newElementsArray;
   newHashTable->nbElements = 0;
   newHashTable->hashFunction = hashFunction;
   newHashTable->compareFunction = compareFunction;

   return newHashTable;
}


void freeHashTable(HashTable* hashTable)
{
   if (hashTable == NULL)
      return;

   Element* listElements = NULL;
   Element* temp = NULL;

   // Free the memory of the array that contain the elements
   for (size_t i = 0; i < hashTable->capacity; i++)
   {
      listElements = hashTable->elementsArray[i].listElements;
      if (listElements != NULL)
      {
         while (listElements->next != NULL)
         {
            temp = listElements;
            listElements = listElements->next;
            free(temp);
         }
         free(listElements);
      }
   }

   // Free the memory of the HashTable
   free(hashTable->elementsArray);
   free(hashTable);
}

void* insertElement(HashTable* hashTable, void* key, void* value)
{
   if (hashTable == NULL || key == NULL || value == NULL)
      return NULL;

   // Resizes the array if the current load factor > LOAD_FACTOR
   float loadFactor = (float)(hashTable->nbElements + 1) / hashTable->capacity;

   if ((loadFactor - LOAD_FACTOR) > FLT_EPSILON)
   {
      if (reHashTable(hashTable) != 0)
         return NULL;
   }

   // Calculating the index of the array corresponding to the key
   size_t index = hashKey(hashTable, (*hashTable->hashFunction)(key));

   // Get the first element of the list
   Element *listElements = hashTable->elementsArray[index].listElements;

   if (listElements != NULL)
   {
      // Check if the key does not already exist
      while (listElements->next != NULL)
      {
         if (!(*hashTable->compareFunction)(listElements->key, key))
         {
            void *temporaryValue = listElements->value;
            listElements->value = value;
            hashTable->nbElements++;
            return temporaryValue;
         }
         listElements = listElements->next;
      }
   }

   // Create the new element
   Element *newElement = (Element*)malloc(sizeof(Element));
   if (newElement == NULL)
      return NULL;

   newElement->key = key;
   newElement->value = value;
   newElement->next = NULL;

   // Add the new element in the list
   if (listElements == NULL)
      hashTable->elementsArray[index].listElements = newElement;
   else
      listElements->next = newElement;

   hashTable->nbElements++;

   return (void*)-1;
}


void* removeElement(HashTable* hashTable, void* key)
{
   if (hashTable == NULL || key == NULL)
      return NULL;

   // Calculating the index of the array corresponding to the key
   size_t index = hashKey(hashTable, (*hashTable->hashFunction)(key));

   // Get the first element of the list
   Element *listElements = hashTable->elementsArray[index].listElements;
   // if the key does not exist
   if (listElements == NULL)
      return NULL;

   Element *previous = NULL;

   // Search the key in the list
   while ((*hashTable->compareFunction)(listElements->key, key) != 0 &&
                                        listElements->next != NULL)
   {
      previous = listElements;
      listElements = listElements->next;
   }

   // Remove the key if it exists
   if ((*hashTable->compareFunction)(listElements->key, key) == 0)
   {
      if (previous != NULL)
         previous->next = listElements->next;
      else
         hashTable->elementsArray[index].listElements = listElements->next;

      void *temporaryValue = listElements->value;
      free(listElements);

      hashTable->nbElements--;

      return temporaryValue;
   }

   return NULL;
}

size_t getNumElements(const HashTable* hashTable)
{
   if (hashTable == NULL)
      return (size_t)-1;

   return hashTable->nbElements;
}

bool hasKey(const HashTable* hashTable, const void* key)
{
   if (hashTable == NULL || key == NULL)
      return false;

   // Calculating the index of the array corresponding to the key
   size_t index = hashKey(hashTable, (*hashTable->hashFunction)(key));

   // Get the first element of the list
   Element *listElements = hashTable->elementsArray[index].listElements;

   // If the key does not exist
   if (listElements == NULL)
      return false;

   // Search the key in the list
   while ((*hashTable->compareFunction)(listElements->key, key) != 0 &&
                                        listElements->next != NULL)
   {
      listElements = listElements->next;
   }

   if ((*hashTable->compareFunction)(listElements->key, key) == 0)
      return true;

   return false;
}

/* Local Function */

static size_t hashKey(const HashTable* hashTable, size_t key)
{
   if (key < hashTable->capacity)
      return key;

   return key % hashTable->capacity;
}

static int reHashTable(HashTable *hashTable)
{
   if (hashTable == NULL)
      return -1;

   // Double the capacity
   size_t newCapacity = hashTable->capacity * 2;

   // Create a larger array
   ElementsArray* newArray = NULL;
   newArray = (ElementsArray*)calloc(newCapacity, sizeof(ElementsArray));
   if (newArray == NULL)
      return -2;

   // Stock the old values ​​of the hash table
   ElementsArray* oldArray = hashTable->elementsArray;
   size_t oldCapacity = hashTable->capacity;

   // Assigns the new values
   hashTable->elementsArray = newArray;
   hashTable->capacity = newCapacity;
   hashTable->nbElements = 0;

   Element *listElements = NULL;
   Element *temp = NULL;

   // Inserts the contents of the old table into the new
   for (size_t i = 0; i < oldCapacity; i++)
   {
      listElements = oldArray[i].listElements;
      if (listElements != NULL)
      {
         while(listElements->next != NULL)
         {
            temp = listElements;
            if (!insertElement(hashTable, temp->key, temp->value))
            {
               free(oldArray);
               return -3;
            }
            listElements = listElements->next;
            free(temp);
         }
         if (!insertElement(hashTable, listElements->key, listElements->value))
         {
            free(oldArray);
            return -3;
         }
         free(listElements);
      }
   }

   // Free memory
   free(oldArray);

   return 0;
}
