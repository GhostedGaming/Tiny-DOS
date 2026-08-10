#include <stdint.h>
#include <stddef.h>

#define MAX_FUNCTIONS 256

uint64_t functions_to_init = 0;

struct node {
    void *entry;
    struct node *next;
};

static struct node node_pool[MAX_FUNCTIONS];
static struct node *head = NULL;

void register_function(void *entry) {
    if (functions_to_init >= MAX_FUNCTIONS) {
        return;
    }

    struct node *new_node = &node_pool[functions_to_init++];

    new_node->entry = entry;
    new_node->next = head;
    head = new_node;
}

int init_kernel() {
    for (uint64_t i = 0; i < functions_to_init; i++) {
        if (node_pool[i].entry == NULL) {
            continue;
        }

        int (*func)(void) = (int (*)(void))node_pool[i].entry;
        
        int res = func();
        if (res != 0) {
            return res;
        }
    }
    return 0;
}
