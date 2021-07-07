/*
- Multi-Core Processor
Most modern CPU's have more than one core. They are like a collection of CPU's, all sitting on the one die. Each of these cores is a physical execution unit, it's a piece of hardware which reads and executes machine code from memory. The cores are connected to the rest of the system via data buses, they can read and write to memory and PCI devices etc.

- Thread
A Core is a physical piece of hardware that executes machine code, a thread is a software abstraction of the same thing. Threads are able to execute code, just like a core, except the Operating System can create many more threads than there are physical cores. And, the OS can divvy out the workload of, say, 100 threads to the 4 cores of a Quad Core processor.
The Operating System by does this "task switching". This means that the OS switches threads on and off, granting threads to a physical core for a small duration of time. After some time, the OS will turn off an executing thread, save its state so the thread can be reloaded later, and switch to another thread.
So a core is a physical piece of hardware, and a thread is an OS controlled execution unit, which is divvied out between physical cores.

- Cores, Threads and Registers
Each core has its own set of registers, including the x86 registers, the MMX, FPU, SSE and AVX registers, as well as debugging registers and performance counters. They are not shared between cores, so Core0's RAX has nothing to do with Core1's RAX. Collectively, the registers form the state of the core. The states of these registers is one of the things which the OS saves and loads when it switches threads to physical cores.
Although a core and a thread are different entities, it is the OS which controls switching, and to us a thread is the same. Each thread has it's own set of registers.

- Caches
Caches are their own topic. They're small portions of memory residing on each core, or shared between the cores, thich is designed to improve performance of programs. On modern x86 CPU's there's two, and sometimes 3 caches. They are called L1, L2 and L3. L1 is the smallest, but fastest, L1 is not shared between cores. L2 and L3 are larger storages, but they are slower, and they may be a shared resource.
Caches are automatically used by the hardware, and we have very little control over them.

________________________________________________________________
|CPU                                                            |
|_______________________________________________________________|
||Core 0              |Core 1              | Core 2            ||
||____________________|____________________|___________________||
||x86 Registers      ||x86 Registers      ||x86 Registers      ||
||RAX, RBX, RCX      ||RAX, RBX, RCX      ||RAX, RBX, RCX      ||
||Stack Pointer      ||Stack Pointer      ||Stack Pointer      ||
||Data Pointer       ||Data Pointer       ||Data Pointer       ||
||Instruction Pointer||Instruction Pointer||Instruction Pointer||
||Etc.               ||Etc.               ||Etc.               ||
||___________________||___________________||___________________||
||X87 FPU Registers  ||X87 FPU Registers  ||X87 FPU Registers  ||
||MMX Registers      ||MMX Registers      ||MMX Registers      ||
||___________________||___________________||___________________||
||SSE/AVX Registers  ||SSE/AVX Registers  ||SSE/AVX Registers  ||
||___________________||___________________||___________________||
||L1 Cache           ||L1 Cache           ||L1 Cache           ||
||___________________||___________________||___________________||
|________/\__________|_________/\_________|_________/\__________|
         ||                    ||                   ||
_________\/____________________\/___________________\/__________
|L2/L3 Shared Cache                                            |
|________/\____________________/\___________________/\_________|
         ||                    ||                   ||
_________\/____________________\/___________________\/__________
|System memory, PCI devices etc.                               |
|______________________________________________________________|

- The general strategy
The general strategy behind multi-threading is simple. Imaging we have a problem which takes a long time for a single core to solve depicted on the left. The single core runs an algorithm to process the data, but it takes a long time.
The idea behind multi-threading is to split the big problem into two or more smaller problems. And, to execute the algorithms on these sub-problems in parallel, using multiple threads. In theory, we would double the speed of our program.

- Windows.h
The functions which create, synchronize and run multiple threads are part of the operating system's kernel. The actual machine code instructions which create multiple threads cannot be called unless you are writing an operating system or driver.
In Windows application programming this means we #include the Windows.h file in our project.

- CreateThread

HANDLE WINAPI CreateThread(
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ SIZE_T dwStackSize,
    _In_ LPTHREAD_START_ROUTINE lpStartAddress,
    _In_opt_ LPVOID lpParameter,
    _In_ DWORD dwCreationFlags,
    _Out_opt_ LPDWORD lpThreadId
);

    * LPSECURITY_ATTRIBUTES
    LPSECURITY_ATTRIBUTES attrib: This is a pointer to a security descriptor structure that can be used to allow and disallow the new threads handle from being inherited. For us, this parameter is irrelevant, we will set it to NULL.

    * SIZE_T stackSize
    This is the size of the new thread's stack in bytes. The parameter will be rounded up to the nearest page size (64k).
    Setting this value to 0 means "use the default stack size" which is the same as the stack size of the calling thread. The default stack size can be changed in the project's properties in the Linker -> System section.
    The creation flags parameter is used to specify if this value (the SIZE_T stackSize parameter) means the commit or reserve size. Reserve is the actual amount of memory initially allocated and the amount the allocated stack memory grows as required.

    * LPTHREAD_START_ROUTINE lpStartAddress
    This is the function that the thread is to execute when it starts. The prototype to the function should look like the following: 
        DWORD WINAPI ThreadFunction(LPVOID parameters);
    Careful: Be very careful when using C++, do not declare it as returning "void" and cast in the CreateThread function. This previously worked fine but may cause crashes on newer hardware.
    without the Microsoft type names, the prototype is:
    unsigned long __stdcall ThreadFunction(void* parameters);

    * LPVOID lpParameter
    This is a pointer to any parameter/s the thread is to be passed as its "void*" or "LPVOID" parameter in the thread's start function, highlighted in blue in the prototype below:
        DWORD WINAPI ThreadFunction(LPVOID parameters);
    If the thread does not need any parameters, then this can be NULL.
    Anything at all be passed as a parameter to the thread and cast in the thread's start function. A void* could point to a single variable, a structure, or an array of 100 pointers to 100 different parameters for the thread, it's very open and flexible.

    * DWORD dwCreationFlags
    This parameter is used to specify how the thread should be created. A value of 0 means the thread should start straight away. A value of 4 means the thread should be created but it should wait for a call to ResumeThread before it starts executing its start function.
    (Note: You can also set bit 16. If bit 16 is set, then the stack size parameter is read as meaning the reserve size, otherwise it is the commit size.)

    * LPDWORD lpThreadId
    This parameter is used if the calling thread wants a record of the newly created thread's ID. This parameter can be NULL if the calling thread doesn't care.
    LPDWORD is the same as "unsigned long*".
*/
/*
*/

#include <iostream>

using namespace std;


extern "C" char GetValueFromASM();

int main()
{
	cout << "ASM Said: " << GetValueFromASM() << endl;
}