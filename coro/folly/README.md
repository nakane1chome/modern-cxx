A basic example of co-routines using the facebook folly library. Implements the same example as ../raw_threads.

You will need to download and build the folly library with a co-routine supporting compiler.

Follow the instructions at https://github.com/facebook/folly. The dependencies are listed there.

~~~
export CC=/usr/bin/gcc-11
export CXX=/usr/bin/g++-11 
export CXXFLAGS=-fcoroutines 
git clone https://github.com/facebook/folly
cd folly
./build.sh --install-dir ~/folly/
find /tmp/ -name "libfmt.a"
cp /tmp/fbcode_builder_getdeps-ZmntZdZopensource2ZfollyZbuildZfbcode_builder/build/fmt-im7g6b0Kua79yCkKWUOn2il0IfXbUvIb279yS94m0EE/libfmt.a .
~~~

It seems to only work with the libfmt.a compiled along side folly but not installed anywhere... TODO - I'm sure there is a better way. 


You will also need the ranges v3 library.

~~~
git clone https://github.com/ericniebler/range-v3
mkdir range-v3/build
cd range-v3/build
export CC=/usr/bin/gcc-11
export CXX=/usr/bin/g++-11 
export CXXFLAGS=-fcoroutines 
cmake .. -DCMAKE_INSTALL_PREFIX=~/ranges-v3
~~~