dotdotdot <- function(data, indices, b,pi){
  x = data[indices]
  y = mean(x) + b - pi;
  temp = b 
  return(y)
}


test.dotdotdot <-function(){

 set.seed(27)
 b = boot(discoveries, dotdotdot, R=100, b=33,pi=63)
 set.seed(27)
 a = pboot(discoveries, dotdotdot, R=100, b=33,pi=63)
 checkEquals(a,b,"Two extra variables passed via ...")
}
