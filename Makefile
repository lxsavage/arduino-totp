CC     = clang
CC_FLAG= -std=c99 -Wall -Werror

all: dist/unixsync dist/codeset

dist/unixsync: utils/unixsync.c
	@mkdir -p dist/
	${CC} ${CC_FLAG} -o $@ $^

dist/codeset: utils/codeset.c
	@mkdir -p dist/
	${CC} ${CC_FLAG} -o $@ $^
