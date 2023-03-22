#include "deque.h"

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
DequeNode createDequeNode(void *restrict data) {
    DequeNode ptr = (DequeNode) malloc(sizeof(deque_node));
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
Deque initialDeque(void) {
    Deque deque = (Deque) malloc(sizeof(deque_define));
    if (!deque) {
        atexit(errorHint);
    }
    deque->mutex = true;
    deque->size = 0;
    deque->head = NULL;
    deque->rear = NULL;
    return deque;
}

/**
 * @brief 往双端队列尾部添加一个新的节点, 这个过程为线程安全
 * @param deque 双端队列实例
 * @param dequeNode 新的节点
 */
void add(Deque deque, DequeNode dequeNode) {
    for (;;) {
        if (deque != NULL) {
            if (deque->mutex) {
                deque->mutex = false;
                if (deque->head == NULL) {
                    deque->head = dequeNode;
                    deque->rear = dequeNode;
                } else {
                    dequeNode->prev = deque->rear;
                    deque->rear->next = dequeNode;
                    deque->rear = dequeNode;
                }
                ++deque->size;
                deque->mutex = true;
                return;
            }
        } else {
            return;
        }
    }
}

/**
 * @brief 销毁双端队列实例
 * @param deque 双端队列实例
 */
void releaseDeque(Deque deque) {
    if (deque != NULL) {
        DequeNode current = deque->head;
        while (NULL != current) {
            DequeNode tmp = current;
            current = current->next;
            cleanupDequeNode(tmp);
        }
        free(deque);
    }
}

/**
 * 清理节点
 * @param dequeNode 双端节点
 */
void cleanupDequeNode(DequeNode restrict dequeNode) {
    if (dequeNode != NULL) {
        free(dequeNode->data);
        free(dequeNode);
    }
}

/**
 * 移除队首元素并返回
 * @param deque 双端队列实例
 * @return 队首节点或者 `NULL`
 */
DequeNode poll(Deque restrict deque) {
    for (;;) {
        if (deque != NULL) {
            if (deque->mutex) {
                deque->mutex = false;
                DequeNode headNode = NULL;

                if (deque->head != NULL) {
                    if (deque->head != deque->rear) {
                        headNode = deque->head;
                        deque->head = deque->head->next;
                        deque->head->prev = NULL;
                    } else {
                        // 重设头尾节点
                        headNode = deque->head;
                        deque->head = deque->rear = NULL;
                    }
                    --deque->size;
                }

                deque->mutex = true;
                return headNode;
            }
        } else {
            return NULL;
        }
    }
}

/**
 * 取得队首元素(这个方法不会删除队首节点)
 * @param deque  双端队列实例
 * @return 队首元素或者 `NULL`
 */
DequeNode peek(Deque restrict deque) {
    for (;;) {
        if (deque != NULL) {
            if (deque->mutex) {
                deque->mutex = false;
                DequeNode choseHead = NULL;

                if (deque->head != NULL) {
                    choseHead = deque->head;
                }

                deque->mutex = true;
                return choseHead;
            }
        } else {
            return NULL;
        }
    }
}

/**
 * 遍历双端队列
 * @param deque 双端队列示例
 * @param accept 消费函数
 */
void TraverseDeque(Deque restrict deque, void(*accept)(DequeNode restrict node)) {
    if (deque) {
        DequeNode restrict current = deque->head;
        while (current != NULL) {
            accept(current);
            current = current->next;
        }
    }
}

/**
 * 消费一个队列双端节点
 * @param deque 双端队列示例
 * @return `null_ptr`
 */
void *consumeDequeNode(void *restrict deque) {
    while (((Deque) deque)->size) {
        DequeNode headNode = poll(deque);
        printf("thread->%llu, current queue size: %d; -> current tmpNode queueNode address is: %p; data is: %d\n",
               pthread_self(),
               ((Deque) deque)->size, headNode, *(int *) headNode->data);
        cleanupDequeNode(headNode);
    }
    return NULL;
}

/**
 * @brief 测试用例
 */
void createDequeAndPthreadTest(void) {
    Deque deque = initialDeque();
    for (int i = 1; i <= 10; ++i) {
        int *restrict data = calloc(1, sizeof(int));
        *data = i;
        DequeNode restrict tmpNode = createDequeNode(data);
        add(deque, tmpNode);
    }

    DequeNode head = peek(deque);
    if (head) {
        printf("the current queue size is: %d, head value is: %d\n", deque->size, *(int *) head->data);
    }
    printf("--------------------------------------------------\n");

    pthread_t id1, id2, id3;
    pthread_create(&id1, NULL, consumeDequeNode, deque);
    pthread_create(&id2, NULL, consumeDequeNode, deque);
    pthread_create(&id3, NULL, consumeDequeNode, deque);

    void *result;
    pthread_join(id1, &result);
    pthread_join(id2, &result);
    pthread_join(id3, &result);

    releaseDeque(deque);
    printf_s("everything done!\n");
}

/**
 *节点消费者测试函数
 * @param node 节点实例
 */
void tmpNodeAcceptor(DequeNode restrict node) {
    printf("current value is: %d\n", *(int *) node->data);
}

/**
 * 测试遍历双端队列
 */
void testTraverse(void) {
    Deque deque = initialDeque();
    for (int i = 1; i <= 10; ++i) {
        int *restrict data = calloc(1, sizeof(int));
        *data = i;
        DequeNode restrict tmpNode = createDequeNode(data);
        add(deque, tmpNode);
    }
    TraverseDeque(deque, tmpNodeAcceptor);
    releaseDeque(deque);
}
