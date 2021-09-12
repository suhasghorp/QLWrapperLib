# QLWrapperLib
AWS Lambda with CPP runtime - hosting QuantLib swap pricer

With AWS Lambda natively supporting C++ runtime, I wanted to share my experinces with hosting a pure C++ QuantLib function as a Lambda in AWS.
I also wanted to demonstrate a complete development environment using Clion and Docker.
Since AWS Lambda runs on Amazon Linux, we must compile our C++ code on Amazon Linux so that we do not need to distribute the Std C library along with our function, reducing the size of the binary.
To achieve this on my local machine, I created a docker image (in successive docker layers) and CLion project was compiled against this docker toolchain.

Take a look in the docker-files directory for the docker files and for build and run instructions, the one you need to connect CLion to is in 5Clion directory.
To run the CLion docker env, use the following command :

_c:\suhas\Docker\5clion>docker run -d --cap-add sys_ptrace -p127.0.0.1:5522:22 clion/remote-cpp-env:1.0
ad0bd3d2f471a8717fe31976de0cd54bede5840bdd79773afb7adb5e97620be7_




