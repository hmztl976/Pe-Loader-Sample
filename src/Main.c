#include "PeLdr.h"
#include "Debug.h"
#include "FakeIAT.h"
static int ShowUsage()
{
	printf("-- PE Loader Sample --\n\n");
	printf("PeLdr [PE-File]\n");
	printf("\n");

	return 0;
}
extern int _CRT_glob;
extern char * _acmdln;
extern wchar_t * _wcmdln;
extern
#ifdef __cplusplus
"C"
#endif
void __wgetmainargs(int*,wchar_t***,wchar_t***,int,int*);
#define BUFSIZE 1024
int main()
{
    wchar_t **enpv, **argv;
	int argc, si = 0;
	__wgetmainargs(&argc, &argv, &enpv, _CRT_glob, &si);
	if(argc < 2)
		return ShowUsage();
    DWORD  retval=0;
    TCHAR  fullname[BUFSIZE];
    retval = SearchPath(NULL, argv[1], NULL, BUFSIZE, fullname, NULL);

    if (retval == 0)
    {
        printf ("GetFullPathName failed (%d)\n", GetLastError());
        return 0;
    }
    else
    {
        _tprintf(TEXT("The full path name is:  %s\n"), fullname);
    }
    DWORD size = wcslen(argv[0]);
    size += 1;
    _acmdln += size;
    _wcmdln += size;
    HINSTANCE hkernelbase = (HINSTANCE)GetModuleHandleA("kernelbase.dll");
    char ** pGetCmdLA;
    wchar_t ** pGetCmdLW;
    pGetCmdLA = *(char***)(GetProcAddress(hkernelbase, "GetCommandLineA")+1);
    *pGetCmdLA += size;
    pGetCmdLW = *(wchar_t***)(GetProcAddress(hkernelbase, "GetCommandLineW")+1);
    *pGetCmdLW += size;

	PE_LDR_PARAM peLdr;
	PeLdrInit(&peLdr);
	PeLdrSetExecutablePathAndInfo(&peLdr, fullname, size);
	PeLdrStart(&peLdr);

	return 0;
}
