#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>
#include <basket.h>
BasketNode *basketList = NULL; 
// 创建一个新的篮子节点  
BasketNode* createBasketNode(const char *itemName, int weight, char *datetime) {  
    BasketNode *newNode = (BasketNode*)malloc(sizeof(BasketNode));  
    if (!newNode) return NULL;  
    strncpy(newNode->basket.item, itemName, sizeof(newNode->basket.item) - 1);  
    newNode->basket.item[sizeof(newNode->basket.item) - 1] = '\0';  
    strncpy(newNode->basket.datetime, datetime, sizeof(newNode->basket.datetime) - 1);
    newNode->basket.datetime[sizeof(newNode->basket.datetime) - 1] = '\0';
    newNode->basket.weight = weight;  
    newNode->next = NULL;  
    return newNode;  
}  
  
// 添加到链表中（简单实现，不检查重复）  
void addToBasketList(BasketNode **head, const char *itemName, int weight,char *datetime) {  
    BasketNode *newNode = createBasketNode(itemName, weight,datetime);  
    if (!newNode) return;  
    newNode->next = *head;  
    *head = newNode;  
}  
  
// 修改链表中指定元素的重量（如果找到）  
void modifyBasketWeight(BasketNode **head, const char *itemName, int newWeight) {  
    BasketNode *current = *head;  
    while (current != NULL) {  
        if (strcmp(current->basket.item, itemName) == 0) {  
            current->basket.weight = newWeight;  
            if (newWeight == 0) { // 如果重量为0，则考虑删除节点（此处不实际删除，仅注释）  
                // 需要额外的逻辑来删除节点，因为简单链表不支持直接删除中间节点而不破坏链表  
            }  
            return;  
        }  
        current = current->next;  
    }  
    // 如果没找到，可以选择在这里添加一个新节点  
    // addToBasketList(head, itemName, newWeight);  
}  
  
// 删除链表中指定元素的节点（如果找到且重量为0）  
void deleteBasketNode(BasketNode **head, const char *itemName) {  
    BasketNode *current = *head;  
    BasketNode *prev = NULL;  
  
    while (current != NULL) {  
        if (strcmp(current->basket.item, itemName) == 0 && current->basket.weight == 0) {  
            if (prev) {  
                prev->next = current->next;  
            } else {  
                *head = current->next;  
            }  
            free(current);  
            return;  
        }  
        prev = current;  
        current = current->next;  
    }  
}  
  
// 遍历链表并打印内容  
void printBasketList(BasketNode *head) {  
    while (head != NULL) {  
        printf("Item: %s, Weight: %d ,Datetime: %s\n", head->basket.item, head->basket.weight,head->basket.datetime);  
        head = head->next;  
    }  
}  
  
// 释放链表占用的内存  
void freeBasketList(BasketNode *head) {  
    while (head != NULL) {  
        BasketNode *temp = head;  
        head = head->next;  
        free(temp);  
    }  
}  
  
// 查询链表中指定元素的重量  
int queryBasketWeight(BasketNode *head, const char *itemName) {  
    while (head != NULL) {  
        if (strcmp(head->basket.item, itemName) == 0) {  
            return head->basket.weight;  
        }  
        head = head->next;  
    }  
    // 如果没有找到，返回-1或其他错误码  
    return -1;  
}  

BasketNode* deleteLightBaskets(BasketNode *head) {
    BasketNode dummy;
    dummy.next = head;
    BasketNode *prev = &dummy, *current = head;

    while (current != NULL) {
        if (current->basket.weight < 10) {
            prev->next = current->next;
            free(current); // 释放内存
            current = prev->next; // 继续向后遍历
        } else {
            prev = current;
            current = current->next;
        }
    }
    return dummy.next;
}