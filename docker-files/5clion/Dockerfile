# CLion remote docker environment (How to build docker container, run and stop it)
#
# Build and run:
#   docker build -t clion/remote-cpp-env:1.0 .
#   docker run -d --cap-add sys_ptrace -p127.0.0.1:5522:22 clion/remote-cpp-env:1.0
#   docker cp <container_id>:/tmp/greeter/cmake-build-docker-debug/greeter.zip .
#   aws lambda create-function --function-name ql-greeter --role arn:aws:iam::734853675260:role/lambda-demo --runtime provided --timeout 15 --memory-size 128 --handler greeter --zip-file fileb://greeter.zip
#   aws lambda update-function-code --function-name ql-greeter --zip-file fileb://greeter.zip
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:2222"

curl -v -X POST 'https://hw3s2wyifc.execute-api.us-east-1.amazonaws.com/default/greeter-lambda?name=Bradley&city=Chicago' -H 'content-type: application/json' -H 'day: Sunday' -d "{ \"time\": \"evening\" }"

curl -v -X POST 'https://hw3s2wyifc.execute-api.us-east-1.amazonaws.com/default/ql-greeter' -H 'content-type: application/json' -d "{\"valDate\": \"2021-08-16\", \"currency\":\"USD\", \"swapType\": \"PAYER\", \"nominal\": 48000000.0, \"startDate\": \"2018-03-14\", \"maturityDate\": \"2028-03-14\", \"fixedLegFrequency\": \"SEMIANNUAL\",\"fixedLegCalendar\": \"UNITEDSTATES\", \"fixedLegConvention\": \"MODIFIEDFOLLOWING\", \"fixedLegDateGenerationRule\": \"BACKWARD\", \"fixedLegRate\": 0.02,\"fixedLegDayCount\": \"ACTUAL360\", \"fixedLegEndOfMonth\": false, \"floatingLegFrequency\": \"QUARTERLY\", \"floatingLegCalendar\": \"UNITEDSTATES\",\"floatingLegConvention\": \"MODIFIEDFOLLOWING\", \"floatingLegDateGenerationRule\": \"BACKWARD\", \"floatingLegSpread\": 0.0007, \"floatingLegDayCount\": \"ACTUAL360\", \"floatingLegEndOfMonth\": false}"
#
# stop:
#   docker stop clion_remote_env
# 
# ssh credentials (test user):
#   user@password 

FROM aquantlib:1.0

RUN yum update tzdata

RUN yum install -y openssh-server passwd

#echo 'Subsystem sftp /usr/lib/openssh/sftp-server'; \ this does not work

RUN ( \
    echo 'LogLevel DEBUG2'; \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    echo 'Subsystem sftp internal-sftp'; \
  ) > /etc/ssh/sshd_config_test_clion \
  && mkdir /run/sshd

RUN useradd -m user && yes password | passwd user

RUN usermod -s /bin/bash user

RUN ssh-keygen -A

RUN echo 'root:password' | chpasswd

CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_test_clion"]