add_library(CalcLib INTERFACE)

# Propagate sources to consumers so THEY compile them
target_sources(CalcLib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Calculator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Calculator.cpp
)

target_include_directories(CalcLib INTERFACE .)
