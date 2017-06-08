FROM warchantua/iroha-dev

RUN apt-get update && apt-get -y upgrade && \
    apt-get -y install libssh2-1-dev libpq-dev

# install gtest
RUN cd /tmp; \
    git clone https://github.com/google/googletest.git; \
    cd googletest; \
    cmake -H. -Bbuild; \
    cmake --build build --target install

# install cpp_redis
RUN cd /tmp; \
    git clone --recursive https://github.com/Cylix/cpp_redis.git; \
    cd cpp_redis; \
    cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON; \
    cmake --build build --target install

# install keccak. thanks for not providing make install
RUN cd /tmp; \
    git clone https://github.com/gvanas/KeccakCodePackage.git; \
    cd KeccakCodePackage; \
    make generic64/libkeccak.a && \
    install -m 644 bin/generic64/libkeccak.a /usr/lib/; \
    install -m 644 bin/generic64/libkeccak.a.headers/* /usr/include/

# install cppfs
RUN cd /tmp; \
    git clone https://github.com/cginternals/cppassist.git; \
    git clone https://github.com/cginternals/cpplocate.git; \
    git clone https://github.com/cginternals/cppexpose.git; \
    git clone https://github.com/cginternals/cppfs.git; \
    cd /tmp/cppassist && ./configure && ./configure pack; \
    cmake --build build --target install; \
    cd /tmp/cpplocate && ./configure && ./configure pack; \
    cmake --build build --target install; \
    cd /tmp/cppexpose && ./configure && ./configure pack; \
    cmake --build build --target install; \
    cd /tmp/cppfs && ./configure && ./configure pack; \
    cmake --build build --target install

# install libpqxx
RUN cd /tmp; \
    git clone -b feature/install-types-fix https://github.com/lebdron/libpqxx.git; \
    cd libpqxx; \
    ./configure --with-pic --disable-documentation && make && make install

# install optional
RUN cd /tmp; \
    git clone https://github.com/akrzemi1/Optional.git; \
    install -m 644 Optional/optional.hpp /usr/include/

# install speedlog
RUN cd /tmp; \
    git clone -b v0.13.0 https://github.com/gabime/spdlog.git; \
    cd spdlog; \
    cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release -DSPDLOG_BUILD_TESTING=OFF; \
    cmake --build build --target install

CMD ["/bin/bash"]
