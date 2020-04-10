all: buddy_test

buddy_test: buddySystem.o buddyTest.o
	gcc -o $@ $^

%.o: %.c
	gcc -c $^

clean:
	rm -rf *.o buddyTest
