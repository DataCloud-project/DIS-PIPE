#include "al.h"


// Compress the dataset by replacing instances of a pattern with a single event
// labeled with the pattern number
int CompressData(PatternList *patternList)
{
   char patLabel[MAXSTR];
   int i, count, numnewevents, dow, time, sensorid, sensorvalue, label;
   Event *newevents = NULL;
   InstanceListNode *instanceListNode = NULL;
   Pattern *pattern = NULL;

   pattern = patternList->head->pattern;
   if (pattern == NULL)
      return(0);
   instanceListNode = pattern->instances->head;
   
   // Mark occurrences of the pattern so that only unmarked events will
   // be saved for the compressed dataset
   numnewevents = MarkEvents(instanceListNode);

   // Create a new sensor event labeled with the pattern number
   sprintf(patLabel, "%s_%d", PATLABEL, iteration);
   StoreLabel(patLabel);
                           // Design new event to represent compressed instance
   newevents = (Event *) malloc(nevents * sizeof(Event));
   if (newevents == NULL)
      MemoryError("CompressData");
   dow = events[pattern->definition->events[0]].dow;
   time = events[pattern->definition->events[0]].time;
   sensorid = events[pattern->definition->events[0]].sensorid;
   sensorvalue = events[pattern->definition->events[0]].sensorvalue;
   label = numlabels - 1;

   count = 0;
   for (i=0; i<nevents; i++)
   {
      // First instance event, replace with pattern event
      if (events[i].copy == NEW)
      {
         newevents[count].dow = dow;
	 newevents[count].time = time;
	 newevents[count].sensorid = sensorid;
	 newevents[count].sensorvalue = sensorvalue;
	 newevents[count].label = label;
	 newevents[count].copy = TRUE;
	 newevents[count].patsize = SizePattern(pattern->definition);
	 newevents[count].begin = events[i].begin; // Distance = pattern length
	 newevents[count].end = events[i].begin + newevents[count].patsize - 1;
         // Prevent overlap of pattern instances with each other or predefined
         while ((count > 0) &&
                (newevents[count-1].end >= newevents[count].begin) &&
                (newevents[count-1].end > newevents[count-1].begin))
            newevents[count-1].end--;
	 count++;
      }
      else if ((events[i].copy == TRUE) || (events[i].copy == PREDEFINED))
      {
         newevents[count].dow = events[i].dow;  // Copy event to compressed data
         newevents[count].time = events[i].time;
         newevents[count].sensorid = events[i].sensorid;
         newevents[count].sensorvalue = events[i].sensorvalue;
         newevents[count].label = events[i].label;
	 newevents[count].copy = events[i].copy;
	 newevents[count].patsize = events[i].patsize;
	 newevents[count].begin = events[i].begin;
	 newevents[count].end = events[i].end;
         // Prevent overlap of pattern instances with each other or predefined
         while ((count > 0) &&
                (newevents[count-1].end >= newevents[count].begin) &&
                (newevents[count-1].end > newevents[count-1].begin))
            newevents[count-1].end--;
	 count++;
      }
   }

   if (numnewevents < nevents)
   {
      free(events);
      events = NULL;
      events = newevents;
      nevents = numnewevents;
      return(1);
   }
   else
   {
      free(newevents);
      return(0);
   }
}


// Mark events that are part of pattern instances
int MarkEvents(InstanceListNode *instanceListNode)
{
   int i, newinstance, numnewevents, mark=0, duplicate=0;
   Instance *instance = NULL;

   numnewevents = nevents;
   for (i=0; i<nevents; i++)
      if (events[i].copy != PREDEFINED)
         events[i].copy = TRUE;

   while (instanceListNode != NULL)
   {
      instance = instanceListNode->instance;
      instance->used = TRUE;
      newinstance = 1;
      for (i=0; i<instance->numEvents; i++) // Mark all events in this instance
      {
         if (events[instance->events[i]].copy != TRUE)     // Check for overlap
	 {
	    duplicate++;
            instance->used = FALSE;             // To overlap or not to overlap
         }
	 else                                  // Mark this event as compressed
	 {
	    if (newinstance == 1)                 // First event in an instance
	    {
	       events[instance->events[i]].copy = NEW;
	       newinstance = 0;
	    }
	    else if (events[instance->events[i]].copy == PREDEFINED)
	    {
	       printf("PREDEFINED\n");
	    }
	    else         // This node will be removed in the compressed dataset
	    {
	       events[instance->events[i]].copy = FALSE;
	       mark++;
	       numnewevents--;
	    }
         }
      }
      instanceListNode = instanceListNode->next;
   }

   if (OutputLevel > 0)
      printf("Marked %d events to compress, %d duplicates, %d numnewevents\n",
         mark, duplicate, numnewevents);

   return(numnewevents);
}


// Store new pattern event label in global array of labels
void StoreLabel(char *patLabel)
{
   adlabels = (char **) realloc(adlabels, (numlabels + 1) * sizeof(char *));
   if (adlabels == NULL)
      MemoryError("StoreLabel");
   adlabels[numlabels] = (char *) malloc(MAXSTR * sizeof(char));
   if (adlabels[numlabels] == NULL)
      MemoryError("StoreLabel");
   strcpy(adlabels[numlabels], patLabel);
   numlabels++;
}
