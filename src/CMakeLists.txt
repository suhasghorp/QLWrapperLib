# Find all headers in the project include folder
file(GLOB HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/QLWrapperLib/*.h")

# Make a static library (tradeoff of memory for better performance)
add_library(
        QLWrapperLib STATIC
        "QLCurveBuilder.cpp" "QLCurveBuilder.h" "conventions.h"
        "QLDynamoDBHelper.cpp" "QLDynamoDBHelper.h"
        "QLConverter.cpp" "QLConverter.h"
        "QLSwapPricer.cpp"
)

# Add an alias so the target can be used while building
add_library(QLWrapperLib::QLWrapperLib ALIAS QLWrapperLib)

# Set the include directory for builds (tests, examples, etc) and for clients
target_include_directories(QLWrapperLib
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${PROJECT_SOURCE_DIR}/src)

# Associate the include files with the target, so they will be installed
set_target_properties(QLWrapperLib PROPERTIES PUBLIC_HEADER "${HEADERS}")

set(AWSSDK_ROOT_DIR, "/usr/local/")

find_package(Threads REQUIRED)

# Locate the aws sdk for c++ package.
find_package(AWSSDK REQUIRED COMPONENTS core s3 dynamodb)


#sudo apt install libtbb-dev
#find_package(TBB REQUIRED tbb)

#find_package(Eigen3 3.3 REQUIRED NO_MODULE)

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(../extern)

#target_link_libraries(QLWrapperLib ${CMAKE_THREAD_LIBS_INIT} ${TBB_IMPORTED_TARGETS} ${Boost_LIBRARIES} Eigen3::Eigen QuantLib)
target_link_libraries(QLWrapperLib ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${AWSSDK_LINK_LIBRARIES} QuantLib)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Set install targets and destinations
install(TARGETS QLWrapperLib
        EXPORT QLWrapperLibTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/QLWrapperLib)

# Export additional install files (for clients using CMake)
install(EXPORT QLWrapperLibTargets
        NAMESPACE QLWrapperLib::
        FILE QLWrapperLibConfig.cmake
        DESTINATION lib/cmake/QLWrapperLib)

