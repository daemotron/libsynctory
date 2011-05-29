# install header files
install(
    FILES synctory.h
    DESTINATION include
)

# install libsynctory directory
install(
    DIRECTORY libsynctory
    DESTINATION include
    FILES_MATCHING PATTERN "*.txt" EXCLUDE
)

add_subdirectory(libsynctory)