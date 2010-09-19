##########################################################################
#                                                                        #
#  SPRINT: Simple Parallel R INTerface                                   #
#  Copyright © 2008,2009 The University of Edinburgh                     #
#                                                                        #
#  This program is free software: you can redistribute it and/or modify  #
#  it under the terms of the GNU General Public License as published by  #
#  the Free Software Foundation, either version 3 of the License, or     #
#  any later version.                                                    #
#                                                                        #
#  This program is distributed in the hope that it will be useful,       #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
#  GNU General Public License for more details.                          #
#                                                                        #
#  You should have received a copy of the GNU General Public License     #
#  along with this program. If not, see <http://www.gnu.or/licenses/>.   #
#                                                                        #
##########################################################################

# This stub function simply calls down to a stub in the library.
pboot <- function (data, statistic, R, sim = "ordinary", stype = "i", 
    strata = rep(1, n), L = NULL, m = 0, weights = NULL, ran.gen = function(d, 
        p) d, mle = NULL, simple = FALSE, ...) 
{
    call <- match.call()
    if (simple && (sim != "ordinary" || stype != "i" || sum(m))) {
        warning("'simple=TRUE' is only valid for 'sim=\"ordinary\", stype=\"i\", n=0, so ignored")
        simple <- FALSE
    }
    if (!exists(".Random.seed", envir = .GlobalEnv, inherits = FALSE)) 
        runif(1)
    seed <- get(".Random.seed", envir = .GlobalEnv, inherits = FALSE)
    n <- NROW(data)
    if ((n == 0) || is.null(n)) 
        stop("no data in call to boot")
    temp.str <- strata
    strata <- tapply(seq_len(n), as.numeric(strata))
    if (sim != "parametric") {
        if ((sim == "antithetic") && is.null(L)) 
            L <- empinf(data = data, statistic = statistic, stype = stype, 
                strata = strata, ...)
        if (sim != "ordinary") 
            m <- 0
        else if (any(m < 0)) 
            stop("negative value of m supplied")
        if ((length(m) != 1L) && (length(m) != length(table(strata)))) 
            stop("length of m incompatible with strata")
        if ((sim == "ordinary") || (sim == "balanced")) {
            if (boot:::isMatrix(weights) && (nrow(weights) != length(R))) 
                stop("dimensions of R and weights do not match")
        }
        else weights <- NULL
        if (!is.null(weights)) 
            weights <- t(apply(matrix(weights, n, length(R), 
                byrow = TRUE), 2, normalize, strata))
        if (!simple) 
            i <- boot:::index.array(n, R, sim, strata, m, L, weights)
        if (stype == "f") 
            original <- rep(1, n)
        else if (stype == "w") {
            ns <- tabulate(strata)[strata]
            original <- 1/ns
        }
        else original <- seq_len(n)
        if (sum(m) > 0) {
            t0 <- statistic(data, original, rep(1, sum(m)), ...)
            lt0 <- length(t0)
        }
        else {
            t0 <- statistic(data, original, ...)
            lt0 <- length(t0)
        }
    }
    else {
        t0 <- statistic(data, ...)
        lt0 <- length(t0)
    }
    t.star <- matrix(NA, sum(R), lt0)
    pred.i <- NULL
    if (sim == "parametric") {
        # loop 1
        for (r in seq_len(R)) t.star[r, ] <- statistic(ran.gen(data, 
            mle), ...)
    }
    else {
        if (!simple && ncol(i) > n) {
            pred.i <- as.matrix(i[, (n + 1L):ncol(i)])
            i <- i[, seq_len(n)]
        }
        if (stype == "f") {
            f <- freq.array(i)
            if (sum(m) == 0) 
                # loop 2
                for (r in seq_len(sum(R))) t.star[r, ] <- statistic(data, 
                  f[r, ], ...)
            # loop 3 
            else for (r in seq_len(sum(R))) t.star[r, ] <- statistic(data, 
                f[r, ], pred.i[r, ], ...)
        }
        else if (stype == "w") {
            f <- freq.array(i)
            if (sum(m) == 0) 
                # loop 4
                for (r in seq_len(sum(R))) t.star[r, ] <- statistic(data, 
                  f[r, ]/ns, ...)
            # loop 5
            else for (r in seq_len(sum(R))) t.star[r, ] <- statistic(data, 
                f[r, ]/ns, pred.i[r, ], ...)
        }
        else if (sum(m) > 0) {
            # loop 6 
            for (r in seq_len(sum(R))) t.star[r, ] <- statistic(data, 
                i[r, ], pred.i[r, ], ...)
        }
        else if (simple) {
            # loop 7 
            print("loop 7")
	    for (r in seq_len(sum(R))) {
                inds <- boot:::index.array(n, 1, sim, strata, m, L, 
                  weights)
                t.star[r, ] <- statistic(data, inds, ...)
            }
        }
        else {
	    # loop 8 
            #for (r in seq_len(sum(R))) t.star[r, ] <- statistic(data, i[r, ], ...)
	    #print("loop 8")
	    strstatistic = substitute(statistic)
	    strdata = substitute(data)
	    t.star = .Call("pboot", strdata, strstatistic,i,lt0,... )
        }
    }
    dimnames(t.star) <- NULL
    if (is.null(weights)) 
        weights <- 1/tabulate(strata)[strata]
    pboot.return(sim, t0, t.star, temp.str, R, data, statistic, 
        stype, call, seed, L, m, pred.i, weights, ran.gen, mle)
}


# Exactly the same as boot.return except for minor changes to the call 
pboot.return <- function (sim, t0, t, strata, R, data, stat, stype, call, seed, 
    L, m, pred.i, weights, ran.gen, mle) 
{
    out <- list(t0 = t0, t = t, R = R, data = data, seed = seed, 
        statistic = stat, sim = sim, call = call)
    if (sim == "parametric") 
        out <- c(out, list(ran.gen = ran.gen, mle = mle))

    else if (sim == "antithetic") 
        out <- c(out, list(stype = stype, strata = strata, L = L))
    else if (sim == "ordinary") {
        if (sum(m) > 0) 
            out <- c(out, list(stype = stype, strata = strata, 
                weights = weights, pred.i = pred.i))
        else out <- c(out, list(stype = stype, strata = strata, 
            weights = weights))
    }
    else if (sim == "balanced") 
        out <- c(out, list(stype = stype, strata = strata, weights = weights))
    else out <- c(out, list(stype = stype, strata = strata))
    class(out) <- "boot"
    out$call[[1L]] = quote(boot) # call needs to use "boot" not "pboot" so that boot methods work
    out
}


#ptest <- function()
#{

    
   # return_val <- .Call("ptest")

    # If the value is numeric then it means that
    # MPI is not initialized and the function should abort
    # and return FALSE
    #if ( is.numeric(return_val) ) {
    #    warning(paste("MPI is not initialized. Function is aborted.\n"))
    #    return_val <- FALSE
    #}

    #return(return_val)
#}
