EXTRA_DIST +=\
	harness/dir_contents\
	harness/Makefile

noinst_HEADERS +=\
	harness/backendmanager.h\
	harness/backendmanager_brass.h\
	harness/backendmanager_chert.h\
	harness/backendmanager_inmemory.h\
	harness/backendmanager_local.h\
	harness/backendmanager_multi.h\
	harness/backendmanager_remote.h\
	harness/backendmanager_remoteprog.h\
	harness/backendmanager_remotetcp.h\
	harness/cputimer.h\
	harness/fdtracker.h\
	harness/index_utils.h\
	harness/unixcmds.h\
	harness/scalability.h\
	harness/testmacros.h\
	harness/testrunner.h\
	harness/testsuite.h\
	harness/testutils.h

testharness_sources =\
	harness/backendmanager.cc\
	harness/backendmanager_multi.cc\
	harness/cputimer.cc\
	harness/fdtracker.cc\
	harness/index_utils.cc\
	harness/scalability.cc\
	harness/testrunner.cc\
	harness/testsuite.cc\
	harness/testutils.cc\
	harness/unixcmds.cc

utestharness_sources =\
	harness/fdtracker.cc\
	harness/utestsuite.cc

if BUILD_BACKEND_BRASS
testharness_sources += harness/backendmanager_brass.cc
endif

if BUILD_BACKEND_CHERT
testharness_sources += harness/backendmanager_chert.cc
endif

if BUILD_BACKEND_INMEMORY
testharness_sources += harness/backendmanager_inmemory.cc
endif

if BUILD_BACKEND_REMOTE
testharness_sources +=\
	harness/backendmanager_remote.cc\
	harness/backendmanager_remoteprog.cc\
	harness/backendmanager_remotetcp.cc
endif
