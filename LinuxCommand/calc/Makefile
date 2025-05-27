src_main=main.c
src_lib=$(wildcard src/*.c)
sub_lib=$(patsubst %.c, %.o, $(src_lib))
target_lib=lib/libexpr.so
target=main

all: $(target)

$(target): $(src_main) $(target_lib)
	@mkdir -p lib
	$(CC) $(src_main) -lexpr -lm -L lib -I include -fPIC -Wl,-rpath=./lib -o $(target)

$(target_lib): $(sub_lib)
	@mkdir -p lib
	$(CC) -shared -o $(target_lib)  $(sub_lib)

%.o:%.c
	$(CC) -c -fpic $< -o $@ -I include -lm

clean:
	rm -f $(target) $(sub_lib) $(target_lib)
	rmdir lib 2>/dev/null || true

.PHONY: all clean