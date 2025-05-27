CC=g++
target=ThreadPool
src_main=./src/main.cc

src_lib=$(wildcard ./src/*.cc)
sub_lib=$(patsubst %.cc, %.o, $(src_lib))
target_lib=./lib/libThreadPool.a

$(target): $(src_main) $(target_lib)
	$(CC) $(src_main) -l ThreadPool -L lib -I include -o $(target)

$(target_lib): $(sub_lib)
	mkdir -p lib
	ar crsv $(target_lib) $(sub_lib)

%.o:%.cc
	$(CC) -c $< -o $@ -I include

clean:
	rm -rf `find . -name "*.o"`
