


#include "stdafx.h"
#include "StlLibraryCriticalSection.h"

#include <thread>

namespace stdLibraryImplementation
{
	StlLibraryCriticalSection::StlLibraryCriticalSection():
		stlLibrary_cs(false)
	{}

	void StlLibraryCriticalSection::Lock()
	{
		EnterCriticalSection(stlLibrary_cs);
	}

	void StlLibraryCriticalSection::Unlock()
	{
		LeaveCriticalSection(stlLibrary_cs);
	}

	void StlLibraryCriticalSection::EnterCriticalSection(std::atomic<bool> &cs)
	{
		///if cs is false, set cs to true, and leave this function. if cs is true, spin lock
		while (cs.exchange(true))
			std::this_thread::yield();		//yield, and other thread can get cpu cycle to avoid starvation
	}

	void StlLibraryCriticalSection::LeaveCriticalSection(std::atomic<bool> &cs)
	{
		///set cs to false atomically
		cs.store(false);
	}

}