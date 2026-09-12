#include <cassert>
#include <cstdint>

#include "petdaq/core/latency.hpp"

int main()
{
    petdaq::LatencyRecorder recorder;

    assert(recorder.percentile(50.0) == 0);
    assert(recorder.percentile(95.0) == 0);
    assert(recorder.percentile(99.0) == 0);

    recorder.record(100);
    recorder.record(200);
    recorder.record(300);
    recorder.record(400);
    recorder.record(500);

    assert(recorder.count() == 5);
    assert(recorder.sample_count() == 5);

    assert(recorder.percentile(0.0) == 100);
    assert(recorder.percentile(50.0) == 300);
    assert(recorder.percentile(95.0) == 400);
    assert(recorder.percentile(99.0) == 400);
    assert(recorder.percentile(100.0) == 500);

    return 0;
}