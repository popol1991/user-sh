#include "def.h"


cmd_list init_cmd_list();
cmd_list create_node(command cmd);
int insert(cmd_list head, cmd_list nocd);
int clear_list(cmd_list head);
int length(cmd_list head);
