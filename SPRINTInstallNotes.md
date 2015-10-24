# Introduction #

Quick install notes for installing SPRINT.


# Ubuntu 10 #
With standard packages providing MPICH2 1.2.1p1 and R 2.10.1.

The Ubuntu R package puts the  R include files in /usr/share/R/include/ instead of the standard $R\_HOME/include or /usr/lib/R/include so the installer cannot find the include files.

To fix this you need to run:

`$ cd /usr/lib/R/`

`$ ln -s /usr/share/R/include .`

Then compile normally

`$ R CMD INSTALL sprint-X.X.tgz`

You also need to have a working mpi setup and mpd running.

# Fedora 13 #
## MPICH2 ##
Install the packages that are needed


`$ yum install R R-devel mpich2 mpich2-devel`


Add a link so sprint can find the R headers


`ln -s /usr/include/R/ /usr/lib/R/include`


compile


`R CMD INSTALL sprint-0.2/`


## openmpi NOT WORKING YET ##
Install the packages that are needed

`$ yum install R R-devel openmpi openmpi-devel`

Put the R include files where the installer expects them

`ln -s /usr/include/R/ /usr/lib/R/include`

Add mpicc to the path

`export PATH=$PATH:/usr/lib/openmpi/bin/`

Add openmpi libs to the path

`export LD_LIBRARY_PATH=/usr/lib/openmpi/lib/:/usr/lib/openmpi/lib/openmpi/`

Compile

`R CMD INSTALL sprint-0.2/`

Gives this error

```
** testing if installed package can be loaded
/usr/lib/R/bin/exec/R: symbol lookup error: /usr/lib/openmpi/lib/openmpi/mca_paffinity_linux.so: undefined symbol: mca_base_param_reg_int
```


# Scientific Linux 5.5 64bit #
## The default version of R does not work ##
I think it is related to not being built with mpicc. Here are the steps if you want to try it.

`# yum install R-devel openmpi openmpi-devel opemmpi-libs`

`# mpi-selector --set openmpi-1.4-gcc-x86_64` or you could use

`# mpi-selector-menu`

`R CMD INSTALL sprint-0.2`

```

 *** caught segfault ***
address (nil), cause 'unknown'

Traceback:
 1: dyn.load(file, DLLpath = DLLpath, ...)
 2: library.dynam(lib, package, package.lib)
 3: loadNamespace(package, c(which.lib.loc, lib.loc), keep.source = keep.source)
 4: doTryCatch(return(expr), name, parentenv, handler)
 5: tryCatchOne(expr, names, parentenv, handlers[[1L]])
 6: tryCatchList(expr, classes, parentenv, handlers)
```
## Compiling R ##
This does work, and you dont need root access if mpi is installed.
`# yum install openmpi openmpi-devel opemmpi-libs`

`$ export CC=mpicc`

`$ wget http://ftp.heanet.ie/mirrors/cran.r-project.org/src/base/R-2/R-2.12.0.tar.gz`

`$ tar xvfz R-2.12.0.tar.gz`

`$ cd R-2.12.0`

`$ ./configure --prefix=$HOME/local/`

`$ make`

`$ make install`

Add this to your .bashrc
```
export MPI_HOME=$HOME/local/
export PATH=$HOME/local/bin/:$PATH
export LD_LIBRARY_PATH=$MPI_HOME/lib/:$LD_LIBRARY_PATH
```

`$ source .bashrc`

`$ R CMD INSTALL sprint-x.x`
