include_directories(../extern/catch2)
include_directories(../extern)

add_executable(test_Converter catch_config.cpp test_Converter.cpp)
target_link_libraries(test_Converter PRIVATE QLWrapperLib)
add_test(NAME Converter_test COMMAND test_Converter)

add_executable(test_CurveBuilder catch_config.cpp test_CurveBuilder.cpp)
target_link_libraries(test_CurveBuilder PRIVATE QLWrapperLib)
add_test(NAME CurveBuilder_test COMMAND test_CurveBuilder)

add_executable(test_DynamoDBHelper catch_config.cpp test_DynamoDBHelper.cpp)
target_link_libraries(test_DynamoDBHelper PRIVATE QLWrapperLib)
add_test(NAME DynamoDBHelper_test COMMAND test_DynamoDBHelper)

add_executable(test_SwapPricer catch_config.cpp test_SwapPricer.cpp)
target_link_libraries(test_SwapPricer PRIVATE QLWrapperLib)
add_test(NAME SwapPricer_test COMMAND test_SwapPricer)

add_executable(test_sofr catch_config.cpp test_sofr.cpp)
target_link_libraries(test_sofr PRIVATE QLWrapperLib)
add_test(NAME sofr_test COMMAND test_sofr)