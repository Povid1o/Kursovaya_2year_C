#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Определение структуры узла очереди
typedef struct Node {
    uint8_t data;
    struct Node* next;
} Node;

// Определение структуры приоритетной очереди
typedef struct Queue {
    Node** data;       // Массив указателей на узлы для каждого приоритета
    int* counts;       // Массив счётчиков элементов для каждого приоритета
    int max_prio;
    int min_prio;
} Queue;

// Инициализация очереди
Queue* init_queue(int max_prio) {
    if (max_prio <= 0) {
        fprintf(stderr, "Максимальный приоритет должен быть положительным.\n");
        return NULL;
    }

    Queue* q = calloc(1, sizeof(Queue));
    if (!q) {
        perror("Ошибка выделения памяти для очереди");
        return NULL;
    }

    q->max_prio = max_prio;
    q->min_prio = max_prio - 1; // Изначально устанавливаем на самый низкий приоритет
    q->data = calloc(max_prio, sizeof(Node*));
    q->counts = calloc(max_prio, sizeof(int));

    if (!q->data || !q->counts) {
        perror("Ошибка выделения памяти для очереди");
        free(q->data);
        free(q->counts);
        free(q);
        return NULL;
    }

    return q;
}

// Добавление элемента в очередь с заданным приоритетом
int enqueue(Queue* q, uint8_t value, int prio) {
    if (!q) {
        fprintf(stderr, "Очередь не инициализирована.\n");
        return -1;
    }

    if (prio < 0 || prio >= q->max_prio) {
        fprintf(stderr, "Недопустимый приоритет: %d\n", prio);
        return -1;
    }

    Node* new_node = malloc(sizeof(Node));
    if (!new_node) {
        perror("Ошибка выделения памяти для нового узла");
        return -1;
    }

    new_node->data = value;
    new_node->next = NULL;

    // Добавление в конец списка приоритета
    if (!q->data[prio]) {
        q->data[prio] = new_node;
    } else {
        Node* temp = q->data[prio];
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = new_node;
    }

    q->counts[prio]++;
    if (prio < q->min_prio) {
        q->min_prio = prio;
    }

    return 0;
}

// Извлечение элемента из очереди
int dequeue(Queue* q, uint8_t* value) {
    if (!q) {
        fprintf(stderr, "Очередь не инициализирована.\n");
        return -1;
    }

    for (int prio = q->min_prio; prio < q->max_prio; prio++) {
        if (q->counts[prio] > 0) {
            Node* node = q->data[prio];
            if (node) {
                *value = node->data;
                q->data[prio] = node->next;
                free(node);
                q->counts[prio]--;

                // Обновление минимального приоритета
                if (q->counts[prio] == 0 && prio == q->min_prio) {
                    for (int i = prio + 1; i < q->max_prio; i++) {
                        if (q->counts[i] > 0) {
                            q->min_prio = i;
                            break;
                        }
                        if (i == q->max_prio - 1) {
                            q->min_prio = q->max_prio - 1;
                        }
                    }
                }

                return 0;
            }
        }
    }

    printf("Очередь пуста.\n");
    return -1;
}

// Освобождение памяти, занятой очередью
void free_queue(Queue* q) {
    if (!q) return;

    for (int prio = 0; prio < q->max_prio; prio++) {
        Node* current = q->data[prio];
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(q->data);
    free(q->counts);
    free(q);
}

int main(void) {
    Queue* q = init_queue(4);
    if (!q) return EXIT_FAILURE;

    enqueue(q, 2, 3);
    enqueue(q, 10, 1);
    enqueue(q, 15, 1);
    enqueue(q, 20, 0);

    for (int i = 0; i < 4; i++) {
        uint8_t d;
        if (dequeue(q, &d) == 0) {
            printf("%d: %d\n", i, d);
        }
    }

    free_queue(q);
    return EXIT_SUCCESS;
}
