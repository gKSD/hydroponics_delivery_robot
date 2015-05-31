cmake_minimum_required(VERSION 2.6) 

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    message ("asdasdasdasd!!!!!!!!!!!!!!")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(Boost_USE_STATIC_LIBS        OFF) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)


find_package( Boost 1.55.0 COMPONENTS date_time filesystem system)
find_package (Threads)
if(Boost_FOUND)
	message ("Boost found!")
	include_directories(${Boost_INCLUDE_DIRS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I /usr/local/include/boost -Wl,-rpath=/usr/local/lib -lboost_system -lboost_thread -lssl -lpthread -lboost_regex")
	add_executable(robot_server main.cpp connection.cpp connection_manager.cpp reply.cpp request_handler.cpp request_parser.cpp server.cpp Server_preferences.cpp Singleton.cpp)
	target_link_libraries(robot_server ${Boost_LIBRARIES})
	target_link_libraries (robot_server ${CMAKE_THREAD_LIBS_INIT})
endif()
