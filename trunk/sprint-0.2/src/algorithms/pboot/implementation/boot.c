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


void SEXP2string(SEXP Sobject, char ** strobj);
int * pbootDecomp(int world, int r);

int boot(int scenario,...)
{
  int worldSize;
  int worldRank;
  int i,j,k, count;
  va_list ap;
  MPI_Status stat;

  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);


  int ltn;  // number of results the statistic returns
  int r;  // how many replications to perform
  SEXP varg; // a SEXP list of all the objects passed in the ... argument
  int lvarg; // number of variables passed in the ... to statistic
  int * ivarg;// index with the length of each string in the svarg
  char ** svarg; // array of strings each the deparse of a argument from the ...
  int * nr; // array to store the number of replications each node will perform
  char * data;
  char * statistic;  
  int ldata, lstatistic;
  double * in_array;

  // Get input variables common to all scenarios
  if (worldRank == 0) {
    va_start(ap, scenario);
    in_array = va_arg(ap, double*); // array to place the results back into
    r = va_arg(ap, int); 
    ltn = va_arg(ap, int); 
    varg = va_arg(ap, SEXP); 
    data = va_arg(ap, char*); // the data to call the statistic on
    statistic = va_arg(ap, char*); // the statistic function that is to be called
    // some useful calculations
    ldata = strlen(data);
    lstatistic = strlen(statistic);
    lvarg = LENGTH(varg);
    ivarg = (int *)malloc(sizeof(int) * lvarg ); 
    svarg = (char **)malloc(sizeof(char *) * lvarg );
    
    // sort out the variable argumenst of the statistic
    for(i=0;i<lvarg;i++){
      SEXP2string(VECTOR_ELT(varg,i),&svarg[i]);
      ivarg[i] = strlen(svarg[i]); 
    }
  }
  // broadcast variables common to all scenarios
  MPI_Bcast(&ltn, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&lstatistic, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&ldata, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&lvarg, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&scenario, 1, MPI_INT, 0, MPI_COMM_WORLD);
  nr =  pbootDecomp(worldSize, r); // the decomposition can be calculated rather than sent
  
  // slaves assign memory for data , varg and statistic strings
  if(worldRank > 0) {
    data = (char *)malloc((sizeof(char) * ldata) + 1);
    statistic = (char *)malloc((sizeof(char) * lstatistic) +1);
    ivarg = (int *)malloc(sizeof(int) * lvarg );
    svarg = (char **)malloc(sizeof(char *) * lvarg );
  }
  

  // send the actual strings
  MPI_Bcast(statistic, lstatistic+1, MPI_CHAR, 0, MPI_COMM_WORLD);
  MPI_Bcast(data, ldata+1, MPI_CHAR, 0, MPI_COMM_WORLD);
  
  // varg is more complex first get the index then loop to fill svarg
  MPI_Bcast(ivarg, lvarg, MPI_INT, 0, MPI_COMM_WORLD);
  for(i=0;i<lvarg;i++){
    if(worldRank > 0) svarg[i] = (char *)malloc((sizeof(char) * (ivarg[i]+1))); // allocate memmory on the slaves
    MPI_Bcast(svarg[i], ivarg[i]+1, MPI_CHAR, 0, MPI_COMM_WORLD);
  }
  free(ivarg);

  // parse the vargs and put into an array of SXP
  SEXP * SEXPvarg;
  SEXPvarg = (SEXP *)malloc(sizeof(SEXP *) * lvarg);
  for(i=0;i<lvarg;i++){
    PROTECT(SEXPvarg[i] =  eval(lang2(install("eval"),
            eval(lang4(install("parse"),mkString("") , mkString(""),mkString(svarg[i])),  R_GlobalEnv)
         ), R_GlobalEnv));
    free(svarg[i]);
  }
  free(svarg);

  // each ranks local results
  double * myresults ;
  myresults = (double *)malloc(sizeof(double) * nr[worldRank] * ltn);
  
  switch(scenario) {
    case 1:
      printf("NOT IMPLEMENTED YET ---- Scenario: 1\n");
      for(i=0; i<nr[worldRank]*ltn;i++){
        myresults[i] = 0.1 + worldRank; 
      }
      break;// end of scenario 1
    case 2:
      printf("NOT IMPLEMENTED YET ---- Scenario: 2\n");
      for(i=0; i<nr[worldRank]*ltn;i++){
        myresults[i] = i; 
      }
      break;// end of scenario 2
    case 3:
      printf("NOT IMPLEMENTED YET ---- Scenario: 3\n");
      for(i=0; i<nr[worldRank]*ltn;i++){
        myresults[i] = i; 
      }
      break;// end of scenario 3
    case 4:
      printf("NOT IMPLEMENTED YET ---- Scenario: 4\n");
      for(i=0; i<nr[worldRank]*ltn;i++){
        myresults[i] = i; 
      }
      break;// end of scenario 4
    case 5:
      printf("NOT IMPLEMENTED YET ---- Scenario: 5\n");
      for(i=0; i<nr[worldRank]*ltn;i++){
        myresults[i] = i; 
      }
      break;// end of scenario 5
    case 6:
      printf("NOT IMPLEMENTED YET ---- Scenario: 6\n");
      for(i=0; i<nr[worldRank]*ltn;i++){
        myresults[i] = i; 
      }
      break;// end of scenario 6
    case 7:
      printf("NOT IMPLEMENTED YET ---- Scenario: 7\n");
      for(i=0; i<nr[worldRank]*ltn;i++){
        myresults[i] = i; 
      }
      break;// end of scenario 7
    case 8: /*
    c = va_arg(ap, int); 
  MPI_Bcast(&c, 1, MPI_INT, 0, MPI_COMM_WORLD);
    ind = va_arg(ap, int *); 
      char * data; // the name of the data object
      int * ind; // the SEXP containing the indices
      int c;  // how many columns in the indices
      int ldata, lstatistic; // the length of strings

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
  SEXP rind, t, s,  result_array, Sdata, pdata;
  myresults = (double *)malloc(sizeof(double) * nr[worldRank] * ltn);
  PROTECT(rind = allocVector(INTSXP,c)); // will be used to store each replications ind
  PROTECT(t = s = allocList(3+lvarg));
         SET_TYPEOF(s, LANGSXP);
  PROTECT(result_array);

  // The power of R is that the data could either be an object ot an expression. So we need to
  // eval the data in case its an expression this creates a SEXP in C rather than using the
  // install in the way we access the statistic function
  PROTECT(pdata = eval(lang4(install("parse"),mkString("") , mkString(""),mkString(data)),  R_GlobalEnv));
  PROTECT(Sdata = eval(lang2(install("eval"),pdata), R_GlobalEnv));
  count = 0;
  int index = 0;
  for(i=0; i<nr[worldRank];i++){
    // build the indices for this replication
    for(j=0;j<c;j++){
      INTEGER(rind)[j] = myind[count];
      count++;
    }

    // build the expression for this replication
    //e = lang4(install(statistic),Sdata, rind, ScalarReal(50));
    t = s; // jump back to the start of the object
    SETCAR(t, install(statistic)); t = CDR(t);
    SETCAR(t, Sdata); t = CDR(t);
    SETCAR(t, rind); t = CDR(t);
    for(k=0; k<lvarg;k++){ // add the varg SEXP objects (the ... ones)
      SETCAR(t, SEXPvarg[k]);
      t = CDR(t);
    }
    // preform the eval
    result_array = eval(s, R_GlobalEnv);
    // get the results out of the REALSXP vector
    for (k=0; k<ltn;k++){
      myresults[index] =REAL(result_array)[k];
      index++;
    }
  }
  free(SEXPvarg);
  free(myind);

*/
      break; // end of scenario 8
    default:
      break;
  }// end of switch

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
  free(nr);
  free(myresults);
  UNPROTECT(0+lvarg);
  return 0;
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


void SEXP2string(SEXP Sobject, char ** strobj){
  SEXP dSobject;
  int i, size=0;
  PROTECT(dSobject = eval(lang3(install("deparse"),Sobject,ScalarInteger(500)),R_GlobalEnv));

  int lobject = LENGTH(dSobject); // how many rows in the deparsed object

  // first find out how big the object is
  for(i=0;i<lobject; i++){
    size += strlen(CHAR(STRING_ELT(dSobject,i)));
  }
  // malloc the memory then fill it up
  *strobj = calloc(size+1, sizeof(char));

  for(i=0;i<lobject; i++){
    strcat(*strobj, CHAR(STRING_ELT(dSobject,i)));
  }

  UNPROTECT(1);
}
  
int * pbootDecomp(int world, int r){
  // calculate how many replications each node will perform
  // dont forget to free the pointer!
  //========================================================
  int i;
  int * nr = (int *)malloc(sizeof(int) * world) ;
  //how many replications will each node perform
  for(i=0; i<world; i++){
    nr[i] = r/world;
  }
  // spread the remainder over the ranks (skipping the master as it has enough to do already)
  int mod = r % world;
  for(i=1; i<mod+1; i++){
   nr[i]++;
  }
  return(nr);
}           
