!CHAOS Framework
=================================================

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

!CHAOS use the standalone BSON C++ implementation, the source is exstracted from orginal mongodb opensource distribution found on http://www.mongodb.org/ (http://www.10gen.com/)
"BSON" stands for "binary JSON" - a binary storage format that is JSON inspired.
This distribution merely rips it out of the bson repository into its own.


For further documentation please visit:
http://chaos.infn.it                        - public information site
https://opensource-confluence.infn.it       - public user and developer site
https://github.com/bisegni/chaosframework   - public source code usefull to collaborate with us

docker container for build chaos: https://cloud.docker.com/_/repository/docker/bisegni/chaos/general

release 1.0.0 (alpha 1 build 3)
