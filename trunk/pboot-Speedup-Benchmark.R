library("boot")
ratio <- function(d, w)
     sum(d$x * w)/sum(d$u * w)


# Stratified resampling for the difference of means.  In this
# example we will look at the difference of means between the final
# two series in the gravity data.
diff.means <- function(d, f)
{    n <- nrow(d)
     gp1 <- 1:table(as.numeric(d$series))[1]
     m1 <- sum(d[gp1,1] * f[gp1])/sum(f[gp1])
     m2 <- sum(d[-gp1,1] * f[-gp1])/sum(f[-gp1])
     ss1 <- sum(d[gp1,1]^2 * f[gp1]) - 
            (m1 *  m1 * sum(f[gp1]))
     ss2 <- sum(d[-gp1,1]^2 * f[-gp1]) - 
            (m2 *  m2 * sum(f[-gp1]))
     c(m1-m2, (ss1+ss2)/(sum(f)-2))
}
grav1 <- gravity[as.numeric(gravity[,2])>=7,]


#  In this example we show the use of boot in a prediction from 
#  regression based on the nuclear data.  This example is taken 
#  from Example 6.8 of Davison and Hinkley (1997).  Notice also 
#  that two extra arguments to statistic are passed through boot.
nuke <- nuclear[,c(1,2,5,7,8,10,11)]
nuke.lm <- glm(log(cost)~date+log(cap)+ne+ ct+log(cum.n)+pt, data=nuke)
nuke.diag <- glm.diag(nuke.lm)
nuke.res <- nuke.diag$res*nuke.diag$sd
nuke.res <- nuke.res-mean(nuke.res)


#  We set up a new data frame with the data, the standardized 
#  residuals and the fitted values for use in the bootstrap.
nuke.data <- data.frame(nuke,resid=nuke.res,fit=fitted(nuke.lm))


#  Now we want a prediction of plant number 32 but at date 73.00
new.data <- data.frame(cost=1, date=73.00, cap=886, ne=0,
                       ct=0, cum.n=11, pt=1)
new.fit <- predict(nuke.lm, new.data)


nuke.fun <- function(dat, inds, i.pred, fit.pred, x.pred)
{
     assign(".inds", inds, envir=.GlobalEnv)
     lm.b <- glm(fit+resid[.inds] ~date+log(cap)+ne+ct+
          log(cum.n)+pt, data=dat)
     pred.b <- predict(lm.b,x.pred)
     remove(".inds", envir=.GlobalEnv)
     c(coef(lm.b), pred.b-(fit.pred+dat$resid[i.pred]))
}

#  Finally a parametric bootstrap.  For this example we shall look 
#  at the air-conditioning data.  In this example our aim is to test 
#  the hypothesis that the true value of the index is 1 (i.e. that 
#  the data come from an exponential distribution) against the 
#  alternative that the data come from a gamma distribution with
#  index not equal to 1.
air.fun <- function(data)
{    ybar <- mean(data$hours)
     para <- c(log(ybar),mean(log(data$hours)))
     ll <- function(k) {
          if (k <= 0) out <- 1e200 # not NA
          else out <- lgamma(k)-k*(log(k)-1-para[1]+para[2])
         out
     }
     khat <- nlm(ll,ybar^2/var(data$hours))$estimate
     c(ybar, khat)
}


air.rg <- function(data, mle)
#  Function to generate random exponential variates.  mle will contain 
#  the mean of the original data
{    out <- data
     out$hours <- rexp(nrow(out), 1/mle)
     out
}



library("RUnit")
library("sprint")
results = matrix(,4,3, dimnames = list(c("weight", "freq", "ordinary", "parametric"),
                               c("boot", "pboot", "speedup")))
set.seed(1)			     
results[1,1] = as.real(system.time(a <- boot(city, ratio, R=400000, stype="w"))[3])
set.seed(1)			     
results[1,2] = as.real(system.time(b <- pboot(city, ratio, R=400000, stype="w"))[3])
checkEquals(a,b,"The results do not match test 1.")
results[1,3] = results[1,1] / results[1,2]

set.seed(1)			     
results[2,1] = as.real(system.time(a <- boot(grav1, diff.means, R=20000, stype="f", strata=grav1[,2]))[3])
set.seed(1)			     
results[2,2] = as.real(system.time(b <- pboot(grav1, diff.means, R=20000, stype="f", strata=grav1[,2]))[3])
checkEquals(a,b,"The results do not match test 2.")
results[2,3] = results[2,1] / results[2,2]

set.seed(1)			     
results[3,1] = as.real(system.time(a <- boot(nuke.data, nuke.fun, R=2000, m=1, fit.pred=new.fit, x.pred=new.data))[3])
set.seed(1)			     
results[3,2] = as.real(system.time(b <- pboot(nuke.data, nuke.fun, R=2000, m=1, fit.pred=new.fit, x.pred=new.data))[3])
checkEquals(a,b,"The results do not match test 3.")
results[3,3] = results[3,1] / results[3,2]

set.seed(1)			     
results[4,1] = as.real(system.time(a <- boot(aircondit, air.fun, R=45000, sim="parametric", ran.gen=air.rg, mle=mean(aircondit$hours)))[3])
set.seed(1)			     
results[4,2] = as.real(system.time(b <- pboot(aircondit, air.fun, R=45000, sim="parametric", ran.gen=air.rg, mle=mean(aircondit$hours)))[3])
checkEquals(a,b,"The results do not match test 4.")
results[4,3] = results[4,1] / results[4,2]

results
pterminate()
