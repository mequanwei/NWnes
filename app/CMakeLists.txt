cmake_minimum_required (VERSION 2.8)


aux_source_directory(. current_dir_src)                                 
add_library(app ${current_dir_src})
