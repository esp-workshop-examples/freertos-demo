# freertos-demo

This ESP-IDF project demonstrates the fundamentals of the following FreeRTOS primitives:
- [Tasks](https://freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/00-Tasks-and-co-routines)
- [Queues](https://freertos.org/Documentation/02-Kernel/02-Kernel-features/02-Queues-mutexes-and-semaphores/01-Queues)
- [Event groups](https://freertos.org/Documentation/02-Kernel/02-Kernel-features/06-Event-groups)

## Examples

Each example can be enabled/disabled in the menuconfig (`idf.py menuconfig`), under

> FreeRTOS Demo Configuration > Example Selection

The examples are **incomplete**, and you will need to fill in the sections marked `TODO` in the comments. A hint is provided!

### Tasks

The [task example](./main/examples/task.c) demonstrates how to create tasks. 

You will have to implement:
- Task creation at `task_create()`.

### Queue

The [queue example](./main/examples/queue.c) demonstrates how to use a queue to pass data between tasks. 

You will have to implement:
- Queue data sending at `queue_send_and_block_if_full()`.
- Queue data receiving at `queue_receive_and_block_if_empty()`.

### Event Group

The [event group example](./main/examples/event_group.c) demonstrates how to use an event group to synchronize two tasks. 

You will have to implement:
- Event group flag setting at `event_group_set()`.
- Event group flag waiting at `event_group_wait()`.