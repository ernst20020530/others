

#include "stdafx.h"
#include "AssemblyCriticalSection.h"


namespace assemblyImplementation
{
	AssemblyCriticalSection::AssemblyCriticalSection():
		assembly_cs(0)
	{}

	void AssemblyCriticalSection::Lock()
	{
		EnterCriticalSection(&assembly_cs);
	}

	void AssemblyCriticalSection::Unlock()
	{
		LeaveCriticalSection(&assembly_cs);
	}

	void AssemblyCriticalSection::EnterCriticalSection(int volatile *_cs)
	{
		__asm {
		spinlock_begin:
			mov eax, dword ptr[_cs]
			mov ecx, 1
			xchg ecx, dword ptr[eax]		//exchange value in ecx and eax
			cmp ecx, 0						
			jne spinlock_begin				//if the value in ecx is not equal to 0, jump back to spinlock_begin
											//the Operating System need to yield via interrupt in spinlock to avoid starvation
											//and other threads can get the cpu cycle
		}
	}

	void AssemblyCriticalSection::LeaveCriticalSection(int volatile *_cs)
	{
		__asm {
			mov eax, dword ptr[_cs]
			mov dword ptr[eax], 0			//set the value of cs to 0
		}
	}
}