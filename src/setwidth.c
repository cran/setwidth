#include <R.h>  /* to include Rconfig.h */
#include <Rinternals.h>
#include <R_ext/eventloop.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

static int setwidth_initialized = 0;
static int setwidth_verbose = 0;
static long oldcolwd = 0;
static int fired = 0;
static int ifd, ofd;
static InputHandler *ih;

void setwidth_Set(void *unused) // Changed to take a void pointer
{
    /* The code to get terminal width is from Vim source code (os_unix.c).
     * Try to get the current window size with an ioctl(). */

    long columns = 0;

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
        if(columns != oldcolwd){
            oldcolwd = columns;

            /* From R-exts: Evaluating R expressions from C */
            /* Create integer value for width - replaces ScalarInteger in SETCAR */
            SEXP width_val = PROTECT(ScalarInteger((int)columns));
            /* Use lang2() API function instead of SET_TYPEOF() to create function call */
            SEXP options_call = PROTECT(lang2(install("options"), width_val));
            /* Set named argument "width" - same as original but on lang2() result */
            SET_TAG(CDR(options_call), install("width"));
            /* Evaluate the call - unchanged from original */
            eval(options_call, R_GlobalEnv);
            /* Unprotect both objects (was UNPROTECT(1) for single allocation) */
            UNPROTECT(2);

            if(setwidth_verbose > 2)
                Rprintf("setwidth: %ld columns\n", columns); // Changed format specifier to %ld
        }
    } else {
        if(setwidth_verbose > 1)
            REprintf("Error on 'setwidth' package: could not detect the terminal width.\n");
    }
}

void handle_winch(int sig){
    if(fired)
        return;
    fired = 1;
    char buf[16];
    *buf = 0;
    if(write(ofd, buf, 1) <= 0)
        REprintf("setwidth error: write <= 0\n");
}

/* Code adapted from CarbonEL.
 * Thanks to Simon Urbanek for the suggestion on r-devel mailing list. */
static void uih(void *data) {
    char buf[16];
    if(read(ifd, buf, 1) < 1)
        REprintf("setwidth error: read < 1\n");
    R_ToplevelExec(setwidth_Set, NULL);
    fired = 0;
}

void setwidth_Start(int *verbose)
{
    setwidth_verbose = *verbose;

    if(setwidth_verbose)
        REprintf("setwidth 1.0-4 loaded\n"); 
    if(setwidth_initialized)
        return;

    setwidth_Set(NULL); // Added NULL argument
    signal(SIGWINCH, handle_winch);

    int fds[2];
    if(pipe(fds) == 0){
        ifd = fds[0];
        ofd = fds[1];
        ih = addInputHandler(R_InputHandlers, ifd, &uih, 32);
    } else {
        REprintf("setwidth error: pipe != 0\n");
        ih = NULL;
    }

    setwidth_initialized = 1;
}

void setwidth_Stop()
{
    if(setwidth_initialized)
        signal(SIGWINCH, SIG_DFL);
    if(ih){
        removeInputHandler(&R_InputHandlers, ih);
        close(ifd);
        close(ofd);
    }
    setwidth_initialized = 0;
}
