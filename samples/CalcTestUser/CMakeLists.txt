add_executable(CalcTestUser
    Main.cpp
    AddTest.cpp
    SubTest.cpp
    MulTest.cpp
    Calculator.h
    Calculator.cpp
)
target_link_libraries(CalcTestUser kmtest::kmtest)
set_target_properties(CalcTestUser PROPERTIES FOLDER Samples)
