!CHAOS Framework



Compile !CHAOS Framework instruction: 

To compile all tree framework(Common [--common], CUToolkit[--cu], UIToolkit[--ui]) togheter must be used the line "sudo scons --all install", the package can be compilide also in separated way substituting --all with the single package parameter.

To removed the instaled file: "sudo scons --all -c clean_installed"

To removed compilation object only: "sudo scons --all -c"

To generate documentation:  Doxygen software is needed and it can be downloaded from http://www.stack.nl/~dimitri/doxygen/ 
                            after installation launch from terminal "doxygen Documentation/chaosdocs"


Library used by CHAOS:

    memcached:  memcached
    msgpack:    msgpack, msgpack-rpc, mpio
    boost:      boost_program_options, boost_system, boost_thread, boost_chrono, boost_regex, 
