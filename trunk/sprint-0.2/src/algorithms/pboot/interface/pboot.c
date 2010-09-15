/**************************************************************************
 *                                                                        *
 *  SPRINT: Simple Parallel R INTerface                                   *
 *  Copyright © 2008,2009 The University of Edinburgh                     *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  any later version.                                                    *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program. If not, see <http://www.gnu.or/licenses/>.   *
 *                                                                        *
 **************************************************************************/

#include <Rdefines.h>
#include "../../../sprint.h"
#include "../../../functions.h"
extern int boot(int n,...);
SEXP getRow(int n, SEXP matrix);

/* ******************************************************** *
 *  The stub for the R side of a very simple test command.  *
 *  Simply issues the command and returns.                  *
 * ******************************************************** */

SEXP pbootL8(SEXP data, SEXP statistic, SEXP ind, SEXP lt0,... ){
  SEXP star;
  int r = nrows(ind); // replications are the number of rows in the index
  PROTECT(star = allocMatrix(REALSXP,r ,asInteger(lt0))); // t.star <- matrix(NA, sum(R), lt0)


  for(int i=0;i<r;i++){
    REAL(star)[i] = *REAL(eval(lang3(install(translateChar(PRINTNAME(statistic))), install(translateChar(PRINTNAME(data))),
                               getRow(i,ind)),R_GlobalEnv)); // ugly as sin. probably wont work with multiple results from a function
  }
  UNPROTECT(1);
  return(star); 
}

SEXP getRow(int n, SEXP matrix){
  // this function returns a single row of a SEXP matrix as a vector
  // only supports INTEGERS at the moment.
  int cols = ncols(matrix);
  int rows = nrows(matrix);
  SEXP row;
  PROTECT(row = allocVector(INTSXP,cols)); // this needs to support all types not just 
  int count = n; // start at the correct offset
  for(int i=0; i<rows; i++){
    INTEGER(row)[i] = INTEGER(matrix)[count];
    count += rows;
  }
  UNPROTECT(1);
  return row;
}

// proved way to complex, maybe next time
SEXP pbootL8BROKEN(SEXP strdata, SEXP strstatistic,
                            SEXP sim, SEXP simple, SEXP n, SEXP R, SEXP strata, SEXP m, SEXP L, SEXP weights, SEXP myindex,... ){

  /* if (sim != "parametric") {
            #  if (!simple) // simply is only allowed for ordinary so should never get here. 
            #   i <- boot:::index.array(n, R, sim, strata, m, L, weights)
            #if (!simple && ncol(i) > n) {
            #  pred.i <- as.matrix(i[, (n + 1L):ncol(i)])
            #  i <- i[, seq_len(n)]
            #} */
  //if ( CHAR(STRING_ELT(sim,0)) != "parametric") { // dont need this
   
   // Build the command to evaluate (this command)
   // will be used to generate the index "i"
   //--------------------------------------------
   SEXP i, e;
   PROTECT(i);
   PROTECT(e = allocVector(LANGSXP, 4));
   SETCAR(e, myindex);
   //SETCAR(e, install("boot::index.array")); // doesnt work cant change the namespace nicely. :(
   SETCADR(e, n);
   SETCADDR(e, R);
   SETCADDDR(e, sim);
   SETCDR(lastElt(e),lang4(strata,m,L,weights));
   i = eval(e, R_GlobalEnv);
  
   if(asLogical(simple) == 0 && nrows(i) > asInteger(n)){
     
   }

   //PrintValue(i);
  UNPROTECT(2);
  return(strdata);
}

SEXP pboot(SEXP strdata, SEXP strstatistic, ...)
{
    SEXP result;
    char **func_results;
    int i, response, worldSize;
    enum commandCodes commandCode;

    MPI_Initialized(&response);
    if (response) {
        DEBUG("MPI is init'ed in ptest\n");
    } else {

        DEBUG("MPI is NOT init'ed in ptest\n");
        PROTECT(result = NEW_INTEGER(1));
        INTEGER(result)[0] = -1;
        UNPROTECT(1);

        return result;
    }

    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    func_results = (char **)malloc(sizeof(char*) * worldSize);
    

    printf("in the interface\n");
    // broadcast command to other processors
    commandCode = PBOOT;
    MPI_Bcast(&commandCode, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

    response = boot(1, func_results);

    PROTECT(result = allocVector(STRSXP, worldSize));

    for(i=0; i < worldSize; i++) {
        // add message to the response vector
        SET_STRING_ELT(result, i, mkChar(func_results[i]));
        free(func_results[i]);
    }
    free(func_results);

    UNPROTECT(1);
    return result;

}

