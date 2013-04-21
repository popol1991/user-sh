#include "def.h"
extern struct pid_node* plist;
joblist jlist;

void cd(char* dir) {
	// implement cd here
}


void history() {
	// implement history here
}

void jobs() {
	// implement jobs here
    int i = 0;
    struct pid_node* p;
    joblist j;

    if (jlist == NULL)
        printf("Thereis no jobs!");
    else {
        for (i=1, j = jlist; j != null; j = j->next, i++) {
           p = j->plist;
           while (p->next != NULL) p = p->next;
           printf("[%d]\t%s\t\t%s\n", i, p->pid, j->state==1?"RUNNING":j->state==0?"STOPPED":"DONE", j->cmd);
        }
    }
}

joblist add_job(struct pid_node* plist, char* cmd) {
    joblist new = malloc(sizeof(struct jobs_node));
    joblist j;
    struct pid_node* p, q;

    new->plist = NULL;
    new->cmd = strdup(cmd);
    new->state = RUNNING;
    new->next = NULL;

    p = plist;
    while (p) {
        q = malloc(sizeof(struct pid_node));
        q->pid = p->pid;
        if (new->plist == NULL)
            new->plist = q;
        else {
            q->next = new->plist;
            new->plist = q;
        }
        p = p->next;
    }

    if (jlist == NULL)
        jlist = new;
    else {
        j = jlist;
        while (j->next != NULL) j = j->next;
        j->next = new;
    }

    return new;
}

void remove_job(int sig, siginfo_t *sip, void* nosed) {
    pid_t pid;
    struct pid_node p, temp;
    int found;
    joblist j;

    if (sip->si_code == CLD_STOPPED)
        return;

    pid = sip-si_pid;
    j = jlist;

    found = 0;
    while (j->next != NULL) {
        p = j->plist;
        if (p->pid == pid) {
            j->plist = j->plist->next;
            free(p);
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
    while (j->next != NULL) {
        if (j->plist == NULL)
            j->state = STOPPED;
    }
}

void sigstp_handler() {
    struct pid_node* p;
    joblist j, job;
    int found = 0;

    if (plist == NULL)
        return;
    for (p = plist; p != NULL; p = p->next)
        kill(p->pid, SIGSTOP);

    job = NULL;
    j = jlist;
    while (j != NULL) {
        for (p = j->plist; p != NULL; p = p->next) 
            if (p->pid = plist->pid) {
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
        job = add_job(plist, user_input);

    job->state = RUNNING;
    job->cmd = realloc(job->cmd, (strlen(job->cmd) + 1) * sizeof(char));
    strcat(job->cmd, "&");
}

void fg(int jid) {
	// implement fg here
}

void bg(int jid) {
	// implement bg here
}
