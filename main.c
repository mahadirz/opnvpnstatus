/*
Version 03022014
Copyright 2014 Mahadir Lab

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include <sys/stat.h>
#include <openssl/blowfish.h>


char path[256] ;
int DEBUG_MODE = 0;

#include "Configuration.c"
#include "utility.c"
#include "OpenVPNConfig.c"
#include "OpenVPNStatus.c"
#include "JSON_Encode.c"
#include "http.c"


int main(int argc, char * argv[])
{
    //if command line set to program
    if(argc == 2)
    {
        if(strcmp(argv[1],"-debug")== 0)
        {
            DEBUG_MODE = 1;
            OpnStatus_webHttp();
            return (0);
        }
        else
        {
            printf("Invalid argument\n\n");
            return (1);
        }
    }
    else
    {

        pid_t process_id = 0;
        pid_t sid = 0;
        // Create child process
        process_id = fork();
        // Indication of fork() failure
        if (process_id < 0)
        {
            printf("Daemon fork failed!\n");
            // Return failure in exit status
            exit(1);
        }
        // PARENT PROCESS. Need to kill it.
        if (process_id > 0)
        {
            printf("\nDaemon successfuly created with pid %d \n", process_id);
            // return success in exit status
            exit(0);
        }
        //unmask the file mode
        umask(0);
        //set new session
        sid = setsid();
        if(sid < 0)
        {
            // Return failure
            printf("Daemon failed!\n");
            exit(1);
        }
        // Change the current working directory
        //chdir("/root/");
        // Close stdin. stdout and stderr
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        // Open a log file in write mode.

        // Implement and call some function that does core work for this daemon.
        // call http which process the function
        OpnStatus_webHttp();
        return 0;
    }
}






