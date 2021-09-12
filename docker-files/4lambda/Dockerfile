ARG tag=latest
FROM aquantlib:1.0
MAINTAINER Suhas Ghorapdkar <suhasghorp@gmail.com>
LABEL Description="Lambda function using QuantLib library on Amazon Linux2"

RUN git clone https://github.com/daniel-fudge/aws-lambda-cpp-cloud9 \
    && cd aws-lambda-cpp-cloud9 \
    && mkdir build \
    && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/install \
    && make \
    && make aws-lambda-package-demo    
    
# docker build -t alambda:1.0 .   
# to run container, 
# docker container run -it alambda:1.0
# to copy zip function file to c:
# docker cp gifted_shirley:/aws-lambda-cpp-cloud9/build/demo.zip .

CMD bash