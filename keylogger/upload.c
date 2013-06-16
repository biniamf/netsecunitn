#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
/*
-----------------------------1727381797619608628249622620
Content-Disposition: form-data; name="file"; filename="log.txt"
Content-Type: text/plain

This log file contains something inside the file.
-----------------------------1727381797619608628249622620
Content-Disposition: form-data; name="submit"

Submit
-----------------------------1727381797619608628249622620--
*/
void upload (int socket)
{
    FILE*       file;
    char        buf[1024];
    int         size, n, fsize;    
    char        header[] =  "POST /upload.php HTTP/1.1\r\n"
                            "Host: netsecsample.netau.net\r\n"                                                                                    
                            "Connection: keep-alive\r\n"
                            "Content-Type: multipart/form-data; boundary=-----------------------------1727381797619608628249622620\r\n"
                            "Content-Length: %d\r\n\r\n";
                            
    char        startbuf[] = "-----------------------------1727381797619608628249622620\r\n"
                             "Content-Disposition: form-data; name=\"file\"; filename=\"log.txt\"\r\n"
                             "Content-Type: text/plain\r\n\r\n";
    char endbuf1[] =  "\r\n-----------------------------1727381797619608628249622620\r\n"
                     "Content-Disposition: form-data; name=\"submit\"\r\n\r\n"                     
                     "Submit\r\n-----------------------------1727381797619608628249622620--\r\n";
    char endbuf[] = "\r\n-----------------------------1727381797619608628249622620--";
    
    if( ( file = fopen( "log.txt", "r") ) != NULL )
    {
        fseek(file, 0L, SEEK_END);
        fsize = ftell(file);
        
        memset(buf, '\0', sizeof(buf));
        sprintf(buf, header, strlen(startbuf) + strlen(endbuf) + fsize);
        //printf("file size: %d, startbuf: %d, endbuf: %d\n", fsize, sizeof(startbuf), sizeof(endbuf));

        send(socket, buf, strlen(buf), 0);
        printf("%s", buf);

        send(socket, startbuf, strlen(startbuf), 0);        
        printf("%s", startbuf);

        fseek(file, 0L, SEEK_SET);
        memset(buf, '\0', sizeof(buf));
        while (!feof(file))
        {
            size = fread(buf, sizeof(buf), 1, file);
                
            n = send(socket, buf, size, 0);
            //send(socket, "\n", 1, 0);
            
            printf("%s", buf);
        }                 


        send(socket, endbuf, strlen(endbuf), 0);
        printf("%s", endbuf);

        fclose( file );        
    }

    //memset(buf, '\0', sizeof(buf));
    //read(socket, buf, sizeof(buf)-1);
    //printf("%s\n", buf);

}

int main()
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80); 
    serv_addr.sin_addr.s_addr = gethostbyname("netsecsample.netau.net");

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 
    else printf("connection sucessful!\n");
    upload(sockfd);
    /*
    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    } 

    if(n < 0)
    {
        printf("\n Read error \n");
    } 
    */
    close(sockfd);
    return 0;
}
