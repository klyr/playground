Deps
----

* https://github.com/HdrHistogram/HdrHistogram_c.git
* https://github.com/stedolan/jq.git

JQ
--

    git clone https://github.com/HdrHistogram/HdrHistogram_c.git
    cd jq
    
    git submodule update --init
    autoreconf -fi
    ./configure --with-oniguruma=builtin --disable-maintainer-mode

    make DESTDIR=${PWD}/.. install

Lib HDR Histogram
-----------------

    git clone https://github.com/HdrHistogram/HdrHistogram_c.git
    cd HdrHistogram_c
    
    cmake .

    make DESTDIR=${PWD}/.. install

Compilation
-----------

    gcc -Wall jqbench.c -o jqbench -I ./usr/local/include -I ./usr/local/include/hdr -L ./usr/local/lib -ljq -lhdr_histogram

Execution
---------

    LD_LIBRARY_PATH=./usr/local/lib ./jqbench
