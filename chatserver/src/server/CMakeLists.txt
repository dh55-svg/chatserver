aux_source_directory(. SRC_LIST)

add_executable(Chatserver ${SRC_LIST})

#target_link_directories(Chatserver muduo_net muduo_base pthread)
target_link_libraries(Chatserver muduo_net muduo_base pthread)