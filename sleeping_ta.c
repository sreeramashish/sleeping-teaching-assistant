/*
 * Sleeping Teaching Assistant - POSIX threads, mutex lock, semaphores
 * Compile: gcc -Wall -pthread sleeping_ta.c -o sleeping_ta
 * Run:     ./sleeping_ta [number_of_students]   (default 6)
 *
 * Idea:
 * - students_sem : counts students waiting in hallway chairs (TA sleeps on it when 0)
 * - ta_sem        : TA signals "I am ready" to ONE waiting student
 * - chairs_mutex : protects the shared counter waiting_students
 * - 3 chairs in the hallway; if all taken, student leaves and comes back later
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_CHAIRS 3
#define HELPS_PER_STUDENT 2          /* each student wants help this many times */

sem_t students_sem;                  /* waiting students (TA sleeps when 0)   */
sem_t ta_sem;                        /* TA available to help one student      */
pthread_mutex_t chairs_mutex = PTHREAD_MUTEX_INITIALIZER;

int waiting_students = 0;            /* occupied chairs (shared, mutex-guarded) */
int total_students = 6;
int students_done = 0;               /* students who got all their help          */

static void pause_random(int max_sec) {         /* sleep 1..max_sec seconds */
    sleep(1 + rand() % max_sec);
}

void *ta_thread(void *arg) {
    (void)arg;
    while (1) {
        printf("[TA] No students. TA is sleeping...\n");
        sem_wait(&students_sem);                /* sleep until a student arrives */

        /* a student is waiting (or woke us): take one off the chair */
        pthread_mutex_lock(&chairs_mutex);
        if (students_done >= total_students) { /* shutdown signal */
            pthread_mutex_unlock(&chairs_mutex);
            break;
        }
        if (waiting_students > 0) waiting_students--;
        int left = waiting_students;
        pthread_mutex_unlock(&chairs_mutex);

        sem_post(&ta_sem);                      /* call the student in */
        printf("[TA] Helping a student. Chairs occupied now: %d\n", left);
        pause_random(3);                        /* helping takes time */
        printf("[TA] Finished helping a student.\n");
    }
    printf("[TA] All students done. TA goes home.\n");
    return NULL;
}

void *student_thread(void *arg) {
    int id = *(int *)arg;
    int helped = 0;

    while (helped < HELPS_PER_STUDENT) {
        printf("    [Student %d] Programming for a while...\n", id);
        pause_random(5);

        pthread_mutex_lock(&chairs_mutex);
        if (waiting_students < NUM_CHAIRS) {
            waiting_students++;
            printf("    [Student %d] Needs help. Sat on a chair (%d/%d occupied).\n",
                   id, waiting_students, NUM_CHAIRS);
            pthread_mutex_unlock(&chairs_mutex);

            sem_post(&students_sem);            /* tell/wake the TA        */
            sem_wait(&ta_sem);                  /* wait until TA is ready */
            printf("    [Student %d] Getting help from the TA.\n", id);
            pause_random(3);
            helped++;
            printf("    [Student %d] Got help (%d/%d).\n", id, helped, HELPS_PER_STUDENT);
        } else {
            pthread_mutex_unlock(&chairs_mutex);
            printf("    [Student %d] No free chair! Will come back later.\n", id);
        }
    }

    pthread_mutex_lock(&chairs_mutex);
    students_done++;
    int all_done = (students_done >= total_students);
    pthread_mutex_unlock(&chairs_mutex);
    if (all_done) sem_post(&students_sem);      /* wake sleeping TA so it can exit */
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc > 1) total_students = atoi(argv[1]);
    if (total_students < 1) total_students = 1;
    srand(42);                                  /* change for different runs */

    sem_init(&students_sem, 0, 0);
    sem_init(&ta_sem, 0, 0);

    pthread_t ta;
    pthread_t *students = malloc(sizeof(pthread_t) * total_students);
    int *ids = malloc(sizeof(int) * total_students);

    pthread_create(&ta, NULL, ta_thread, NULL);
    for (int i = 0; i < total_students; i++) {
        ids[i] = i + 1;
        pthread_create(&students[i], NULL, student_thread, &ids[i]);
    }
    for (int i = 0; i < total_students; i++) pthread_join(students[i], NULL);
    pthread_join(ta, NULL);

    sem_destroy(&students_sem);
    sem_destroy(&ta_sem);
    pthread_mutex_destroy(&chairs_mutex);
    free(students); free(ids);
    printf("Simulation finished.\n");
    return 0;
}
