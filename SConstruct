# -*- mode: python; -*-
# build file for ChaosLibrary
# this requires scons
# you can get from http://www.scons.org
# then just type scons

EnsureSConsVersion(0, 98, 4) # this is a common version known to work

import os
import sys
import imp
import types
import re
import shutil
import urllib
import urllib2
import socket
import time
import socket
import subprocess


buildMode = ARGUMENTS.get('mode', 'release')   #holds current mode
debugcflags = ['-Wall', '-GX', '-EHsc', '-DDEBUG']   #extra compile flags for debug
releasecflags = ['-O2', '-EHsc', '-DNDEBUG']         #extra compile flags for release

print '**** Compiling in ' + buildMode + ' mode...'

# --- options ----

options = {}

def add_option( name, help , nargs , contibutesToVariantDir , dest=None ):

    if dest is None:
        dest = name

    AddOption( "--" + name , 
                dest=dest,
                type="string",
                nargs=nargs,
                action="store",
                help=help )

    options[name] = { "help" : help ,
                      "nargs" : nargs , 
                      "contibutesToVariantDir" : contibutesToVariantDir ,
                      "dest" : dest } 

def get_option( name ):
    return GetOption( name )

def has_option( name ):
    x = get_option( name )
    if x is None:
        return False

    if x == False:
        return False

    if x == "":
        return False

    return True

def get_variant_dir():

    a = []

    for name in options:
        o = options[name]
        if not has_option( o["dest"] ):
            continue
        if not o["contibutesToVariantDir"]:
            continue

        if o["nargs"] == 0:
            a.append( name )
        else:
            a.append( name + "-" + get_option( name ) )

    s = "build/"

    if len(a) > 0:
        a.sort()
        s += "/".join( a ) + "/"

    return s


    
#---------add option tomake the Chaos Lib Shared
commonlibName = 'chaos_common'
cutoolkitlibName = 'chaos_cutoolkit'
libNameUIToolkit = 'chaos_uitoolkit'
add_option( "static", "build the chaos library as shared shared" , 0 , False )
add_option( "prefix", "build the chaos library as shared static" , 0 , False )
add_option( "all", "build all !CHAOS library" , 0 , False )
add_option( "common", "build !CHOAS Common library" , 0 , False )
add_option( "ui", "build !CHOAS UIToolkit library" , 0 , False )
add_option( "cu", "build !CHOAS CUToolkit library" , 0 , False )
add_option( "cpu", "Number of concurrent compilation unit" , 0 , False )

if has_option( "prefix" ):
    installDir = GetOption( "prefix" )
else:
    installDir = "/usr/local"

print "prefix:",installDir

#check the option

num_cpu = int(os.environ.get('NUM_CPU', 2))
SetOption('num_jobs', num_cpu)
print "Compile using -j ", GetOption('num_jobs')

env = Environment()  # Initialize the environment
env.Append(LIBPATH = ['/usr/lib', '/usr/local/lib'])
env.Append(CPPPATH='.')        
#setup the version
subprocess.call(os.path.abspath("chaos/common/version.sh"), shell=True)
 
#make sure the sconscripts can get to the variables
#Export('env', 'buildMode', 'debugcflags', 'releasecflags')

if buildMode == 'debug':
    env.Append(CCFLAGS=debugcflags)
else:
    env.Append(CCFLAGS=releasecflags)
   
#compile common
if has_option( "all" ) or has_option( "common" ) or  has_option( "ui" ) or  has_option( "cu" ) :
    print "Compile !CHAOS common layer"
    sourceFiles = Glob( "chaos/common/*.cpp" )
    sourceFiles += Glob( "chaos/common/bson/*.cpp" )
    sourceFiles += Glob( "chaos/common/bson/util/*.cpp" )
    sourceFiles += Glob( "chaos/common/bson/lib/*.cpp" )
    sourceFiles += Glob( "chaos/common/bson/lib/*.c" )
    sourceFiles += Glob( "chaos/common/configuration/*.cpp" )
    sourceFiles += Glob( "chaos/common/pool/*.cpp" )
    sourceFiles += Glob( "chaos/common/data/*.cpp" )
    sourceFiles += Glob( "chaos/common/thread/*.cpp" )
    sourceFiles += Glob( "chaos/common/utility/*.cpp" )
    sourceFiles += Glob( "chaos/common/general/*.cpp" )
    sourceFiles += Glob( "chaos/common/pqueue/*.cpp" )
    sourceFiles += Glob( "chaos/common/rpc/*.cpp" )
    sourceFiles += Glob( "chaos/common/rpcnet/*.cpp" )
    sourceFiles += Glob( "chaos/common/dispatcher/*.cpp" )
    sourceFiles += Glob( "chaos/common/action/*.cpp" )
    sourceFiles += Glob( "chaos/common/io/*.cpp" )
    sourceFiles += Glob( "chaos/common/exception/*.cpp" )

    if has_option( "static" ):
        commonlibName = str( env.StaticLibrary(commonlibName, sourceFiles)[0] )
    else:
        commonlibName = str( env.SharedLibrary(commonlibName, sourceFiles, LIBS=['boost_program_options', 'boost_system','boost_thread','boost_chrono','boost_regex','memcached','msgpack','msgpack-rpc','mpio'])[0] )
    print "Lib file created: " + commonlibName
    
    #headers
    for id in [ "chaos/common/", "chaos/common/bson/", "chaos/common/bson/util/", "chaos/common/bson/lib/", "chaos/common/configuration/", "chaos/common/pool/", "chaos/common/data/" , "chaos/common/thread/" , "chaos/common/utility/" , "chaos/common/general/" , "chaos/common/pqueue/" , "chaos/common/rpc/", "chaos/common/rpcnet/", "chaos/common/dispatcher/" , "chaos/common/action/" , "chaos/common/io/" , "chaos/common/exception/"]:
        for hFile in Glob( id + "*.h" ):
            installHeaderPath = installDir + "/" + "include" + "/" + id
            env.Install( installHeaderPath , hFile )
            env.Clean("clean_installed",installHeaderPath)
            env.Alias('install', installHeaderPath)

    #lib
    print "install library into: "+installDir + "/lib/"
    env.Clean("clean_installed",installDir + "/lib/"+ commonlibName)
    env.Install( installDir + "/lib/", commonlibName )
    


if has_option( "all" ) or has_option( "cu" ):
    sourceFiles = Glob( "chaos/cu_toolkit/*.cpp" )
    sourceFiles += Glob( "chaos/cu_toolkit/DataManager/*.cpp" )
    sourceFiles += Glob( "chaos/cu_toolkit/DataManager/DataBuffer/*.cpp" )
    sourceFiles += Glob( "chaos/cu_toolkit/ControlSystemValue/*.cpp" )
    sourceFiles += Glob( "chaos/cu_toolkit/ControlManager/*.cpp" )
    sourceFiles += Glob( "chaos/cu_toolkit/ConfigurationManager/*.cpp" )
    sourceFiles += Glob( "chaos/cu_toolkit/CommandManager/*.cpp" )


    if has_option( "static" ):
        cutoolkitlibName = str( env.StaticLibrary(cutoolkitlibName, sourceFiles)[0] )
    else:
        cutoolkitlibName = str( env.SharedLibrary(cutoolkitlibName, sourceFiles, LIBS=['boost_program_options', 'boost_system','boost_thread','boost_chrono','boost_regex','memcached','msgpack','msgpack-rpc','mpio', 'chaos_common'])[0] )
    print "Lib file created: " + cutoolkitlibName
    
    #headers
    for id in [ "chaos/cu_toolkit/", "chaos/cu_toolkit/DataManager/", "chaos/cu_toolkit/DataManager/DataBuffer/", "chaos/cu_toolkit/ControlSystemValue/", "chaos/cu_toolkit/ControlManager/", "chaos/cu_toolkit/ControlException/", "chaos/cu_toolkit/ConfigurationManager/","chaos/cu_toolkit/CommandManager/"]:
        for hFile in Glob( id + "*.h" ):
            installHeaderPath = installDir + "/" + "include" + "/" + id
            env.Install( installHeaderPath , hFile )
            env.Clean("clean_installed",installHeaderPath)
            env.Alias( 'install' , installHeaderPath )

    #lib
    print "install library into: "+installDir + "/lib/"
    env.Clean("clean_installed",installDir + "/lib/"+ cutoolkitlibName)
    env.Install( installDir + "/lib/", cutoolkitlibName )



if has_option( "all" )  or has_option( "ui" ):
    sourceFiles = Glob( "chaos/ui_toolkit/*.cpp" )
    sourceFiles += Glob( "chaos/ui_toolkit/Common/*.cpp" )
    sourceFiles += Glob( "chaos/ui_toolkit/HighLevelApi/*.cpp" )
    sourceFiles += Glob( "chaos/ui_toolkit/LowLevelApi/*.cpp" )
    sourceFiles += Glob( "chaos/ui_toolkit/DataSystem/*.cpp" )
    sourceFiles += Glob( "chaos/ui_toolkit/RpcSystem/*.cpp" )

    if buildMode == 'debug':
        env.Append(CCFLAGS=debugcflags)
    else:
        env.Append(CCFLAGS=releasecflags)

    if has_option( "static" ):
        libName = str( env.StaticLibrary(libNameUIToolkit, sourceFiles)[0] )
    else:
        libNameUIToolkit = str( env.SharedLibrary(libNameUIToolkit, sourceFiles, LIBS=['boost_program_options', 'boost_system','boost_thread','boost_chrono','boost_regex','memcached','msgpack','msgpack-rpc','mpio',  'chaos_common'])[0] )
    print "Lib file created: " + libNameUIToolkit
    
    #headers
    for id in [ "chaos/ui_toolkit/", "chaos/ui_toolkit/Common/", "chaos/ui_toolkit/HighLevelApi/", "chaos/ui_toolkit/LowLevelApi/", "chaos/ui_toolkit/DataSystem/", "chaos/ui_toolkit/RpcSystem/"]:
        for hFile in Glob( id + "*.h" ):
            installHeaderPath = installDir + "/" + "include" + "/" + id
            env.Install( installHeaderPath , hFile )
            env.Clean("clean_installed",installHeaderPath)
            env.Alias('install', installHeaderPath)

    #lib
    print "install library into: "+installDir + "/lib/"
    env.Clean("clean_installed",installDir + "/lib/"+ libNameUIToolkit)
    env.Install( installDir + "/lib/", libNameUIToolkit )
    #env.Alias('install', [installDir + "/lib/"])

env.Alias('install', [installDir + "/lib/"])
env.Alias('install', [installDir + "/include/"])