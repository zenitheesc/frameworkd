set(sources
	src/utilities/config-handler/config-handler.cpp
	src/classes/daemon/daemon.cpp
	src/classes/static-service-proxy/static-service-proxy.cpp
	src/classes/locked-storage/locked-storage.cpp
	src/classes/dbus-handler/dbus-handler.cpp
	src/classes/service-proxy/service-proxy.cpp
	src/classes/routine-service-proxy/routine-service-proxy.cpp
	src/classes/service-handler/service-handler.cpp
)

set(exe_sources
	${sources}
)

set(headers
	src/utilities/config-handler/config-handler.hpp
	src/classes/daemon/daemon.hpp
	src/classes/iservice/iservice.hpp
	src/classes/static-service-proxy/static-service-proxy.hpp
	src/classes/locked-storage/locked-storage.hpp
	src/classes/dbus-handler/dbus-handler.hpp
	src/classes/service-proxy/service-proxy.hpp
	src/classes/routine-service-proxy/routine-service-proxy.hpp
	src/classes/service-handler/service-handler.hpp
)

set(test_sources
	src/daemon_test.cpp
)
