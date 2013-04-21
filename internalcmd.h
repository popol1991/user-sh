void cd(char* dir);
void jobs();
void history();
void pwd();
void fg(int jid);
void bg(int jid);
void sigstp_handler(); 
void remove_job(int, siginfo_t *, void*);
joblist add_job(struct pid_node*, char*);
