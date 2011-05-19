include_directories(${libsynctory_SOURCE_DIR}/src/include ${libsynctory_SOURCE_DIR}/src/lib ${libsynctory_BINARY_DIR}/src/include)
link_directories(${libsynctory_BINARY_DIR}/src/lib)

add_executable(generate_testfile generate_testfile.c)
add_executable(test_fingerprint test_fingerprint.c)
target_link_libraries(test_fingerprint synctory)
