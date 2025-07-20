#pragma once 

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>
#include <x86intrin.h>

using std::string;
using std::hash;
using std::thread;
using std::vector;
using std::lock_guard;
using std::mutex;

using namespace std::this_thread;
using u32 = uint32_t;
using ll = long long;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

/**
 * A single result from the profiler.
 * 
 * @param name The name of the function or scope.
 * @param start The start time of the function or scope.
 * @param end The end time of the function or scope.
 * @param ThreadID The thread ID of the function or scope.
 */
struct ProfileResult {
    string name;
    ll start, end;
    u32 ThreadID;
};

/**
 * A session to manage the collection of profiling results.
 * Its destructor will write all collected results to a file.
 * 
 * @param name The name of the session.
 * @param filePath The file path to write the results to.
 */
class ProfilerSession {
    public:
        ProfilerSession(const string& name, const string& filePath = "profile_results.json") :
            name_(name),
            filePath_(filePath) {}
    
        ~ProfilerSession() {
            write_profile_results_();
        }

    private:
        void write_profile_results_();
        string name_;
        string filePath_;
};

/**
 * Singleton profiler class.
 * It collects and stores the profiling results in a thread safe way.
 */
class Profiler {
    public:
        static Profiler& get() {
            static Profiler instance;
            return instance;
        }
        
        void add_result(const ProfileResult& result) {
            lock_guard<mutex> lock(mutex_);
            results_.push_back(result);
        }

        const vector<ProfileResult>& get_results() const {
            return results_;
        }

        void clear() {
            lock_guard<mutex> lock(mutex_);
            results_.clear();
        }
    
    private:
        Profiler() = default;
        ~Profiler() = default;

        Profiler(const Profiler&) = delete;
        Profiler& operator=(const Profiler&) = delete;

        mutex mutex_;
        vector<ProfileResult> results_;
};

/**
 * Timer class that uses RAII to automatically record the start and end times of a scope.
 */
class InstrumentationTimer {
    public:
        InstrumentationTimer(const string& name): 
            name_(name), stopped_(false) {
                StartTimePoint_ = Clock::now();
            }
        
        ~InstrumentationTimer() {
            if (!stopped_) {
                stop();
            }
        }
        void stop() {
            auto endTimePoint = Clock::now();
            ll start = std::chrono::time_point_cast<std::chrono::microseconds>(StartTimePoint_).time_since_epoch().count();
            ll end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
            // Thread ID (this identifies the thread that the scope was in)
            u32 threadID = static_cast<u32>(hash<thread::id>{}(std::this_thread::get_id()));
            
            Profiler::get().add_result({name_, start, end, threadID});
            stopped_ = true;
        }

    private:
        const string name_;
        TimePoint StartTimePoint_;
        bool stopped_;
};

// ============== MACROS ==============
// These will be compiled away to nothing if PROFILING_ENABLED is not 1 for release builds
#if PROFILING_ENABLED
    // Helper macros to concatenate tokens (for unique name creation)
    #define PROFILE_CONCAT_INTERNAL(a, b) a##b
    #define PROFILE_CONCAT(a, b) PROFILE_CONCAT_INTERNAL(a, b)
    #define PROFILE_UNIQUE_NAME(base) PROFILE_CONCAT(base, __LINE__)

    // Main macros 
    #define PROFILE_SESSION(name, filePath) ProfilerSession session(name, filePath)
    #define PROFILE_FUNCTION() InstrumentationTimer PROFILE_UNIQUE_NAME(timer)(__func__)
    #define PROFILE_SCOPE(name) InstrumentationTimer PROFILE_UNIQUE_NAME(timer)(name)
#else 
    #define PROFILE_SESSION(name, filePath)
    #define PROFILE_FUNCTION()
    #define PROFILE_SCOPE(name)
#endif

inline void ProfilerSession::write_profile_results_() {
    std::ofstream output_file(filePath_);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filePath_ << std::endl;
        return;
    }

    output_file << "{\"otherData\": {";
    output_file << "\"sessionName\":\"" << name_ << "\",";
    output_file << "\"timestamp\":\"" << std::time(nullptr) << "\",";
    output_file << "\"version\":\"1.0\"";
    output_file << "},\"traceEvents\":[";


    const auto& results = Profiler::get().get_results();
    for (size_t i = 0; i < results.size(); i++) {
        const auto& result = results[i];

        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');

        if (i > 0) {
            output_file << ",";
        }

        output_file << "{";
        output_file << "\"cat\":\"function\",";
        output_file << "\"dur\":" << (result.end - result.start) << ',';
        output_file << "\"name\":\"" << name << "\",";
        output_file << "\"ph\":\"X\",";
        output_file << "\"pid\":0,";
        output_file << "\"tid\":" << result.ThreadID << ",";
        output_file << "\"ts\":" << result.start;
        output_file << "}";
    }

    output_file << "]}";
    output_file.close();

    Profiler::get().clear();
}
