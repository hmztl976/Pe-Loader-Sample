#ifndef FAKEIAT_H_INCLUDED
#define FAKEIAT_H_INCLUDED
#include <windows.h>

void InitFakeIAT(PVOID base, int size);

void FakeAddress(PIMAGE_THUNK_DATA data, DWORD address, LPCSTR name);

#endif // FAKEIAT_H_INCLUDED
