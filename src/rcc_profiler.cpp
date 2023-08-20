#include "rcc_profiler.h"
#include <stdint.h>
#include <sys/time.h>

/**
 * @brief  Get the frequency of the OS timer. 
 * 
 * (run `sysctl -a` and see `hw.tbfrequency`)
 * 
 * @return Frequency of the OS timer in Hz.
 */
inline uint64_t getProfilerOsTimerFrequency()
{
    return 24000000;
}

/**
 * @brief  Read the current value of the OS timer.
 *
 * This function retrieves the current time from the system's timer, converting 
 * the result into a single value based on the timer's frequency.
 * 
 * @return The current OS timer value.
 */
inline uint64_t readProfilerOsTimer()
{
	timeval Value;
	gettimeofday(&Value, 0);
	
	uint64_t Result = getProfilerOsTimerFrequency()*(uint64_t)Value.tv_sec + (uint64_t)Value.tv_usec;
	return Result;
}

/**
 * @brief  Read the current value of the CPU timer.
 *
 * This function directly accesses the ARM architecture's virtual counter 
 * (CNTVCT_EL0) to retrieve the current CPU timer value.
 * 
 * @return The current CPU timer value.
 */
inline uint64_t readProfilerCpuTimer() {
    uint64_t val;
    asm volatile("mrs %0, cntvct_el0" : "=r" (val));
    return val;
}

/**
 * @brief  Calculate the time difference in seconds between two timer values.
 * 
 * @param  After The timer value representing the later point in time.
 * @param  Before The timer value representing the earlier point in time.
 * 
 * @return The difference in time between the two timer values in seconds.
 */
inline float64_t getProfilerTimeDifferenceInSec(uint64_t Before, uint64_t After)
{
    float64_t Result = float64_t(After - Before) / (float64_t)getProfilerOsTimerFrequency();
    return Result;
}

/**
 * @brief Initializes the profiler by allocating memory for profiler entries.
 * 
 * This function allocates an initial space for 16 profiler entries and sets the global size.
 * It should be called once at the start of the profiling session.
 */
void initializeProfiler()
{
    if (!gIsProfilerInitialized) {
        gProfilerEntries = (profiler_entry*)malloc(sizeof(profiler_entry) * 16);
        gProfilerEntriesCapacity = 16;
        gIsProfilerInitialized = true;
    }
}

/**
 * @brief Finalizes the profiler and releases any dynamically allocated memory.
 * 
 * This function releases the memory used for profiler entries and resets global counters.
 * It should be called once at the end of the profiling session to ensure no memory leaks.
 */
void finalizeProfiler()
{
    if (gIsProfilerInitialized) {
        float64_t BaseTime = 0;
        json_value JsonMemberArray[gProfilerEntriesSize - 1];

        // gProfilerEntries[gProfilerEntriesSize - 1] indicates the total program duration
        for (size_t i = 0; i < gProfilerEntriesSize - 1; i++) {
            json_object JsonObjectProfilerEntry;
            const char* Name = gProfilerEntries[i].Name;
            float64_t Elapsed = gProfilerEntries[i].Elapsed * 1000000.0;
            addJsonMember(&JsonObjectProfilerEntry, "cat", "function");
            addJsonMember(&JsonObjectProfilerEntry, "dur", Elapsed);
            addJsonMember(&JsonObjectProfilerEntry, "name", Name);
            addJsonMember(&JsonObjectProfilerEntry, "ph", "X");
            addJsonMember(&JsonObjectProfilerEntry, "pid", 0.0);
            addJsonMember(&JsonObjectProfilerEntry, "tid", 0.0);
            addJsonMember(&JsonObjectProfilerEntry, "ts", BaseTime);
            BaseTime += Elapsed;

            JsonMemberArray[i].Type = JSON_TYPE_MEMBER; // Important!!!
            JsonMemberArray[i].Child = (json_member*)malloc(sizeof(json_member));
            if (JsonMemberArray[i].Child == nullptr) {
                printf("[ERROR] malloc() failed (%s)", __func__);
            }
            memcpy(JsonMemberArray[i].Child, JsonObjectProfilerEntry.First, sizeof(json_member));
        }

        json_object Result;
        addJsonMember(&Result, "traceEvents", JsonMemberArray, gProfilerEntriesSize - 1);

        writeJsonObjectToFile(Result, "./data/profiler_result.json");

        // Free JsonMemberArray memory
        for (size_t i = 0; i < gProfilerEntriesSize - 1; i++) {
            free(JsonMemberArray[i].Child);
        }
        
        gProfilerEntriesCapacity = 0;
        gProfilerEntriesSize = 0;
        free(gProfilerEntries);
        gProfilerEntries = nullptr;
    }
}

/**
 * @brief Prints the results of the profiler.
 * 
 * This function iterates over all the profiler entries and prints each entry's name,
 * elapsed time in milliseconds, and the ratio of the elapsed time to the total elapsed time
 * of the last profile entry.
 * 
 * @note This function assumes that the last entry in gProfilerEntries represents the total elapsed time.
 */
void printProfilerResult()
{
    printf("[Profiler Result]\n");
    for (size_t i = 0; i < gProfilerEntriesSize; i++) {
        const char* Name = gProfilerEntries[i].Name;
        float64_t Elapsed = gProfilerEntries[i].Elapsed;
        printf("\tname: %s\n\telapsed: %6.3lf ms (%.3lf %%)\n",
            Name, Elapsed * 1000.0, 100.0 * Elapsed / gProfilerEntries[gProfilerEntriesSize - 1].Elapsed);
    }
}

// DEPRECATED
/**
 * @brief Estimates the CPU frequency using the profiler's OS timer frequency.
 * 
 * This function waits for a predetermined time using the OS timer and then measures
 * the ticks that have passed in the CPU timer during this period. Using this information,
 * it estimates the CPU frequency.
 * 
 * @return The estimated CPU frequency in Hz.
 * 
 * @note This function depends on `getProfilerOsTimerFrequency()`, `readProfilerCpuTimer()`, 
 * and `readProfilerOsTimer()` functions to retrieve relevant timings. Ensure they are 
 * properly implemented and accurate for best results.
 */
// inline uint64_t estimateProfilerCpuFreqency()
// {
//     static uint64_t EstimatedCpuFrequency = 0;

//     // Returns the previous result immediately if estimation had already been done.
//     if (EstimatedCpuFrequency != 0) {
//         return EstimatedCpuFrequency;
//     }

//     // Define how long we want to wait for our measurement (in milliseconds)
//     const uint64_t MillisecondsToWait = 100;

//     // Retrieve the OS timer's frequency
//     uint64_t OsFreq = getProfilerOsTimerFrequency();

//     // Record the starting ticks of both the CPU and OS timers
//     uint64_t CpuStart = readProfilerCpuTimer();
//     uint64_t OsStart = readProfilerOsTimer();

//     // Calculate the duration (in OS timer ticks) we want to wait
//     uint64_t OsWaitTime = OsFreq * MillisecondsToWait / 1000;

//     uint64_t OsElapsed = 0;
//     uint64_t OsEnd = 0;

//     // Wait until the desired duration has passed on the OS timer
//     while(OsElapsed < OsWaitTime)
//     {
//         OsEnd = readProfilerOsTimer();
//         OsElapsed = OsEnd - OsStart;
//     }

//     // Record the ending tick of the CPU timer after the wait
//     uint64_t CpuEnd = readProfilerCpuTimer();
    
//     // Calculate the number of ticks that passed on the CPU timer during our wait
//     uint64_t CpuElapsed = CpuEnd - CpuStart;

//     // Avoid division by zero and compute the estimated CPU frequency
//     if(OsElapsed)
//     {
//         EstimatedCpuFrequency = OsFreq * CpuElapsed / OsElapsed;
//     }

//     // Return the estimated CPU frequency
//     return EstimatedCpuFrequency;
// }
