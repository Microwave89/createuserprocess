#include "global.h"
#include "nttypes.h"

void dispError(NTSTATUS status) {
	ULONG dummy;
	syscallStub(0x14D, status, 0, 0, NULL, 0, &dummy);
}


NTSTATUS createStandardProcess(PUNICODE_STRING pProcessImageName) {
	PS_CREATE_INFO procInfo;
	RTL_USER_PROCESS_PARAMETERS userParams;
	PS_ATTRIBUTE_LIST attrList;
	PS_PROTECTION protectionInfo;

	NTSTATUS status = STATUS_PENDING;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	///We should supply a minimal environment (environment variables). Following one is simple yet fits our needs. 
	char data[2 * sizeof(ULONGLONG)] = { 'Y', 0x00, 0x3D, 0x00, 'Q', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	if (pProcessImageName) {
		protectionInfo.Signer = (UCHAR)PsProtectedSignerNone;
		protectionInfo.Type = (UCHAR)PsProtectedTypeNone;
		protectionInfo.Audit = 0;

		RtlSecureZeroMemory(&userParams, sizeof(RTL_USER_PROCESS_PARAMETERS));
		RtlSecureZeroMemory(&attrList, sizeof(PS_ATTRIBUTE_LIST) - sizeof(PS_ATTRIBUTE));
		RtlSecureZeroMemory(&procInfo, sizeof(PS_CREATE_INFO));

		userParams.Length = sizeof(RTL_USER_PROCESS_PARAMETERS);
		userParams.MaximumLength = sizeof(RTL_USER_PROCESS_PARAMETERS);
		attrList.TotalLength = sizeof(PS_ATTRIBUTE_LIST) - sizeof(PS_ATTRIBUTE);
		procInfo.Size = sizeof(PS_CREATE_INFO);

		userParams.Environment = (WCHAR*)data;
		userParams.EnvironmentSize = sizeof(data);
		userParams.EnvironmentVersion = 0;
		userParams.Flags = RTL_USER_PROCESS_PARAMETERS_NORMALIZED;

		attrList.Attributes[0].Attribute = PsAttributeValue(PsAttributeImageName, FALSE, TRUE, FALSE);
		attrList.Attributes[0].Size = pProcessImageName->Length;
		attrList.Attributes[0].Value = (ULONG_PTR)pProcessImageName->Buffer;

		status = NtCreateUserProcess(&hProcess, &hThread, MAXIMUM_ALLOWED, MAXIMUM_ALLOWED, NULL, NULL, 0, THREAD_CREATE_FLAGS_CREATE_SUSPENDED, &userParams, &procInfo, &attrList);
	}
	else {
		status = STATUS_INVALID_PARAMETER;
	}

	return status;
}


NTSTATUS createProtectedProcess(PS_PROTECTED_SIGNER protectedSigner, PS_PROTECTED_TYPE protectedType) {
	UNICODE_STRING uImageName;
	PS_CREATE_INFO procInfo;
	RTL_USER_PROCESS_PARAMETERS userParams;
	PS_ATTRIBUTE_LIST attrList;
	PS_PROTECTION protectionInfo;

	NTSTATUS status = STATUS_PENDING;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	WCHAR imageName[] = { L"\\??\\C:\\Windows\\System32\\svchost.exe" };
	///We should supply a minimal environment (environment variables). Following one is simple yet fits our needs. 
	char data[2 * sizeof(ULONGLONG)] = { 'Y', 0x00, 0x3D, 0x00, 'Q', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	if ((protectedSigner < PsProtectedSignerMax) && (protectedType < PsProtectedTypeMax)) {
		protectionInfo.Signer = (UCHAR)protectedSigner;
		protectionInfo.Type = (UCHAR)protectedType;
		protectionInfo.Audit = 0;

		uImageName.Buffer = imageName;
		uImageName.Length = sizeof(imageName) - sizeof(UNICODE_NULL);
		uImageName.MaximumLength = sizeof(imageName);

		RtlSecureZeroMemory(&userParams, sizeof(RTL_USER_PROCESS_PARAMETERS));
		RtlSecureZeroMemory(&attrList, sizeof(PS_ATTRIBUTE_LIST));
		RtlSecureZeroMemory(&procInfo, sizeof(PS_CREATE_INFO));

		userParams.Length = sizeof(RTL_USER_PROCESS_PARAMETERS);
		userParams.MaximumLength = sizeof(RTL_USER_PROCESS_PARAMETERS);
		attrList.TotalLength = sizeof(PS_ATTRIBUTE_LIST);
		procInfo.Size = sizeof(PS_CREATE_INFO);

		userParams.Environment = (WCHAR*)data;
		userParams.EnvironmentSize = sizeof(data);
		userParams.EnvironmentVersion = 0;
		userParams.Flags = RTL_USER_PROCESS_PARAMETERS_NORMALIZED;

		attrList.Attributes[0].Attribute = PsAttributeValue(PsAttributeProtectionLevel, FALSE, TRUE, TRUE);
		attrList.Attributes[0].Size = sizeof(UCHAR);
		attrList.Attributes[0].Value = *(UCHAR*)&protectionInfo;

		attrList.Attributes[1].Attribute = PsAttributeValue(PsAttributeImageName, FALSE, TRUE, FALSE);
		attrList.Attributes[1].Size = uImageName.Length;
		attrList.Attributes[1].Value = (ULONG_PTR)uImageName.Buffer;

		status =  NtCreateUserProcess(&hProcess, &hThread, MAXIMUM_ALLOWED, MAXIMUM_ALLOWED, NULL, NULL, PROCESS_CREATE_FLAGS_PROTECTED_PROCESS, THREAD_CREATE_FLAGS_CREATE_SUSPENDED, &userParams, &procInfo, &attrList);
	}
	else {
		status = STATUS_INVALID_PARAMETER;
	}

	return status;
}


NTSTATUS forkProcess(void) {
	PS_CREATE_INFO procInfo;

	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;

	procInfo.Size = sizeof(PS_CREATE_INFO);

	return NtCreateUserProcess(&hProcess, &hThread, MAXIMUM_ALLOWED, MAXIMUM_ALLOWED, NULL, NULL, PROCESS_CREATE_FLAGS_INHERIT_FROM_PARENT, THREAD_CREATE_FLAGS_CREATE_SUSPENDED, NULL, &procInfo, NULL);
}


void mymain(void) {
	UNICODE_STRING uImageName;
	WCHAR imageName[] = { L"\\??\\C:\\Windows\\System32\\svchost.exe" };

	if (!forkProcess())
		NtTerminateProcess(INVALID_HANDLE_VALUE, 0);

	uImageName.Buffer = imageName;
	uImageName.Length = sizeof(imageName) - sizeof(UNICODE_NULL);
	uImageName.MaximumLength = sizeof(imageName);

	dispError(createStandardProcess(&uImageName));
	dispError(createProtectedProcess(PsProtectedSignerLsa, PsProtectedTypeProtectedLight));

	NtTerminateProcess(INVALID_HANDLE_VALUE, 0);
}
