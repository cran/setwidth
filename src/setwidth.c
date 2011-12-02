
#include <R.h>  /* to include Rconfig.h */
#include <Rinternals.h>
#include <Rinterface.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

int setwidth_initialized = 0;
int setwidth_verbose = 0;


void setwidth_Set()
{
    /* The code to get terminal width is from Vim source code (os_unix.c).
     * Try to get the current window size with an ioctl(). */

    long columns = 0;
    char *p;

# ifdef TIOCGWINSZ
    struct winsize ws;
    if(isatty(1))
        if (ioctl(1, TIOCGWINSZ, &ws) == 0)
            columns = ws.ws_col;
# else
#  ifdef TIOCGSIZE
    struct ttysize ts;
    if(isatty(1))
        if (ioctl(1, TIOCGSIZE, &ts) == 0)
            columns = ts.ts_cols;
#  endif
# endif

    if(columns > 0){

        /* From R-exts: Evaluating R expressions from C */
        SEXP s, t;
        PROTECT(t = s = allocList(2));
        SET_TYPEOF(s, LANGSXP);
        SETCAR(t, install("options"));
        t = CDR(t);
        SETCAR(t, ScalarInteger((int)columns));
        SET_TAG(t, install("width"));
        eval(s, R_GlobalEnv);
        UNPROTECT(1);

        if(setwidth_verbose > 2)
            Rprintf("setwidth: %d columns\n", columns);
    } else {
        if(setwidth_verbose > 1)
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
