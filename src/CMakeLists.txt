add_library(YieldCurve_Lib STATIC
    model.cpp
    utils.cpp
    common.cpp
)

target_include_directories(YieldCurve_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(YieldCurve main.cpp)
target_link_libraries(YieldCurve YieldCurve_Lib)
