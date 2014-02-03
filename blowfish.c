/*
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


#include "http.h"

#define IVEC_VAL "12345678"

void OpnStatus_webHttp(void)
{
    OpnStatusResult * Opnstats ;
    int NUM_OF_STATUS_FILES,len,turn;
    char * JSON;
    unsigned char *encrypted ,*decrypted,ivec[8] ;
    Ourconf myconf;

    BF_KEY bf_key;

    char http_octet_header[] = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream; charset=UTF-8\r\n\r\n";
    char http_html_header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
    char http_json_header[] = "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=UTF-8\r\n\r\n";

    //Read configuration file
    ReadOurConf(&myconf);
    strcpy(path,myconf.path);


    char * httpresponse;
    int one = 1, client_fd;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        if(DEBUG_MODE == 1)
            printf("can't open socket\n");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(myconf.port);

    if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1)
    {
        close(sock);
        if(DEBUG_MODE == 1)
            printf("Socket binding failed\n");
    }

    listen(sock, 5);
    while (1)
    {
        client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
        if(DEBUG_MODE == 1)
            printf("got new connection\n");

        if (client_fd == -1)
        {
            if(DEBUG_MODE == 1)
                printf("Can't accept connection\n");
            continue;
        }

        //always reset to null before use by realloc
        Opnstats =NULL;
        encrypted = decrypted = NULL;

        NUM_OF_STATUS_FILES = ReadConf_InDirectory(path,&Opnstats);
        JSON = NULL;
        JSON = FormatToJSON(Opnstats,NUM_OF_STATUS_FILES);


        if(DEBUG_MODE == 1)
            printf("RAW JSON: %s\n",JSON);

        //enable encryption
        if(myconf.enc_blowfish == 1)
        {

            //encryption begin
            encrypted = (unsigned char* ) calloc(strlen(JSON)+1, sizeof(char));
            BF_set_key(&bf_key,strlen(myconf.password),(unsigned char* ) myconf.password);
            memcpy(ivec,IVEC_VAL,8);
            turn = 0;
            BF_cfb64_encrypt((unsigned char *)JSON,encrypted,(long) strlen(JSON),&bf_key,ivec,&turn,BF_ENCRYPT);

            //write data to client
            write(client_fd, encrypted, strlen(JSON) );

            FILE *pfile = fopen("json.txt","wb");
            fwrite(encrypted,1,strlen(JSON),pfile);
            fclose(pfile);

            //show decrypted data
            decrypted = (unsigned char* ) calloc(strlen(JSON)+1, sizeof(char));
            memcpy(ivec,IVEC_VAL,8);
            turn = 0;
            BF_cfb64_encrypt(encrypted,decrypted,(long) strlen(JSON),&bf_key,ivec,&turn,BF_DECRYPT);
            if(DEBUG_MODE == 1)
                printf("\n\nDecrypted data:\n%s\n",decrypted);


            FREE_IF_NOT_NULL(decrypted);

            FREE_IF_NOT_NULL(encrypted);
        }
        else
        {
            httpresponse = (char* ) calloc(strlen(http_octet_header)+ strlen(JSON) + 4,sizeof(char));
            strcpy(httpresponse,http_json_header);
            strcat(httpresponse,JSON);
            strcat(httpresponse,"\r\n");
            if(DEBUG_MODE == 1)
                printf("\nHttpResponse: %s\n",httpresponse);
            write(client_fd, httpresponse, strlen(httpresponse) ); /*-1:'\0'*/
            //free the allocated memory
            free(httpresponse);
        }


        close(client_fd);


        FREE_IF_NOT_NULL(JSON);
        FormatToJSON_free(Opnstats,NUM_OF_STATUS_FILES);

    }
    ReadOurCOnf_free(&myconf);
}

