#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>

struct stNode {
    struct stNode *prev;
    struct stNode *next;
    uint32_t data;    
};

typedef struct stNode Node_t;

Node_t* create(void);
Node_t* setData(uint32_t data); 
Node_t* delete(Node_t* p);
Node_t* getHead(void);
Node_t* cleanup(Node_t* p);
void addlast(Node_t* head, uint32_t data);
void addfirst(Node_t* head, uint32_t data);
void printall(Node_t *head);
void insertAt(Node_t *head, uint32_t data, uint32_t pos);

Node_t* create(void)
{
    Node_t* ptr = malloc(sizeof(Node_t));
    if(NULL == ptr)
    {
        fprintf(stderr, "Malloc failed!\n");
    }
    ptr->prev = NULL;
    ptr->next = NULL;
    ptr->data = 0;
    return ptr;
}

Node_t* delete(Node_t* p)
{
    if(NULL == p)
    {
        fprintf(stderr, "Null pointer!\n");
        return p;
    }
    // Make before break--
    p->prev->next = p->next;
    p->next->prev = p->prev;
    p = cleanup(p); // No longer a dangling pointer
    return p;
}

Node_t* cleanup(Node_t* p)
{
    free(p);
    return NULL;
}

Node_t* getHead(void)
{
    Node_t* p = create();
    p->prev = p->next = NULL;
    return p;
}

void addfirst(Node_t* head, uint32_t data)
{
    Node_t *newNode = create();
    if(NULL == newNode)
    {
        fprintf(stderr, "Node not allocated!\n");
        return;
    }

    newNode->data = data;
    newNode->prev = head;
    newNode->next = head->next;
    newNode->next->prev = newNode;
    head->next = newNode;
}

void addlast(Node_t* head, uint32_t data)
{
    Node_t* p = head;
    Node_t* newNode = create();

    if(NULL == newNode)
    {
        fprintf(stderr, "Node not allocated!\n");
        return;
    }

    if(NULL==p->next)
    {
        p->next = newNode;
    }
    else
    {
        while(p->next != NULL)
        {
            p = p->next;
        }

        p->next = newNode;
    }

    newNode->prev = head;
    newNode->next = NULL;
    newNode->data = data;
}

void insertAt(Node_t *head, uint32_t data, uint32_t pos)
{
    int i = 0;
    Node_t* p = head;
    Node_t* newNode = create();
    if(NULL == head)
    {
        printf("List head is not set!\n");
        return;
    }
    else if(NULL == head->next && pos != 1)
    {
        printf("List is empty, so insert at %i isn't sensible, will be inserted last instead!\n", pos);
        addlast(head, data);
        return;
    }
    
    for(i = 0; i < pos-1; i++)
    {
        p = p->next;
        if(NULL == p)
        {
            printf("List is not deep enough, so insert at %i isn't sensible, will inserted at last instead!\n", pos);
            addlast(head, data);
            return;
        }
    }
    newNode->data = data;
    newNode->prev = p;
    newNode->next = p->next;
    p->next->prev = newNode;
    p->next = newNode;
}

void printall(Node_t *head)
{
    Node_t* p = head;
    printf("List: ");
    if(NULL == head || NULL == head->next)
    {
        printf("empty!\n");
        return;
    }
    do {
        p = p->next;
        printf("%i ", p->data);
    }
    while(p->next != NULL);
    printf("\n");
}

void sortAscending(Node_t* head)
{
    Node_t* p = head;
    if(NULL == head || NULL == head->next)
    {
        printf("List empty!\n");
        return;
    }

    while(p->next!=NULL)
    {
        
    }
}

void find_delete(Node_t* head, uint32_t Key)
{
    Node_t *p = head;
    if(NULL == head || NULL == head->next)
    {
        printf("List empty!\n");
        return;
    }

    while(p->next != NULL)
    {
        if(p->data == Key)
        {
            p = delete(p);
            return;
        }
        else
        {
            p = p->next;
        }
    }
    printf("Not found!\n");
}

int main(void)
{
    Node_t* head = getHead();
    addlast(head, 10);
    printall(head);
    addlast(head, 20);
    printall(head);
    addlast(head, 30);
    printall(head);
    addfirst(head, 5);
    printall(head);
    addlast(head, 40);
    printall(head);
    addfirst(head, 44);
    printall(head);
    insertAt(head, 88, 4);
    printall(head);
    insertAt(head, 88, 40);
    printall(head);
    addlast(head, 50);
    printall(head);
    find_delete(head, 20);

    printall(head);
    // Wrap-up
    head = cleanup(head);

    printf("Exiting..\n");
    return 0;
}



