
#include "stdafx.h"
#include <list>
#include <algorithm>
#include <thread>

#include "StlLibraryCriticalSection.h"
#include "WindowsCriticalSectionh.h"
#include "AssemblyCriticalSection.h"

#define BOOST_TEST_MODULE example
#include <boost\test\included\unit_test.hpp>
#include <boost\test\data\test_case.hpp>

typedef std::list<std::pair<int, int>> SingleUnit;
typedef std::list<SingleUnit> MultiUnit;

/**
@brief		global count accessed by multi thread to be incremented.
			Use STL Library critical section, Windows critical section, assembly critical section to protect count
*/

int count = 0;

/**
use global variable to store the data to prove the crtical section works
*/
MultiUnit	g_multiUnit;
SingleUnit	g_singleUnit;

const int increment_rounds = 10;

namespace TestStlLibraryCriticalSection
{
	///Use STL Library critical section
	stdLibraryImplementation::StlLibraryCriticalSection stlLibraryCriticalSection;
	void Increment(int id, int interval)
	{
		for (int k = 0; k < increment_rounds; ++k)
		{
			stlLibraryCriticalSection.Lock();		///enter critical section
			g_singleUnit.clear();
			for (int i = 0; i < increment_rounds; ++i)
			{
				g_singleUnit.push_back(std::make_pair(id, count++));
				std::this_thread::sleep_for(std::chrono::milliseconds(interval));
			}
			g_multiUnit.push_back(g_singleUnit);
			stlLibraryCriticalSection.Unlock();		///leave critical section
		}
	}
}

namespace TestWindowsCriticalSection
{
	///use Windows critical section
	windowsImplementation::WindowsCriticalSection windowsCriticalSection;
	void Increment(int id, int interval)
	{
		for (int k = 0; k < increment_rounds; ++k)
		{
			windowsCriticalSection.Lock();		///enter critical section
			g_singleUnit.clear();
			for (int i = 0; i < increment_rounds; ++i)
			{
				g_singleUnit.push_back(std::make_pair(id, count++));
				std::this_thread::sleep_for(std::chrono::milliseconds(interval));
			}
			g_multiUnit.push_back(g_singleUnit);
			windowsCriticalSection.Unlock();	///leave critical section
		}
	}
}

namespace TestAssemblyCriticalSection
{
	///use assembly critical section
	assemblyImplementation::AssemblyCriticalSection assemblyCriticalSection;
	void Increment(int id, int interval)
	{
		for (int k = 0; k < increment_rounds; ++k)
		{
			assemblyCriticalSection.Lock();		///enter critical section
			g_singleUnit.clear();
			for (int i = 0; i < increment_rounds; ++i)
			{
				g_singleUnit.push_back(std::make_pair(id, count++));
				std::this_thread::sleep_for(std::chrono::milliseconds(interval));
			}
			g_multiUnit.push_back(g_singleUnit);
			assemblyCriticalSection.Unlock();	///leave critical section
		}
	}
}

template<typename TestFunc>
void TestPro(TestFunc func, int threadCount, int incrementInterval)
{
	///start multi thread to test critical section
	std::list<std::thread> threadList;
	for (int i = 0; i < threadCount; threadList.push_back(std::move(std::thread(func, ++i, incrementInterval))));
	std::for_each(threadList.begin(), threadList.end(), [](std::thread &t) {t.join(); });

	SingleUnit	mergedRecords;
	std::for_each(g_multiUnit.begin(), g_multiUnit.end(), [&](const SingleUnit &singleUnit) {
		///In the critical section, check the thread id, if any thread id in singleUnit is not unique, multipile threads access in critical section
		auto itMax = std::max_element(singleUnit.begin(), singleUnit.end(), [](const std::pair<int, int> &data1, const std::pair<int, int> &data2) { return data1.first < data2.first; });
		auto itMin = std::min_element(singleUnit.begin(), singleUnit.end(), [](const std::pair<int, int> &data1, const std::pair<int, int> &data2) { return data1.first < data2.first; });
		bool singleUnitNotEmpty = itMax != singleUnit.end() && itMin != singleUnit.end();
		BOOST_TEST(singleUnitNotEmpty, "count value recorded in single critical section is empty");
		if (singleUnitNotEmpty)		BOOST_TEST(itMax->first == itMin->first, "multipile threads access in critical section");

		std::for_each(singleUnit.begin(), singleUnit.end(), [&](std::pair<int, int> record) {mergedRecords.push_back(record); });	///tranform log data into single list
	});

	/**
	check if the log of count value is in ascending order, an each value is unique, 
	and adjacent count value is expected to be incremented by 1
	*/
	BOOST_TEST(std::is_sorted(mergedRecords.begin(), mergedRecords.end(), [](const std::pair<int, int> &data1, const std::pair<int, int> &data2) {return data1.second < data1.second; }), "logged count value is not in ascending order");
	bool adjacentIncremented = std::adjacent_find(mergedRecords.begin(), mergedRecords.end(), [](const std::pair<int, int> &data1, const std::pair<int, int> &data2) {return data2.second != data1.second + 1; }) == mergedRecords.end();
	BOOST_TEST(adjacentIncremented, "adjacent count value is not incremented");
}

BOOST_AUTO_TEST_SUITE(test_suite)

/**
unit test arguments. input the cartesian products of test argument into test case
*/
int threadsCounts[] = { 2, 3, 4, 5 };		///in each test, test 2, 3, 4, 5 threads respectively
int incrementIntervalInMilliseconds[] = { 0, 5, 10, 15 };		///in each test, interval between count value increment is 0, 5, 10, 15 milliseconds

///test assemblyCriticalSection with cartesian products of test arguments
BOOST_DATA_TEST_CASE(test_assemblyCriticalSection, boost::unit_test::data::make(threadsCounts) * incrementIntervalInMilliseconds, threadsCount, incrementIntervalInMillisecond)
{
	count = 0;
	g_multiUnit.clear();
	TestPro(TestAssemblyCriticalSection::Increment, threadsCount, incrementIntervalInMillisecond);
}

///test windowsCriticalSection with cartesian products of test arguments
BOOST_DATA_TEST_CASE(test_windowsCriticalSection, boost::unit_test::data::make(threadsCounts) * incrementIntervalInMilliseconds, threadsCount, incrementIntervalInMillisecond)
{
	count = 0;
	g_multiUnit.clear();
	TestPro(TestWindowsCriticalSection::Increment, threadsCount, incrementIntervalInMillisecond);
}

///test stdLibraryCriticalSection with cartesian products of test arguments
BOOST_DATA_TEST_CASE(test_stdLibraryCriticalSection, boost::unit_test::data::make(threadsCounts) * incrementIntervalInMilliseconds, threadsCount, incrementIntervalInMillisecond)
{
	count = 0;
	g_multiUnit.clear();
	TestPro(TestStlLibraryCriticalSection::Increment, threadsCount, incrementIntervalInMillisecond);
}


BOOST_AUTO_TEST_SUITE_END()


