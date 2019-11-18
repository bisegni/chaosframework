cmake_minimum_required(VERSION 3.0.0)
project(ChaosCustomDesignerWidget)

option(CHAOS_PROMETHEUS "Enable Prometheus metrics support" OFF)

MESSAGE(">>Find the Qt5 library")
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Designer REQUIRED)

# Get Qt root directory
get_property(_filepath TARGET "Qt5::Core" PROPERTY LOCATION_RELEASE)
get_filename_component(_dir ${_filepath} PATH)
IF(UNIX AND NOT APPLE)
    set(QT_ROOT_DIR "${_dir}/..")
ELSE()
    set(QT_ROOT_DIR "${_dir}/../..")    
ENDIF()

SET(QT_PLUGINS_DIR ${QT_ROOT_DIR}/plugins)

SET(CMAKE_CXX_STANDARD 11)

MESSAGE(">>Find includes in corresponding build directories")
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set the output folder where your program will be created
SET(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

MESSAGE(">>Instruct CMake to run moc automatically when needed.")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

MESSAGE(">>QT Version: " ${Qt5Widgets_VERSION})
MESSAGE(">>QT WIDGET DIR: " ${Qt5Widgets_INCLUDE_DIRS})
MESSAGE(">>QT Designer DIR: " ${Qt5Designer_LIBRARIES})

SET(QT_QMAKE_EXECUTABLE ${QT_ROOT_DIR}/gcc_64/bin/qmake)
MESSAGE(STATUS "QT_QMAKE_EXECUTABLE: " ${QT_QMAKE_EXECUTABLE})

#INCLUDE_DIRECTORIES(${QT_LIB_DIR})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Designer_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR_QtCore})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../../../usr/local/include)
LINK_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../../../usr/local/lib)

MESSAGE(">>Tell CMake to create the ccs executable")

QT5_WRAP_CPP(HEADERS_MOC ${QT_HEADERS}) 
QT5_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})

set(SOURCE
    CDatasetAttributeLabelMenu.cpp
    CChangeSetGlobalCommitPlugin.cpp
    CDatasetAttributeHealthLabelPlugin.cpp
    CDatasetAttributeImagePlugin.cpp
    CDatasetAttributeLabelDialog.cpp
    CDatasetAttributeLabelPlugin.cpp
    CDatasetAttributeSetValueComboBoxDialog.cpp
    CDatasetAttributeSetValueComboBoxMenuExtension.cpp
    CDatasetAttributeSetValueComboBoxPlugin.cpp
    CDatasetAttributeSetValueLineEditPlugin.cpp
    CUOnlineLedIndicatorPlugin.cpp
    ChaosWidgets.cpp
    ../../theme/theme.qrc)

set(SOURCE ${SOURCE}
    ../../widget/designer/ChaosBaseUI.cpp
    ../../widget/designer/ChaosBaseDatasetUI.cpp
    ../../widget/designer/ChaosBaseDatasetAttributeUI.cpp
    ../../widget/designer/ChaosBaseCommandButtonUI.cpp
    ../../widget/designer/ui/CChangeSetGlobalCommit.cpp
    ../../widget/designer/ui/CDatasetAttributeHealthLabel.cpp
    ../../widget/designer/ui/CDatasetAttributeImage.cpp
    ../../widget/designer/ui/CDatasetAttributeLabel.cpp
    ../../widget/designer/ui/CDatasetAttributeSetValueComboBox.cpp
    ../../widget/designer/ui/CDatasetAttributeSetValueLineEdit.cpp
    ../../widget/designer/ui/CUOnlineLedIndicator.cpp)

set(CP_LINK_LIB    Qt5::Core
                    Qt5::Designer)

add_library(ChaosCustomDesignerWidget SHARED ${SOURCE})

target_link_libraries(ChaosCustomDesignerWidget ${CP_LINK_LIB})
IF(UNIX AND NOT APPLE)
MESSAGE(">>Add Linux Library")
    # ADD_LIBRARY(ChaosCustomDesignerWidget SHARED ${CUSTOM_WIDGET_SOURCE})
    install(TARGETS ChaosCustomDesignerWidget DESTINATION lib)
    INSTALL( TARGETS ChaosCustomDesignerWidget DESTINATION ${QT_PLUGINS_DIR}/designer)
ELSE()
    MESSAGE(">>Add OSX Library")
    #Bundle (OSX)
    # the property added to Info.plist

    INSTALL( TARGETS ChaosCustomDesignerWidget  DESTINATION "${CMAKE_SOURCE_DIR}/ccs.app/Contents/PlugIns/designer")
    INSTALL( TARGETS ChaosCustomDesignerWidget DESTINATION ${QT_PLUGINS_DIR}/designer)
ENDIF()
