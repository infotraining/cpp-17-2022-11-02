##################
# Target
get_filename_component(DIRECTORY_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" TARGET_MAIN ${DIRECTORY_NAME})

####################
# Sources & headers
aux_source_directory(. SRC_LIST)
file(GLOB HEADERS_LIST "*.h" "*.hpp")

add_executable(${TARGET_MAIN} ${SRC_LIST} ${HEADERS_LIST})
target_link_libraries(${TARGET_MAIN} PRIVATE Catch2::Catch2WithMain)

#----------------------------------------
# Libraries
#----------------------------------------
find_package(Boost)
target_link_libraries(${TARGET_MAIN} PRIVATE Boost::boost)

# if(MSVC)
# else()
# 	find_package(TBB CONFIG REQUIRED)
# 	find_package(Threads REQUIRED)
# 	target_link_libraries(${TARGET_MAIN} PRIVATE TBB::tbb Threads::Threads stdc++fs)
# endif() 

file(COPY tokens.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})