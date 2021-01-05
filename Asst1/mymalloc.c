#include<stdlib.h>
#include<stdio.h>
#include "mymalloc.h"

#define MEM_SIZE 4096

static char myblock[MEM_SIZE];

//Given that c is the first byte of metadata, returns 1 if the first bit is set (i.e. there is malloced data) and 0 otherwise.
int isSet(char c)
{
  return (c >> 7) & 1;
}
//Given that c is the first byte of metadata, flips the first bit.
char flipBit(char c)
{
  return c ^ (1 << 7);
}
//Given that c is the first byte of metadata, set the first bit to 0.
char unsetBit(char c)
{
  return (c > 0) ? c : c & 127;
}
//Given that c1 and c2 are the two bytes of metadata, returns the number of bytes malloced. The number of bytes malloced is the binary number represented by the last 5 bits of c1 and all 8 bits of c2.
int allocatedBytes(char c1, char c2)
{
  int b1Rep = (c1 & 31) << 8;
  int b2Rep = c2 & 255;
  return b1Rep + b2Rep;
}
//Given that myblock[index] is the location of the first byte of a metablock and size is the total number of bytes to be malloced, stores the metadata and sets the metablock in use.
void setMeta(int index, size_t size)
{
  char c2 = size & 255;
  char c1 = flipBit((size >> 8) & 31);
  myblock[index] = c1;
  myblock[index + 1] = c2;
  return;
}
void printArr()
{
  int i;
  for (i = 0; i < MEM_SIZE;i++)
    {
      printf("%d ", myblock[i]);
    }
  printf("\n");
}
//Starting from the beginning of myblock, returns the index of the first unallocated space. This function will return -1 if no empty space could be found.(Note that the last two spaces of myblock will not be considered empty, as a malloc needs two bytes of metadata to allocate any data.)
int findFirstEmpty(int i)
{
  while (i < MEM_SIZE && isSet(myblock[i]) == 1)
    {
      i += allocatedBytes(myblock[i], myblock[i+1]);
    }
  return (i < MEM_SIZE - 2) ? i : -1;
}
//Taking in the index found by findFirstEmpty(), returns the index of the next set metablock. If findFirstEmpty() returns -1, this function returns -1. If no set metablock could be found from the starting index, this function returns MEM_SIZE, since the remainder of the memory should be available for allocation.
int findNextSetBlock(int i)
{
  if (i == -1) return -1;
  while (i < MEM_SIZE && isSet(myblock[i]) == 0)
    {
      if (allocatedBytes(myblock[i], myblock[i+1]) == 0)
	{
	  return MEM_SIZE;
	}
      i += allocatedBytes(myblock[i], myblock[i+1]);
    }
  return (i < MEM_SIZE) ? i : MEM_SIZE;
}
//Takes in the size of the allocation and returns a void * pointer to the first byte of allocated memory if an allocation was possible. If allocation was not successful, prints an informative message specifying the file and line number where the error occurred. 
void * mymalloc(size_t size, char * f, int l)
{
  
  //Returns NULL when user asks to malloc 0 bytes.
  if (size == 0)
    {
      printf("ERROR in line %d of %s: Request to malloc() 0 bytes.\n", l, f);
      return NULL;
    }
  //Returns NULL when user asks to malloc > (4096 - 2) bytes.
  if (size > 4094)
    {
      printf("ERROR in line %d of %s: Not enough memory to malloc() %lu bytes.\n", l, f, size);
      return NULL;
    }
  
  //Traverses memory to find a space of appropriate size.
  int startIndex = findFirstEmpty(0);
  int endIndex = findNextSetBlock(startIndex);
  while (startIndex != -1 && endIndex - startIndex < (size + 2))
    {
      startIndex = findFirstEmpty(endIndex);
      endIndex = findNextSetBlock(startIndex);
    }
  //No empty space could be found.
  if (startIndex == -1)
    {
      printf("ERROR in line %d of %s: Not enough memory to malloc() %lu byte", l, f, size);
      if ((int)size != 1) printf("s");
      printf(".\n");
      return NULL;
    }
  //A split occurs here whenever an allocation uses less space than the unused metablock allows.
  int prevSize = allocatedBytes(myblock[startIndex], myblock[startIndex + 1]) - 2;
  if ((int)size < prevSize - 1)
    {
      setMeta(startIndex + size + 2, prevSize - size);
      myblock[startIndex + size + 2] = unsetBit(myblock[startIndex + size + 2]);
    }
  //If the new size is one less byte than the previous metablock size, the user receives 1 extra byte, as 1 byte is not enough space to hold the metadata of a split.
  if ((int)size == prevSize - 1)
    {
      size++;
    }
  //Sets the metablock at myblock[startIndex].
  setMeta(startIndex, size + 2);
  
  return (void *)&myblock[startIndex + 2];
}
//Merges contiguous memory blocks that are not in use. Calls to merge will be made after an allocation has been freed successfully.
void merge()
{
  int i = 0;
  int iData = allocatedBytes(myblock[i], myblock[i+1]);
  while (i < MEM_SIZE - 1 && iData != 0)
    {
      if (isSet(myblock[i]) == 1)
	{
	  i += iData;
	}
      else
	{
	  int j = i + iData;	  
	  while (j < MEM_SIZE - 1 && isSet(myblock[j]) == 0
		 && allocatedBytes(myblock[j], myblock[j+1]) != 0)
	    {
	      iData += allocatedBytes(myblock[j], myblock[j+1]);
	      j += allocatedBytes(myblock[j], myblock[j+1]);
	    }
	  setMeta(i, iData);
	  myblock[i] = unsetBit(myblock[i]);
	  i = j;
	}
       iData = allocatedBytes(myblock[i], myblock[i+1]);
    }
}
//Given a void * pointer, runs a series of tests to check whether the pointer is valid. If so, notifies mymalloc.c that the memory corresponding to the pointer is now available for other purposes. Otherwise, prints an informative message specifying the filename and line where the error occurred.
void myfree(void * pointer, char * f, int l)
{
  char * p = (char *)pointer;
  //Checks for NULL pointer.
  if (p == NULL)
    {
      printf("ERROR in line %d of %s: NULL pointer cannot be freed.\n", l, f);
      return;
    }
  //Checks whether the pointer is out of bounds.
  if (p < &myblock[0] || p > &myblock[MEM_SIZE - 1])
    {
      printf("ERROR in line %d of %s: Pointer memory address is out of bounds.\n", l, f);
      return;
    }
  int i = 0;
  while (i < MEM_SIZE - 1)
    {
      //Pointer points to the beginning of some block of memory.
      if (p == &myblock[i] + 2)
	{
	  if (isSet(myblock[i]) == 1)
	    {
	      //Pointer corresponds to a valid memory allocation. Sets the block out of use and merges. 
	      *(p - 2) = flipBit(*(p - 2));
	      merge();
	      return;
	    }
	  else
	    {
	      //Pointer attempts to free memory that is already out of use.
	      printf("ERROR in line %d of %s: No memory allocated at pointer memory address.\n", l, f);
	      return;
	    }
	}
      else if (p < &myblock[i] + 2)
	{
	  //We have overstepped the pointer. Invalid memory allocation.
	  printf("ERROR in line %d of %s: No memory allocated at pointer memory address.\n", l, f);
	  return;
	}
      else
	{
	  if (allocatedBytes(myblock[i], myblock[i+1]) == 0)
	    {
	      //Memory array can no longer be traversed. Pointer unreachable.
	      printf("ERROR in line %d of %s: No memory allocated at pointer memory address.\n", l, f);
	      return;
	    }
	  else
	    {
	      i += allocatedBytes(myblock[i], myblock[i+1]);
	    }
	}
    }
  printf("ERROR in line %d of %s: No memory allocated at pointer memory address.\n", l, f);
  return;
}
