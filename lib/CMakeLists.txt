cmake_minimum_required(VERSION 3.13)
project(feed_uvl_finding_comparatively)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${CMAKE_SOURCE_DIR}/includes/rapidxml")
include_directories("${CMAKE_SOURCE_DIR}/src")

add_compile_options(-std=c++17)
add_compile_options(-lstdc++fs)

add_executable(feed_uvl_finding_comparatively
        main_fcic.cpp

        src/graph/concept_node.h

        src/model/frequency_corpus_parser.h
        src/model/frequency_model.h
        src/model/corpus_parser.h
        src/model/model.h
        src/model/pipeline.h

        src/model_builder/xml_reader.h
        src/model_builder/directory_walker.h
        src/model_builder/algorithm_runner.h
        src/util/str_util.h
        src/io/json.h src/dtree/decision_tree.h src/dtree/decision_tree.cpp src/util/math_util.h find_occurences.h)

target_compile_features(feed_uvl_finding_comparatively PRIVATE cxx_std_17)