NAME=TX8-asm
OUT_DIR=out
CC_OPTS=-std=c11 -I../src -Isrc -Wall -Wextra -Wno-implicit-function-declaration -Wno-unused-function -Wno-unneeded-internal-declaration
LINK_OPTS=
OUT_BIN=$(OUT_DIR)/tx8-asm
SOURCES=$(OUT_DIR)/tx8.tab.c $(OUT_DIR)/tx8.yy.c src/main.c src/assembler.c src/asm_types.c

TEST_SOURCES=test/main.c
OUT_TEST=$(OUT_DIR)/tx8-asm.test

.PHONY: default
default: build

.PHONY: format
format:
	@find src -iname *.h -o -iname *.c | xargs clang-format -i
	@find test -iname *.h -o -iname *.c | xargs clang-format -i

.PHONY: lint
lint:
	@find src -iname *.h -o -iname *.c -exec clang-tidy {} -- -Isrc \;
	@find test -iname *.h -o -iname *.c -exec clang-tidy {} -- -Isrc -Itest \;

.PHONY: build
build: mk_out_dir
	@flex -o $(OUT_DIR)/tx8.yy.c src/tx8.l
	@bison -o $(OUT_DIR)/tx8.tab.c --verbose -d src/tx8.y
	@clang $(CC_OPTS) $(LINK_OPTS) -o $(OUT_BIN) $(SOURCES)
	@rm $(OUT_DIR)/tx8.tab.c $(OUT_DIR)/tx8.tab.h $(OUT_DIR)/tx8.yy.c $(OUT_DIR)/tx8.output

mk_out_dir:
	@mkdir -p out

.PHONY: test
test: build
	@$(OUT_BIN) < test.tx8
