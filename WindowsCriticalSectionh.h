#pragma once


extern unsigned volatile __int32 win_cs;

namespace windowsImplementation
{

	/**
	implement critical section by Windows API
	*/

	class WindowsCriticalSection
	{
	public:
		WindowsCriticalSection();
		void Lock();
		void Unlock();
	private:
		void EnterCriticalSection(unsigned __int32 volatile *cs);
		void LeaveCriticalSection(unsigned __int32 volatile *cs);
		unsigned volatile __int32 win_cs;
	};
}