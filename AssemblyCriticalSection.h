#pragma once


namespace assemblyImplementation
{

	/**
	implement critical section by assembly x86
	*/

	class AssemblyCriticalSection
	{
	public:
		AssemblyCriticalSection();
		void Lock();
		void Unlock();
	private:
		void EnterCriticalSection(int volatile *_cs);
		void LeaveCriticalSection(int volatile *_cs);

		int volatile assembly_cs;
	};
}