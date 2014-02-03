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



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "utility.h"
#include "OpenVPNConfig.h"

int ReadOpenVPNConf ( char * filename,MiniOpenVPNConf *conf)
{
	FILE *ffd;
	char  line[BUFSIZ], fullpath[256]= {"\0"},*pch;

	CombinePath(filename,fullpath);

	if ((ffd = fopen(fullpath, "r")) == NULL)
		return 0;


	//set the status empty
	strcpy(conf->status,"\0");

	while(!feof(ffd))
	{
		if (feof(ffd)) break;

		fgets(line,BUFSIZ,ffd);

		pch = strtok(line," ");

		if(!strcmp(str_tolower(pch),"port"))
		{
			pch = strtok(NULL," ");
			conf->port = atoi(pch);
		}
		else if(!strcmp(str_tolower(pch),"proto"))
		{
			pch = strtok(NULL," ");
			strcpy(conf->proto,pch);
		}
		else if(!strcmp(str_tolower(pch),"status"))
		{
			pch = strtok(NULL," ");
			if(!isalnum(pch[strlen(pch)-1]))
				pch[strlen(pch)-1] = '\0';
            //for linux remove cariage return \r
            if(pch[strlen(pch)-1] == '\r')
				pch[strlen(pch)-1] = '\0';
			strcpy(conf->status,pch);
		}
		else { /*Unknown leave it*/}


	}
	return 1;

}
