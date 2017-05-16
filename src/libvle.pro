include(../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source
CONFIG += create_pc create_prl no_install_prl

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR" \
           "VERSION_MINOR=$$VERSION_MINOR" \
           "VERSION_PATCH=$$VERSION_PATCH" \
           "VERSION_EXTRA=\"$$VERSION_EXTRA\""

VERSION = 0

TEMPLATE = lib

TARGET = vle-$$VERSION_ABI

HEADERS = vle/manager/ExperimentGenerator.hpp \
  vle/manager/Simulation.hpp \
  vle/manager/Manager.hpp \
  vle/manager/Types.hpp \
  vle/oov/Plugin.hpp \
  vle/translator/MatrixTranslator.hpp \
  vle/translator/GraphTranslator.hpp \
  vle/utils/PackageTable.hpp \
  vle/utils/Filesystem.hpp \
  vle/utils/Tools.hpp \
  vle/utils/Exception.hpp \
  vle/utils/Template.hpp \
  vle/utils/DownloadManager.hpp \
  vle/utils/Parser.hpp \
  vle/utils/i18n.hpp \
  vle/utils/Context.hpp \
  vle/utils/Array.hpp \
  vle/utils/Spawn.hpp \
  vle/utils/Algo.hpp \
  vle/utils/unit-test.hpp \
  vle/utils/Deprecated.hpp \
  vle/utils/DateTime.hpp \
  vle/utils/ContextPrivate.hpp \
  vle/utils/Package.hpp \
  vle/utils/Types.hpp \
  vle/utils/RemoteManager.hpp \
  vle/utils/Rand.hpp \
  vle/devs/DynamicsWrapper.hpp \
  vle/devs/Thread.hpp \
  vle/devs/DynamicsDbg.hpp \
  vle/devs/View.hpp \
  vle/devs/ExternalEventList.hpp \
  vle/devs/InitEventList.hpp \
  vle/devs/Time.hpp \
  vle/devs/DynamicsInit.hpp \
  vle/devs/ModelFactory.hpp \
  vle/devs/DynamicsObserver.hpp \
  vle/devs/Executive.hpp \
  vle/devs/Dynamics.hpp \
  vle/devs/ExternalEvent.hpp \
  vle/devs/test/oov.hpp \
  vle/devs/ViewEvent.hpp \
  vle/devs/InternalEvent.hpp \
  vle/devs/Scheduler.hpp \
  vle/devs/ObservationEvent.hpp \
  vle/devs/Coordinator.hpp \
  vle/devs/Simulator.hpp \
  vle/devs/RootCoordinator.hpp \
  vle/value/Map.hpp \
  vle/value/Boolean.hpp \
  vle/value/Table.hpp \
  vle/value/User.hpp \
  vle/value/XML.hpp \
  vle/value/String.hpp \
  vle/value/Double.hpp \
  vle/value/Null.hpp \
  vle/value/Set.hpp \
  vle/value/Tuple.hpp \
  vle/value/Integer.hpp \
  vle/value/Value.hpp \
  vle/value/Matrix.hpp \
  vle/DllDefines.hpp \
  vle/vpz/Conditions.hpp \
  vle/vpz/SaxStackValue.hpp \
  vle/vpz/View.hpp \
  vle/vpz/Structures.hpp \
  vle/vpz/Output.hpp \
  vle/vpz/Model.hpp \
  vle/vpz/Observable.hpp \
  vle/vpz/BaseModel.hpp \
  vle/vpz/AtomicModel.hpp \
  vle/vpz/Classes.hpp \
  vle/vpz/Vpz.hpp \
  vle/vpz/Dynamics.hpp \
  vle/vpz/Observables.hpp \
  vle/vpz/Base.hpp \
  vle/vpz/SaxStackVpz.hpp \
  vle/vpz/Outputs.hpp \
  vle/vpz/Project.hpp \
  vle/vpz/Views.hpp \
  vle/vpz/Condition.hpp \
  vle/vpz/Dynamic.hpp \
  vle/vpz/CoupledModel.hpp \
  vle/vpz/Port.hpp \
  vle/vpz/SaxParser.hpp \
  vle/vpz/ModelPortList.hpp \
  vle/vpz/Class.hpp \
  vle/vpz/Experiment.hpp \
  vle/vle.hpp

SOURCES = vle/manager/ExperimentGenerator.cpp \
  vle/manager/Simulation.cpp \
  vle/manager/Manager.cpp \
  vle/oov/Plugin.cpp \
  vle/translator/GraphTranslator.cpp \
  vle/translator/MatrixTranslator.cpp \
  vle/vle.cpp \
  vle/utils/Filesystem.cpp \
  vle/utils/DateTime.cpp \
  vle/utils/ContextSettings.cpp \
  vle/utils/Package.cpp \
  vle/utils/ContextModule.cpp \
  vle/utils/PackageTable.cpp \
  vle/utils/DownloadManager.cpp \
  vle/utils/Template.cpp \
  vle/utils/Parser.cpp \
  vle/utils/Context.cpp \
  vle/utils/Rand.cpp \
  vle/utils/Exception.cpp \
  vle/utils/Tools.cpp \
  vle/utils/RemoteManager.cpp \
  vle/utils/details/PackageManager.cpp \
  vle/utils/details/PackageParser.cpp \
  vle/devs/RootCoordinator.cpp \
  vle/devs/ExternalEvent.cpp \
  vle/devs/ModelFactory.cpp \
  vle/devs/DynamicsWrapper.cpp \
  vle/devs/Simulator.cpp \
  vle/devs/View.cpp \
  vle/devs/Dynamics.cpp \
  vle/devs/Time.cpp \
  vle/devs/ViewEvent.cpp \
  vle/devs/ExternalEventList.cpp \
  vle/devs/InitEventList.cpp \
  vle/devs/InternalEvent.cpp \
  vle/devs/Scheduler.cpp \
  vle/devs/Executive.cpp \
  vle/devs/DynamicsDbg.cpp \
  vle/devs/Coordinator.cpp \
  vle/value/Null.cpp \
  vle/value/Map.cpp \
  vle/value/Boolean.cpp \
  vle/value/Table.cpp \
  vle/value/XML.cpp \
  vle/value/Double.cpp \
  vle/value/Tuple.cpp \
  vle/value/Value.cpp \
  vle/value/Integer.cpp \
  vle/value/Matrix.cpp \
  vle/value/String.cpp \
  vle/value/Set.cpp \
  vle/vpz/AtomicModel.cpp \
  vle/vpz/Model.cpp \
  vle/vpz/Observable.cpp \
  vle/vpz/Classes.cpp \
  vle/vpz/Dynamic.cpp \
  vle/vpz/Outputs.cpp \
  vle/vpz/View.cpp \
  vle/vpz/Dynamics.cpp \
  vle/vpz/CoupledModel.cpp \
  vle/vpz/Output.cpp \
  vle/vpz/SaxStackVpz.cpp \
  vle/vpz/Views.cpp \
  vle/vpz/Project.cpp \
  vle/vpz/BaseModel.cpp \
  vle/vpz/Conditions.cpp \
  vle/vpz/Observables.cpp \
  vle/vpz/Class.cpp \
  vle/vpz/SaxStackValue.cpp \
  vle/vpz/Vpz.cpp \
  vle/vpz/SaxParser.cpp \
  vle/vpz/Condition.cpp \
  vle/vpz/Experiment.cpp \
  vle/vpz/ModelPortList.cpp

macx {
  SOURCES += vle/utils/details/SpawnUnix.cpp \
    vle/utils/details/PathUnix.cpp

  QMAKE_CXXFLAGS += -iwithsysroot /usr/include/libxml2
  QMAKE_CXXFLAGS += -I/usr/local/opt/boost/include

  LIBS += -lxml2
}

win32 {
  SOURCES += vle/utils/details/SpawnWin.cpp \
    vle/utils/details/UtilsWin.cpp \
    vle/utils/details/PathWin32.cpp \
}

unix:!macx {
  SOURCES += vle/utils/details/SpawnUnix.cpp \
    vle/utils/details/PathUnix.cpp

  PKGCONFIG += libxml-2.0

  CONFIG += link_pkgconfig
}

target.path = $$LIBDIR

header_files.path = $$INCLUDEDIR/vle
header_files.files = vle/vle.hpp vle/DllDefines.hpp

header_files_devs.path = $$INCLUDEDIR/vle/devs
header_files_devs.files = vle/devs/Dynamics.hpp vle/devs/DynamicsWrapper.hpp vle/devs/Executive.hpp vle/devs/ExternalEvent.hpp vle/devs/ExternalEventList.hpp vle/devs/InitEventList.hpp vle/devs/ObservationEvent.hpp vle/devs/Time.hpp

header_files_manager.path = $$INCLUDEDIR/vle/manager
header_files_manager.files = vle/manager/ExperimentGenerator.hpp vle/manager/Manager.hpp vle/manager/Simulation.hpp vle/manager/Types.hpp

header_files_oov.path = $$INCLUDEDIR/vle/oov
header_files_oov.files = vle/oov/Plugin.hpp

header_files_translator.path = $$INCLUDEDIR/vle/translator
header_files_translator.files = vle/translator/GraphTranslator.hpp vle/translator/MatrixTranslator.hpp

header_files_utils.path = $$INCLUDEDIR/vle/utils
header_files_utils.files = vle/utils/Algo.hpp vle/utils/Array.hpp vle/utils/Context.hpp vle/utils/DateTime.hpp vle/utils/Deprecated.hpp vle/utils/DownloadManager.hpp vle/utils/Exception.hpp vle/utils/Filesystem.hpp vle/utils/Package.hpp vle/utils/PackageTable.hpp vle/utils/Parser.hpp vle/utils/Rand.hpp vle/utils/RemoteManager.hpp vle/utils/Spawn.hpp vle/utils/Template.hpp vle/utils/Tools.hpp vle/utils/Types.hpp vle/utils/unit-test.hpp

header_files_value.path = $$INCLUDEDIR/vle/value
header_files_value.files = vle/value/Boolean.hpp vle/value/Double.hpp vle/value/Integer.hpp vle/value/Map.hpp vle/value/Matrix.hpp vle/value/Null.hpp vle/value/Set.hpp vle/value/String.hpp vle/value/Table.hpp vle/value/Tuple.hpp vle/value/User.hpp vle/value/Value.hpp vle/value/XML.hpp

header_files_vpz.path = $$INCLUDEDIR/vle/vpz
header_files_vpz.files = vle/vpz/Base.hpp vle/vpz/Classes.hpp vle/vpz/Class.hpp vle/vpz/Condition.hpp vle/vpz/Conditions.hpp vle/vpz/Dynamic.hpp vle/vpz/Dynamics.hpp vle/vpz/Experiment.hpp vle/vpz/Model.hpp vle/vpz/Observable.hpp vle/vpz/Observables.hpp vle/vpz/Output.hpp vle/vpz/Outputs.hpp vle/vpz/Port.hpp vle/vpz/Project.hpp vle/vpz/Structures.hpp vle/vpz/View.hpp vle/vpz/Views.hpp vle/vpz/Vpz.hpp vle/vpz/AtomicModel.hpp vle/vpz/CoupledModel.hpp vle/vpz/BaseModel.hpp vle/vpz/ModelPortList.hpp

QMAKE_PKGCONFIG_FILE = vle-$${VERSION_ABI}
QMAKE_PKGCONFIG_NAME = vle-$$VERSION_ABI
QMAKE_PKGCONFIG_DESCRIPTION = VLE multimodelling and Simulation tools
QMAKE_PKGCONFIG_PREFIX = $$PREFIX
QMAKE_PKGCONFIG_LIBDIR = $$LIBDIR
QMAKE_PKGCONFIG_INCDIR = $$INCLUDEDIR
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}

INSTALLS += target header_files header_files_devs header_files_manager header_files_oov header_files_translator header_files_utils header_files_value header_files_vpz
