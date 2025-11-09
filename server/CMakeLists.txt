add_executable(server_app server.cpp)

target_link_libraries(server_app PRIVATE nfs_common)

target_include_directories(server_app PRIVATE
    ${PROJECT_SOURCE_DIR}/common
)
