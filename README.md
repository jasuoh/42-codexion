*This project has been created as part of the 42 curriculum by jawosylu.*

# Codexion

> Race against time in this thrilling concurrency challenge! Orchestrate multiple coders competing for limited USB dongles using POSIX threads, mutexes, and smart scheduling — master resource synchronization before burnout strikes.

## Description

Codexion is a variation of the classic **dining philosophers problem**. Several coders sit around a circular table with a shared Quantum Compiler in the middle. Between every two coders lies one USB dongle, so there are as many dongles as coders. To compile, a coder needs **both** dongles next to them at the same time.

Every coder repeats the same cycle:

```
take two dongles -> compile -> put them back -> debug -> refactor -> ...
```

If a coder does not **start** compiling within `time_to_burnout` ms after the start of their last compile (or the start of the simulation), they **burn out** and the simulation ends. It also ends once every coder has compiled at least `number_of_compiles_required` times.

The goal is to share a limited resource between threads **without deadlocks, without starvation and without data races**, while respecting:

- a **cooldown** after a dongle is released,
- a **fair scheduler** (`fifo` or `edf`) that decides who gets a dongle,
- a **monitor thread** that reports a burnout within 10 ms.

Each coder is a POSIX thread, each dongle is protected by a mutex, and waiting coders sleep on a condition variable.

## Instructions

### Build

```bash
cd coders
make        # builds ./codexion
make clean  # removes object files
make fclean # removes object files and the binary
make re     # full rebuild
```

The project is compiled with `cc -Wall -Wextra -Werror -pthread`.

### Run

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Meaning | Valid values |
|---|---|---|
| `number_of_coders` | coders = dongles | integer > 0 |
| `time_to_burnout` | ms a coder survives without starting to compile | integer >= 0 |
| `time_to_compile` | ms spent compiling (holding 2 dongles) | integer >= 0 |
| `time_to_debug` | ms spent debugging | integer >= 0 |
| `time_to_refactor` | ms spent refactoring | integer >= 0 |
| `number_of_compiles_required` | the simulation stops once everyone compiled this often | integer > 0 |
| `dongle_cooldown` | ms a released dongle stays unavailable | integer >= 0 |
| `scheduler` | arbitration policy | `fifo` or `edf` |

Invalid input (negative numbers, non-digits, values above `INT_MAX`, wrong scheduler, wrong argument count) is rejected with an error message on stderr.

### Output

Every state change is printed as `timestamp_in_ms coder_id action`:

```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
300 1 is refactoring
...
800 3 burned out
```

### Examples

```bash
./codexion 5 800 200 100 100 3 0 fifo   # nobody burns out, stops after 3 compiles each
./codexion 4 410 200 100 100 5 0 edf    # tight but feasible, nobody burns out
./codexion 4 310 200 100 100 5 0 fifo   # not feasible -> "310 X burned out"
./codexion 1 800 200 100 100 3 0 fifo   # one dongle only -> "800 1 burned out"
```

### Checking for leaks and races

```bash
valgrind --leak-check=full ./codexion 5 800 200 100 100 3 0 fifo
cc -Wall -Wextra -Werror -pthread -g -fsanitize=thread src/*.c -o codexion_tsan
setarch -R ./codexion_tsan 5 800 200 100 100 3 0 edf   # setarch: needed on newer Linux kernels
# macOS: leaks -atExit -- ./codexion 5 800 200 100 100 3 0 fifo
```

Note: `valgrind --tool=helgrind` reports "pthread_cond_{signal,broadcast}: dubious: associated lock is not held" from **inside glibc's** `pthread_cond_timedwait`. This is a known helgrind false positive with modern glibc; ThreadSanitizer reports no races.

## Background

### Threads

A **thread** is a lightweight unit of execution inside a single program. All threads of a process share the same memory (global variables, heap, functions) and run independently, so they can do several things at once.

> Analogy: a **process** is a separate building, **threads** are the rooms inside the same building.

Every thread has its own:

- thread ID
- CPU registers and instruction pointer
- stack (and stack pointer)
- scheduling priority
- return value
- signal mask

| Pros | Cons |
|---|---|
| lightweight and fast compared to processes | shared memory: concurrent access can corrupt data |
| uses multiple CPU cores | bugs depend on timing and are hard to reproduce |
| easy data sharing through shared memory | needs explicit synchronization |

### Mutexes

> Analogy: two friends share one notebook. If both write at the same time, the text gets messy and one overwrites the other. So only one person writes at a time and the other waits.

A **mutex** (mutual exclusion lock) is exactly that rule for threads: lock it, work on the shared data, unlock it. Only one thread can hold a mutex at a time, all others block on `pthread_mutex_lock` until it is released.

### Condition variables

A mutex says *"only one at a time"*. A **condition variable** says *"sleep until something changed"*. A thread waiting on a condition variable releases its mutex atomically and sleeps; another thread wakes it up with `signal` or `broadcast`. This avoids *busy waiting* (asking "now? now? now?" in a loop and burning CPU time).

### A bit of history

- **1965** — Edsger W. Dijkstra publishes *Solution of a Problem in Concurrent Programming Control*, the first solution to the **mutual exclusion** problem. In the same year he gives his students an exam exercise about five computers competing for shared tape drives; Tony Hoare later retells it as the **dining philosophers**. Codexion is a direct descendant of it.
- **1964** — J. W. J. Williams introduces the **binary heap** as part of heapsort, the data structure behind the scheduler queues here.
- **1971** — Coffman, Elphick and Shoshani describe the four conditions that must **all** hold for a deadlock (*System Deadlocks*), today known as the **Coffman conditions**.
- **1973** — Liu and Layland analyze **Earliest Deadline First** scheduling for hard real-time systems.
- **1973/74** — Per Brinch Hansen and C. A. R. Hoare introduce **monitors**: a lock plus condition variables bundled around shared data. Mutex + `pthread_cond_t` is the low-level version of this idea.
- **1988** — The first **POSIX** standard (*Portable Operating System Interface*, IEEE 1003.1) is published to make Unix-like systems compatible. The name was suggested by Richard Stallman.
- **1995** — **POSIX threads** (IEEE Std 1003.1c-1995) standardize the `pthread_*` API used in this project. Before that, every Unix vendor had its own thread library.

### The pthread functions used

| Function | What it does |
|---|---|
| `pthread_create(&thread, NULL, routine, arg)` | starts a new thread running `routine(arg)`. Returns **0 on success**, an error number otherwise. The thread starts immediately, so everything it needs must be ready before. |
| `pthread_join(thread, NULL)` | waits until the thread has finished. Without it, `main` could free memory the threads still use. |
| `pthread_mutex_init / _destroy` | creates / destroys a mutex. Destroy only after no thread uses it anymore. |
| `pthread_mutex_lock / _unlock` | enters / leaves the critical section. Every path out of a locked section must unlock, and a normal mutex must never be locked twice by the same thread (self deadlock). |
| `pthread_cond_init / _destroy` | creates / destroys a condition variable. |
| `pthread_cond_wait(&cond, &mutex)` | atomically releases `mutex` and sleeps; locks `mutex` again before returning. Always used inside a `while` loop because of spurious wakeups. |
| `pthread_cond_timedwait(&cond, &mutex, &abstime)` | same, but also wakes up at the **absolute** time `abstime` (`struct timespec`: seconds + nanoseconds). |
| `pthread_cond_broadcast(&cond)` | wakes **all** threads waiting on `cond`. |

Other allowed functions: `gettimeofday` (timestamps), `usleep` (sleeping in microseconds), `write`, `printf`, `malloc`, `free`, `memset`, `strcmp`, `strlen`.

## Technical choices

### Data structures

```
t_config   global settings, stop flag, ticket counter, print/sim mutexes
t_coder    thread, id, times_compiled, last_compile_start, state_mutex, left/right dongle
t_dongle   id, state (FREE/USED), available_at, mutex, cond, queue
t_heap     priority queue of t_request (array based binary min-heap)
t_request  coder, seq (ticket number), deadline
```

Coder `i` sits between dongle `i` and dongle `i + 1`, the last coder wraps around to dongle 1, so coder 1 sits next to coder `n`. With a single coder there is only one dongle and `right` is `NULL`.

### Scheduler: a heap per dongle

Every dongle owns a **binary min-heap** of waiting requests (the subject requires a priority queue, C has none in its standard library). The heap is an array; for index `i` the parent is `(i - 1) / 2` and the children are `2i + 1` and `2i + 2`. Push sifts the new element **up**, pop moves the last element to the root and sifts it **down**. The best request is always `items[0]`.

- **fifo**: smaller `seq` (earlier ticket) is better.
- **edf**: earlier `deadline = last_compile_start + time_to_burnout` is better, ties are broken by `seq`.

A dongle is only wanted by its two neighbours, so a heap never holds more than 2 requests (`HEAP_CAP 2`). The implementation is still generic and was tested with a capacity of 10.

### Taking two dongles at once

A coder builds **one** request and pushes the **same** request (same `seq`, same deadline) into the queues of **both** of its dongles. It takes the dongles only when it can take **both**, and then takes them together. A coder therefore never holds one dongle while waiting for the other.

### Passing a blocked coder (bypass rule)

A strict "only the first in both queues may take" rule creates **convoys**: a free dongle stays unused because the first in its queue is itself blocked by its other neighbour. With 4 coders this serialized the whole table at start (`4 410 200 100 100` burned out although it is feasible).

So a coder may take a free dongle even if it is not first in that queue, **but only if** the first one still has enough time:

```
deadline_of_first > now + time_to_compile + dongle_cooldown + BYPASS_MARGIN
```

The passing coder gives the dongle back and its cooldown is over before the first one's deadline, so the first one loses nothing. As that deadline gets closer, nobody may pass anymore and the waiting coder is served next. `BYPASS_MARGIN` (2 ms) covers the millisecond truncation of `gettimeofday` and the 1 ms wake-up interval.

## Blocking cases handled

### Deadlock (Coffman conditions)

A deadlock needs all four Coffman conditions at once:

1. **Mutual exclusion**: a dongle can only be used by one coder. *Required by the subject.*
2. **Hold and wait**: holding one resource while waiting for another. **Broken**: a coder takes both dongles at once or none (`take_both_dongles`).
3. **No preemption**: nobody can take a dongle away from its holder. *Kept.*
4. **Circular wait**: a cycle of coders each waiting for the next. **Broken** for the mutexes: when two dongle mutexes are locked together, the lower dongle id is always locked first, so no cycle of lock requests can form.

### Starvation and liveness

- All waiting requests share one global order (same `seq`/deadline in both queues). The globally best request is first in both of its queues and is served as soon as its dongles are free.
- With **edf**, the coder closest to burning out is the most urgent one.
- The bypass rule keeps dongles in use without ever delaying a coder past its deadline (see above).

Tested 10 times each without a single burnout: `5 800 200 100 100 3 0` (fifo/edf), `4 410 200 100 100 5 0` (fifo/edf), `200 800 200 100 100 2 0` (fifo/edf), `5 800 200 100 100 5 50 edf`, `3 700 200 100 100 5 0 edf`. Infeasible settings such as `4 310 200 100 100` still burn out as expected.

### Dongle cooldown

`return_dongle` sets `available_at = now + dongle_cooldown`. `can_get_dongle` refuses a dongle until that time has passed. Since the end of a cooldown triggers no broadcast, waiting coders use `pthread_cond_timedwait` with a 1 ms timeout and recheck.

### Precise burnout detection

- The monitor checks every coder roughly every 0.5 ms, well within the required 10 ms.
- `last_compile_start` is updated at the **start** of compiling, before the coder sleeps.
- `custom_sleep` sleeps in 500 µs steps towards a fixed target time. Delays do not add up, and a sleeping coder notices the end of the simulation within ~0.5 ms instead of finishing a long `usleep`.

### Log serialization

- Every line is printed while holding `print_mutex`, so two lines never mix.
- The timestamp is taken **inside** the lock, so timestamps are always ascending.
- Once `stop` is set, `log_message` prints nothing.
- The monitor prints `burned out` **and** sets `stop` while holding `print_mutex`. No coder can print in between, and every later attempt sees `stop` and stays silent.

### Single coder

There is only one dongle, so compiling is impossible. The coder takes it, waits until the monitor reports the burnout at `time_to_burnout`, puts it back and ends.

## Thread synchronization mechanisms

| Primitive | Count | Protects / used for |
|---|---|---|
| `sim_mutex` | 1 | `stop` flag and `next_seq` ticket counter |
| `print_mutex` | 1 | standard output (one line at a time) |
| `state_mutex` | 1 per coder | `last_compile_start`, `times_compiled` (written by the coder, read by the monitor) |
| `dongle->mutex` | 1 per dongle | `state`, `available_at` and the request queue of that dongle |
| `dongle->cond` | 1 per dongle | waiting coders sleep here, `return_dongle` broadcasts |
| monitor thread | 1 | detects burnout / completion and sets `stop` |

### Lock order

- `print_mutex` -> `sim_mutex` (in `log_message` and `write_burnout`)
- lower dongle mutex -> higher dongle mutex (in `take_both_dongles`)
- `sim_mutex` is always the last lock taken: nobody locks another mutex while holding it.
- No mutex is ever held while sleeping or compiling.

### Examples of prevented race conditions

- **Two coders take the same dongle**: `state` is only checked and set to `USED` while holding `dongle->mutex`.
- **Duplicate ticket numbers**: reading and incrementing `next_seq` happen in **one** `sim_mutex` section. With two separate sections, two coders could both read 5.
- **Monitor reads a half-updated value**: the monitor copies `last_compile_start` / `times_compiled` under the coder's `state_mutex`, releases it, then compares.
- **Stop flag never seen**: `stop` is only read through `get_stop`. A plain read in a loop could be cached in a register by the compiler and is undefined behaviour while another thread writes it.
- **Lost wakeup**: `return_dongle` changes `state` and `available_at` **before** `pthread_cond_broadcast`, all under the same mutex. Waiting coders recheck their condition in a `while` loop.
- **Line printed after "burned out"**: see *Log serialization*.

### Thread-safe communication between coders and monitor

- Coders and monitor never talk directly.
- The monitor reads coder state through `state_mutex` and ends the simulation through `set_stop`.
- Every coder notices `stop`:
  - while sleeping: `custom_sleep`, within ~0.5 ms;
  - while waiting for dongles: `timedwait` loop, within ~1 ms;
  - before printing: `log_message`.

## Resources

### Threads, pthreads and POSIX

- [POSIX Threads (Wikipedia)](https://en.wikipedia.org/wiki/Pthreads) — introduction to threads with the pthreads library
- [POSIX (Wikipedia)](https://en.wikipedia.org/wiki/POSIX) — history and the people behind the standard
- [Mastering concurrency in C with pthreads](https://dev.to/emanuelgustafzon/mastering-concurrency-in-c-with-pthreads-a-comprehensive-guide-56je)
- [POSIX threads programming tutorial (randu.org)](https://randu.org/tutorials/threads/)
- [Thread functions in C (GeeksforGeeks)](https://www.geeksforgeeks.org/c/thread-functions-in-c-c/)
- [pthread_join return value (DelftStack)](https://www.delftstack.com/howto/c/pthread_join-return-value-in-c/)
- [Multithreading tutorial (codewithc)](https://www.codewithc.in/tutorials/multithreading)
- Linux man pages: `man pthread_create`, `man pthread_cond_timedwait`, `man gettimeofday`

### Synchronization, race conditions, deadlocks

- [Stanford CS110: multithreading, condition variables, semaphores (PDF)](https://web.stanford.edu/class/archive/cs/cs110/cs110.1202/static/lectures/11-multithreading-cvs-semaphores.pdf) — very good material on preventing race conditions
- [Lock (Wikipedia)](https://en.wikipedia.org/wiki/Lock_(computer_science))
- [Monitor (Wikipedia)](https://en.wikipedia.org/wiki/Monitor_(synchronization))
- [Race condition in operating systems (GeeksforGeeks)](https://www.geeksforgeeks.org/operating-systems/race-condition-in-operating-systems/)
- [Race condition vulnerability (GeeksforGeeks)](https://www.geeksforgeeks.org/operating-systems/race-condition-vulnerability/)
- [Condition wait and signal (GeeksforGeeks)](https://www.geeksforgeeks.org/linux-unix/condition-wait-signal-multi-threading/)
- [Deadlock and the Coffman conditions (Wikipedia)](https://en.wikipedia.org/wiki/Deadlock_(computer_science))

### Dining philosophers and scheduling

- [Dining philosophers problem (Wikipedia)](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Dining philosophers problem (GeeksforGeeks)](https://www.geeksforgeeks.org/operating-systems/dining-philosophers-problem/)
- [Dining philosophers in Java (Baeldung)](https://www.baeldung.com/java-dining-philoshophers)
- [Earliest deadline first scheduling (Wikipedia)](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- [Binary heap (Wikipedia)](https://en.wikipedia.org/wiki/Binary_heap)

### Tools

- [ThreadSanitizer (Clang docs)](https://clang.llvm.org/docs/ThreadSanitizer.html)

### Videos I came across during my research

- https://www.youtube.com/watch?v=LOfGJcVnvAk
- https://www.youtube.com/watch?v=ldJ8WGZVXZk
- https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2
- https://www.youtube.com/watch?v=FY9livorrJI&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2&index=4
- https://www.youtube.com/watch?v=K1aoimUYTK8
- https://www.youtube.com/watch?v=oq29KUy29iQ&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2&index=5
- https://www.youtube.com/watch?v=raLCgPK-Igc&t=424s

### How AI was used

I used Claude (Anthropic) as a tutor during this project:

- **Explanations**: threads, mutexes, condition variables, `timespec`, binary heaps, sift up / sift down, Coffman conditions, lock ordering, and how a monitor thread works.
- **Code review and debugging**: reviewing my own code step by step and pointing out bugs (unlocks missing on early returns, inverted conditions, copying a struct that contains a mutex, a self deadlock through a nested `print_mutex` lock, data races on `stop` and `times_compiled`).
- **Testing**: running valgrind, helgrind, ThreadSanitizer and repeated burnout tests, and analyzing the logs. This is how the convoy starvation was found.
- **Documentation**: the function comments in the source files and this README were drafted with AI from my notes and then reviewed by me.

All other code was written by me, with AI explaining concepts and reviewing.
