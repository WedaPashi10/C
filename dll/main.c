// Type your code here, or load an example.
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>

// Write a doubly linked list
// Find an element
// Delete the element if found

typedef struct stNode {
    
    uint32_t u32Element;
    struct stNode *pPrevNode;
    struct stNode *pNextNode;

} NodeType;

static NodeType *pHeadNode;

void DLL_insertLast(uint32_t val)
{
    NodeType *pTrav = pHeadNode; 
    NodeType *pTemp = (NodeType*)malloc(sizeof(NodeType));
    if(!pTemp)
    {
        fprintf(stderr, "Malloc faliled.. ");
        return;
    }

    while(pTrav->pNextNode != NULL)
    {
        pTrav = pTrav->pNextNode;
    }

    pTrav->pNextNode = pTemp;
    pTemp->pNextNode = NULL;
    pTemp->pPrevNode = pTrav;
    pTemp->u32Element = val;
    
    printf("\nNode Element %u", pTemp->u32Element);
}

void DLL_printList(void)
{
    if(pHeadNode == NULL)
    {
        printf("List is empty!");
    }

    NodeType *pTemp = pHeadNode->pNextNode;

    printf("\nList is: ");
    while(pTemp != NULL)
    {
        printf("%u ", pTemp->u32Element);
        pTemp = pTemp->pNextNode;
    }
}

void DLL_deleteElement(uint32_t Key)
{
    NodeType *pTrav = pHeadNode->pNextNode;

    while(pTrav->pNextNode != NULL)
    {
        if(pTrav->u32Element == Key)
        {
            pTrav->pPrevNode->pNextNode = pTrav->pNextNode;
            pTrav->pNextNode->pPrevNode = pTrav->pPrevNode;
            free(pTrav);
            pTrav = NULL;
            break;
        }
        else
        {
            pTrav = pTrav->pNextNode;
        }
    }
}

int main(void)
{
    NodeType Head;
    Head.pPrevNode = NULL;
    Head.pNextNode = NULL;
    Head.u32Element = 0;
    pHeadNode = &Head;

    DLL_insertLast(10);
    DLL_insertLast(20);
    DLL_insertLast(30);
    DLL_insertLast(40);
    DLL_insertLast(50);
    DLL_printList();
    DLL_deleteElement(30);
    DLL_printList();

    printf("\n\n");
    return 0;
}
