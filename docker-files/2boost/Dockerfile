#docker build --build-arg boost_version=1.76.0 --build-arg boost_dir=boost_1_76_0 -t aboost:1.0 .
ARG tag=latest
FROM abase:1.0
MAINTAINER Suhas Ghorapdkar <suhasghorp@gmail.com>
LABEL Description="Building Boost C++ libraries on Amazon Linux2"

ARG boost_version
ARG boost_dir
ENV boost_version ${boost_version}

RUN wget https://boostorg.jfrog.io/artifactory/main/release/${boost_version}/source/${boost_dir}.tar.gz \
    && tar xfz ${boost_dir}.tar.gz \
    && rm ${boost_dir}.tar.gz \
    && cd ${boost_dir} \
    && ./bootstrap.sh \
    && ./b2 --without-python --prefix=/usr -j 4 link=shared runtime-link=shared install \
    && cd .. && rm -rf ${boost_dir} && ldconfig

CMD bash