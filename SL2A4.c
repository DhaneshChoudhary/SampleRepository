//BT20CSE034 Dhanesh Choudhary
//Software Lab-2 Assignment 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 4181
#define N 20

int FIB[N];

typedef struct AllocateNodeTag
{
    char name[50];
    int start;
    int end;
    int size;
    int count;
    int mark;
    int pos; // postion in freelist

    struct AllocateNodeTag *next;
    struct AllocateNodeTag *adjacent1;
    struct AllocateNodeTag *adjacent2;
    struct AllocateNodeTag *adjacent3;

} allocNode;

allocNode *head = NULL; // head pointer to allocted list

typedef struct Free_list_tag
{
    int start;
    int size;
    struct Free_list_tag *next;
} FreeNode;

typedef struct FreePtrs
{
    int fib; // fibonacii size allowed;
    FreeNode *lptr;
} FreePtrs;

//===================================================================================//
FreePtrs FREEPTRS[N];

void fibgen()
{
    int a, b, c;
    a = 0;
    c = 0;
    b = 1;
    int i = 0;
    FIB[0] = 0;
    FIB[1] = 1;
    i = 2;
    while (c < SIZE)
    {
        c = a + b;
        FIB[i] = c;
        i++;
        a = b;
        b = c;
    }
}

void Initialise(FreeNode *ptr)
{
    int i;
    for (i = 0; i < N; i++)
    {
        FREEPTRS[i].fib = FIB[i];
        FREEPTRS[i].lptr = NULL;
    }
    FREEPTRS[i - 1].lptr = ptr;
}

int FindPos(int size)
{
    int l = 2;
    int h = N - 1;
    int ans = 2;

    while (l <= h)
    {
        int mid = (l + h) / 2;

        if (FIB[mid] >= size)
        {
            ans = mid;
            h = mid - 1;
        }
        else // fib[mid]<size
            l = mid + 1;
    }
    return ans;
}
//---------------------------------------------------------------------------------------------------//
void insertAtFreeList(int i, int start_pnt) // insert at position i refrence to x
{

    FreeNode *temp = (FreeNode *)malloc(sizeof(FreeNode));
    temp->size = FIB[i];
    temp->start = start_pnt;

    if (FREEPTRS[i].lptr == NULL)
    {
        FREEPTRS[i].lptr = temp;
        temp->next = NULL;
    }
    else
    {
        FreeNode *free_prev = NULL;
        FreeNode *freetemp = FREEPTRS[i].lptr;

        while (freetemp != NULL && freetemp->start < temp->start)
        {
            free_prev = freetemp;
            freetemp = freetemp->next;
        }

        if (free_prev == NULL)
        {
            FREEPTRS[i].lptr = temp;
            temp->next = freetemp;
        }
        else
        {
            free_prev->next = temp;
            temp->next = freetemp;
        }
    }
}

//--------------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------//
allocNode *insertInAlloc(char *t_name, int pos, int size)
{
    FreeNode *ptr = FREEPTRS[pos].lptr;

    allocNode *new_node = (allocNode *)malloc(sizeof(allocNode));
    new_node->next = NULL;
    strcpy(new_node->name, t_name);
    new_node->size = ptr->size;

    new_node->start = ptr->start;
    new_node->end = ptr->start + ptr->size - 1;
    new_node->pos = pos;

    new_node->count = 0;
    new_node->mark = 0;

    new_node->adjacent1 = NULL;
    new_node->adjacent2 = NULL;
    new_node->adjacent3 = NULL;

    allocNode *prev = NULL;

    if (head == NULL)
    {
        head = new_node;
    }
    else
    {
        allocNode *ahead = head;

        while (ahead != NULL && ahead->start < new_node->start)
        {
            prev = ahead;
            ahead = ahead->next;
        }
        if (prev == NULL)
        {
            head = new_node;
            new_node->next = ahead;
        }
        else
        {
            prev->next = new_node;
            new_node->next = ahead;
        }
    }
    FREEPTRS[pos].lptr = ptr->next;
    free(ptr);

    return new_node;
}
//--------------------------------------------------------------------------------------------------//

allocNode *Malloc(char *t_name, int size)
{

    int pos = FindPos(size);

    if (FREEPTRS[pos].lptr == NULL)
    {
        int next_pos = pos + 1;

        int found = 0;
        while (next_pos < N && !found)
        {
            if (FREEPTRS[next_pos].lptr != NULL)
                found = 1;
            else
                next_pos++;
        }

        if (!found) // if no position is empty means is heap is FUll for this allocation
        {
            printf("Heap FULL!!!\n");
        }
        else
        {
            // if  we found next position break in smaller chunks i.e fib[i-1],fib[i-2]
            while (FREEPTRS[pos].lptr == NULL)
            {
                FreeNode *ptr = FREEPTRS[next_pos].lptr;
                /*--at--*/
                insertAtFreeList(next_pos - 1, ptr->start + FIB[next_pos - 2]);
                insertAtFreeList(next_pos - 2, ptr->start);

                FREEPTRS[next_pos].lptr = ptr->next;
                free(ptr);
                next_pos -= 2; // now break smaller further if didnt get desire size.
            }
        }
    }

    if (FREEPTRS[pos].lptr != NULL)
    {
        return insertInAlloc(t_name, pos, size);
    }
    else
        return NULL;
}

///----------------------------------------------------------------------------------//
void merge_free()
{
    for (int i = 2; i < N - 2; i++)
    {
        if (FREEPTRS[i].lptr != NULL)
        {
            // look for its buddy..
            if (FREEPTRS[i + 1].lptr != NULL)
            {
                FreeNode *temp = FREEPTRS[i].lptr;
                FreeNode *prev_temp = NULL;

                while (temp != NULL && FREEPTRS[i + 1].lptr != NULL)
                {
                    FreeNode *buddy = FREEPTRS[i + 1].lptr;
                    FreeNode *prev_buddy = NULL;

                    while (buddy != NULL)
                    {
                        if (temp->start + temp->size == buddy->start)
                        {
                            // combine and insert to next list..
                            insertAtFreeList(i + 2, temp->start);

                            // delete buddy and temp
                            if (prev_temp == NULL)
                                FREEPTRS[i].lptr = temp->next;
                            else
                                prev_temp->next = temp->next;

                            // delete buddy
                            if (prev_buddy == NULL)
                                FREEPTRS[i + 1].lptr = buddy->next;
                            else
                                prev_buddy->next = buddy->next;

                            free(temp);
                            free(buddy);
                        }
                        else
                        {
                            prev_buddy = buddy;
                            buddy = buddy->next;
                        }
                    }
                    prev_temp = temp;
                    temp = temp->next;
                }
            }
        }
    }
}
///----------------------------------------------------------------------------------//

void Free(char *t_name)
{
    allocNode *tptr = head;
    allocNode *prevptr = NULL;

    if (tptr == NULL)
    {
        printf("heap is Empty !!!\n");
        return;
    }
    else
    {
        while (tptr != NULL && strcmp(tptr->name, t_name) != 0)
        {
            prevptr = tptr;
            tptr = tptr->next;
        }
        if (tptr == NULL)
        {
            printf("%s Not found in list \n", t_name);
            return;
        }
        else
        {

            if (prevptr != NULL)
            {
                printf("Memory  %s is freed\n", t_name);
                prevptr->next = tptr->next;
            }
            else
            {
                printf("Memory  %s is freed\n", t_name);
                head = tptr->next;
            }
        }
    }

    insertAtFreeList(tptr->pos, tptr->start);
    free(tptr);

    merge_free();
}

///----------------------------------------------------------------------------------//

///----------------------------------------------------------------------------------//
void Print_alloc_list()
{
    allocNode *lptr = head;
    if (lptr != NULL)
    {
        while (lptr != NULL)
        {
            printf("name:%s\n", lptr->name);
            printf("Size allocated :%d\n", lptr->size);
            lptr = lptr->next;
            printf("\n");
        }
    }
    else
    {
        printf("Empty  list!!!!\n");
    }
}
///----------------------------------------------------------------------------------//
///----------------------------------------------------------------------------------//
void Print_Free_list()
{
    FreeNode *temp;
    int i = 2;
    while (i < N)
    {
        if (FREEPTRS[i].lptr != NULL)
        {
            temp = FREEPTRS[i].lptr;

            printf("size %d IS:\n", FREEPTRS[i].fib);
            while (temp != NULL)
            {
                printf("start=%d \t\tend=%d	SIZE=%d\n", temp->start, temp->start + temp->size - 1, temp->size);
                temp = temp->next;
            }
            printf("\n");
        }

        i++;
    }
}

typedef struct ROOT
{
    allocNode *root;
} ROOT;

void Create_memoryArea(ROOT **root1, ROOT **root2)
{
    ROOT *temp = *root1;
    temp->root = Malloc("5", 2);
    temp->root->count = 2;

    (*root2)->root = temp->root->adjacent1 = Malloc("1", 3);
    temp->root->adjacent1->count = 3;

    temp->root->adjacent1->adjacent1 = Malloc("2", 3);
    temp->root->adjacent1->adjacent1->count = 1;
    temp->root->adjacent1->adjacent2 = Malloc("9", 8);
    temp->root->adjacent1->adjacent2->count = 2;
    temp->root->adjacent1->adjacent3 = Malloc("10", 13);
    temp->root->adjacent1->adjacent3->count = 2;

    allocNode *temp2 = Malloc("7", 2);
    temp2->count = 0;
    temp2->adjacent1 = temp->root->adjacent1;
    temp2->adjacent2 = Malloc("8", 2);
    temp2->adjacent2->count = 2;
    temp2->adjacent2->adjacent1 = temp->root->adjacent1->adjacent2;

    allocNode *temp3 = Malloc("3", 2);
    temp3->adjacent1 = temp2->adjacent2;
    temp3->adjacent2 = temp->root->adjacent1->adjacent3;
}

///----------------------------------------------------------------------------------//

void Print_ROOT(allocNode *node)
{
    if (node == NULL)
        return;
    printf("%s ", node->name);

    if (node->adjacent1 != NULL || node->adjacent2 != NULL || node->adjacent3 != NULL)
    {
        printf("-[ ");
        Print_ROOT(node->adjacent1);
        Print_ROOT(node->adjacent2);
        Print_ROOT(node->adjacent3);
        printf(" ]");
    }
}

void refCnt()
{
    allocNode *temp, *prev;

    int flag = 1;
    while (flag)
    {
        flag = 0;
        temp = head;
        prev = NULL;

        while (temp != NULL)
        {
            int flag2 = 0;

            if (temp->count == 0)
            {
                flag = 1;

                flag2 = 1;

                if (temp->adjacent1 != NULL)
                {
                    (temp->adjacent1->count) -= 1;
                }
                if (temp->adjacent2 != NULL)
                {
                    (temp->adjacent2->count) -= 1;
                }
                if (temp->adjacent3 != NULL)
                {
                    (temp->adjacent3->count) -= 1;
                }

                // delete node or free up the data;
                allocNode *temp2;
                if (prev == NULL)
                {
                    temp2 = head;
                    temp = head = head->next;
                }
                else
                {
                    temp2 = temp;
                    prev->next = temp->next;
                    temp = temp->next;
                }

                // insert into free list.
                insertAtFreeList(temp2->pos, temp2->start);
                free(temp2);
            }

            if (flag2 == 0)
            {
                prev = temp;
                temp = temp->next;
            }
        }
    }
}

///----------------------------------------------------------------------------------//
void mark_algo(ROOT *n)
{
    allocNode *temp = n->root;
    allocNode *tail = NULL;
    allocNode *middle = NULL;

    while (temp != NULL)
    {
        if (temp->mark == 0)
        {
            temp->mark = 1;
        }
        if (temp->adjacent1 != NULL && temp->adjacent1->mark == 0)
        {
            tail = middle;
            middle = temp;
            temp = temp->adjacent1;
        }
        else if (temp->adjacent2 != NULL && temp->adjacent2->mark == 0)
        {
            tail = middle;
            middle = temp;
            temp = temp->adjacent2;
        }
        else if (temp->adjacent3 != NULL && temp->adjacent3->mark == 0)
        {
            tail = middle;
            middle = temp;
            temp = temp->adjacent3;
        }
        else
        {
            temp = middle;
            middle = tail;
            tail = NULL;
        }
    }
}

void Sweep_algo()
{
    allocNode *temp, *prev;

    temp = head;
    prev = NULL;

    while (temp != NULL)
    {
        int flag = 0;

        if (temp->mark == 0)
        {

            flag = 1;

            allocNode *temp2;
            if (prev == NULL)
            {
                temp2 = head;
                temp = head = head->next;
            }
            else
            {
                temp2 = temp;
                prev->next = temp->next;
                temp = temp->next;
            }

            insertAtFreeList(temp2->pos, temp2->start);
            free(temp2);
        }
        else
        {
            temp->mark = 0;
        }

        if (flag == 0)
        {
            prev = temp;
            temp = temp->next;
        }
    }
}

void marks_sweep(ROOT *root1, ROOT *root2)
{
    // mark phase
    mark_algo(root1);

    mark_algo(root2);
    // sweep phase
    Sweep_algo();
}
///----------------------------------------------------------------------------------//

int main()
{

    FreeNode *ptr = (FreeNode *)malloc(sizeof(FreeNode));
    ptr->start = 0;
    ptr->next = NULL;
    ptr->size = SIZE;

    fibgen(); // genrate fibonacci series and intialize Free list;
    Initialise(ptr);

    ROOT *root1 = (ROOT *)malloc(sizeof(ROOT));
    ROOT *root2 = (ROOT *)malloc(sizeof(ROOT));
    root1->root = NULL;
    root2->root = NULL;

   printf("\n------HEAP ALLOCATED DATA-----\n");
    Print_alloc_list();

    printf("\nroot1-->");
    Print_ROOT(root1->root);
    printf("\nroot2-->");
    Print_ROOT(root2->root);

    printf("\n---------FREE LIST----------\n");
    Print_Free_list();

    printf("\n==================================================================\n");
    printf("                           MENU                              \n");
    printf("==================================================================\n");
    printf("1.Simulate reference counting\n");
    printf("2.Simulate mark and sweep\n");

    int choice;
    printf("Enter choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
    {
        refCnt;
    }
    break;
    case 2:
    {
        marks_sweep(root1, root2);
    }
    break;

    default:
        printf("\nEntered Wrong choice!! Try again..\n");
        break;
    }

    printf("\n------HEAP ALLOCATED DATA after GC-----\n");

    Print_alloc_list();
    printf("\n---------FREE LIST after GC----------\n");
    Print_Free_list();

    return 0;
}
