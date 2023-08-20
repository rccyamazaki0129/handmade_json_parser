#ifndef RCC_PROFILER_H_
#define RCC_PROFILER_H_

#include "rcc_common.h"
#include "rcc_json_object.h"
#include <stdint.h>
#include <stdio.h>

#define PROFILE_FUNC profiler_entry Profiler(__func__)
#define PROFILE_BLOCK(x) profiler_entry Profiler(x)
#define PROFILE_MAX_ENTRIES 128

inline uint64_t getProfilerOsTimerFrequency();
inline uint64_t readProfilerOsTimer();
inline uint64_t readProfilerCpuTimer();
inline float64_t getProfilerTimeDifferenceInSec(uint64_t Before, uint64_t After);

void initializeProfiler();
void finalizeProfiler();
void printProfilerResult();

typedef struct profiler_entry profiler_entry;

static profiler_entry* gProfilerEntries;
static size_t gProfilerEntriesCapacity = 0;
static size_t gProfilerEntriesSize = 0;
static bool32_t gIsProfilerInitialized = false;

// TODO: Export profiling result as a JSON file.
/**
 * @brief A structure that represents a single profiler entry.
 * 
 * This structure is used to automatically record the start and finish time of a code block.
 * When an instance of this structure is created, it captures the start time. When the instance is
 * destroyed (goes out of scope), it captures the finish time, calculates the elapsed time, and
 * stores the result in a global array (if initialized).
 * 
 * @note If the global profiler entries array is not initialized, the result is printed immediately
 * upon the instance's destruction.
 */
struct profiler_entry
{
    const char* Name;      //!< The name associated with this profiler entry.
    uint64_t Start;        //!< The start time in CPU ticks.
    uint64_t Finish;       //!< The finish time in CPU ticks.
    float64_t Elapsed;     //!< The elapsed time in seconds.

    /**
     * @brief Constructor that initializes a profiler entry with a given name.
     * 
     * This constructor captures the start time using the readProfilerCpuTimer() function.
     * 
     * @param ProfilingName The name associated with this profiler entry.
     */
    profiler_entry(const char* ProfilingName) {
        Name = ProfilingName;
        Start = readProfilerCpuTimer();
    }

    /**
     * @brief Destructor that captures the finish time and calculates the elapsed time.
     * 
     * If the global profiler entries array is initialized and there's enough space, the result
     * is stored in the array. If the array is full, it's resized. If the array is not initialized,
     * the result is printed immediately.
     */
    ~profiler_entry() {
        Finish = readProfilerCpuTimer();
        Elapsed = getProfilerTimeDifferenceInSec(Start, Finish);
        if (gProfilerEntries != nullptr) {
            if (gProfilerEntriesSize >= gProfilerEntriesCapacity) {
                gProfilerEntriesCapacity *= 2;
                gProfilerEntries = (profiler_entry*)realloc(gProfilerEntries, sizeof(profiler_entry) * gProfilerEntriesCapacity);
            }
            memcpy(&gProfilerEntries[gProfilerEntriesSize++], this, sizeof(profiler_entry));
        }
        else {
            // initializeProfiler() has not been called, or finalizeProfiler() has already been called.
            printf("[Profiler Result]\n\tname: %s\n\telapsed: %.3lf (ms)\n", Name, Elapsed * 1000.0);
        }
    }
};


// DEPRECATED
// inline uint64_t estimateProfilerCpuFreqency();

#endif
