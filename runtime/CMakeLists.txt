add_executable(mnrt-runner
  src/runner.cpp
  src/CpuDevice.cpp
)
target_include_directories(mnrt-runner PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mnrt-runner PRIVATE dl)