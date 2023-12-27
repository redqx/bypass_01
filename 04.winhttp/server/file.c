#include "head.h"

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

 

void mm_readfile(char* argc_fpath,char** argc_fileMem,int* argc_fileLen)
{
    FILE* fd;
    long file_size;
    char* buffer;

    // �򿪶������ļ�
    fd = fopen(argc_fpath, "rb");
    if (fd == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // ��ȡ�ļ���С
    fseek(fd, 0, SEEK_END);
    file_size = ftell(fd);
    rewind(fd);

    // �����ڴ�洢�ļ�����
    buffer = (char*)malloc(file_size + 256);
    if (buffer == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    // ��ȡ�ļ����ݵ��ڴ�
    if (fread(buffer, 1, file_size, fd) != file_size) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }

    // ���������ʹ���ڴ��е��ļ����ݣ�buffer��

    // �ر��ļ����ͷ��ڴ�
    fclose(fd);
    //free(buffer);
    *argc_fileLen = file_size;
    *argc_fileMem = buffer;
    return;
}
