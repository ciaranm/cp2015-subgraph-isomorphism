TIMEOUT := 100
THREADS := $(shell nproc )
STACK_SPACE := 1048576

all :
	$(MAKE) -f Makefile.sequential TIMEOUT=$(TIMEOUT) STACK_SPACE=$(STACK_SPACE)
	$(MAKE) -f Makefile.lad TIMEOUT=$(TIMEOUT) STACK_SPACE=$(STACK_SPACE)
	$(MAKE) -f Makefile.vf2 TIMEOUT=$(TIMEOUT)
	$(MAKE) -j1 -f Makefile.snd TIMEOUT=$(TIMEOUT)
	$(MAKE) -j1 -f Makefile.threaded TIMEOUT=$(TIMEOUT) THREADS=$(THREADS) STACK_SPACE=$(STACK_SPACE)
