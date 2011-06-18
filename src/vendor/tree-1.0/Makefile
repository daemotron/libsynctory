CFLAGS = -g -Wall -Wreturn-type -Werror

all : run

test : test.c tree.h .FORCE
	$(CC) $(CFLAGS) -o $@ $<

run : test test2
	./test > test.log
	-@if cmp test.out test.log; then echo "test: success"; else echo "test: FAILURE"; fi
	./test2 < test2.in > test2.log
	-@if cmp test2.out test2.log; then echo "test2: success"; else echo "test2: FAILURE"; fi

clean : .FORCE
	rm -f *~ test test2 *.log

.FORCE :
