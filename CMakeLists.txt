cmake_minimum_required(VERSION 3.20)

project(pet_daq
    VERSION 0.1.0
    DESCRIPTION "Low-latency PET detector data acquisition simulation"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Keep warnings enabled because this is intended to become production-style C++.
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
    )
endif()

add_library(petdaq_core INTERFACE)
target_include_directories(petdaq_core
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(pet_daq_demo
    src/main.cpp
)

target_link_libraries(pet_daq_demo PRIVATE petdaq_core)

if (UNIX AND NOT APPLE)
    target_link_libraries(pet_daq_demo PRIVATE pthread)
endif()

# Optional sanitizers:
# cmake -S . -B build-asan \
#   -DCMAKE_BUILD_TYPE=Debug \
#   -DPETDAQ_ENABLE_SANITIZERS=ON
option(PETDAQ_ENABLE_SANITIZERS "Enable ASan and UBSan" OFF)

if (PETDAQ_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(pet_daq_demo PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -O3
    )
    target_link_options(pet_daq_demo PRIVATE
        -fsanitize=address,undefined
    )
endif()

enable_testing()

add_executable(pet_daq_spsc_test
    tests/spsc_ring_buffer_test.cpp
)

target_link_libraries(pet_daq_spsc_test
    PRIVATE petdaq_core
)

add_test(
    NAME pet_daq_spsc_test
    COMMAND pet_daq_spsc_test
)

add_executable(pet_daq_calibration_test
    tests/calibration_test.cpp
)

target_link_libraries(pet_daq_calibration_test
    PRIVATE petdaq_core
)

add_test(
    NAME pet_daq_calibration_test
    COMMAND pet_daq_calibration_test
)

add_executable(pet_daq_event_processor_batch_test
    tests/event_processor_batch_test.cpp
)

target_link_libraries(pet_daq_event_processor_batch_test
    PRIVATE petdaq_core
)

add_test(
    NAME pet_daq_event_processor_batch_test
    COMMAND pet_daq_event_processor_batch_test
)

add_executable(pet_daq_event_batch_test
    tests/event_batch_test.cpp
)

target_link_libraries(pet_daq_event_batch_test
    PRIVATE petdaq_core
)

add_test(
    NAME pet_daq_event_batch_test
    COMMAND pet_daq_event_batch_test
)

add_executable(pet_daq_batch_processing
    benchmarks/batch_processing.cpp
)

target_link_libraries(pet_daq_batch_processing
    PRIVATE petdaq_core
)

if (UNIX AND NOT APPLE)
    target_link_libraries(
        pet_daq_batch_processing
        PRIVATE pthread
    )
endif()

add_executable(pet_daq_latency_percentile_test
    tests/latency_percentile_test.cpp
)

target_link_libraries(
    pet_daq_latency_percentile_test
    PRIVATE petdaq_core
)

add_test(
    NAME pet_daq_latency_percentile_test
    COMMAND pet_daq_latency_percentile_test
)

add_executable(pet_daq_pipeline_invariants_test
    tests/pipeline_invariants_test.cpp
)

target_link_libraries(
    pet_daq_pipeline_invariants_test
    PRIVATE petdaq_core
)

add_test(
    NAME pet_daq_pipeline_invariants_test
    COMMAND pet_daq_pipeline_invariants_test
)

add_executable(pet_daq_spsc_stress_test
    tests/spsc_stress_test.cpp
)

target_link_libraries(
    pet_daq_spsc_stress_test
    PRIVATE petdaq_core
)

add_test(
    NAME pet_daq_spsc_stress_test
    COMMAND pet_daq_spsc_stress_test
)

add_custom_target(pet_daq_test_all
    COMMAND ${CMAKE_CTEST_COMMAND}
        --test-dir ${CMAKE_BINARY_DIR}
        --output-on-failure
    DEPENDS
        pet_daq_demo
        pet_daq_spsc_test
        pet_daq_spsc_stress_test
        pet_daq_calibration_test
        pet_daq_latency_percentile_test
        pet_daq_pipeline_invariants_test
)

add_executable(pet_daq_reconstruction_test
    tests/reconstruction_test.cpp
)

target_link_libraries(
    pet_daq_reconstruction_test
    PRIVATE petdaq_core
)

add_test(
    NAME pet_daq_reconstruction_test
    COMMAND pet_daq_reconstruction_test
)

add_executable(pet_daq_image_writer_test
    tests/image_writer_test.cpp
)

target_link_libraries(
    pet_daq_image_writer_test
    PRIVATE petdaq_core
)

add_test(
    NAME pet_daq_image_writer_test
    COMMAND pet_daq_image_writer_test
)

add_executable(pet_daq_reconstruction_demo
    examples/reconstruction_demo.cpp
)

target_link_libraries(
    pet_daq_reconstruction_demo
    PRIVATE petdaq_core
)

add_executable(pet_daq_reconstruction_processing
    benchmarks/reconstruction_processing.cpp
)

target_link_libraries(
    pet_daq_reconstruction_processing
    PRIVATE petdaq_core
)