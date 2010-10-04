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

#include <stdio.h>
#include <stdarg.h>
#include <mpi.h>
#include <string.h>

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <Rdefines.h>

/**
 * The test command simple prints out rank and communicator size.
 **/

int boot(int n,...)
{
  int worldSize;
  int worldRank;
  int i,j,k;
  va_list ap;
  MPI_Status stat;

  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
  
  //printf("I am rank %i \n", worldRank);
  // Parse arguments and broadcast those needed.
  // ======================================================================

  char * data; // the name of the data object
  char * statistic; // the string of the statistic name
  int * ind; // the SEXP containing the indices
  int ltn;  // number of results the statistic returns
  int c;  // how many columns in the indices
  int r;  // how many replications to perform
  int *nr; // array to store the number of replications each node will perform

  int ldata, lstatistic; // the length of strings
  int count;
  double *in_array; // this is the array the results are passed out via, r length
  
  // Get input variables
  if (worldRank == 0) {
    va_start(ap, n);
    in_array = va_arg(ap, double*); // array to place the results back into
    data = va_arg(ap, char*); // the data to call the statistic on
    statistic = va_arg(ap, char*); // the statistic function that is to be called
    ltn = va_arg(ap, int); // the indices to use
    r = va_arg(ap, int); // the indices to use
    c = va_arg(ap, int); // the indices to use
    ind = va_arg(ap, int *); // the indices to use
    va_end(ap);
    // some useful calculations
    ldata = strlen(data);
    lstatistic = strlen(statistic);
  }
  
  // send variables to slaves
  MPI_Bcast(&ltn, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&c, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&lstatistic, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&ldata, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // slaves assign memory for data and statistic strings
  if(worldRank > 0) {
    data = (char *)malloc((sizeof(char) * ldata) + 1);
    statistic = (char *)malloc((sizeof(char) * lstatistic) +1);
  }
  // send the actual strings
  MPI_Bcast(statistic, lstatistic+1, MPI_CHAR, 0, MPI_COMM_WORLD);
  MPI_Bcast(data, ldata+1, MPI_CHAR, 0, MPI_COMM_WORLD);

  //printf("rank: %i stat: %s data: %s rows: %i cols: %i\n",worldRank,  statistic, data, r, c);
  
  // calculate how many replications each node will perform
  //================================================
  nr = (int *)malloc(sizeof(int) * worldSize) ;
  //how many replications will each node perform 
  for(i=0; i<worldSize; i++){
    nr[i] = r/worldSize;
  }
  // spread the remainder over the ranks (skipping the master as it has enough to do already)
  int mod = r % worldSize;
  for(i=1; i<mod+1; i++){
   nr[i]++;
  }
  

  // allocate array for my indices
  int * myind; 
  myind = (int *)malloc(sizeof(int) * c * nr[worldRank]) ;

  if(worldRank == 0){ // send indice decomposition to nodes
    count = nr[0] * c; // skip those for master
    for(i=0; i<(nr[0] *c);i++) myind[i] = ind[i];
    for(i=1; i<worldSize;i++){
      MPI_Send(&ind[count], nr[i] * c, MPI_INT, i, 4, MPI_COMM_WORLD);
      count += (nr[i] * c);
    }
  } else { // receive my indices decomposition
    MPI_Recv(myind, nr[worldRank] * c, MPI_INT, 0, 4, MPI_COMM_WORLD,&stat);
  }

  // perform my ranks replications
  double * myresults; 
  SEXP rind, e, result_array, Sdata, pdata;
  myresults = (double *)malloc(sizeof(double) * nr[worldRank] * ltn);
  PROTECT(rind = allocVector(INTSXP,c)); // will be used to store each replications ind 
  PROTECT(e = allocVector(LANGSXP, 1));
  PROTECT(result_array);
  // The power of R is that the data could either be an object ot an expression. So we need to 
  // eval the data in case its an expression this creates a SEXP in C rather than using the 
  // install in the way we access the statistic function
  PROTECT(pdata = eval(lang4(install("parse"),mkString("") , mkString(""),mkString(data)),  R_GlobalEnv));
  PROTECT(Sdata = eval(lang2(install("eval"),pdata), R_GlobalEnv));
  count = 0;
  int index = 0;
  for(i=0; i<nr[worldRank];i++){
    for(j=0;j<c;j++){
      INTEGER(rind)[j] = myind[count];
      count++;
    }
    // preform the eval
    //PrintValue(rind);
    e = lang3(install(statistic),Sdata, rind);
    result_array = eval(e, R_GlobalEnv);
    //PrintValue(result_array);
    // get the results out of the REALSXP vector
    for (k=0; k<ltn;k++){
      myresults[index] =REAL(result_array)[k];
      index++;
    }
  }

  // get back results
  if(worldRank == 0){
    for(i=0;i<nr[worldRank]*ltn;i++) in_array[i] = myresults[i];
    count = nr[0]*ltn; 
    for(i=1;i<worldSize;i++){
      MPI_Recv(&in_array[count], nr[i]*ltn, MPI_DOUBLE, i, 5, MPI_COMM_WORLD,&stat);
      count += nr[i]*ltn;
    }
  } else {  // send results
    MPI_Send(myresults, nr[worldRank]*ltn, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);
  } 

  UNPROTECT(5);
  return 0;
}


/* SEXP star;


  for(int i=0;i<r;i++){
    REAL(star)[i] = *REAL(eval(lang3(install(translateChar(PRINTNAME(statistic))), install(translateChar(PRINTNAME(data))),
                               getRow(i,ind)),R_GlobalEnv)); // ugly as sin. probably wont work with multiple results from a function
  }
  UNPROTECT(1);
  return(star);
} */

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

