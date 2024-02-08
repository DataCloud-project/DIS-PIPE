#include "al.h"


// Extend a pattern by adding a prefix or suffix to the pattern instances
PatternList *ExtendPattern(Pattern *pattern)
{
   int newInstanceListIndex = 0;
   InstanceList *newInstanceList = NULL;
   InstanceListNode *newInstanceListNode = NULL;
   Instance *newInstance = NULL;
   Pattern *newPattern = NULL;
   PatternList *extendedPatterns = NULL;
   PatternListNode *newPatternListNode = NULL;

   extendedPatterns = AllocatePatternList();
   newInstanceList = ExtendInstances(pattern->instances);
   newInstanceListNode = newInstanceList->head;

   // Add new extended pattern, make sure not duplicating pattern in list
   while (newInstanceListNode != NULL)
   {
      newInstance = newInstanceListNode->instance;
      newPattern = CreatePatternFromInstance(newInstance);
      if (!MemberOfPatternList(newPattern, extendedPatterns))
      {
	 AddInstancesToPattern(newPattern, newInstance, newInstanceList,
	                       newInstanceListIndex);

	 // Add newPattern to head of extendedPatterns list
	 newPatternListNode = AllocatePatternListNode(newPattern);
	 newPatternListNode->next = extendedPatterns->head;
	 extendedPatterns->head = newPatternListNode;
      }
      else FreePattern(newPattern);
      newPattern = NULL;

      newInstanceListNode = newInstanceListNode->next;
      newInstanceListIndex++;
   }
   FreeInstanceList(newInstanceList);

   return(extendedPatterns);
}


// Extend instances by adding prefix and/or suffix to pattern instances
InstanceList *ExtendInstances(InstanceList *instanceList)
{
   InstanceList *newInstanceList = NULL;
   InstanceListNode *instanceListNode = NULL;
   Instance *instance = NULL;
   Instance *newInstance = NULL;
   int i;

   newInstanceList = AllocateInstanceList();
   instanceListNode = instanceList->head;

   // Sequence through list of instances, extending each one
   // Do not include events which are part of a predefined activity
   while (instanceListNode != NULL)
   {
      instance = instanceListNode->instance;
 
      for (i=0; i<instance->numEvents; i++)
      {
	                                                       // Add a prefix
	 if (instance->events[0] != 0)
	 {
	    if (events[instance->events[0] - 1].copy != PREDEFINED)
	    {
               newInstance = CreateExtendedInstance(instance, BEFORE);
	       InstanceListInsert(newInstance, newInstanceList);
	    }
	 }
	                                                       // Add a suffix
	 if (instance->events[instance->numEvents-1] != (nevents - 1))
	 {
	    if (events[instance->events[instance->numEvents-1] + 1].copy !=
	        PREDEFINED)
	    {
               newInstance = CreateExtendedInstance(instance, AFTER);
	       InstanceListInsert(newInstance, newInstanceList);
	    }
         }
      }
      instanceListNode = instanceListNode->next;
   }

   return(newInstanceList);
}


// Extend sequence by included sensor event that occurs before or after
// sequence
Instance *CreateExtendedInstance(Instance *instance, BOOLEAN where)
{
   int i;
   Instance *newInstance = NULL;

   newInstance = AllocateInstance(instance->numEvents + 1);
   
   // save pointer to the instance before this extension
   newInstance->parentInstance = instance;

   if (where == BEFORE)   // Add event before sequence list to new sequence
   {
      newInstance->events[0] = instance->events[0] - 1;
      for (i=0; i<instance->numEvents; i++)
         newInstance->events[i+1] = instance->events[i];
   }
   else                   // Add event after sequence list to new sequence
   {
      for (i=0; i<instance->numEvents; i++)
         newInstance->events[i] = instance->events[i];
      newInstance->events[instance->numEvents] =
         instance->events[instance->numEvents-1] + 1;
   }

   return(newInstance);
}


// Make extended sequence into a separate pattern
Pattern *CreatePatternFromInstance(Instance *instance)
{
   Pattern *newPattern = AllocatePattern();
   newPattern->definition = InstanceToSequence(instance);

   return(newPattern);
}


// Add instance from instanceList to pattern's instances if the instance
// matches the pattern's definition.
void AddInstancesToPattern(Pattern *pattern, Instance *patternInstance,
   InstanceList *instanceList, int index)
{
   int counter=0;
   InstanceListNode *instanceListNode = NULL;
   Instance *instance = NULL;
   Seq *instanceSequence = NULL;

   if (instanceList != NULL)
   {
      pattern->instances = AllocateInstanceList();

      // Insert patternInstance onto list of instances
      patternInstance->used = TRUE;
      InstanceListInsert(patternInstance, pattern->instances);
      pattern->numInstances++;
      instanceListNode = instanceList->head;
      while (instanceListNode != NULL)
      {
         if (instanceListNode->instance != NULL)
	 {
	    instance = instanceListNode->instance;
	    if (ALLOWOVERLAP ||
	        (InstanceListOverlap(instance, pattern->instances) == FALSE))
	    {
	       instanceSequence = InstanceToSequence(instance);

	       // To avoid processing duplicates, skip all entries before
	       // and including this instance (they were checked in a
	       // previous call) and skip the instance if it already
	       // matched with another pattern.
	       if ((counter > index) && (instance->used == FALSE))
	       {
                  if (Match(pattern->definition, instanceSequence, INEXACT))
	          {
	             instance->used = TRUE;
	             InstanceListInsert(instance, pattern->instances);
	             pattern->numInstances++;
	          }
	       }
	       FreeSequence(instanceSequence);
               instanceSequence = NULL;
	    }
	    counter++;
	 }
	 instanceListNode = instanceListNode->next;
      }
   }
}
