// Measuring CPU frequency by comparing the Time Stamp Counter (TSC) against a known time reference.
#include <x86intrin.h>
#include <sys/time.h>
#include <stdint.h>
#include <iostream>

typedef uint64_t u64;
typedef double f64;

static u64 GetOSTimerFreq(void) {
    return 1000000; // in unix smallest unit returned by gettimeofday is microseconds
}

//Note: Returns the number of microseconds since the start of the system i think 
static u64 ReadOSTimer(void) {
    struct timeval value;
    gettimeofday(&value, 0);

    u64 result = GetOSTimerFreq()*(u64)value.tv_sec + (u64)value.tv_usec;
    return result;
}

// Returns the value of the timer count of the CPU
inline u64 ReadCPUTimer(void) {
    return __rdtsc();
}

static u64 EstimateCPUTimerFreq(void) {
	u64 MillisecondsToWait = 100;
	u64 OSFreq = GetOSTimerFreq();

	u64 CPUStart = ReadCPUTimer();
	u64 OSStart = ReadOSTimer();
	u64 OSEnd = 0;
	u64 OSElapsed = 0;
	u64 OSWaitTime = OSFreq * MillisecondsToWait / 1000;
	while(OSElapsed < OSWaitTime)
	{
		OSEnd = ReadOSTimer();
		OSElapsed = OSEnd - OSStart;
	}
	
	u64 CPUEnd = ReadCPUTimer();
	u64 CPUElapsed = CPUEnd - CPUStart;
	
	u64 CPUFreq = 0;
	if(OSElapsed)
	{
		CPUFreq = OSFreq * CPUElapsed / OSElapsed;
	}
	
	return CPUFreq;
}

/*
// We just get the timer counts/ticks for 1 second or for specific milliseconds of OS time, which gives us the approx for clock freq
int main(int ArgCount, char **Args) {
    u64 MillisecondsToWait = 1000;
    if (ArgCount == 2) {
        MillisecondsToWait = atol(Args[1]);
    }
    u64 OSFreq = GetOSTimerFreq();
    std::cout << "OSFreq: " << OSFreq << std::endl;

    u64 CPUStart = ReadCPUTimer();
    u64 OSStart = ReadOSTimer();
    u64 OSEnd = 0;
    u64 OSElapsed = 0;
    u64 OSWaitTime = OSFreq * MillisecondsToWait / 1000;
    // run for about 1 second or for specified milliseconds
    while (OSElapsed < OSWaitTime){
        OSEnd = ReadOSTimer();
        OSElapsed = OSEnd - OSStart;
    }

    u64 CPUEnd = ReadCPUTimer();
    u64 CPUElapsed = CPUEnd - CPUStart;
    f64 CPUFreq = 0;
    if (OSElapsed) {
        //           rate of real time passage * (number of CPU cycle that occured during that time)
        CPUFreq = ((f64)OSFreq / OSElapsed) * CPUElapsed;
    }
    f64 CPUElapsedSeconds = (f64)CPUElapsed / CPUFreq;

    std::cout << "OS Timer: " << OSStart << " -> " << OSEnd << " = " << OSElapsed << " elapsed" << std::endl;
    std::cout << "OS Seconds: " << (f64)OSElapsed/(f64)OSFreq << std::endl;
    std::cout << "CPU Timer: " << CPUStart << " -> " << CPUEnd << " = " << CPUElapsed << " elapsed" << std::endl;
    std::cout << "CPU Elapsed Seconds: " << CPUElapsedSeconds << std::endl;
    std::cout << "CPU Freq: " << CPUFreq / 1000000000 << " GHz (guessed) " << std::endl;
    return 0;
}
*/