MYDIR=./commands
BIN=./bin/

build:
	@for dir in $(MYDIR)/* ; do \
		for file in $$dir/*.c ; do \
				if [ -f "$$file" ]; then \
					gcc -o $(BIN)$$(basename $$file .c) $$file; \
				fi \
			done \
	done
