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


#include "OpenVPNStatus.h"

int ReadConf_InDirectory(char * dir,OpnStatusResult ** Opnstats){
	DIR * dp;
	struct dirent *ep;
	char tmpname[25];
	MiniOpenVPNConf tmp_opnconf;
	int NUM_FILES_STATUS=0,NUM_OF_STATS_ENTRIES=0;


	//debug header in function ReadConf_InDirectory
	printf("\n#### ReadConf_InDirectory ######\n");

	//open directory
	dp = opendir (dir);
	if (dp != NULL)
	{
		while ((ep = readdir (dp)))
		{
			//store original filename
			strcpy(tmpname,ep->d_name);
			if(IsConfExt(ep->d_name) == 1)
			{
				if(DEBUG_MODE == 1)
					printf("\n-------\n");
				printf("Parsing config file %s\n",tmpname);
				if(ReadOpenVPNConf(tmpname,&tmp_opnconf))
				{
					if(Is_FileExist(tmp_opnconf.status))
					{

						//Create dynamic array of opnstats to store all stats from all status files
						*Opnstats = (OpnStatusResult *) realloc(*Opnstats,sizeof(OpnStatusResult) * (NUM_FILES_STATUS+1));
						if(*Opnstats == NULL)
						{
							if(DEBUG_MODE == 1)
								printf("Heap error in realloc\n");
							continue;
						}
						(*Opnstats)[NUM_FILES_STATUS].BasicConf = tmp_opnconf;

						if(DEBUG_MODE == 1)
							printf("Reading status file %s \n",(*Opnstats)[NUM_FILES_STATUS].BasicConf.status);

						if((NUM_OF_STATS_ENTRIES = ReadStatusFile((*Opnstats)[NUM_FILES_STATUS].BasicConf.status,&(*Opnstats)[NUM_FILES_STATUS]))>0)
						{
							if(DEBUG_MODE == 1)
								printf("NUM_OF_STATS_ENTRIES: %d\n",NUM_OF_STATS_ENTRIES);
							(*Opnstats)[NUM_FILES_STATUS].TotalStatus = NUM_OF_STATS_ENTRIES;
							NUM_FILES_STATUS++;
						}
						else
						{
							if(DEBUG_MODE == 1)
								printf("No client connected ReadOpenVPNConf() returned:%d\n",NUM_OF_STATS_ENTRIES);
							continue;
						}

					}
					else
					{
						if(DEBUG_MODE == 1)
							printf("%s not exist\n",tmp_opnconf.status);
						continue;
					}
				}

			}
			if(DEBUG_MODE == 1)
				printf("-------\n");
		}
		(void) closedir (dp);
	}
	else
		if(DEBUG_MODE == 1)
			printf ("Couldn't open the directory");

	return NUM_FILES_STATUS;
}



int ReadStatusFile(char *filename,OpnStatusResult * opnstats){
	FILE * pfile;
	char buffer[BUFSIZ],*pch,fullpath[256];
	int i,j,entry=0;

	//reset to null to prevent error in realloc
	opnstats->ArrStatus = NULL;

	CombinePath(filename,fullpath);
	pfile = fopen(fullpath,"r");

	//check if cannot open file for read
	if(pfile == NULL){
		if(DEBUG_MODE == 1)
			printf("Error open file %s\n",filename);
		return 0;
	}

	for(i=0; i>-1; i++){
		if(feof(pfile))
			break;
		fgets(buffer,BUFSIZ,pfile);

		//proceed when at line 4-1 onwards
		if(i >=3){
			//stop when reaching ROUTING TABLE
			if(strcmp(buffer,"ROUTING TABLE\n") == 0)
				break;

			//create dynamic array of ArrStatus
			opnstats->ArrStatus = (OpnStatus *) realloc(opnstats->ArrStatus,sizeof(OpnStatus)*(entry+1));
			if(opnstats->ArrStatus == NULL){
				if(DEBUG_MODE == 1)
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
	return entry;

}
