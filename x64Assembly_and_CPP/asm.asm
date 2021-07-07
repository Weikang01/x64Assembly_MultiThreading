;HANDLE WINAPI CreateThread(
;    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
;    _In_ SIZE_T dwStackSize,
;    _In_ LPTHREAD_START_ROUTINE lpStartAddress,
;    _In_opt_ LPVOID lpParameter,
;    _In_ DWORD dwCreationFlags,
;    _Out_opt_ LPDWORD lpThreadId
;);
;
;    * LPSECURITY_ATTRIBUTES
;    LPSECURITY_ATTRIBUTES attrib: This is a pointer to a security descriptor structure that can be used to allow and disallow the new threads handle from being inherited. For us, this parameter is irrelevant, we will set it to NULL.
;
;    * SIZE_T stackSize
;    This is the size of the new thread's stack in bytes. The parameter will be rounded up to the nearest page size (64k).
;    Setting this value to 0 means "use the default stack size" which is the same as the stack size of the calling thread. The default stack size can be changed in the project's properties in the Linker -> System section.
;    The creation flags parameter is used to specify if this value (the SIZE_T stackSize parameter) means the commit or reserve size. Reserve is the actual amount of memory initially allocated and the amount the allocated stack memory grows as required.
;
;    * LPTHREAD_START_ROUTINE lpStartAddress
;    This is the function that the thread is to execute when it starts. The prototype to the function should look like the following: 
;        DWORD WINAPI ThreadFunction(LPVOID parameters);
;    Careful: Be very careful when using C++, do not declare it as returning "void" and cast in the CreateThread function. This previously worked fine but may cause crashes on newer hardware.
;    without the Microsoft type names, the prototype is:
;    unsigned long __stdcall ThreadFunction(void* parameters);
;
;    * LPVOID lpParameter
;    This is a pointer to any parameter/s the thread is to be passed as its "void*" or "LPVOID" parameter in the thread's start function, highlighted in blue in the prototype below:
;        DWORD WINAPI ThreadFunction(LPVOID parameters);
;    If the thread does not need any parameters, then this can be NULL.
;    Anything at all be passed as a parameter to the thread and cast in the thread's start function. A void* could point to a single variable, a structure, or an array of 100 pointers to 100 different parameters for the thread, it's very open and flexible.
;
;    * DWORD dwCreationFlags
;    This parameter is used to specify how the thread should be created. A value of 0 means the thread should start straight away. A value of 4 means the thread should be created but it should wait for a call to ResumeThread before it starts executing its start function.
;    (Note: You can also set bit 16. If bit 16 is set, then the stack size parameter is read as meaning the reserve size, otherwise it is the commit size.)
;
;    * LPDWORD lpThreadId
;    This parameter is used if the calling thread wants a record of the newly created thread's ID. This parameter can be NULL if the calling thread doesn't care.
;    LPDWORD is the same as "unsigned long*".

.data
extern CreateThread: proc

.code

ThreadStartProc proc
	mov rax, 1000000
	LoopHead:
	lock inc dword ptr [rcx]

	dec	rax
	jnz LoopHead
	
	ret
ThreadStartProc endp

CreateThreadInASM proc
	push rbp  ; Save the caller's RBP (stack frame)
	mov rbp, rsp  ; Save rsp of stack frame to rbp

	push 0 ; Thread ID
	push 0 ; Creation flags, start immediately

	sub rsp, 20h  ; Reserve 4 quads of shadow space

	mov r9, rcx   ; Move *i to r9
	mov rcx, 0    ; Security attributes
	mov rdx, 0    ; Stack size, use the same stack size as the calling thread
	mov r8, ThreadStartProc

	call CreateThread

	mov rsp, rbp
	pop rbp  ; Restore the caller's RBP
	ret
CreateThreadInASM endp

end