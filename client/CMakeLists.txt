add_executable(client_app client.cpp)

target_link_libraries(client_app PRIVATE nfs_common)

target_include_directories(client_app PRIVATE
    ${PROJECT_SOURCE_DIR}/common
)
