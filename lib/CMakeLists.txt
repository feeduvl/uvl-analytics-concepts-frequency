cmake_minimum_required(VERSION 3.13)
project(feed_uvl_finding_comparatively)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-std=c++17 -lstdc++fs")

include_directories("${CMAKE_SOURCE_DIR}/includes/rapidxml")
include_directories("${CMAKE_SOURCE_DIR}/src")

add_executable(feed_uvl_finding_comparatively
        main.cpp

        src/graph/concept_node.h

        src/model/accepter_algo_1.h
        src/model/algo_1_model.h
        src/model/file_accepter.h
        src/model/model.h
        src/model/model_depth_wrapper.h

        src/model_builder/xml_reader.h
        src/model_builder/directory_walker.h
        src/model_builder/frequency_manager.h
        src/util/str_util.h
        src/io/json.h)
