!CHAOS Framework

Compile !CHAOS Framework instruction: 

To compile all tree framework(Common [--common], CUToolkit[--cu], UIToolkit[--ui]) togheter must be used the line "sudo scons --all --install copy", the package can be compilide also in separated way substituting --all with the single package parameter.

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
                    boost_log                   (http://sourceforge.net/projects/boost-log/files/latest/download)
 

for qt client download and install qwt(http://qwt.sourceforge.net/qwtinstall.html)