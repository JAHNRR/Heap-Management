#include <stdio.h>
#define HEAPSIZE 100

typedef struct metadata
{
    size_t size; // unsigned int (in bytes)
    struct metadata *next;
} metadata;

char Heap[HEAPSIZE];

metadata *Freelist = (void *)Heap;

void InitializeFreelist()
{
    Freelist->size = HEAPSIZE - sizeof(metadata);
    Freelist->next = NULL;
}

void *MyAllocate(size_t reqsize)
{
    metadata *retval, *ptr, *NewFreeBlock, *prev;
    prev = ptr = Freelist;
    retval = NULL;

    while (ptr->size < reqsize && ptr != NULL) // finding first fit block
    {
        prev = ptr;
        ptr = ptr->next;
    }

    if (ptr == NULL)
    {
        printf("cant find a fit slot for size %zu\n", reqsize);
        return retval;
    }

    else // found a first fit slot
    {
        int diff = ptr->size - (reqsize);

        if (diff <= sizeof(metadata)) // fully allocate the block
        {
            if (ptr == Freelist)
            {
                Freelist = ptr->next;
            }
            else
            {
                prev->next = ptr->next;
            }
            ptr->next = NULL;
            retval = ptr + 1; // leaving the sizeof matadata
        }

        else // splitting
        {
            metadata *nextfree, *newfree;
            nextfree = ptr->next; // newfree->next = nextfree & prev->next = newfree

            ptr->next = NULL;
            ptr->size = reqsize;
            retval = ptr + 1;

            char *a;
            a = (void *)ptr;
            a = a + sizeof(metadata) + reqsize;
            newfree = (void *)a;
            newfree->next = nextfree;
            newfree->size = diff - sizeof(metadata);

            if (Freelist == ptr)
                Freelist = newfree;
        }
    }
    return retval;
}

void MyFree(void *p)
{
    metadata *newfree;
    metadata *allo = p;
    allo--;
    int size = allo->size;
    if (p < (void *)Freelist)
    {
        if (p + size == (void *)Freelist)
        {
            p = p - sizeof(metadata);
            newfree = (void *)p;
            newfree->size = size + sizeof(metadata) + Freelist->size;//merge
            newfree->next = Freelist->next;
            Freelist = newfree;
            return;
        }
        else
        {
            p = p - sizeof(metadata);
            newfree = (void *)p;
            newfree->size = size;
            newfree->next = Freelist;
            Freelist = newfree;
        }
    }

    else
    {
        metadata *prev, *ptr;
        prev = NULL;
        ptr = Freelist;
        while ((void *)ptr < p)
        {
            prev = ptr;
            ptr = ptr->next;
        }

        if ((void *)ptr - (void *)prev == 2 * sizeof(metadata) + size + prev->size)//case1
        {
            prev->next = ptr->next;
            prev->size = prev->size + sizeof(metadata) + size + sizeof(metadata) + ptr->size;
        }

        else if ((void *)p - (void *)prev == prev->size + 2 * sizeof(metadata))//case2
        {
            prev->size += sizeof(metadata) + size;
        }

        else if ((void *)ptr - (void *)p == size)//case3
        {
            metadata *newmeta;
            p = p - sizeof(metadata);
            newmeta = (void *)p;
            newmeta->next = ptr->next;
            newmeta->size = size + sizeof(metadata) + ptr->size;
            prev->next = newmeta;
        }
        else
        {
            metadata *newmeta;
            p = p - sizeof(metadata);
            newmeta = (void *)p;
            newmeta->next = ptr;
            newmeta->size = size;
            prev->next = newmeta;
        }
    }
}

void PrintFreelist()
{
    metadata *ptr;
    ptr = Freelist;
    while (ptr != NULL)
    {
        printf("[size = %d] ", ptr->size);
        ptr = ptr->next;
    }
    printf("|END\n\n");
}

void main()
{
    InitializeFreelist();

    int *a, *b, *c, *d;
    a = MyAllocate(sizeof(int));
    b = MyAllocate(sizeof(int));
    c = MyAllocate(sizeof(int));
    d = MyAllocate(sizeof(int));

    printf("after allocating a,b,c,d\n");
    PrintFreelist();

    MyFree(a);
    printf("after freeing a\n");
    PrintFreelist();

    MyFree(d);
    printf("after freeing \n");
    PrintFreelist();

    char *e;
    e = MyAllocate(sizeof(char));
    printf("after allocating e\n");
    PrintFreelist();
    MyFree(e);
    printf("afterfreeing e\n");
    PrintFreelist();
}