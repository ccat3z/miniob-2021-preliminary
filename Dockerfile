# This Dockerfile is just for testing if it can be built in the competition's test environment
FROM gcc:8.3

# Install dependencies
ENV MAKEFLAGS=-j8
WORKDIR /usr/src
RUN git clone https://gitlab.kitware.com/cmake/cmake.git --branch v3.20.6 --depth 1 \
    && cd cmake \
    && ./bootstrap && make install \
    && cd /usr/src && rm -rf cmake
RUN git clone https://github.com/libevent/libevent --branch release-2.1.12-stable --depth 1 \
    && cd libevent \
    && mkdir build \
    && cd build \
    && cmake .. -DEVENT__DISABLE_OPENSSL=ON \
    && make install \
    && cd /usr/src && rm -rf libevent
RUN git clone https://github.com/open-source-parsers/jsoncpp.git --depth 1 \
    && cd jsoncpp \
    && mkdir build \
    && cd build \
    && cmake -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF .. \
    && make install \
    && cd /usr/src && rm -rf jsoncpp
RUN git clone https://github.com/google/googletest --depth 1 \
    && cd googletest \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make install \
    && cd /usr/src && rm -rf googletest

# Install miniob
ENV LD_LIBRARY_PATH=/usr/local/lib
COPY . /usr/src/miniob
RUN cd /usr/src/miniob \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make install