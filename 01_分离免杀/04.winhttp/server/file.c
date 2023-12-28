#include "head.h"

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

 

void mm_readfile(char* argc_fpath,char** argc_fileMem,int* argc_fileLen)
{
    FILE* fd;
    long file_size;
    char* buffer;

    // 打开二进制文件
    fd = fopen(argc_fpath, "rb");
    if (fd == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // 获取文件大小
    fseek(fd, 0, SEEK_END);
    file_size = ftell(fd);
    rewind(fd);

    // 分配内存存储文件内容
    buffer = (char*)malloc(file_size + 256);
    if (buffer == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    // 读取文件内容到内存
    if (fread(buffer, 1, file_size, fd) != file_size) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }

    // 在这里可以使用内存中的文件内容（buffer）

    // 关闭文件和释放内存
    fclose(fd);
    //free(buffer);
    *argc_fileLen = file_size;
    *argc_fileMem = buffer;
    return;
}
