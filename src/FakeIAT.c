#include "FakeIAT.h"

PVOID pOriginalIATBase;
int IATsize;
PDWORD pMyIAT;
LPCSTR * pMyNameTable;
PBYTE FuncList;
int FakeFunction();

void InitFakeIAT(PVOID base, int size)
{
    pOriginalIATBase = base;
    IATsize = size;
    pMyIAT = (PDWORD)malloc(size);
    pMyNameTable = (LPCSTR *)malloc(size);
    FuncList = (PBYTE)VirtualAlloc(NULL, size,
                                   MEM_COMMIT | MEM_TOP_DOWN,
                                   PAGE_EXECUTE_READWRITE);
}

DWORD pFakeFunc = FakeFunction;

//不支持使用ordinal方式导入的函数，例如mfc库函数

void FakeAddress(PIMAGE_THUNK_DATA data, DWORD address, LPCSTR name)
{
    PBYTE ptr = (PBYTE)address;
    if((ptr[0] == 0x8b && ptr[1] == 0xff) ||
       (ptr[0] == 0xff && ptr[1] == 0x25))
       //MOV edi, edi  8B FF
       //windows 系统函数起始指令，用于hot-patching
       //GetProcessId及类似函数通过读取本程序fs段寄存器工作
       //起始部位不是本条指令，所以并未hook
       //JMP  dword ptr [abs32]  FF 25 xx xx xx xx
    {
        int offset = ((int)data - (int)pOriginalIATBase) / 4;
        pMyIAT[offset] = address;
        pMyNameTable[offset] = name;
        offset = offset * 8;
        FuncList[offset] = 0xff;//CALL dword ptr [abs32]  FF 15 xx xx xx xx
        FuncList[offset + 1] = 0x15;
        *(PDWORD)(FuncList + offset + 2) = (DWORD)&pFakeFunc;
        FuncList[offset + 6] = 0xc3;//RET
        //data->u1.Function = address;
        data->u1.Function = (DWORD)(FuncList + offset);
    }
    else//dll导出变量
    {
        data->u1.Function = address;
    }
}

DWORD lastAddr = 0;
int times = 0;
int FakeFunction()
{
    DWORD dwECX, dwEDX;
    PBYTE retAddr, retAddr2;
    #ifdef __GNUC__
    //可能用于传参 所以进行保护
    __asm__ __volatile__("movl %%ecx, %[dwECX]" : [dwECX] "=m" (dwECX) :);
    __asm__ __volatile__("movl %%edx, %[dwEDX]" : [dwEDX] "=m" (dwEDX) :);
    __asm__ __volatile__("movl 4(%%ebp), %[retAddr]" : [retAddr] "=r" (retAddr) :);
    __asm__ __volatile__("movl 8(%%ebp), %[retAddr2]" : [retAddr2] "=r" (retAddr2) :);
	#else
	#error
    #endif // __GNUC__
    PDWORD offset = (PDWORD)(retAddr - 6);
    DWORD index = ((DWORD)offset - (DWORD)FuncList) / 8;
    if(lastAddr != retAddr2)
    {
        times = 1;
        lastAddr = retAddr2;
        printf("\ncalling %s at %p for times %0.5d", pMyNameTable[index], retAddr2, times);
    }
    else
    {
        times++;
        printf("\b\b\b\b\b%0.5d", times);
    }
    DWORD dwEP = pMyIAT[index];
    #ifdef __GNUC__
    __asm__ __volatile__ ("movl %0, %%eax\n\t"
                         "movl %[dwECX], %%ecx\n\t"
                         "movl %[dwEDX], %%edx\n\t"
                         "leave\n\t"
                         "add $4, %%esp\n\t"
                         "jmp %%eax\n\t"
            :: "r" (dwEP), [dwECX] "m" (dwECX), [dwEDX] "m" (dwEDX) : "%eax");
	#else
	#error
    #endif // __GNUC__
    return 0;
}
