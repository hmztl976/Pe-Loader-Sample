#ifndef _PE_LDR_H
#define _PE_LDR_H

#include <tchar.h>
#include <windows.h>
#include <winnt.h>

typedef struct {
	TCHAR					*pTargetPath;
	PIMAGE_DOS_HEADER		pDosHeader;
	PIMAGE_NT_HEADERS		pNtHeaders;

	BOOLEAN					bLoadFromBuffer;
	DWORD					dwImage;
	DWORD					dwImageSizeOnDisk;

	DWORD					dwLoaderBase;
	DWORD					dwLoaderRelocatedBase;

	DWORD					dwMapBase;

    BOOLEAN                 bNeedToReloc;
} PE_LDR_PARAM;

VOID PeLdrInit(PE_LDR_PARAM *pe);
BOOL PeLdrSetExecutablePathAndInfo(PE_LDR_PARAM *pe, TCHAR *pExecutable, DWORD dwCommandOffset);
BOOL PeLdrSetExecutableBuffer(PE_LDR_PARAM *pe, PVOID pExecutable, DWORD dwLen);
BOOL PeLdrStart(PE_LDR_PARAM *pe);

#endif
