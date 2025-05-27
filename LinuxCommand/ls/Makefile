src_main=main.c

src_lib=$(wildcard ./src/*.c)
sub_lib=$(patsubst %.c, %.o, $(src_lib))
#target_lib=./lib/libsort.so
target_lib=./lib/libsort.a

target=main

$(target): $(src_main) $(target_lib)
	@mkdir -p lib
	$(CC) $(src_main) -l sort -L lib -I include -o $(target)

$(target_lib): $(sub_lib)
	@mkdir -p lib
	#$(CC) -shared -o $(target_lib)  $(sub_lib)
	ar rcs $(target_lib) $(sub_lib)

%.o:%.c
	$(CC) -c  $< -o $@ -I include

clean:
	rm -rf main lib
	rmdir lib 2>/dev/null || true

.PHONY: all clean