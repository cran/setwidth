# This file is part of setwidth R package
# 
# It is distributed under the GNU General Public License.
# See the file ../LICENSE for details.
# 
# (c) 2011 Jakson Aquino: jalvesaq@gmail.com
# 
###############################################################

.onLoad <- function(libname, pkgname) {
    library.dynam("setwidth", pkgname, libname, local = FALSE)

    if(is.null(getOption("setwidth.verbose")))
        options(setwidth.verbose = 0)

    termenv <- Sys.getenv("TERM")
    if(interactive() && termenv != "" && termenv != "dumb"){
        .C("setwidth_Start", as.integer(getOption("setwidth.verbose")), PACKAGE="setwidth")
    }
}

.onUnload <- function(libpath) {
    .C("setwidth_Stop", PACKAGE="setwidth")
    library.dynam.unload("setwidth", libpath)
}

