/*******************************************************************************
 * Author      : Matthew Mohamed
 * Name        : minishell.c
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"

volatile sig_atomic_t interrupted = 0;

// Signal handler for SIGINT.
void sigint_handler(int sig) {
    // Set the flag; 
    interrupted = 1;
    wait(NULL);
    printf("\n");
}

// Function to install the signal handler.
void install_signal_handler() {
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; 
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

/*Function to change the working dir to the home dir of the user*/
char *cdHome(){
    //This is how you get the home dir of the user.
    struct passwd *pw = getpwuid(getuid());
    if(chdir(pw->pw_dir) == -1) {
        fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
        return NULL;
    }
    // Return the current working directory.
    return getcwd(NULL, 0);
}


/*Function to exit the shell*/
void exit_shell(){
    exit(EXIT_SUCCESS);
}

void pwd_shell(){

    char *pathname = getcwd(NULL,0);
    if(pathname == NULL){
        fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
    }else{
        printf("%s\n", pathname); 
    }
    free(pathname);
}

void lf(){
    DIR* dp;
    struct dirent* dirp;
    char *cwd = getcwd(NULL, 0);

    dp = opendir(cwd); 
    if (dp == NULL) {
        fprintf(stderr, "Cannot open %s\n", cwd);
        exit(EXIT_FAILURE);
    }
    free(cwd);
    //loop through all the files
    while ((dirp = readdir(dp)) != NULL) { 
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;
        printf("%s\n", dirp->d_name);
    }

    closedir(dp);
}



void lp(){
    DIR *procDir;
    struct dirent *entry;
    int count = 0;

    procDir = opendir("/proc");
    if (procDir == NULL) {
        perror("opendir /proc");
        return;
    }

    while ((entry = readdir(procDir)) != NULL) {  

        if(atoi(entry -> d_name) != 0){
            //printf("%s\n", entry->d_name);
            // Build the path for the process directory
            
            char procDirPath[PATH_MAX];
            snprintf(procDirPath, sizeof(procDirPath), "/proc/%s", entry->d_name);
            
            struct stat st;
            if (stat(procDirPath, &st) == -1) {
                perror("stat");
                continue;
            }
            
            // Get the username from the UID
            struct passwd *pwd = getpwuid(st.st_uid);
            char *username = pwd->pw_name; 


            char cmdPath[PATH_MAX];
            snprintf(cmdPath, sizeof(cmdPath), "/proc/%s/cmdline", entry->d_name);

            FILE *fp = fopen(cmdPath, "r");
            if (fp == NULL) {
                perror("fopen");
            } else {
                char *line = NULL;
                size_t len = 0;
                ssize_t nread = getline(&line, &len, fp);
                if (nread != -1) {
                    // Print cmdline content
                    printf("%s %s %s\n",entry->d_name, username, line);
                }
                free(line);
                fclose(fp);
            }
        }
        
    }

    closedir(procDir);

}

int main(){

    char *pathname = NULL; 
    install_signal_handler(); // Set up SIGINT handling.
    while(1){
        // Free previous pathname if it exists.
        if (pathname != NULL) {
            free(pathname);
        }
        pathname = getcwd(NULL, 0);
    
        printf("%s[%s]%s", BLUE, pathname, DEFAULT); // How to print the dir in blue.
        printf(">");

        char *line = NULL;
        size_t len = 0;
        size_t nread = 0;
        nread = getline(&line, &len, stdin);

        if (nread == -1) {
            if (errno == EINTR) {
                clearerr(stdin);
                interrupted = 0;
                free(line);
                continue;
            } else {
                fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno));
                free(line);
                continue;
            }
        }

        if (nread > 0 && line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
            nread--;
        }
        len = strlen(line)-1;

        // Find the index of the first space
        int i = 0;
        while (line[i] != ' ' && line[i] != '\n' && line[i] != '\0') {
            i++;
        }

        char arg1[i + 1];
        strncpy(arg1, line, i);
        arg1[i] = '\0';


        int j = i;
        if (line[j] == ' ') {
            j++;
        }


        int arg2_len = nread - j;
        char arg2[arg2_len + 1];  
        strncpy(arg2, line + j, arg2_len);
        arg2[arg2_len] = '\0';

        //printf("arg1 is: \"%s\"\n", arg1);
        //printf("%s\n", arg2);

        if(( ( strcmp(arg1,"cd") == 0) && (strcmp(arg2,"") == 0)  )|| ( (strcmp(arg1,"cd") == 0) && (strcmp(arg2,"") == 0) ) ){
            free(pathname);
            pathname = cdHome();
            free(line);
            continue;
        }else if( ( strcmp(arg1,"cd") == 0) && (strcmp(arg2,"") != 0) ){
            //check if arg2 has more than one argument.
            if (strchr(arg2, ' ') != NULL) {
                fprintf(stderr, "Error: Too many arguments to cd.\n");
                free(line);
                continue;
            }

            if(strcmp(arg2,"~") == 0){
                free(pathname);
                pathname = cdHome();
                free(line);
                continue;
            }
            if (chdir(arg2) == -1) {
                fprintf(stderr, "Error: Cannot change directory to %s.\n", strerror(errno));
            } else {
                free(pathname);
                pathname = getcwd(NULL, 0);
            }
            free(line);
            continue;
        }else if(( strcmp(arg1,"exit") == 0) && (strcmp(arg2,"") == 0)){
            free(line);
            free(pathname);
            exit_shell();
        }else if(( strcmp(arg1,"pwd") == 0) && (strcmp(arg2,"") == 0)){
            pwd_shell();
            free(line);
            continue;
        }else if(( strcmp(arg1,"lf") == 0) && (strcmp(arg2,"") == 0)){
            lf();
            free(line);
            continue;
        }else if(( strcmp(arg1,"lp") == 0) && (strcmp(arg2,"") == 0)){
            lp();
            free(line);
            continue;
        }else if(( strcmp(arg1,"echo") == 0)){
            printf("%s\n", arg2);
            free(line);
            continue;
        }else{
            pid_t p = fork();
            if (p < 0) { 
                fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno)); 
            }else if (p == 0) { 
                char *args[] = { arg1, arg2, NULL };
                execvp(args[0], args);
                fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                exit(EXIT_FAILURE);
            }else{
                //fix this function 
                if(wait(NULL) < 0 && errno != EINTR){
                    fprintf(stderr, "Error: wait failed. %s.\n", strerror(errno));
                }
            }
        free(line);

        }
        //free(pathname);
    }

    return 0;
}
