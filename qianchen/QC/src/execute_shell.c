#include "includes.h"

#define WD_SHELL_PATH "/bin/sh"

//return 0 is success
int execute(const char *cmd_line, int quiet)
{
    int pid, status, rc;
	sighandler_t old_handler;
 

    const char *new_argv[4];
    new_argv[0] = WD_SHELL_PATH;
    new_argv[1] = "-c";
    new_argv[2] = cmd_line;
    new_argv[3] = NULL;

   	old_handler = signal(SIGCHLD, SIG_DFL);	

    pid = fork();
    if (pid == 0) {             /* for the child process:         */
        /* We don't want to see any errors if quiet flag is on */
        if (quiet)
            close(2);
        if (execvp(WD_SHELL_PATH, (char *const *)new_argv) == -1) { /* execute the command  */
            LOG_STD_INFO(stderr, "execvp(): %s\n", strerror(errno));
        } else {
            LOG_STD_INFO(stderr, "execvp() failed\n");
        }
        exit(1);
    }

    /* for the parent:      */
    //LOG_NORMAL_INFO("Waiting for PID %d to exit\n", pid);
    rc = waitpid(pid, &status, 0);
    //LOG_NORMAL_INFO("Process PID %d exited\n", rc);
 	
	signal(SIGCHLD, old_handler);
   
    if (-1 == rc) {
        LOG_ERROR_INFO("waitpid() failed (%s)\n", strerror(errno));
        return 1; /* waitpid failed. */
    }

    if (WIFEXITED(status)) {
        return (WEXITSTATUS(status));
    } else {
        /* If we get here, child did not exit cleanly. Will return non-zero exit code to caller*/
        LOG_NORMAL_INFO("Child may have been killed.\n");
        return 1;
    }
}

