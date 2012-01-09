!CHAOS Framework



Compile !CHAOS Framework instruction: 

To compile all tree framework(Common [--common], CUToolkit[--cu], UIToolkit[--ui]) togheter must be used the line "sudo scons --all install", the package can be compilide also in separated way substituting --all with the single package parameter.

To removed the instaled file: "sudo scons --all -c clean_installed"

To removed compilation object only: "sudo scons --all -c"

To generate documentation:  Doxygen software is needed and it can be downloaded from http://www.stack.nl/~dimitri/doxygen/ 
                            after installation launch from terminal "doxygen Documentation/chaosdocs"


Library used by CHAOS:
    libmemcached:   libevent                    (http://libevent.org/)
                    libmemcached                (http://libmemcached.org/libMemcached.html)
    memcached:      memcached                   (http://memcached.org/)
    msgpack:        msgpack                     (https://github.com/msgpack)
                    msgpack-rpc                 (https://github.com/msgpack/msgpack-rpc)
                    mpio                        (https://github.com/frsyuki/mpio)
    boost:          boost_program_options 
                    boost_system
                    boost_thread
                    boost_chrono
                    boost_regex                 (www.boost.org)
Boost logging v2 
for install boost logging v2(http://torjo.com/log2/index.html)
Step:
    cd /usr/local/include/boost/
    svn co http://svn.boost.org/svn/boost/sandbox/logging/boost/logging 
