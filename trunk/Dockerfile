# Start with a configurable base image
ARG IMG="debian:unstable"
FROM "${IMG}"

# Update the package lists
RUN apt-get update

# Copy the current directory to the container and continue inside it
COPY "." "/mnt"
WORKDIR "/mnt"

# Install the packages needed for the build
RUN env DEBIAN_FRONTEND=noninteractive apt-get install --yes \
 subversion \
 pkg-config automake autoconf libtool make \
 flex bison \
 libreadline-dev libexif-dev \
 libjpeg-dev libpng-dev libtiff-dev libgif-dev \
 libsdl1.2-dev libaa1-dev  \
 wget ghostscript man2html \
 gcc g++

# continue as an unpriviledged user
RUN useradd "user"
RUN chown --recursive "user:user" "."
USER "user"

# Build and test
RUN ./autogen.sh
RUN ./configure CXXFLAGS='-fPIC -O2 -g'
RUN make clean
RUN make
RUN src/fim --version
RUN src/fim --help
# 
