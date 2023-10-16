#include "al.h"


// Compute the value of a pattern
// The MDL value of the sequence is calculated, which is estimated as
// |pattern| + |data(pattern)|, or size of the pattern in number of sensor events
// plus size of data which each instance of the pattern is replaced by a
// single event labeled with the pattern number
void EvaluatePattern(Pattern *pattern)
{
   int i, newsize;
   float sum, diff;
   InstanceListNode *node = NULL;

   if ((pattern->definition->numEvents) == 1)
      pattern->value = (float) pattern->numInstances / (float) nevents;
   else
   {
      sum = (float) 0;
      node = pattern->instances->head;
      // Compute mean and standard deviation of pattern instance start times
      for (i=0; i<pattern->numInstances; i++)
      {
         sum += (float) events[node->instance->events[0]].time;
         node = node->next;
      }
      pattern->meantime = sum / (float) pattern->numInstances;
   
      sum = (float) 0;
      node = pattern->instances->head;
      for (i=0; i<pattern->numInstances; i++)
      {
         diff = (float) events[node->instance->events[0]].time -
                pattern->meantime;
         sum += diff * diff;
         node = node->next;
      }
      pattern->stddevtime = sqrt(sum / (float) pattern->numInstances);
   
          // Compress the data by replacing each pattern instance with a 
          // single event labeled by the pattern name.  We also need to
          // add the pattern definition to the data description.
      if (EVALUATE == 0)
         newsize = nevents -
                   (SizePattern(pattern->definition) * pattern->numInstances) +
                   pattern->numInstances + pattern->definition->numEvents;
      else if (EVALUATE == 1)
         newsize = nevents -
                  (pattern->definition->numEvents * pattern->numInstances) +
   	          pattern->numInstances + pattern->definition->numEvents;
      else newsize = nevents -
                  (pattern->definition->numEvents * pattern->numInstances) +
   	          pattern->numInstances;
   
      if (newsize == 0)
         pattern->value = (float) 0;
      else pattern->value = (float) datasize / (float) newsize;    // Compression
      //pattern->value *= (float) TWEIGHT * pattern->stddevtime;
   }
}


// This match algorithm computes the Damerau-Levenshtein distance between
// the two sequences.  A value of 1 is returned if the distance is less
// than a threshold ratio of the size of the larger sequence.
// If the distance is less than a threhsold then the sequence is considered to be
// an instance of the pattern.
int Match(Seq *def1, Seq *def2, int similarity)
{
   int distance, length;
   float ratio;

   if (similarity == EXACT)
      return(ExactMatch(def1, def2));

   if (def1->numEvents > def2->numEvents)
      length = def1->numEvents;
   else length = def2->numEvents;
   distance = ComputeDistance(def1, def2);
   ratio = (float) distance / (float) length;
   if (ratio < MATCHTHRESHOLD)
      return(1);
   else return(0);
}


// Implementation of the Damerau-Levenshtein distance function
int ComputeDistance(Seq *def1, Seq *def2)
{
   int i, j, d1, d2, i1, j1, d, length, **h, returnvalue, *da, db, *list;

   d1 = def1->numEvents;
   d2 = def2->numEvents;
   length = d1 * d2;
   h = (int **) malloc((d1 + 2) * sizeof(int *));
   if (h == NULL)
      MemoryError("ComputeDistance");
   for (i=0; i<d1+2; i++)
   {
      h[i] = (int *) malloc((d2 + 2) * sizeof(int));
      if (h[i] == NULL)
         MemoryError("ComputeDistance");
      for (j=0; j<d2+2; j++)
         h[i][j] = 0;
   }
   list = (int *) malloc(4 * sizeof(int));
   if (list == NULL)
      MemoryError("ComputeDistance");

   h[0][0] = length;
   for (i=0; i<d1; i++)
   {
      h[i+1][1] = i;
      h[i+1][0] = length;
   }
   for (i=0; i<d2; i++)
   {
      h[1][i+1] = i;
      h[0][i+1] = length;
   }
   da = (int *) malloc(numlabels * sizeof(int));  // da is size of alphabet
   if (da == NULL)
      MemoryError("ComputeDistance");
   for (i=0; i<numlabels; i++)
      da[i] = 0;
   for (i=1; i<=d1; i++)
   {
      db = 0;
      for (j=1; j<=d2; j++)
      {
         i1 = da[events[def2->events[j-1]].label];
	 j1 = db;
	 if (Similar(events[def1->events[i-1]].label,
	             events[def2->events[j-1]].label) == TRUE)
            d = 0;
	 else d = 1;
	 if (d == 0)
	    db = j;
	 list[0] = h[i][j] + d;
	 list[1] = h[i+1][j] + 1;
	 list[2] = h[i][j+1] + 1;
	 list[3] = h[i1][j1] + (i - i1 - 1) + 1 + (j - j1 - 1);
         h[i+1][j+1] = minlist(list, 4);
      }
      da[events[def1->events[i-1]].label] = i;
   }
   returnvalue = h[d1+1][d2+1];

   for (i=0; i<d1+2; i++)
      free(h[i]);
   free(h);
   free(da);
   free(list);
   return(returnvalue);
}


// Return the smallest value in an array
int minlist(int *list, int num)
{
   int i, minvalue;

   minvalue = list[0];
   for (i=1; i<num; i++)
      if (list[i] < minvalue)
         minvalue = list[i];

   return(minvalue);
}


// Determine if two sequences match exactly
int ExactMatch(Seq *def1, Seq *def2)
{
   int i;

   if (def1->numEvents != def2->numEvents)
      return(0);

   if (def1->numEvents == 0)
      return(1);

   for (i=0; i<def1->numEvents; i++)
      if (events[def1->events[i]].label != events[def2->events[i]].label)
         return(0);

   return(1);
}
