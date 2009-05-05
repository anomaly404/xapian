noinst_HEADERS +=\
	matcher/andmaybepostlist.h\
	matcher/andnotpostlist.h\
	matcher/andpostlist.h\
	matcher/branchpostlist.h\
	matcher/collapser.h\
	matcher/emptysubmatch.h\
	matcher/exactphrasepostlist.h\
	matcher/externalpostlist.h\
	matcher/extraweightpostlist.h\
	matcher/localmatch.h\
	matcher/mergepostlist.h\
	matcher/msetcmp.h\
	matcher/msetpostlist.h\
	matcher/multiandpostlist.h\
	matcher/orpostlist.h\
	matcher/phrasepostlist.h\
	matcher/queryoptimiser.h\
	matcher/remotesubmatch.h\
	matcher/selectpostlist.h\
	matcher/valuegepostlist.h\
	matcher/valuerangepostlist.h\
	matcher/xorpostlist.h

EXTRA_DIST +=\
	matcher/dir_contents\
	matcher/Makefile

if BUILD_BACKEND_REMOTE
lib_src +=\
	matcher/remotesubmatch.cc
endif
# Make sure we always distribute this source.
EXTRA_DIST +=\
	matcher/remotesubmatch.cc

lib_src +=\
	matcher/andmaybepostlist.cc\
	matcher/andnotpostlist.cc\
	matcher/andpostlist.cc\
	matcher/branchpostlist.cc\
	matcher/collapser.cc\
	matcher/emptysubmatch.cc\
	matcher/exactphrasepostlist.cc\
	matcher/externalpostlist.cc\
	matcher/localmatch.cc\
	matcher/mergepostlist.cc\
	matcher/msetcmp.cc\
	matcher/msetpostlist.cc\
	matcher/multiandpostlist.cc\
	matcher/multimatch.cc\
	matcher/orpostlist.cc\
	matcher/phrasepostlist.cc\
	matcher/queryoptimiser.cc\
	matcher/rset.cc\
	matcher/selectpostlist.cc\
	matcher/valuegepostlist.cc\
	matcher/valuerangepostlist.cc\
	matcher/xorpostlist.cc