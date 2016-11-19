FROM regexpress/base:latest

COPY CppTester.cpp /root
COPY CppTester.h /root
COPY CppTesterTest.cpp /root
COPY json.hpp /root

RUN apk update && \
    apk add --no-cache --virtual .build-deps g++=5.3.0-r0 && \
    cd /root && \
    g++ -std=c++11 CppTester.cpp -o CppTester && \
    g++ -std=c++11 CppTesterTest.cpp -o CppTesterTest && \
    echo "arg=();for var in \"\$@\";do arg+=(\$(echo -n \"\$var\" | base64 -d)); done; ./CppTester \"\${arg[@]}\"" > run.sh && \
    chmod 755 run.sh && \
    apk del .build-deps && \
    rm -rf /tmp/*
    
ENTRYPOINT ["/bin/bash", "/root/run.sh"]