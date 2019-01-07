# Metadata service

[TOC]

The ***Metadata Service***(MDS) is a chaos process that act as separation layer from !CHAOS nodes and backend implementations(live cache, database and storage). It is developed keep in mind the High availability and scalability. It abstract the backend usage to other chaos node, the mds need to live upon a three different backend, on for the live caching, one for metadata management and one for storage.

##Configuration
Below is shown a standard configuration file.
```
#publishing port
rpc-server-port=5000

#Enable the parallels execution for rpc request
rpc-domain-scheduler-type=1

#Specify the driver to use for metadata management
persistence-impl=MongoDB
persistence-servers=hostname:port
#these parameter are driver specific
persistence-kv-param=db:[db name]
persistence-kv-param=pwd:[username]
persistence-kv-param=user:[password]

#Specify the driver to use for live caching
cache-driver=Couchbase
cache-servers=hostname:port
#these parameter are driver specific
cache-driver-kvp=bucket:[bucket name name]
cache-driver-kvp=user:[username]
cache-driver-kvp=pwd:[password]

obj-storage-driver=MongoDB
obj-storage-driver-server_url=hostname:port
#these parameter are driver specific
obj-storage-kvp=db:[db name]
obj-storage-kvp=pwd:[username]
obj-storage-kvp=user:[password]

cron-job-check-repeat-time=30
cron-job-ageing-management-repeat-time=30
```

##Driver Specification
The MDS Executable can be linked at many implementation of the each three driver, but only one can be used and all three driver need to be specified at launch time.

### Object storage
The object storage driver has the role to persists metric and DAQ data on a backend. It need to be specified at startup with the parameter obj-storage-driver.
[Old MongoDB Driver](storage/legacy-mongodb.md)
[New MongoDB Driver](storage/new-mongodb.md)
