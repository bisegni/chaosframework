//
//  connection_type.h
//  CHAOSFramework
//
//  Created by bisegni on 02/08/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#ifndef connection_type_h
#define connection_type_h

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace connection_adapter {
                class AbstractConnectionAdapter;
            }
            //! Proxy type definition
            typedef enum {
                /*!Specify a proxy that permit to act ans a driver at low level,
                 no specified portocol is used messange and request need to be
                 managed by developer */
                ProxyTypeRawDriver
            } ProxyType;

            //! Proxy type definition
            typedef enum {
                /*!Specify the extended json used
                 for BSON serialization to use in
                 comunication */
                SerializationTypeBSONJson
            } SerializationType;

            //! Proxy type definition
            typedef enum {
                /*!Specify protocol used to
                 connect to remote endpoint */
                ConnectionTypeHTTP
            } ConnectionType;

            /*!
             Templated interface that give rule for the the instantiation of a class with one param constructor.
             */
            template <typename R, typename p1 >
            class ObjectInstancerP1 {
            public:
                virtual ~ObjectInstancerP1(){};
                virtual R* getInstance(p1 _p1) = 0;
            };

            /*!
             Templated interface that give rule for the the instantiation of a class with two param constructor.
             */
            template <typename R, typename p1, typename p2 >
            class ObjectInstancerP2 {
            public:
                virtual ~ObjectInstancerP2(){};
                virtual R* getInstance(p1 _p1, p2 _p2) = 0;
            };


            /*!
             Templated class that permit to instantiate the superclas of
             a base class. This class permit to check this rule at compiletime
             */
            template <typename T, typename R, typename p1>
            class TypedObjectInstancerP1:
            public ObjectInstancerP1<R, p1> {
            public:
                R* getInstance(p1 _p1) {
                    return new T(_p1);
                }
            };

            /*!
             Templated class that permit to instantiate the superclas of
             a base class. This class permit to check this rule at compiletime
             */
            template <typename T, typename R, typename p1, typename p2 >
            class TypedObjectInstancerP2:
            public ObjectInstancerP2<R, p1, p2> {
            public:
                R* getInstance(p1 _p1, p2 _p2) {
                    return new T(_p1, _p2);
                }
            };

            template<typename T>
            struct UnitConnection {
                const ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter> connection_adapter;
                const ChaosUniquePtr<T> unit_proxy;

                UnitConnection(ChaosUniquePtr<connection_adapter::AbstractConnectionAdapter>& _protocol_adapter,
                               ChaosUniquePtr<T> _unit_proxy):
                connection_adapter(MOVE(_protocol_adapter)),
                unit_proxy(MOVE(_unit_proxy)){}
            };
        }
    }
}

#endif /* connection_type_h */
