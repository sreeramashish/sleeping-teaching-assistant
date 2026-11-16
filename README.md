# The Sleeping Teaching Assistant

Group 8 - CSE 302 - SRM University AP - November 2026

Instructor: Syed Sameen Ahmad Rizvi

## Problem statement

A teaching assistant helps one student at a time and sleeps when nobody needs help. Arriving students wake the TA, or wait in one of N hallway chairs while the TA is busy. If every chair is occupied, they leave and try again later. A mutex protects the chair count; semaphores signal waiting students and TA availability.

## Compile and run

```sh
gcc -Wall -pthread sleeping_ta.c -o sleeping_ta
./sleeping_ta 6
./sleeping_ta 10
```

The default is 6 students, 3 chairs and 2 helps per student. Use Linux with GCC and POSIX threads/semaphores. macOS does not support these unnamed semaphores without changes.

## Sample output

Excerpt from the supplied report (6 students); thread timing can vary.

```text
[TA] No students. TA is sleeping...
    [Student 1] Programming for a while...
    [Student 2] Programming for a while...
    [Student 3] Programming for a while...
    [Student 4] Programming for a while...
    [Student 5] Programming for a while...
    [Student 6] Programming for a while...
    [Student 2] Needs help. Sat on a chair (1/3 occupied).
[TA] Helping a student. Chairs occupied now: 0
    [Student 2] Getting help from the TA.
```

## Interactive site

Open index.html directly in a browser, or visit https://sreeramashish.github.io/sleeping-teaching-assistant/. No build tools or dependencies are required.

The JavaScript visualization models mutex-protected chair updates and the two semaphore signals, not native POSIX threads. It uses a finite class of six students, each requesting help twice; full-chair arrivals retry. C source is reproduced from Appendix A of the report, without fixes. The visualization shows one complete help session at a time; the C listing has independent TA/student sleeps and no help-completion handshake. Neither POSIX semaphore wake order nor this visualization promises FIFO fairness.
