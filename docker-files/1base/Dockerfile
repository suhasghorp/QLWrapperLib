#docker build -t abase:1.0 .
ARG tag=latest
FROM amazonlinux:${tag}
MAINTAINER Suhas Ghorapdkar <suhasghorp@gmail.com>
LABEL Description="Base Image with AWS SDK and CPP runtime on Amazon Linux2"

RUN yum update -y \
 && yum groupinstall "Development Tools" -y \
 && yum install wget libcurl-devel openssl-devel -y 

RUN wget https://cmake.org/files/v3.16/cmake-3.16.9.tar.gz \
  && tar -xvzf cmake-3.16.9.tar.gz \
  && cd cmake-3.16.9 \
  && ./bootstrap \
  && make && make install


RUN git clone --recurse-submodules https://github.com/aws/aws-sdk-cpp.git \
  && cd aws-sdk-cpp \
  && mkdir build \
  && cd build \
  && cmake .. -DBUILD_ONLY="s3;dynamodb;lambda" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DENABLE_UNITY_BUILD=ON \
    -DCUSTOM_MEMORY_MANAGEMENT=OFF \
    -DENABLE_UNITY_BUILD=ON \
  && make \
  && make install
  
RUN git clone https://github.com/awslabs/aws-lambda-cpp-runtime.git \
  && cd aws-lambda-cpp-runtime \
  && mkdir build && cd build \
  && cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF \    
  && make && make install
  
RUN curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip" \
    && unzip awscliv2.zip \
    && ./aws/install

COPY credentials /root/.aws/credentials
COPY config /root/.aws/config

  
CMD bash