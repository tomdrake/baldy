ddddataframe <- function(data, indices, b){
  x = data[indices]
  return(x + mean(b[,1]))
}

library(boot)
library(RUnit)

 set.seed(27)
 a = boot(discoveries, ddddataframe, R=100, b=trees)
 set.seed(27)
library(sprint)
 b = pboot(discoveries, ddddataframe, R=100, b=trees)
pterminate()
 checkEquals(a,b,"dataframe passed via ...")

