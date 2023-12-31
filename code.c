#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t table_semaphore, pen_sem, paper_sem, question_paper_sem, student_queue, teacher_queue, counter_mutex;

int student_count = 0, teacher_count = 0;

void* student(void* arg) {
    int id = *((int*)arg);
    printf("Student %d has entered.\n", id);
    if (id == 1) {
        sem_post(&pen_sem);
    } else if (id == 2) {
        sem_post(&paper_sem);
    } else {
        sem_post(&question_paper_sem);
    }
    sem_wait(&table_semaphore);  
    if (id == 1) {
        sem_wait(&paper_sem);
    } else if (id == 2) {
        sem_wait(&question_paper_sem);
    } else {
        sem_wait(&pen_sem);
    }

    printf("Student %d is working on the assignment.\n", id);
    sem_post(&table_semaphore);

    printf("Student %d has completed the assignment.\n", id);
}

void* teacher(void* arg) {
    int id = *((int*)arg);
    printf("Teacher %d has entered.\n", id);

    sem_wait(&counter_mutex);
    teacher_count++;
    if (student_count > 0) {
        student_count--;
        sem_post(&student_queue);
    } else {
        sem_post(&teacher_queue);
    }
    sem_post(&counter_mutex);

    sem_wait(&teacher_queue);  

    printf("Teacher %d is issuing books.\n", id);

    sem_post(&teacher_queue);  
}

int main() {
    pthread_t teacher_threads[2], student_threads[3];
    int teacher_ids[2] = {1, 2};
    int student_ids[3] = {1, 2, 3};

    sem_init(&table_semaphore, 0, 1);
    sem_init(&pen_sem, 0, 0);
    sem_init(&paper_sem, 0, 0);
    sem_init(&question_paper_sem, 0, 0);
    sem_init(&student_queue, 0, 0);
    sem_init(&teacher_queue, 0, 0);
    sem_init(&counter_mutex, 0, 1);

    for (int i = 0; i < 2; i++) {
        pthread_create(&teacher_threads[i], NULL, teacher, &teacher_ids[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_create(&student_threads[i], NULL, student, &student_ids[i]);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(teacher_threads[i], NULL);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(student_threads[i], NULL);
    }

    return 0;
}
