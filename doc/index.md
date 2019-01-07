![logo_chaos_col](logo_chaos_col.png)

[TOC]

# Description
The !CHAOS project aims to develop a new concept of control system and data acquisition framework by providing, with a high level of abstraction, all the services needed for controlling and managing a large scientific, or non-scientific, infrastructure.
!CHAOS redefines the Control System paradigm by introducing the new concept of Control Service, i.e. a distributed, scalable provider offering, to a general class of clients, high- performance services such as data sharing, commands dispatching, continuous historical data archiving, configuration tools, middle-layer resource management, etc ...

The key features and development strategies of !CHAOS are: scalability of performances and size:
* integration of all functionalities
* abstraction of services, devices and data
* easy and modular customisation
* extensive data catching for performance boost high-performance internet software technologies.

# Principal Components
Here are briefly described the components of the system.

##Control Unit (CU)
CU interfaces with the !CHAOS hardware by using a driver and developing both control and data acquisition.
At software level, CU is an exe file built using the !CHAOS libraries, developed in C language.
see documentation [here](control-unit/index.md)

## Execution Unit (EU)
performs the feedback control of the CU via a dedicated driver. It captures data from CDS, processes them and sends commands to the feedback of the CU.

## Client toolkits (UI)
monitors data and shows them to the users. The impact of the UI on the infrastructure is strictly related to:
the quantity of data to monitor;
the type of the user interface used (command line, web, ...)
see documentation [here](micro-unit-toolkit/index.md)

## Metadata Server (MDS)
is aimed to manage configuration metadata of the other components of the architecture. The required characteristics are:
High availability
High scalability is not required since the load of the metadata server is not proportional to the workload of the control system, but is only related to the number of system components.

## CHAOS Data Server (CDS)
is responsible of data archiving and distribution from and to the Control Units, Execution Units and User Interfaces. Data management includes the following activities:
data storage reading the latest data available; querying historical data.
CDS should provide the following characteristics: high performances;
high scalability; multi-threading.


link to specialised pages:
