#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "mymalloc.h"
//Workload A
double workLoadA()
{
  struct timespec start, stop;

  double runTime = 0;
  char * p1;
  int i;
  int j;

  for (i = 0; i < 50; i++)
    {
      clock_gettime(CLOCK_MONOTONIC, &start);
      for (j = 0; j < 120; j++)
	{
	  p1 = malloc(1);
	  free(p1);
	}
      clock_gettime(CLOCK_MONOTONIC, &stop);
      runTime += (stop.tv_sec - start.tv_sec)*1000000;
      runTime += (stop.tv_nsec - start.tv_nsec)/1000.0;
    }
  return runTime / 50;
}
//Workload B
double workLoadB()
{
  struct timespec start, stop;
  double runTime = 0;
  char * arr[120];
  int i;
  int j;

  for (i = 0; i < 50; i++)
    {
      clock_gettime(CLOCK_MONOTONIC, &start);
      for (j = 0; j < 120; j++)
	{
	  arr[j] = malloc(1);
	}
      for (j = 0; j < 120; j++)
	{
	  free(arr[j]);
	}
      clock_gettime(CLOCK_MONOTONIC, &stop);
      runTime += (stop.tv_sec - start.tv_sec)*1000000;
      runTime += (stop.tv_nsec - start.tv_nsec)/1000.0;
    }
  return runTime / 50;
}
//Workload C
double workLoadC()
{
  struct timespec start, stop;
  double runTime = 0;
  char * arr[120];
  int i;
  int addIndex;
  int freeIndex;
  int choice;
  
  for (i = 0; i < 50; i++)
    {
      addIndex = 0;
      freeIndex = 0;
      clock_gettime(CLOCK_MONOTONIC, &start);
      while (addIndex < 120)
	{
	  choice = rand() % 2;
	  //malloc here
	  if (choice == 1 || addIndex == freeIndex)
	    {
	      arr[addIndex] = malloc(1);
	      addIndex++;
	    }
	  //free here
	  else
	    {
	      free(arr[freeIndex]);
	      freeIndex++;
	    }
	}
      while (freeIndex < 120)
	{
	  free(arr[freeIndex]);
	  freeIndex++;
	}

      clock_gettime(CLOCK_MONOTONIC, &stop);
      runTime += (stop.tv_sec - start.tv_sec)*1000000;
      runTime += (stop.tv_nsec - start.tv_nsec)/1000.0;
    }  
  return runTime/50;
}
//Workload D
double workLoadD()
{
  struct timespec start, stop;
  double runTime = 0;
  int i;
  int j;
  int randSize;
  char * arr[200];

  for (i = 0; i < 50; i++)
    {
      clock_gettime(CLOCK_MONOTONIC, &start);
      for (j = 0; j < 200; j++)
	{
	  randSize = (rand() % 15) + 1;
	  arr[j] = malloc(randSize);
	}
      for (j = 0; j < 200; j++)
	{
	  free(arr[j]);
	}
      for (j = 0; j < 200; j++)
	{
	  randSize = (rand() % 15) + 1;
	  arr[j] = malloc(randSize);
	}
      for (j = 0; j < 200; j++)
	{
	  free(arr[j]);
	}
      clock_gettime(CLOCK_MONOTONIC, &stop);
      runTime += (stop.tv_sec - start.tv_sec)*1000000;
      runTime += (stop.tv_nsec - start.tv_nsec)/1000.0;
    }
  return runTime/50;
}
//Workload E
double workLoadE()
{ 
  struct timespec start, stop;
  double runTime = 0;
  char * arr[100];
  int i;
  int j;
  for (i = 0; i < 50; i++)
    {
      clock_gettime(CLOCK_MONOTONIC, &start);
      for (j = 0; j < 50; j++)
	{
	  arr[j] = malloc(j+1);
	  arr[j+50] = malloc(50-j);
	}
      for (j = 0; j < 100; j++)
	{
	  free(arr[j]);
	}
      for (j = 0; j < 50; j++)
	{
	  arr[j] = malloc(50-j);
	  arr[j+50] = malloc(j+1);
	}
      for (j = 0; j < 100; j++)
	{
	  free(arr[j]);
	} 
      clock_gettime(CLOCK_MONOTONIC, &stop);
      runTime += (stop.tv_sec - start.tv_sec)*1000000;
      runTime += (stop.tv_nsec - start.tv_nsec)/1000.0;
    }
  return runTime / 50;
}
int main(int argc, char ** argv)
{
  printf("The average runtime of workload A is %f microseconds.\n", workLoadA());
  
  printf("The average runtime of workload B is %f microseconds.\n", workLoadB());
 
  printf("The average runtime of workload C is %f microseconds.\n", workLoadC());

  printf("The average runtime of workload D is %f microseconds.\n", workLoadD());
  
  printf("The average runtime of workload E is %f microseconds.\n", workLoadE());
  
  return 0;
}
 
