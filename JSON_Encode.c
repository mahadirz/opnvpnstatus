
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

#include "JSON_Encode.h"

void FormatToJSON_free(OpnStatusResult OpnStruct[],int Total)
{
	int i;
	for(i=0; i<Total; i++)
	{
		FREE_IF_NOT_NULL(OpnStruct[i].ArrStatus);
	}
	FREE_IF_NOT_NULL(OpnStruct);
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

	if(Total <= 0){
		OutputText = (char*) malloc(strlen("[{\"client\":\"empty\"}\"]")+1);
		strcpy(OutputText,"[{\"client\":\"empty\"}\"]");
		return OutputText;
	}

	//allocate 1 char + NULL terminated char
	OutputText = (char*) malloc(sizeof(char) * 2);
	strcpy(OutputText,"{");

	for(i=0; i< Total; i++){
		//clear tmp
		strcpy(tmp,"\0");
		for(j=0; j<OpnStruct[i].TotalStatus; j++){
			if(j== 0){
				sprintf(tmp,"[\"%s\",\"%s\",%s,%s,\"%s\"]",OpnStruct[i].ArrStatus[j].CommonName,OpnStruct[i].ArrStatus[j].RealAddress, \
                    OpnStruct[i].ArrStatus[j].BytesR,OpnStruct[i].ArrStatus[j].BytesS,OpnStruct[i].ArrStatus[j].ConnectedSince);
			}
			else {
				sprintf(tmp,"%s,[\"%s\",\"%s\",%s,%s,\"%s\"]",tmp,OpnStruct[i].ArrStatus[j].CommonName,OpnStruct[i].ArrStatus[j].RealAddress, \
                    OpnStruct[i].ArrStatus[j].BytesR,OpnStruct[i].ArrStatus[j].BytesS,OpnStruct[i].ArrStatus[j].ConnectedSince);
			}

		}

		//temporarily store the tmp to other variable
		tmp2 = (char*) realloc(tmp2,sizeof(char)*strlen(tmp)+1);
		strcpy(tmp2,tmp);

		//puts(stats);
		if(i == 0){
			sprintf(tmp,"\"%d_%s\":[%s]",\
				OpnStruct[i].BasicConf.port,OpnStruct[i].BasicConf.proto,tmp2);
		}
		else{
			sprintf(tmp,",\"%d_%s\":[%s]",\
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
