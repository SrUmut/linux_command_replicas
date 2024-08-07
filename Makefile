MYDIR=./commands
BIN=./bin/

build: $(MYDIR)/*.c
	@for file in $(MYDIR)/*.c ; do \
		if [ -f "$$file" ]; then \
			gcc -o $(BIN)$$(basename $$file .c) $$file; \
		fi \
	done
