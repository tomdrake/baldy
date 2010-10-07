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

ddd <- function(data, indices, myi, myframe, myvector){
  x = data[indices]
  y = mean(myvector)
  z = sqrt(myi)
  tmp = var(myframe[,1])
  return(mean(x) + z * y - tmp)
}

test.ddd <- function(){
  set.seed(1)
  a = boot(faithful[,2] , ddd, R=100, myi=3, myframe=trees, myvector=discoveries)
  set.seed(1)
  b = pboot(faithful[,2] , ddd, R=100, myi=3, myframe=trees, myvector=discoveries)
  checkEquals(a,b,"Three variables passed via ...")
}

