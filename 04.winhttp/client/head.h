#pragma once
enum operateID
{
    EXEC,
    KEY16,
};
void load_shellcode(char* argc_buf, int argc_bufsize);

void xxtea(char* c, char* k, unsigned int len, int mod);