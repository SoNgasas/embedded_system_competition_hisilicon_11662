#ifndef BASKET_H
#define BASKET_H

typedef struct Basket {  
    char item[10];  
    int weight;
    char datetime[64];
} Basket;  
  
typedef struct BasketNode {  
    Basket basket;  
    struct BasketNode *next;  
} BasketNode;  

BasketNode* createBasketNode(const char *itemName, int weight,char *datetime);
void addToBasketList(BasketNode **head, const char *itemName, int weight,char *datetime);
void modifyBasketWeight(BasketNode **head, const char *itemName, int newWeight);
void deleteBasketNode(BasketNode **head, const char *itemName);
void printBasketList(BasketNode *head);
int queryBasketWeight(BasketNode *head, const char *itemName);
void freeBasketList(BasketNode *head);
BasketNode* deleteLightBaskets(BasketNode *head);
extern BasketNode *basketList;
#endif