cmake_minimum_required(VERSION 3.15)
project(test)

#搜索目录，PROJECT_SOURCE_DIR源文件路径，在执行cmake .. 时，..就是路径, 将路径与变量SRC绑定
#aux_source_directory(${PROJECT_SOURCE_DIR} SRC) 
# 同样也可以用file设置源文件路径,相对复杂
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

#将可执行文件路径设置为此路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build)
#set(EXECUTABLE_OUTPUT_PATH /home/pigsy/cmake-gtest/CMakeCourses/test4/build) 

#如果头文件和.cpp 文件不在同一个目录，而在.cpp文件中又不想直接保护头文件的全部路径
#就可以在此指定头文件的路径
include_directories(${PROJECT_SOURCE_DIR}/include ../nlohmann)

#设置源文件编程语言特性
set(CMAKE_CXX_STANDARD 20) 

# 设置构建类型为 Debug (可选但推荐)
set(CMAKE_BUILD_TYPE Debug)

# 设置 GDB 调试参数
# ===================
# 以下参数同时适用于 C 和 C++：
set(DEBUG_FLAGS
    -g                # 生成调试信息（必需）
    -O0               # 禁用优化（保持代码原始结构）
    -fno-omit-frame-pointer  # 保留栈帧指针
    -fno-inline       # 禁用函数内联
    -Wall             # 开启所有警告
    -Wextra           # 额外的警告
    -Wno-unused-parameter  # 忽略未使用参数的警告
    -lredis++
    -lhiredis
    -lpthread
    #-std=c++17 # 但是版本比较老的可以带上-std=c++17
)

option(ENABLE_LOGGING "Enable console logging" OFF)  # 默认禁用
if(ENABLE_LOGGING)
    add_compile_definitions(__CONSOLE_LOG)
    message(STATUS "Console logging ENABLED")
else()
    message(STATUS "Console logging DISABLED")
endif()

option(DEBUG_LOGGING "Enable DEBUG logging" OFF)  # 默认开启
if(DEBUG_LOGGING)
    add_compile_definitions(__DEBUG_LOG)
    message(STATUS "DEBUG logging ENABLED")
else()
    message(STATUS "DEBUG logging DISABLED")
endif()

option(INFO_LOGGING "Enable DEBUG logging" ON)  # 默认开启
if(INFO_LOGGING)
    add_compile_definitions(__INFO_LOG)
    message(STATUS "INFO logging ENABLED")
else()
    message(STATUS "INFO logging DISABLED")
endif()

#将可执行文件的依赖文件目录设置为SRC
add_executable(a.out ${SRC}) 

# 链接 Google Test 库和 pthread
# target_link_libraries(a.out gtest gtest_main pthread log4cpp)
target_link_libraries(a.out 
    pthread
    log4cpp
    fmt
    redis++ 
    hiredis
)


#指定库生成的路径 如果后面的路径不存在，就会自动创建
#set(LIBRARY_OUTPUT_PATH /home/pigsy/my_test_lib)

#指定生成的库的名字 参数SHARED生成动态库calc.so  STATIC静态库calc.a
#add_library(calc SHARED ${SRC})
#add_library(calc STATIC ${SRC})
