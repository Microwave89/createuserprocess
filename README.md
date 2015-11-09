# createuserprocess
Three Examples of Directly Using Vista's NtCreateUserProcess

Whereas it was necessary to perform four system calls (NtOpenFile, NtCreateSection, NtCreateProcess(Ex), NtCreateThread(Ex)) in order to create a new usermode process in old XP days, much of the process creation internals has been moved to the Windows kernel in Vista+.
In order to launch a new process there is only a single system service to be called, that is, "NtCreateUserProcess".
Consequently, the complexity of NtCreateUserProcess is somewhat greater than the one of for example NtCreateProcessEx.
I already managed to make NtCreateUserProcess working back in spring 2015. However, I did not properly note the parameters and flags (and its meanings) used and, what is most important, the former implementation relied upon calling two apparently crucial RtlXxx calls (RtlCreateProcessParameters(Ex), RtlNormalizeProcessParameters).
Having the implemetation not rely on too much RtlXxx calls is particularly important when using things like raw syscalls and tables of them in order to avoid using absolute addresses.

The three examples do not rely on any imports but NtCreateUserProcess and NtTerminateProcess.
- If executable is started it first forks itself and exits.
- Then the clone senses that it has been forked and does not further attempt to fork itself but continues execution.
- The forked process now attempts to create a protected process "svchost.exe".
- Then it creates a nonprotected "svchost.exe" and terminates itself.
