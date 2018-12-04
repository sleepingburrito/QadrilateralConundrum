//loading files
#include "DataManager.h"
DataManager_Files loadFileBuffer;
#define FOLDERNAME "DATA"
#define CD_BUFFER loadFileBuffer.address
#define CD_BUFFER_SIZE loadFileBuffer.size

void * LoadCDFileBuffer(char * filename){
	loadFileBuffer.filename = filename;
	printf("loading %s\n", loadFileBuffer.filename);
	DataManager_LoadDatas(FOLDERNAME, &loadFileBuffer);
	printf("done\n");
	return CD_BUFFER;
}