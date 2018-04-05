
#include <stdio.h>
#include <string.h>

#define MDebug(format, ...) do { if(_MDebug_Enabled) printf( "(DEBUG:) %s:%d - " format "\n", __FILE__, __LINE__, ##__VA_ARGS__ ); } while(0)

// Ob die MDebug-Funktion/Makro etwas drucken soll oder nicht
extern bool _MDebug_Enabled; // defined in Win4.cpp

