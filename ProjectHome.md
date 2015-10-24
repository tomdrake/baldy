# pboot Parallel bootstrapping for R using SPRINT #
Using the [SPRINT](http://www.r-sprint.org/) framework, I am creating a version of the standard R boot function that runs in parallel. Allowing the function to take advantage of multi core processors, clusters and HPC systems.

The function aims to be an identical replacement, taking the same arguments and returning the same results.

## Initial speedup results ##
> Benchmarks are based on the examples given in the boot [manual](http://stat.ethz.ch/R-manual/R-devel/library/boot/html/boot.html) but with replications increased to give serial version run times of roughly 40 seconds.

Scientific Linux VM with default open-mpi.
```
[laurence@localhost baldy]$ mpiexec -n 6 R --slave -f pboot-Speedup-Benchmark.R
             boot  pboot  speedup
weight     56.906 33.440 1.701734
freq       57.166 12.399 4.610533
ordinary   55.138 11.786 4.678262
parametric 50.673 21.890 2.314893

```

Run on an Ubuntu 10 Virtual Machine default MPICH2, using 6 cores.
```
laurence@gamma:~/baldy$ mpiexec -n 6 R --slave -f pboot-Speedup-Benchmark.R
             boot  pboot  speedup
weight     42.140 24.392 1.727616
freq       43.065  8.401 5.126175
ordinary   41.654  7.502 5.552386
parametric 36.993 12.975 2.851098
```

On a Centrino Duo running Ubuntu 10.04 with compiled openMPI and R.
```
[~/code/baldy] $ mpiexec -n 2 R --slave -f pboot-Speedup-Benchmark.R 
             boot  pboot  speedup
weight     36.391 23.446 1.552120
freq       32.570 17.453 1.866155
ordinary   32.800 17.840 1.838565
parametric 27.722 17.705 1.565772

```



<wiki:gadget url="http://google-code-feed-gadget.googlecode.com/svn/trunk/gadget.xml" up\_feeds="https://code.google.com/feeds/p/baldy/svnchanges/basic" border="0"/>