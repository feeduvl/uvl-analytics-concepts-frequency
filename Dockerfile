FROM debian:latest

RUN apt update && apt install -y build-essential cmake
RUN gcc --version
WORKDIR /app

COPY . .

WORKDIR lib

RUN g++ -Wall main.cpp \
        ./src/graph/concept_node.h \
        ./src/model/accepter_algo_1.h \
        ./src/model/algo_1_model.h \
        ./src/model/file_accepter.h \
        ./src/model/model.h \
        ./src/model/model_depth_wrapper.h \
        ./src/model_builder/xml_reader.h \
        ./src/model_builder/directory_walker.h \
        ./src/model_builder/frequency_manager.h \
        ./src/util/str_util.h \
        ./src/io/json.h \
        -o feed_uvl_finding_comparatively -std=c++17 -lstdc++fs

# RUN cmake -S . -B . && make

RUN echo "Result after CMake build: " && ls

RUN yes | apt install python3 python3-pip && rm -rf /var/lib/apt/lists/*

COPY requirements.txt requirements.txt
RUN pip3 install --upgrade pip -r requirements.txt

WORKDIR ..
EXPOSE 9658
CMD [ "python3", "./app.py" ]