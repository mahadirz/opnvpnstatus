#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "utility.h"

char * str_tolower(char * str)
{
	int i;
	char newstr[100];
	strcpy(newstr,str); //prevent direct editing the content
	for(i=0; newstr[i]!='\0';i++){
		newstr[i] = tolower(newstr[i]);
	}
	return newstr;
}

void CombinePath(const char * filename,char * fullpath)
{
	//reset fullpath
	strcpy(fullpath,"\0");
	if(path[strlen(path)-1] == '/') { //-1 last char
		strcat(fullpath,path);
		strcat(fullpath,filename);
	}
	else
	{
		strcat(fullpath,path);
		strcat(fullpath,"/");
		strcat(fullpath,filename);
	}
}

/**
Check if file exist
return 1 if exist
or 0 if not exist or
error open the file
**/
int Is_FileExist(char * filename){
    char fullpath[256];
    FILE *pfile;
    //strcpy(fullpath,path);
    //strcat(fullpath,filename);
    CombinePath(filename,fullpath);
    if((pfile = fopen(fullpath,"r"))){
        fclose(pfile);
        return 1;
    }
    return 0;
}

/**
Check whether the file is
conf extension
**/
int IsConfExt(char  filename[])
{
    char * pch=NULL;
    int i=0,valid=0;
    pch = strtok(filename,".");
    while(pch != NULL)
    {
        if(i == 1 && strcmp(str_tolower(pch),"conf") == 0)
            valid=1;
		else if(i==2) // mean other extension after .conf
			valid = 0;
        pch = strtok(NULL,".");
        i++;
    }
    return valid;
}

