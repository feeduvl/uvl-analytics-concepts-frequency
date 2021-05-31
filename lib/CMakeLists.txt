cmake_minimum_required(VERSION 3.13)
project(feed_uvl_finding_comparatively)

set(CMAKE_CXX_STANDARD 17)

include_directories(includes/rapidxml)
include_directories(src)

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
        src/util/str_util.h src/io/json.h)
