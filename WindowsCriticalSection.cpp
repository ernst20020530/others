

#include "stdafx.h"
#include "WindowsCriticalSectionh.h"

#include <Windows.h>

namespace windowsImplementation
{
	WindowsCriticalSection::WindowsCriticalSection():
		win_cs(0)
	{}

	void WindowsCriticalSection::Lock()
	{
		EnterCriticalSection(&win_cs);
	}

	void WindowsCriticalSection::Unlock()
	{
		LeaveCriticalSection(&win_cs);
	}

	void WindowsCriticalSection::EnterCriticalSection(unsigned __int32 volatile *cs)
	{
		/**
		InterlockedCompareExchange is atomic operation
		if the value of cs is 0, jump out of while loop, if the value of cs is 1, spin lock
		if the value of cs is 0, set cs to 1, if the value of cs is 1, no operation is performed
		*/
		while (::InterlockedCompareExchange(cs, 1, 0) == 1)
			::Sleep(0);					//yield, and other thread can get cpu cycle to avoid starvation
	}

	void WindowsCriticalSection::LeaveCriticalSection(unsigned __int32 volatile *cs)
	{
		/**
		if the value of cs is 1, set cs to 0.
		if the value of cs is 0, no operation is performed
		*/
		::InterlockedCompareExchange(cs, 0, 1);
	}
}