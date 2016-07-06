#include "ParallelUtils.h"

#if defined(_WIN32)
#	include <Windows.h>
#	include <ppl.h>
#else
#	include <future>
#endif

NAMESPACE_UTILITY

int ParallelUtils::ProcessorCount()
{
#if defined(_WIN32)
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
#else
	return std::thread::hardware_concurrency();
#endif
}

void ParallelUtils::ParallelFor(size_t From, size_t To, const std::function<void(size_t)> &F)
{
#if defined(_WIN32)
	concurrency::parallel_for(From, To, [&](size_t i)
	{
		F(i);
	});
#else
	std::vector<std::future<void>> futures;

	for (size_t i = From; i < To; ++i)
	{
		auto fut = std::async([i, F]()
		{
			F(i);
		});
		futures.push_back(std::move(fut));
	}

	for (size_t i = 0; i < futures.size(); ++i)
		futures[i].wait();

	futures.clear();
#endif
}

NAMESPACE_UTILITYEND