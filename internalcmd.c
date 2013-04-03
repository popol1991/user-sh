#include "def.h"

Env_History envhis;

void cd(char* dir) {
	// implement cd here
}


void history() {
	// implement history here

	int i,j=1;
	int start =0;
	//printf("start: %d, end: %d\n", envhis->start, envhis->end);
	if(envhis->start <= envhis->end)
	{
		if(envhis->his_cmd[envhis->start] == NULL)
			start = envhis->start+1;
		else
			start = envhis->start;
		for(i=start;i <= envhis->end;i++)
		{
			printf("%d\t%s\n",j,envhis->his_cmd[i]);
			j++;
		}
	}
	else
	{
		for(i=envhis->start;i < CMD_NUMBERS;i++)
		{
			printf("%d\t%s\n",j,envhis->his_cmd[i]);
			j++;
		}
		for(i=0;i <= envhis->end;i++)
		{
            printf("%d\t%s\n",j,envhis->his_cmd[i]);
			j++;
		}
	}
}

void jobs() {
	// implement jobs here
}

void fg(int jid) {
	// implement fg here
}

void bg(int jid) {
	// implement bg here
}
