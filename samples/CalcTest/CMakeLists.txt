add_executable(CalcTest
    Main.cpp
    AddTest.cpp
    SubTest.cpp
    MulTest.cpp
    ConstTest.cpp
)
target_link_libraries(CalcTest kmtest::kmtest CalcLib)
set_target_properties(CalcTest PROPERTIES FOLDER Samples)

if(WDK_FOUND)
    wdk_add_driver(CalcTestDriver
        # no need in Main.cpp for the driver
        AddTest.cpp
        SubTest.cpp
        MulTest.cpp
        ConstTest.cpp
        CalcTestDriver.cmd
    )
    target_link_libraries(CalcTestDriver kmtest::kmtest CalcLib)
    set_target_properties(CalcTestDriver PROPERTIES FOLDER Samples)

    # copy the driver test command file to the output directory so it can be easily run from there
    add_custom_command(TARGET CalcTestDriver POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/CalcTestDriver.cmd
            $<TARGET_FILE_DIR:CalcTestDriver>/CalcTestDriver.cmd
        VERBATIM
    )
endif()