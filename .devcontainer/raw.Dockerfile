FROM sandervalstar/devcontainer:centos_cpp_basic_v1


# Make sure manpages will get downloaded
RUN sed -i '/tsflags=nodocs/d' /etc/yum.conf

# Install packages
RUN yum -y install cmake-2.8.12.2 \
    && yum -y install cmake3-3.14.7
    # 
    # Configure cmake versions so you can switch versions using
    # 'alternatives --config cmake'
RUN alternatives --install /usr/local/bin/cmake cmake /usr/bin/cmake 10 \
--slave /usr/local/bin/ctest ctest /usr/bin/ctest \
--slave /usr/local/bin/cpack cpack /usr/bin/cpack \
--slave /usr/local/bin/ccmake ccmake /usr/bin/ccmake \
--family cmake
RUN alternatives --install /usr/local/bin/cmake cmake /usr/bin/cmake3 20 \
--slave /usr/local/bin/ctest ctest /usr/bin/ctest3 \
--slave /usr/local/bin/cpack cpack /usr/bin/cpack3 \
--slave /usr/local/bin/ccmake ccmake /usr/bin/ccmake3 \
--family cmake
    #
    # Install clang and clang static analyzer
RUN yum -y install llvm-toolset-7-5.0.1 \
    && yum -y install llvm-toolset-7-clang-tools-extra-5.0.1 \
    && yum -y install llvm-toolset-7-clang-analyzer-5.0.1 \
    && printf "\nsource scl_source enable llvm-toolset-7\n\n" >> ~/.bashrc \
    # 
    # Install more static analyzers
    && yum -y install cppcheck-1.90 \
    #
    # Install pip
    && yum -y install python3-3.6.8 \
    && yum -y install python3-pip \
    && yum -y install python-pip \
    #
    # Install meson tools
    && pip3 install meson==0.51.2 \
    && pip3 install ninja==1.9.0.post1 \
    # 
    # Install code coverage tools
    && pip3 install lxml==4.4.1 \
    && pip3 install pyyaml==5.1.2 \
    && pip install lxml==4.4.1 \
    && pip install pyyaml==5.1.2 \
    #
    # Install epel-release sofware
    && yum -y --enablerepo=extras install epel-release \
    && yum -y install lcov-1.13
