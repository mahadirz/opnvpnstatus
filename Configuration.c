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


#include "Configuration.h"
#include "utility.h"

void ReadOurCOnf_free(Ourconf * c)
{
	FREE_IF_NOT_NULL(c->password);
}

int ReadOurConf (Ourconf *conf)
{
	FILE *ffd;
	char name[BUFSIZ], value[BUFSIZ];

	//init password to null
	conf->password = NULL;

	if ((ffd = fopen(CONFIG_FILE, "r")) == NULL)
	{
		if(DEBUG_MODE == 1)
			printf("Malloc error in function ReadOurConf\n");
		return 0;
	}

	while(!feof(ffd))
	{
		if (feof(ffd)) break;

		if (fscanf(ffd, "%s %[^\n]\n", name, value) != 2) continue;

		//trim white space at end
		if(isspace(value[strlen(value)-1]))
			value[strlen(value)-1] = '\0';

		//check key pair
		if(strcmp(str_tolower(name),"openvpnpath") == 0)
		{
			strcpy(conf->path,value);
		}
		else if(strcmp(str_tolower(name),"port") == 0)
		{
			conf->port = atoi(value);
		}
		else if(strcmp(str_tolower(name),"blowfish_encrypt") == 0)
		{
			if(strcmp(str_tolower(value),"true") == 0)
				conf->enc_blowfish = 1;
            else
                conf->enc_blowfish = 0;
		}
		else if(strcmp(str_tolower(name),"password") == 0)
		{
			conf->password = (char*) malloc(strlen(value)+1);
			if(conf->password == NULL)
			{
				if(DEBUG_MODE == 1)
					printf("Malloc error in function ReadOurConf\n");
				return 0;
			}
			strcpy(conf->password,value);
		}

	}
	return 1; //success
}
