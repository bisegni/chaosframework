# Changelog
All notable changes to this project will be documented in this file.
The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## 0.9.1 - 2019-05-14
### Changed
- Control Unit driver singleton now are created using two different mode. According to driver init parameter, normal string or json object. In this case if is resent a key "driver_param" the json content of this key will be used as key for search the singleton instance of the driver
- MDS source code has been redesigned to simplify code

### Added
- New tre job has been added to gitlab pipeline for manage the deploy on centos7. In this case the compilation is in "release" mode.
- Pipeline now are triggere only via "web" or "schedule" in association with the variable BUILD_TYPE with the values "test", "deploy", "coverity".
- new bash script for the management for the creation of new changelog entry
- New kind of storage driver has been added. Hybrid Driver uses mongodb as index database for complex query and additional technology for store the daq data. Actually has been implemented the cassandra storage layer for Hybrid driver

### Deploy notes
- A new implementation of storage driver has been added. The new mongodb db driver has been used

### Fixed
- The three driver (persistence, storage and cache) are now accessed by a single reference. Internally every driver need to achive the pooling to creare connection on request

---
