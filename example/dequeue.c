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
 * 移除队首元素并返回
 * @param dequeue 双端队列实例
 * @return 队首节点或者 `NULL`
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
 * 取得队首元素(这个方法不会删除队首节点)
 * @param dequeue  双端队列实例
 * @return 队首元素或者 `NULL`
 */
DequeueNode peek(Dequeue restrict dequeue) {
    for (;;) {
        if (dequeue != NULL) {
            if (dequeue->mutex) {
                dequeue->mutex = false;
                DequeueNode drawback = NULL;

                if (dequeue->head != NULL) {
                    drawback = dequeue->head;
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
 * 消费一个队列双端节点
 * @param dequeue 双端队列示例
 * @return `null_ptr`
 */
void *consumeQueueNode(void *restrict dequeue) {
    while (((Dequeue) dequeue)->size) {
        DequeueNode headNode = poll(dequeue);
        printf("thread->%llu, current queue size: %d; -> current tmpNode queueNode address is: %p; data is: %d\n",
               pthread_self(),
               ((Dequeue) dequeue)->size, headNode, *(int *) headNode->data);
        cleanupQueueNode(headNode);
    }
    return NULL;
}

/**
 * @brief 测试用例
 */
void createQueueTest(void) {
    Dequeue dequeue = initialDequeue();
    for (int i = 1; i <= 10; ++i) {
        int *restrict data = calloc(1, sizeof(int));
        *data = i;
        DequeueNode restrict tmpNode = createQueueNode(data);
        add(dequeue, tmpNode);
    }

    DequeueNode head = peek(dequeue);
    if (head) {
        printf("the current queue size is: %d, head value is: %d\n", dequeue->size, *(int *) head->data);
    }
    printf("--------------------------------------------------\n");

    pthread_t id1, id2, id3;
    pthread_create(&id1, NULL, consumeQueueNode, dequeue);
    pthread_create(&id2, NULL, consumeQueueNode, dequeue);
    pthread_create(&id3, NULL, consumeQueueNode, dequeue);

    void *result;
    pthread_join(id1, &result);
    pthread_join(id2, &result);
    pthread_join(id3, &result);

    releaseDequeue(dequeue);
    printf_s("everything done!\n");
}
