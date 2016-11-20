#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include "nmrpd.h"

#ifdef NMRPFLASH_OSX
#include <mach/mach_time.h>
#endif

volatile sig_atomic_t g_interrupted = 0;

time_t time_monotonic()
{
#ifndef NMRPFLASH_WINDOWS
#ifndef NMRPFLASH_OSX
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec;
#else
	static double factor = 0.0;
	mach_timebase_info_data_t timebase;
	if (factor == 0.0) {
		mach_timebase_info(&timebase);
		factor = (double)timebase.numer / timebase.denom;
	}

	return round(mach_absolute_time() * factor / 1e9);
#endif
#else
	return round(GetTickCount() / 1000.0);
#endif
}

char *lltostr(long long ll, int base)
{
	static char buf[32];
	snprintf(buf, sizeof(buf) - 1, (base == 16 ? "%llx" : (base == 8 ? "%llo" : "%lld")), ll);
	return buf;
}

uint32_t bitcount(uint32_t n)
{
	uint32_t c;
	for (c = 0; n; ++c) {
		n &= n - 1;
	}
	return c;
}

uint32_t netmask(uint32_t count)
{
	return htonl(count <= 32 ? 0xffffffff << (32 - count) : 0);
}

void xperror(const char *msg)
{
	if (errno != EINTR) {
		perror(msg);
	} else {
		printf("\n");
	}
}
