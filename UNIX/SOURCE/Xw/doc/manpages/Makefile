
install:
	if [ ! -d /opt/man/cat3 ]; then \
	  mkdir -p /opt/man/cat3; \
	fi
	for i in *.n; do \
	  cp $$i /opt/man/cat3/`basename $$i .n`.1; \
	done