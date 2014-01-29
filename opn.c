/*
BETA Version 29012014
Copyright 2014 Mahadir Lab

ermission is hereby granted, free of charge, to any person obtaining a copy
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


void webHttp(void)
{
    char conf[10240], tmpname[256];
    Opnconf s;
    OpnStatusResult * opnstats;
    int NUM_FILES,NUM_OF_STATS_ENTRIES,i;
    //JSON
    char *JSON = NULL;

    DIR * dp;
    struct dirent *ep;


    char httpheader[] = "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=UTF-8\r\n\r\n";
    char * httpresponse;
    int one = 1, client_fd;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        err(1, "can't open socket");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1)
    {
        close(sock);
        err(1, "Can't bind");
    }

    listen(sock, 5);
    while (1)
    {
        client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
        printf("got connection\n");

        if (client_fd == -1)
        {
            perror("Can't accept");
            continue;
        }

        //open directory
        dp = opendir (path);

        NUM_FILES = i = 0;
        strcpy(conf,"\0");
        strcpy(tmpname,"\0");
        opnstats  =  NULL;
        httpresponse = NULL;


        if (dp != NULL)
        {
            while ((ep = readdir (dp)))
            {
                strcpy(tmpname,ep->d_name);
                if(IsConfExt(ep->d_name) == 1)
                {

                    parse_conffile(tmpname,conf);
                    parseConf(conf,&s);

                    //Create dynamic array of opnstats to store all stats from all status files
                    opnstats = (OpnStatusResult *) realloc(opnstats,sizeof(OpnStatusResult) * (NUM_FILES+1));

                    opnstats[NUM_FILES].BasicConf = s;
                    NUM_OF_STATS_ENTRIES = ReadStatusFile(opnstats[NUM_FILES].BasicConf.status,&opnstats[NUM_FILES]);
                    opnstats[NUM_FILES].TotalStatus = NUM_OF_STATS_ENTRIES;

                    NUM_FILES++;
                }
            }
            (void) closedir (dp);
        }
        else
            perror ("Couldn't open the directory");

        JSON = NULL;
        JSON = FormatToJSON(opnstats,NUM_FILES);
        //printf("%s\n\n",JSON);

        httpresponse = (char*) malloc(strlen(httpheader)+ strlen(JSON) + 4);
        strcpy(httpresponse,httpheader);
        strcat(httpresponse,JSON);
        strcat(httpresponse,"\r\n");

        //printf("%s",httpresponse);


        write(client_fd, httpresponse, strlen(httpresponse) - 1); /*-1:'\0'*/
        close(client_fd);


        //free the allocated memory
        free(httpresponse);
        free(JSON);
        for(i=0; i<NUM_FILES; i++)
        {
            free(opnstats[i].ArrStatus);
        }
        free(opnstats);
    }
}


int IsConfExt(char  filename[])
{
    char * pch=NULL;
    int i=0;
    pch = strtok(filename,".");
    while(pch != NULL)
    {
        if(i == 1 && strcmp(pch,"conf") == 0)
            return 1;
        pch = strtok(NULL,".");
        i++;
    }
    return 0;
}

int parse_conffile(char * configpath,char * tout){
	FILE * pfile;
	char buffer[1025] ,tmp[100];
	int i=0;

	strcpy(tmp,path);
	strcat(tmp,configpath);
	pfile = fopen(tmp,"r");

	//check if cannot open file for read
	if(pfile == NULL){
		return 0;
	}

	while(!feof(pfile)){
		fgets(buffer,1024,pfile);


		//remove hash comment
		//if (regex_search (buffer,regex("#.*") ))
		//printf("%c\n",buffer[0]);
		if(buffer[0] == '#')
			continue;

		//remove semicolon comment
		//if (regex_search (buffer,regex(";.*") ))
		if(buffer[0] == ';')
			continue;

		//remove empty line
		//if (regex_search (buffer,regex("^\n$") ))
		if(buffer[0] == '\n')
			continue;

		//size += strlen(buffer)+1 ;
		//output = (char *) realloc(output,size*sizeof(char));
		if(i == 0)
			strcpy(tout,buffer);
		else
			strncat(tout,buffer,strlen(buffer));
		i++;
	}
	fclose(pfile);
	return 1;

}


void parseConf(char * config,Opnconf * c){
	char * pch,conc[100]={'\0'};
	int n=0,i,cc=0;
	//read the config
	for(i=0; i  < strlen(config); i++){
		if(config[i] == '\n' && conc[0] != '\0'){
			conc[cc] = '\0';
			cc = 0;
			pch = strtok(conc," ");
			while(pch != NULL){
				if(n == 1){
					strcpy(c->status,pch);
					n=0;
					break;
				}
				else if(n == 2){
					strcpy(c->port,pch);
					n=0;
					break;
				}
				else if(n == 3){
					strcpy(c->proto,pch);
					n=0;
					break;
				}
				else if(strcmp(pch,"status") == 0)
					n = 1;
				else if(strcmp(pch,"port") == 0)
					n = 2;
				else if(strcmp(pch,"proto") == 0)
					n = 3;
				pch = strtok (NULL, " ");
			}
			strcpy(conc,"\0");
			continue;
		}
		conc[cc] = config[i];
		cc++;
	}
}



int ReadStatusFile(char *filename,OpnStatusResult * opnstats){
	FILE * pfile;
	char buffer[1025],*pch,tmp[100];
	int i,j,entry=0;

	//reset to null to prevent error in realloc
	opnstats->ArrStatus = NULL;

	strcpy(tmp,path);
	strcat(tmp,filename);
	pfile = fopen(tmp,"r");

	//check if cannot open file for read
	if(pfile == NULL){
		printf("Error open\n");
		return 0;
	}

	for(i=0; i>-1; i++){
		if(feof(pfile))
			break;
		fgets(buffer,1024,pfile);

		//proceed when at line 4-1 onwards
		if(i >=3){
			//stop when reaching ROUTING TABLE
			if(strcmp(buffer,"ROUTING TABLE\n") == 0)
				break;

			//create dynamic array of ArrStatus
			opnstats->ArrStatus = (OpnStatus *) realloc(opnstats->ArrStatus,sizeof(OpnStatus)*(entry+1));
			if(opnstats->ArrStatus == NULL){
				printf("Error allocate memory");
				exit(1);
			}

			pch = strtok(buffer,",");
			j=0;
			while(pch != NULL){

				if(j==0){
					strcpy(opnstats->ArrStatus[entry].CommonName,pch);
					//debug
					//printf("%s\n",opnstats->ArrStatus[j].CommonName);
				}
				else if(j==1){
					strcpy(opnstats->ArrStatus[entry].RealAddress,pch);
					//debug
					//printf("%s\n",opnstats->ArrStatus[j].RealAddress);
				}
				else if(j==2){
					strcpy(opnstats->ArrStatus[entry].BytesR,pch);
					//debug
					// printf("%s\n",opnstats->ArrStatus[j].BytesR);
				}
				else if(j==3){
					strcpy(opnstats->ArrStatus[entry].BytesS,pch);
					//debug
					// printf("%s\n",opnstats->ArrStatus[j].BytesS);
				}
				else if(j==4){
					pch[strlen(pch)-1] = '\0';
					strcpy(opnstats->ArrStatus[entry].ConnectedSince,pch);
					//debug
					//printf("%s\n",opnstats->ArrStatus[j].ConnectedSince);
				}



				j++;
				pch = strtok(NULL,",");
			}
			entry++;
		}
	}
	fclose(pfile);
	return j;

}

/**
Format the OpnStatusResult structure into
JSON
Currenlty only support 60KB size of string per
status file
**/
char * FormatToJSON(OpnStatusResult OpnStruct[],int Total){
	int i,j;
	char tmp[61440]; //60KB
	char * tmp2=NULL;
	char * OutputText;

	//allocate 1 char + NULL terminated char
	OutputText = (char*) malloc(sizeof(char) * 2);
	strcpy(OutputText,"{");

	for(i=0; i< Total; i++){
		//clear tmp
		strcpy(tmp,"\0");
		for(j=0; j<OpnStruct[i].TotalStatus; j++){
			if(j== 0){
				sprintf(tmp,"[\"%s\",%s,%s,\"%s\"]",OpnStruct[i].ArrStatus[j].CommonName,OpnStruct[i].ArrStatus[j].BytesR,\
					OpnStruct[i].ArrStatus[j].BytesS,OpnStruct[i].ArrStatus[j].ConnectedSince);
			}
			else {
				sprintf(tmp,"%s,[\"%s\",%s,%s,\"%s\"]",tmp,OpnStruct[i].ArrStatus[j].CommonName,OpnStruct[i].ArrStatus[j].BytesR,\
					OpnStruct[i].ArrStatus[j].BytesS,OpnStruct[i].ArrStatus[j].ConnectedSince);
			}

		}

		//temporarily store the tmp to other variable
		tmp2 = (char*) realloc(tmp2,sizeof(char)*strlen(tmp)+1);
		strcpy(tmp2,tmp);

		//puts(stats);
		if(i == 0){
			sprintf(tmp,"\"%s_%s\":[%s]",\
				OpnStruct[i].BasicConf.port,OpnStruct[i].BasicConf.proto,tmp2);
		}
		else{
			sprintf(tmp,",\"%s_%s\":[%s]",\
				OpnStruct[i].BasicConf.port,OpnStruct[i].BasicConf.proto,tmp2);
		}

		OutputText = (char*) realloc(OutputText,sizeof(char) * (strlen(tmp)+strlen(OutputText)+1));
		strncat(OutputText,tmp,strlen(tmp));

	}

	//free the memory resources tmp2
	free(tmp2);

	OutputText = (char*) realloc(OutputText,sizeof(char) * (strlen(OutputText)+2));
	if(OutputText == NULL){
		//error handling
	}
	strcat(OutputText,"}");
	//strcpy(OutputText,store);

	return OutputText;
}
