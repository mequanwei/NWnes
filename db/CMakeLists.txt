cmake_minimum_required (VERSION 2.8)


aux_source_directory(. current_dir_src)                                 
add_library(db ${current_dir_src})
target_link_libraries(db -lmysqlclient)
