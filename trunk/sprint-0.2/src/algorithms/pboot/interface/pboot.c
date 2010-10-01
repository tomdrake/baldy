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

/* ******************************************************** *
 *  The stub for the R side of a very simple test command.  *
 *  Simply issues the command and returns.                  *
 * ******************************************************** */

SEXP pboot(SEXP data, SEXP statistic, SEXP ind, SEXP lt0,... ){
    SEXP result;
    double *func_results;
    int i, j, response, worldSize;
    enum commandCodes commandCode;
    int r = nrows(ind); // replications are the number of rows in the index
    int c = ncols(ind); // replications are the number of columns in the index
    int ltn = asInteger(lt0); // the number of results the statistical function returns   
 
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

    func_results = (double *)malloc(sizeof(double) * r * ltn); // this needs to be changed to support matrix results

    // broadcast command to other processors
    commandCode = PBOOT;
    MPI_Bcast(&commandCode, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

    // convert the ind from (horrible) SEXP format to C array
    int * cind;
    cind = (int *)malloc(sizeof(int) * r * c);
    int count = 0;
    int cindx = 0; // position in the cind
    for(i=0;i<r;i++){
      cindx = i;
      for(j=0;j<c;j++){
        cind[count] = INTEGER(ind)[cindx];
	count++;
	cindx += r;
      }
    }
    response = boot(1, func_results, translateChar(PRINTNAME(data)), translateChar(PRINTNAME(statistic)), ltn, r, c, cind);

    PROTECT(result = allocMatrix(REALSXP,r ,ltn)); // t.star <- matrix(NA, sum(R), lt0)
    
 
    // jump through array to create SEXP in correct order
    count = 0;
    cindx = 0;
    for(i=0; i < ltn; i++) {
      cindx = i;
      for (j=0; j <r; j++){
        REAL(result)[count]= func_results[cindx];
        count++;
        cindx += ltn;
      }
    }
    
    //PrintValue(result);
    free(func_results);
    UNPROTECT(1); 
    return result;
}
