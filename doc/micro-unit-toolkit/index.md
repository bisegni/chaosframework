# Micro Unit toolkit

[TOC]

The ***Micro Unit Toolkit***(MUT) provides a way to connect with the external unit layer exposed by control unit toolkit. It is build with very little dependency, most of which are included directly in MUT source as amalgamated source file.

## Why another toolkit
<p align="justify">
This new toolkit is created keeping in mind that it should compile on most platform starting from a very old operating system and hardware to the new realtime operation system. In general where !CHAOS framework can't be compiled or run the micro unit toolkit permit extends this limitation. The toolkit is the remote counterpart of the [External Unit Server (EUS)](../control-unit/external_unit.md) hosted by the CUT.
</p>

## Dependency
The library that are used by the toolkit are:
* [jsonccp](https://github.com/open-source-parsers/jsoncpp) used for the data serialization
* [mongoose](https://github.com/cesanta/mongoose) used for web-socket http management

## Architecture
<p align="justify">
The internal MUT architecture is base on two different Layers, on that abstract the connection implementation and one that realize the unit implementation. The toolkit is totally developed to an event based interaction with user. The unit toolkit uses the connection adapter to create a connection to the remote EUS and  exchange data, with the remote counterpart, with the chosen unit proxy.
</p>
<p align="justify">
The Exchanged data consists on message that are exchanged by each endpoint in an asynchronous way. Each one can, spontaneously, send a message to the other counterpart. The semantic of the messages follow the key/value logic. Different serialization can optimize the way to code this semantic[example json, bson, avro , etc...]
</p>

### Connection Adapter(CAL)
<p align="center"> <img src="../img/mut_ca.png"></p>
<p align="justify">
The abstract adapter implement all primitive communication for send message and request and contains a queue of received spontaneous message from remote endpoint or response to a request response. The subclass need to implement four abstract method:
</p>
```cpp
virtual int connect() = 0;
virtual void poll(int32_t milliseconds_wait = 100) = 0;
virtual int sendRawMessage(chaos::micro_unit_toolkit::data::DataPackUniquePtr& message) = 0;
virtual int close() = 0;
```

<p align="justify">
The subclass need to manage by itself the connection state and a queue of message to send, pool is called repetitively to permit to implementation to send data and receive new message. When something has been received, subclass need to call the below superclass method to send new arrival in the queue of the received messages.
</p>
```cpp
void handleReceivedMessage(chaos::micro_unit_toolkit::data::DataPackSharedPtr& received_message);
```
<p align="justify">
The design is very basic and not use any thread or async things, so every additional feature need to be implemented by the user. In this way the adapter can be adapted also into very bare-bone or limited resources hardware. The abstraction itself implements the external unit logic communication layer exposing this public method:
</p>

```cpp
const ConnectionState& getConnectionState() const;
int sendMessage(data::DataPackUniquePtr& message);
int sendRequest(data::DataPackUniquePtr& message, uint32_t& request_id);
bool hasMoreMessage();
chaos::micro_unit_toolkit::data::DataPackSharedPtr getNextMessage();
bool hasResponse();
bool hasResponseAvailable(uint32_t request_id);
```
<p align="justify">
Below is shown the flow of a connection by his states:
<p align="center"> <img src="../img/mut_connection_flow.png"></p>
</p>

Implemented superclass and protocols:
* [http connection adapter](./http_connection_adapter.md)


#### HTTP Connection Adapter
<p align="justify">
The http connection adapter realize a JSON based protocol transmitter over web-socket connection with the remote EUS exposed by control unit.
</p>

### Unit Proxy(UPL)
<p align="center"> <img src="../img/mut_up.png"></p>
<p align="justify">
the unit proxy uses the connection and abstract data representation to exchange dat with the [EUS](../control-unit/external_unit.md). In other world the type of connection and serialization are managed at connection level. In any way, in whatever serialization will be created, the messages will ever contains a notions of key and value, so the semantic will never changes.   
</p>
<p align="justify">
Unit Proxy is the base class for all implemented unit at this time only the Raw Driver unit is implemented. In future will be implemented the whole Control Unit protocol stack for totally externalize a real CU. Unit proxy realize the base transport layer for unit object, it define the semantic of send and receive message and request.
</p>

* Message are data that are send o received by each connection endpoint.
* <p align="justify">Requests are message that need a response by the endpoint that received it. The request with the message also delivers the is of the request. This code need to be send with the message that need to be used as response.</p>
