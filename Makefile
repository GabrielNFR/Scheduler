scheduler: scheduler.c
	gcc -Wall -Wextra -o scheduler scheduler.c -lm
clean:
	rm -f scheduler
	