# createuserprocess
Three Examples of Directly Using Vista's NtCreateUserProcess

NOTE: Protected Process creation applies only to Windows 8.1+!

Whereas it was necessary to perform four system calls (NtOpenFile, NtCreateSection, NtCreateProcess(Ex), NtCreateThread(Ex)) in order to create a new ready to run usermode process in old XP days, much of the process creation internals has been moved to the Windows kernel in Vista+.
In order to launch a new process there is only a single system service to be called, that is, "NtCreateUserProcess".
Consequently, the complexity of NtCreateUserProcess is somewhat greater than the one of for example NtCreateProcessEx.
I already managed to make NtCreateUserProcess working back in spring 2015. However, I did not properly note the parameters and flags (and its meanings) used and, what is most important, the former implementation relied upon calling two apparently crucial ntdll.dll library calls (RtlCreateProcessParameters(Ex), RtlNormalizeProcessParameters).

Having the implementation not rely on any library imports allows for bypassing any usermode hooks by leveraging the technique used in the syscalltest PoC.

In the main.c file there are 3 examples for successfully calling NtCreateUserProcess with the very minimum of information supplied.
forkProcess() shows how to fork, createStandardProcess shows how to launch an arbitrary (native) non-protected process, and createProtectedProcess takes both a PsProtectedSignerXxx as well as a PsProtectedTypeXxx value and attempts to create a protected process with the specified protection options. Note that in all examples the process is created suspended.

The three examples do not rely on any imports but NtCreateUserProcess and NtTerminateProcess both which are pure system service calls and thus can be described with solely a NT syscall number.

The examples are merged in a single example entirely implemented in main().
Function of combined example is as follows:
- If the executable is started it first forks itself and exits.
- Then the clone senses that it has been forked and does not further attempt to fork itself but continues execution.
- The forked process now attempts to create a protected process "svchost.exe".
- Then it creates a nonprotected "svchost.exe" and terminates itself.

Since there is missing almost the entire Windows subsystem most program won't run longer than 0.1 ms before they will crash or simply exit. "NtCreateUserProcess" is meant to be used in conjunction with native processes only.
You can make most of it if you choose a process which does not force itself to run protected (not like services.exe or csrss.exe). Then you can overwrite the ntdll!LdrInitializeThunk function with your shellcode (ntdll.dll is always mapped into any process unlike you unmap it!) which ideally does not rely on anything but... raw system calls :) 
