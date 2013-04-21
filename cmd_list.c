#include "def.h"

cmd_list init_cmd_list() {
    cmd_list node;
    node = malloc(sizeof(struct command_list));
    node->cmd = NULL;
    node->next = NULL;
    return node;
}

cmd_list create_node(command cmd) {
    int i;
    cmd_list p = malloc(sizeof(struct command_list));
    p->cmd = cmd;
    p->next = NULL;
    return p;
}

int insert(cmd_list head, cmd_list node) {
    cmd_list p = head;
    
    // insert
    while (p->next) 
	p = p->next;
    p->next = node;
    node->next = NULL;
    return 0;
}

int clear_list(cmd_list head) {
    cmd_list h, p;
    command cmd;
    
    h = head->next;
    
    while (h != NULL) {
	p = h;
	cmd = p->cmd;
	h = h->next;
	free(cmd);
	free(p);
    }

    head->next = NULL;
    return 0;
}

int length(cmd_list head) {
    cmd_list p = head;
    int count = 0;
    while (p->next) {
	p = p->next;
	count++;
    }
    return count;
}
