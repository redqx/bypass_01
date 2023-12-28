#pragma once
#define PORT 8080
#define BUFFER_SIZE 1024

enum operateID
{
    EXEC,
};


//struct net_data
//{
//	int id_operate;
//	char* k16;
//	char* data;
//};

void handle_request(int client_socket);
void mm_readfile(char* argc_fpath, char** argc_fileMem, int* argc_fileLen);
void handle_http_request(char* request, char* argc_method, char* argc_fpath, char* argc_fname);




//流量包加密
void* genKey16();
void xxtea(char* c, char* k, unsigned int len, int mod);
