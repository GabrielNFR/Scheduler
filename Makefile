scheduler: scheduler.c
	gcc -Wall -Wextra -o scheduler scheduler.c
clean:
	rm -f scheduler
	