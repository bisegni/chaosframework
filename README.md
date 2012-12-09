!CHAOS Framework
==============

[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/9f6c09e680fe3ff497ed71957e210ffb "githalytics.com")](http://githalytics.com/macdeveloper/chaosframework)

The !CHAOS project is aiming at the development of a new concept of control system and data acquisition framework 
providing, with a high level of abstraction, all the services needed for controlling and managing a large scientific, 
or non-scientific, infrastructure.  

!CHAOS redefines Control System paradigm by introducing the new concept of Control Service, i.e. a distributed, 
scalable provider offering to a general class of clients high-performance services such us data sharing, commands 
dispatching, continuous historical data archiving, configuration tools, middle-layer resource management etc.  


Key features and development strategies of !CHAOS are:  

- scalability of performances and size 
- integration of all functionalities 
- abstraction of services,  devices and data 
- easy and modular customization 
- extensive data caching for performance boost 
- use of high-performance internet sw technologies

!CHAOS use the standalone BSON C++ implementation, the source is exstraccted from orginal mongodb opensource distribution found on http://www.mongodb.org/ (http://www.10gen.com/)
"BSON" stands for "binary JSON" - a binary storage format that is JSON inspired.
This distribution merely rips it out of the bson repository into its own.


Compile !CHAOS Framework instruction: 

To compile all three framework cmake is to beused with "cmake .". There is a bootstra.sh script file that try to download al dependence library and put all into {chaos root directory}/external and compiled staff into {chaos root directory}/usr/local



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


To develop with zmqdriver DEV_WITH_ZMQ variablemust me defined