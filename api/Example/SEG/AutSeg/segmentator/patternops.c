#include "al.h"


// Allocate memory and initialize values for a pattern data structure
Pattern *AllocatePattern()
{
   Pattern *pattern = NULL;

   pattern = (Pattern *) malloc(sizeof(Pattern));
   if (pattern == NULL)
      MemoryError("AllocatePattern");

   pattern->definition = NULL;
   pattern->numInstances = 0;
   pattern->instances = NULL;
   pattern->value = -1.0;
   pattern->meantime = -1.0;
   pattern->stddevtime = -1.0;

   return(pattern);
}


// Allocate memory and initialize values for a pattern list node data structure
PatternListNode *AllocatePatternListNode(Pattern *pattern)
{
   PatternListNode *patternListNode = NULL;

   patternListNode = (PatternListNode *) malloc(sizeof(PatternListNode));
   if (patternListNode == NULL)
      MemoryError("PatternListNode");
   patternListNode->pattern = pattern;
   patternListNode->next = NULL;

   return(patternListNode);
}


// Allocate memory and initialize values for a pattern list data structure
PatternList *AllocatePatternList(void)
{
   PatternList *patternList = NULL;

   patternList = (PatternList *) malloc(sizeof(PatternList));
   if (patternList == NULL)
      MemoryError("AllocatePatternList");
   patternList->head = NULL;

   return(patternList);
}


// Allocate memory and initialize values for a pattern instance data structure
Instance *AllocateInstance(int numevents)
{
   Instance *instance = NULL;

   instance = (Instance *) malloc(sizeof(Instance));
   if (instance == NULL)
      MemoryError("AllocateInstance");
   instance->numEvents = numevents;
   instance->events = NULL;
   instance->used = FALSE;
   if (numevents > 0)
   {
      instance->events = (int *) malloc(numevents * sizeof(int));
      if (instance->events == NULL)
         MemoryError("AllocateInstance");
   }
   instance->parentInstance = NULL;

   return(instance);
}


// Allocate memory and initialize values for a pattern instance list
// data structure
InstanceList *AllocateInstanceList(void)
{
   InstanceList *instanceList = NULL;
   
   instanceList = (InstanceList *) malloc(sizeof(InstanceList));
   if (instanceList == NULL)
      MemoryError("AllocateInstanceList");
   instanceList->head = NULL;

   return(instanceList);
}


// Allocate memory and initialize values for a pattern instance list node
// data structure
InstanceListNode *AllocateInstanceListNode(Instance *instance)
{
   InstanceListNode *instanceListNode = NULL;

   instanceListNode = (InstanceListNode *) malloc(sizeof(InstanceListNode));
   if (instanceListNode == NULL)
      MemoryError("AllocateInstanceListNode");
   instanceListNode->instance = instance;
   instanceListNode->next = NULL;

   return(instanceListNode);
}


// Allocate memory and initialize values for a sequence data structure
Seq *AllocateSequence(int numevents)
{
   Seq *seq = NULL;

   seq = (Seq *) malloc(sizeof(Seq));
   if (seq == NULL)
      MemoryError("AllocateSequence");

   seq->numEvents = numevents;
   seq->events = NULL;
   if (numevents > 0)
   {
      seq->events = (int *) malloc(numevents * sizeof(int));
      if (seq->events == NULL)
	MemoryError("AllocateSequence");
   }

   return(seq);
}


// Insert a new instance list node at the head of an instance list
void InstanceListInsert(Instance *instance, InstanceList *instanceList)
{
   InstanceListNode *instanceListNode = NULL;

   instanceListNode = AllocateInstanceListNode(instance);
   instanceListNode->next = instanceList->head;
   instanceList->head = instanceListNode;
}


// Insert pattern into patternList if not already there.
// List is maintained in nonincreasing order by pattern value.
void PatternListInsert(Pattern *pattern, PatternList *patList, int max)
{
   int numPatterns = 0;
   BOOLEAN inserted = FALSE;
   PatternListNode *patIndex = NULL;
   PatternListNode *patIndexPrevious = NULL;
   PatternListNode *newPatListNode = NULL;

   newPatListNode = AllocatePatternListNode(pattern);

   // if patList is empty, insert new pattern
   if (patList->head == NULL)
   {
      patList->head = newPatListNode;
      return;
   }

   // if pattern already on patList, remove it
   patIndex = patList->head;
   while ((patIndex != NULL) && (patIndex->pattern->value >= pattern->value))
   {
      if (patIndex->pattern->value == pattern->value)
      {
         if (Match(patIndex->pattern->definition, pattern->definition, EXACT))
	 {
	    FreePatternListNode(newPatListNode);
            newPatListNode = NULL;
	    return;
	 }
      }
      patIndex = patIndex->next;
   }

   // pattern is unique so insert it in sorted list
   patIndex = patList->head;
   while (patIndex != NULL)
   {
      if (!inserted)
      {
         if (patIndex->pattern->value < pattern->value)
         {
            newPatListNode->next = patIndex;
	    if (patIndexPrevious != NULL)
	       patIndexPrevious->next = newPatListNode;
	    else patList->head = newPatListNode;
            patIndex = newPatListNode;
	    inserted = TRUE;
         }
         else if (patIndex->next == NULL)
         {
	    // Special case where location is at end of list
	    // Put pattern there for now
	    patIndex->next = newPatListNode;
	    inserted = TRUE;
         }
      }

      numPatterns++;
      if ((max > 0) && (numPatterns > max))
      {
         if (patIndexPrevious != NULL) // max exceeded, delete rest of list
	    patIndexPrevious->next = NULL;
	 while (patIndex != NULL)
	 {
	    patIndexPrevious = patIndex;
	    patIndex = patIndex->next;
	    FreePatternListNode(patIndexPrevious);
            patIndexPrevious = NULL;
	 }
      }
      else
      {
         patIndexPrevious = patIndex;
         patIndex = patIndex->next;
      }
   }

   if (!inserted)
   {
      FreePatternListNode(newPatListNode);
      newPatListNode = NULL;
   }
}


// Free an instance list and its components
void FreeInstanceList(InstanceList *instanceList)
{
   InstanceListNode *inode1 = NULL, *inode2 = NULL;

   if (instanceList != NULL)
   {
      inode1 = instanceList->head;
      while (inode1 != NULL)
      {
         inode2 = inode1;
	 inode1 = inode1->next;
	 free(inode2);
      }
      free(instanceList);
   }
}


// Free a pattern and its components
void FreePattern(Pattern *pattern)
{
   if (pattern != NULL)
   {
      FreeInstanceList(pattern->instances);
      free(pattern->definition);
      free(pattern);
   }
}


// Free a pattern list node and its components
void FreePatternListNode(PatternListNode *patListNode)
{
   if (patListNode != NULL)
   {
      FreePattern(patListNode->pattern);
      free(patListNode);
   }
}


// Free a pattern list and its components
void FreePatternList(PatternList *patList)
{
   PatternListNode *pnode1 = NULL;
   PatternListNode *pnode2 = NULL;

   if (patList != NULL)
   {
      pnode1 = patList->head;
      while (pnode1 != NULL)
      {
         pnode2 = pnode1;
	 pnode1 = pnode1->next;
	 FreePattern(pnode2->pattern);
	 free(pnode2);
      }
      free(patList);
   }
}


// Free a sequence and its components
void FreeSequence(Seq *seq)
{
   if (seq != NULL)
   {
      free(seq->events);
      free(seq);
   }
}


// Print a sensor event label
void PrintLabel(int label)
{
   printf("(%d) %s", label, adlabels[label]);
}


// Print a sensor event
void PrintEvent(Event event)
{
   printf("event ");
   PrintLabel(event.label);
   printf(" [%d %d]", event.begin, event.end);
   printf("\n");
}


// Print a sequence of sensor events
void PrintSequence(Seq *seq)
{
   int i;

   if (seq != NULL)
   {
      printf(" Sequence(%d):\n", seq->numEvents);
      // print events
      for (i=0; i<seq->numEvents; i++)
      {
         printf("   ");
	 PrintEvent(events[seq->events[i]]);
      }
   }
}


// Print a pattern
void PrintPattern(Pattern *pattern)
{
   if (pattern != NULL)
   {
      printf("Pattern: value = %f", (float) pattern->value);
      printf(", number of instances = %d\n", pattern->numInstances);
      printf("   meantime = %f, stddevtime = %f\n",
             pattern->meantime, pattern->stddevtime);
   }

   // print pattern
   if (pattern->definition != NULL)
      PrintSequence(pattern->definition);

   if (OutputLevel > 1)
   {
      printf("\n   Instances:\n");
      PrintInstanceList(pattern);
   }
}


// Print a pattern list
void PrintPatternList(PatternList *patList)
{
   int numPatterns = 1;
   PatternListNode *patListNode = NULL;

   if (patList != NULL)
   {
      patListNode = patList->head;
      while (patListNode != NULL)
      {
         printf("%d ", numPatterns);
	 numPatterns++;
	 PrintPattern(patListNode->pattern);
	 printf("\n");
	 patListNode = patListNode->next;
      }
   }
}


// Compute the size of a pattern as the sum of the size of
// the member events
int SizePattern(Seq *definition)
{
   int i, total=0;

   for (i=0; i<definition->numEvents; i++)
      total += events[definition->events[i]].patsize;

   return(total);
}


// Print a sequence instance
void PrintInstance(Instance *instance)
{
   int i;

   if (instance != NULL)
   {
      // print events
      for (i=0; i<instance->numEvents; i++)
      {
         printf("   ");
	 PrintLabel(events[instance->events[i]].label);
	 printf(" [%d]", events[instance->events[i]].begin);
      }
   }
}


// Print a sequence instance list
void PrintInstanceList(Pattern *pattern)
{
   int i = 0;
   InstanceListNode *instanceListNode = NULL;

   if (pattern->instances != NULL)
   {
      instanceListNode = pattern->instances->head;
      while (instanceListNode != NULL)
      {
	 i++;
         printf("\n   Instance %d", i);
	 PrintInstance(instanceListNode->instance);
	 instanceListNode = instanceListNode->next;
      }
   }
}


// Determine of a pattern is a member of a pattern list
BOOLEAN MemberOfPatternList(Pattern *pattern, PatternList *patternList)
{
   PatternListNode *patternListNode = NULL;
   BOOLEAN found = FALSE;

   if (patternList != NULL)
   {
      patternListNode = patternList->head;
      while ((patternListNode != NULL) && (!found))
      {
         if (Match(pattern->definition,
	           patternListNode->pattern->definition, EXACT))
	    found = TRUE;
         patternListNode = patternListNode->next;
      }
   }

   return(found);
}


// Convert a pattern instance to a sequence representation
Seq *InstanceToSequence(Instance *instance)
{
   int i;
   Seq *newSequence = NULL;

   newSequence = AllocateSequence(instance->numEvents);
   for (i=0; i<instance->numEvents; i++)
      newSequence->events[i] = instance->events[i];

   return(newSequence);
}


// Determine if a pattern overlaps with any sequence in the instance list
BOOLEAN InstanceListOverlap(Instance *instance, InstanceList *instanceList)
{
   InstanceListNode *instanceListNode = NULL;

   if (instanceList != NULL)
   {
      instanceListNode = instanceList->head;
      while (instanceListNode != NULL)
      {
         if (instanceListNode->instance != NULL)
	    if (InstanceOverlap(instance, instanceListNode->instance))
	       return(TRUE);
         instanceListNode = instanceListNode->next;
      }
   }

   return(FALSE);
}


// Determine if two pattern instances overlap
// Two sequences overlap if they have any sensor event in common
BOOLEAN InstanceOverlap(Instance *instance1, Instance *instance2)
{
   int i, j, ne1, ne2;

   ne1 = instance1->numEvents;
   ne2 = instance2->numEvents;
   for (i=0; i<ne1; i++)
      for (j=0; j<ne2 && instance2->events[j]<=instance1->events[i]; j++)
         if (instance1->events[i] == instance2->events[j])
	    return(TRUE);

   return(FALSE);
}


// Determine if two sensor labels are similar
// Labels are similar of the computed sim vlaues are above a threshold value
BOOLEAN Similar(int label1, int label2)
{
   if (label1 == label2)
      return(TRUE);
   else return(FALSE);

   if ((label1 >= numsim) || (label2 >= numsim))
      return(FALSE);

   if (((float) sim[label1][label2] > MITHRESHOLD) ||
       ((float) sim[label2][label1] > MITHRESHOLD))
      return(TRUE);
   else return(FALSE);
}
