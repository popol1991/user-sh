#include "def.h"
extern struct pid_node* plist;
extern char* cmd_input;
Env_History envhis;
joblist jlist;

void cd(char* dir) {
	// implement cd here
    if(chdir(dir) < 0)
        printf("%s\t the wrong path name.\n",dir);
}


void history() {
	// implement history here
    int i,j=1;
    int start =0;

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
    int i = 0;
    struct pid_node* p;
    joblist j;

    if (jlist == NULL)
        printf("There is no jobs!");
    else {
        for (i=1, j = jlist; j != NULL; j = j->next, i++) {
           printf("[%d]\t%s\t\t%s\n", i, j->state==1?"RUNNING":j->state==0?"STOPPED":"DONE", j->cmd);
        }
    }
}

struct pid_node* plistdup(struct pid_node* src) {
    struct pid_node* dest, *p, *n;
    p = src;
    dest = NULL;
    while (p != NULL) {
        n = malloc(sizeof(struct pid_node));
        n->pid = p->pid;
        n->next = dest;
        dest = n;
        p = p->next;
    }
    return dest;
}

joblist add_job(struct pid_node* plist, char* cmd) {
    joblist new = malloc(sizeof(struct jobs_node));
    joblist j;
    struct pid_node* p, *n;

    new->plist = NULL;
    new->cmd = strdup(cmd);
    new->state = RUNNING;
    new->next = NULL;
    
    new->plist = plistdup(plist);

    if (jlist == NULL)
        jlist = new;
    else {
        j = jlist;
        while (j->next != NULL) j = j->next;
        j->next = new;
    }

    return new;
}

/* mark a job as DONE if and only if all the processes  *
 * of the job have existed                              */
void remove_job(int sig, siginfo_t *sip, void* nosed) {
    pid_t pid;
    struct pid_node* p, *temp;
    int found;
    joblist j, pre_j;

    // printf("Exit code: %d\n", sip->si_code);
    if (sip->si_code != CLD_EXITED)
        return;


    pid = sip->si_pid;
    j = jlist;

    found = 0;
    while (j != NULL) {
        if (j->plist == NULL)
            break;
        p = j->plist;
        if (p->pid == pid) {
            j->plist = j->plist->next;
            free(p);
            found = 1;
        } else {
            while (p->next != NULL) {
                if (p->next->pid == pid) {
                    temp = p->next;
                    p->next = temp->next;
                    free(temp);
                    found = 1;
                    break;
                }
                p = p->next;
            }
        }
        if (found) break;
        j = j->next;
    }

    j = jlist;
    while (j != NULL) {
        if (j->plist == NULL)
            j->state = DONE;
        j = j->next;
    }
}

void clear_plist() {
    struct pid_node* p;
    while (plist != NULL) {
        p = plist;
        plist = plist->next;
        free(p);
    }
}

void sigstp_handler() {
    struct pid_node* p;
    joblist j, job;
    int found = 0, jid;

    if (plist == NULL)
        return;
    for (p = plist; p != NULL; p = p->next)
        kill(p->pid, SIGSTOP);

    job = NULL;
    j = jlist;
    while (j != NULL) {
        for (p = j->plist; p != NULL; p = p->next) 
            if (p->pid == plist->pid) {
                found = 1;
                break;
            }
        if (found) {
            job = j;
            break;
        }
        j = j->next;
    }

    if (job == NULL)
        job = add_job(plist, cmd_input);

    job->state = STOPPED;
    job->cmd = realloc(job->cmd, (strlen(job->cmd) + 1) * sizeof(char));
    strcat(job->cmd, "&");
    
    j = jlist;
    jid = 1;
    while (j != NULL && j != job) jid++;
    printf("[%d]\tStopped\t\t%s\n", jid, job->cmd);
    
    clear_plist();
}

void fg(int jid) {
	// implement fg here
    joblist j, job;
    struct pid_node* p, h;
    int count, found = 0;

    for (count = 1, j = jlist; j != NULL; count++, j = j->next) {
        if (count == jid) {
            job = j;
            found = 1;
            break;
        }
    }

    if (!found) 
        printf("fg: %d: There is no this task!\n", jid);
    else {
        // send SIGCONT to all child process
        plist = plistdup(job->plist);

        job->state = RUNNING;
        job->cmd[strlen(job->cmd) - 1] = '\0';
        job->cmd = realloc(job->cmd, (strlen(job->cmd) - 1) * sizeof(char));

        printf("%s\n", job->cmd);

        for (p = plist; p != NULL; p = p->next) {
            kill(p->pid, SIGCONT);
        }
        for (p = plist; p != NULL; p = p->next) {
            int status;
            waitpid(p->pid, &status, WUNTRACED);
        }
    }

}

void bg(int jid) {
	// implement bg here
    joblist j, job;
    struct pid_node* p;
    int count, found = 0;

    for (count = 1, j = jlist; j != NULL; count++, j = j->next) {
        if (count == jid) {
            job = j;
            found = 1;
            break;
        }
    }

    if (!found)
        printf("bg: %d: Theres is no this task!\n", jid);
    else {
        job->state = RUNNING;
        printf("[%d]\t%s\n", jid, job->cmd);
        
        for (p = job->plist; p != NULL; p = p->next) {
            int status;
            kill(p->pid, SIGCONT);
        }
    }
    
}

void pwd()
{
    // implement pwd here
    char curdir[MAXDIRLEN];
         
    getcwd(curdir,sizeof(curdir));
    printf("%s\n",curdir);
}
