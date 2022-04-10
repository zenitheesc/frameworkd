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
	include/frameworkd/utilities/config-handler/config-handler.hpp
	include/frameworkd/classes/daemon/daemon.hpp
	include/frameworkd/classes/iservice/iservice.hpp
	include/frameworkd/classes/static-service-proxy/static-service-proxy.hpp
	include/frameworkd/classes/locked-storage/locked-storage.hpp
	include/frameworkd/classes/dbus-handler/dbus-handler.hpp
	include/frameworkd/classes/service-proxy/service-proxy.hpp
	include/frameworkd/classes/routine-service-proxy/routine-service-proxy.hpp
	include/frameworkd/classes/service-handler/service-handler.hpp
)

set(test_sources
	#src/daemon_test.cpp
)
