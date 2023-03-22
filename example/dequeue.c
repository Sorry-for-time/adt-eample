#include "dequeue.h"

/**
 * @brief 错误提示
 */
void errorHint(void) {
    printf("malloc can't not allocate memory\n");
}

/**
 * @brief 创建队列节点
 * @param data 数据
 * @return 队列节点
 */
DequeueNode createQueueNode(void *restrict data) {
    DequeueNode ptr = (DequeueNode) malloc(sizeof(dequeue_node));
    if (ptr == NULL) {
        atexit(errorHint);
    }
    ptr->data = data;
    ptr->next = NULL;
    ptr->prev = NULL;
    return ptr;
}

/**
 * @brief 初始化队列
 * @return 双端队列
 */
Dequeue initialDequeue(void) {
    Dequeue dequeue = (Dequeue) malloc(sizeof(dequeue_define));
    if (!dequeue) {
        atexit(errorHint);
    }
    dequeue->mutex = true;
    dequeue->size = 0;
    dequeue->head = NULL;
    dequeue->rear = NULL;
    return dequeue;
}

/**
 * @brief 往双端队列尾部添加一个新的节点, 这个过程为线程安全
 * @param dequeue 双端队列实例
 * @param dequeueNode 新的节点
 */
void add(Dequeue dequeue, DequeueNode dequeueNode) {
    for (;;) {
        if (dequeue != NULL) {
            if (dequeue->mutex) {
                dequeue->mutex = false;
                if (dequeue->head == NULL) {
                    dequeue->head = dequeueNode;
                    dequeue->rear = dequeueNode;
                } else {
                    dequeueNode->prev = dequeue->rear;
                    dequeue->rear->next = dequeueNode;
                    dequeue->rear = dequeueNode;
                }
                ++dequeue->size;
                dequeue->mutex = true;
                return;
            }
        } else {
            return;
        }
    }
}

/**
 * @brief 销毁双端队列实例
 * @param dequeue 双端队列实例
 */
void releaseDequeue(Dequeue dequeue) {
    if (dequeue != NULL) {
        DequeueNode current = dequeue->head;
        while (NULL != current) {
            DequeueNode tmp = current;
            current = current->next;
            free(tmp->data);
            free(tmp);
        }
        free(dequeue);
    }
}

/**
 * 清理节点
 * @param dequeueNode 双端节点
 */
void cleanupQueueNode(DequeueNode restrict dequeueNode) {
    if (dequeueNode != NULL) {
        free(dequeueNode->data);
        free(dequeueNode);
    }
}

/**
 * 取得队首元素(线程安全)
 * @param dequeue 双端队列实例
 * @return 首节点
 */
DequeueNode poll(Dequeue restrict dequeue) {
    for (;;) {
        if (dequeue != NULL) {
            if (dequeue->mutex) {
                dequeue->mutex = false;
                DequeueNode drawback = NULL;

                if (dequeue->head != NULL) {
                    if (dequeue->head != dequeue->rear) {
                        drawback = dequeue->head;
                        dequeue->head = dequeue->head->next;
                        dequeue->head->prev = NULL;
                    } else {
                        // 重设头尾节点
                        drawback = dequeue->head;
                        dequeue->head = dequeue->rear = NULL;
                    }
                    --dequeue->size;
                }

                dequeue->mutex = true;
                return drawback;
            }
        } else {
            return NULL;
        }
    }
}

/**
 * @brief 测试用例
 */
void createQueueTest(void) {
    Dequeue dequeue = initialDequeue();
    for (int i = 1; i <= 100; ++i) {
        int *restrict data = calloc(1, sizeof(int));
        *data = i;
        DequeueNode restrict tmpNode = createQueueNode(data);
        add(dequeue, tmpNode);
    }

    DequeueNode tmp = poll(dequeue);
    printf("current tmpNode queueNode address is: %p; data is: %d\n", tmp, *(int *) tmp->data);
    cleanupQueueNode(tmp);
    printf("the queue size is: %d\n", dequeue->size);

    DequeueNode current = dequeue->head;
    while (current != NULL) {
        int d = *((int *) current->data);
        printf("current queueNode address is: %p; data domain value is: %d\n", current, d);
        current = current->next;
    }
    releaseDequeue(dequeue);
}
