#pragma once

#include <atomic>

namespace stdLibraryImplementation
{

	/**
	implement critical section by stl library
	*/

	class StlLibraryCriticalSection
	{
	public:
		StlLibraryCriticalSection();
		void Lock();
		void Unlock();
	private:
		void EnterCriticalSection(std::atomic<bool> &cs);
		void LeaveCriticalSection(std::atomic<bool> &cs);
		std::atomic<bool> stlLibrary_cs;
	};
}