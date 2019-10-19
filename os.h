#ifndef _os_h_
#define _os_h_

#ifdef __unix__         
#elif defined(_WIN32) || defined(WIN32) 
#define OS_Windows
#endif

#endif // _os_h_