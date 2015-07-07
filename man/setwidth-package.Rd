\name{setwidth-package}
\alias{setwidth-package}
\alias{setwidth}
\docType{package}
\title{
Automatically set the value of options("width") when the terminal emulator is
resized
}
\description{
  This package should not be used with Graphical User Interfaces, such as
  Windows RGui, RStudio, RKward, JGR, Rcmdr and other interfaces which have
  their own engine to display R output. The functions of this package only
  work if R is compiled for Linux systems and it is running interactively in a
  terminal emulator. The terminal emulator might have been called by a text
  editor, such as Vim, Gedit, Kate or Geany.
}
\details{
  The package will print information on the R Console if its
  \samp{setwidth.verbose} option was set to a numeric value bigger than zero: 
  
  \preformatted{
  options(setwidth.verbose = 1) # Print startup message
  options(setwidth.verbose = 2) # Print error message when unable to set width
  options(setwidth.verbose = 3) # Print width value
  }
 
  The package does not have any user visible R function. When it is loaded,
  the SIGWINCH signal starts to be handled by a C function that updates the
  value of \code{options("width")}. The handle will not be activated if
  \code{interactive() == FALSE} or the value of the environment variable
  \samp{TERM} is either empty or \samp{"dumb"}.

  To manually test whether the package is working properly on your system you
  may repeatedly resize the terminal emulator and print a long vector, like
  1:300.

  To disable the automatic setting of \code{options("width")} do:

  \preformatted{
  detach("package:setwidth", unload = TRUE)
  }
}
\author{
Jakson Alves de Aquino \email{jalvesaq@gmail.com},
with some code copied from Vim.
}

\seealso{
  The package \pkg{colorout} colorizes R output when running in a terminal emulator.
}
\keyword{ package }
