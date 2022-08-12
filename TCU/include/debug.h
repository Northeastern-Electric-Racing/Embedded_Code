#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG   // If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG 
   #define DPRINT(...)    Serial.print(__VA_ARGS__)     // defines printing as normal
   #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   
#else
   #define DPRINT(...)     // now defines a blank line
   #define DPRINTLN(...)  
#endif

#endif