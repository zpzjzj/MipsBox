#include <windows.h>
#include <conio.h>
#include <process.h>

extern void CreateDispThd(void);     //thread dispatch
extern void CreateKeyboardThd(void);
extern void ClearScreen(void);            // Screen clear 
extern void ShutDown(void);               // Program shutdown 
extern void WriteTitle(void);    // Display title bar information 

extern HANDLE  hConsoleOut;                 // Handle to the console 
extern HANDLE  hRunMutex;                   // "Keep Running" mutex 
extern HANDLE  hScreenMutex;                // "Screen update" mutex
extern int     ThreadNr;                    // Number of threads started 
extern CONSOLE_SCREEN_BUFFER_INFO csbiInfo; // Console information 
