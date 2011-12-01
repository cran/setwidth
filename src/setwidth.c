
#include <R.h>  /* to include Rconfig.h */
#include <Rinternals.h>
#include <Rinterface.h>

#include <pty.h>
#include <signal.h>

int setwidth_initialized = 0;
int setwidth_verbose = 0;


void setwidth_Set()
{
    struct winsize	ws;
    int fd = 1, columns = 0;

    /* From Vim source code (os_unix.c) */
    /* When stdout is not a tty, use stdin for the ioctl(). */
    if (ioctl(fd, TIOCGWINSZ, &ws) == 0)
        columns = ws.ws_col;

    if(columns > 0){

        /* From R-exts: Evaluating R expressions from C */
        SEXP s, t;
        PROTECT(t = s = allocList(2));
        SET_TYPEOF(s, LANGSXP);
        SETCAR(t, install("options"));
        t = CDR(t);
        SETCAR(t, ScalarInteger(columns));
        SET_TAG(t, install("width"));
        eval(s, R_GlobalEnv);
        UNPROTECT(1);

        if(setwidth_verbose > 1)
            Rprintf("setwidth: %d columns\n", columns);
    } else {
        REprintf("Error on 'setwidth' package: could not detect the terminal width.\n");
    }
}

void handle_winch(int sig){
    signal(SIGWINCH, SIG_IGN);
    setwidth_Set();
    signal(SIGWINCH, handle_winch);
}

void setwidth_Start(int *verbose)
{
    setwidth_verbose = *verbose;
    setwidth_initialized = 1;
    if(setwidth_verbose)
        REprintf("setwidth 0.9-0 loaded\nCopyright (C) 2011 Jakson A. Aquino\n"); 
    setwidth_Set();
    signal(SIGWINCH, handle_winch);
}

void setwidth_Stop()
{
    if(setwidth_initialized)
        signal(SIGWINCH, SIG_DFL);
    setwidth_initialized = 0;
}
