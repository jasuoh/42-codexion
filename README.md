# 42-codexion
Race against time in this thrilling concurrency challenge! Orchestrate multiple coders competing for limited USB dongles using POSIX threads, mutexes, and smart scheduling—master resource synchronization before burnout strikes.


What is a Thread:
Its like lightweight works inside a single programm (main function).
They share the same memory (vars, funcs)
Run independently, do multiple stuff at once.

Example analogy:
Process = seperate building
Thread = rooms inside the same building

Thread has unique stuff:
- Thread ID
- CPU Register
- Stack Ptr
- Priority
- Return Value
- Signal Mask ??

Pros:
- Lightweight and fast compared to processes
- Great for multi core cpus
- shared memory

Cons:
- shared memory


Mutexes:
Description:
Good Example is sharing a notebook with a friend and trying to write in it at the same time. It will just get 
messy and you probably overwrite and get problems. To avoid this only one person is writing at a time and then the other one.
Thats the idea behind mutexes for threads.



Pthread(Posix threads)

Notes:

Arguments for argc, argv:
number_of_coders > 0
time_to_burnout >= 0
time_to_compile >= 0
time_to_debug >= 0
time_to_refactor >= 0
number_of_compiles_required > 0
dongle_cooldown >= 0
scheduler - decision who choose the dongle if number_of_coders > 1
    fifo - first in first out == arrival order
    edf - earlierst deadline first scheduling == decision (https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
    => heapq (priority)

monitor thread - use mutex to protect state
    logging example:
    0 1 has taken a dongle. 
    timestamp_in_ms(current) X(coder number) has taken a dongle
    https://en.wikipedia.org/wiki/Monitor_(synchronization)
    

Allowed functions:
pthread_create - each coder one thread
pthread_join


pthread_mutex_init - proecting dongle state with pthread_muted_t, pthread_cond_t manage waiting queues 
https://en.wikipedia.org/wiki/Lock_(computer_science)

lock the mutex then do the function and then unlock so only one thread is kinda working on one var, func
pthread_mutex_lock 
pthread_mutex_unlock
pthread_mutex_destroy

pthread_cond_init 
https://www.geeksforgeeks.org/operating-systems/race-condition-in-operating-systems/
https://www.geeksforgeeks.org/operating-systems/race-condition-vulnerability/
pthread_cont_wait
pthread_cond_timedwait
pthread_cond_broadcast
pthread_cond_destroy

gettimeoftheday
usleep
write
malloc
free 
printf 
fprintf - writing in a fd (bsp file) instead of printing
strcmp
strlen
atoi
memset



Compile with the following flag: -pthread
- pthread_create(pthread_t *thread, const pthread_attr_t *attr (NULL usually), *start_routine function, void *arg for start_routine)
    Return: 
    0 on success, else error code number
    Example: 
        sult_code = pthread_create(&threads[i], NULL, perform_work, &thread_args[i])
-   pthread_join(pthread_t thread, void **retval)
        functions wait to the specified thread to terminate, 
    https://www.delftstack.com/howto/c/pthread_join-return-value-in-c/ 
    


Race conditions:
- 

Sources for README.md:
- https://en.wikipedia.org/wiki/Pthreads (Example Introduction into threads using the pthreads lib)
- https://en.wikipedia.org/wiki/POSIX (Interesting article about the Creators behind the POSIX (Portable Operating System Interface IEEE))

TODO:
- https://dev.to/emanuelgustafzon/mastering-concurrency-in-c-with-pthreads-a-comprehensive-guide-56je 
- https://randu.org/tutorials/threads/
- https://www.geeksforgeeks.org/c/thread-functions-in-c-c/

-  https://web.stanford.edu/class/archive/cs/cs110/cs110.1202/static/lectures/11-multithreading-cvs-semaphores.pdf 
    gutes pdf zu multithreading und mutexes zur prevetion von race conds
- https://en.wikipedia.org/wiki/Dining_philosophers_problem
- https://www.geeksforgeeks.org/operating-systems/dining-philosophers-problem/
- https://dev.to/vivekyadav200988/mastering-multithreading-in-c-programming-a-deep-dive-with-in-depth-explanations-and-advanced-concepts-245g
- https://www.codewithc.in/tutorials/multithreading
- https://www.baeldung.com/java-dining-philoshophers
some yt videos i came across my jorney of research;
- https://www.youtube.com/watch?v=LOfGJcVnvAk
- https://www.youtube.com/watch?v=ldJ8WGZVXZk
- https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2 
- https://www.youtube.com/watch?v=FY9livorrJI&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2&index=4
- https://www.youtube.com/watch?v=K1aoimUYTK8
- https://www.youtube.com/watch?v=oq29KUy29iQ&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2&index=5
- https://www.youtube.com/watch?v=raLCgPK-Igc&t=424s


check for leaks at macos: leaks -atExit -- ./a.out