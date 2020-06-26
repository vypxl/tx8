NAME=TX8
OUT_DIR=out
SOURCES=src/cpu.c src/debug.c
CC_OPTS=-std=c11 -Wall -Wextra -Werror
LINK_OPTS=-lc -lm
OUT_LIB=$(OUT_DIR)/tx8-core.so

TEST_SOURCES=test/main.c
OUT_TEST=$(OUT_DIR)/tx8-core.test

.PHONY: default
default: build

.PHONY: gen_compile_commands
gen_compile_commands: mk_out_dir
	-clang -c -MJ compile_commands.json $(CC_OPTS) $(SOURCES) $(TEST_SOURCES)
	@sed -i -e '1s/^/[\n/' -e '$$s/,$$/\n]/' compile_commands.json
	@rm *.o

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
	clang $(CC_OPTS) -shared $(LINK_OPTS) -o $(OUT_LIB) $(SOURCES)

mk_out_dir:
	@mkdir -p out

.PHONY: test
test: build
	@clang $(CC_OPTS) -o $(OUT_TEST) $(OUT_LIB) $(TEST_SOURCES)
	@$(OUT_TEST)
