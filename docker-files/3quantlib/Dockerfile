ARG tag=latest
FROM aboost:1.0
MAINTAINER Suhas Ghorapdkar <suhasghorp@gmail.com>
LABEL Description="Building QuantLib library on Amazon Linux2"

ARG quantlib_version
ENV quantlib_dir ${quantlib_dir}

RUN wget https://github.com/lballabio/QuantLib/releases/download/QuantLib-v${quantlib_version}/QuantLib-${quantlib_version}.tar.gz \
    && tar xfz QuantLib-${quantlib_version}.tar.gz \
    && rm QuantLib-${quantlib_version}.tar.gz \
    && cd QuantLib-${quantlib_version} \
    && ./configure CXXFLAGS="-std=c++17 -g -O2 -Wno-deprecated-declarations" --enable-shared=no --enable-static=yes --enable-error-lines --enable-error-functions --enable-std-classes \
    && make -j 4 \
    && make check \
    && make install \
    && cd .. && rm -rf QuantLib-${quantlib_version} && ldconfig
    
# docker build --build-arg quantlib_version=1.23 -t aquantlib:1.0 .   

CMD bash



