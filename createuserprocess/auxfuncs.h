#pragma once
extern NTSTATUS syscallStub(ULONG syscallNum, ...);
//__forceinline extern void myTerminate(void);
extern BOOLEAN mymemcmp(char* src1, char* src2, SIZE_T length);