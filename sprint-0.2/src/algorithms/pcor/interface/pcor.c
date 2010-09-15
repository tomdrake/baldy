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

extern int correlation(int n,...);

/* **************************************************************** *
 *  Accepts information from R, signals the need for a corelation,  *
 *  gets response and returns it.                                   *
 * **************************************************************** */
SEXP pcor(SEXP data, SEXP out_file, SEXP distance)
{
    SEXP result = NULL;
    SEXP dataSize;

    int width, height, worldSize=-1;
    enum commandCodes commandCode;
    int response = 0;
    char *file_out;

    // Check that MPI is initialized
    MPI_Initialized(&response);
    if (response) {
        DEBUG("\nMPI is init'ed in pcor\n");
    } else {
        DEBUG("\nMPI is NOT init'ed in pcor\n");
        PROTECT(result = NEW_INTEGER(1));
        INTEGER(result)[0] = -1;
        UNPROTECT(1);

        return result;
    }

    // Check how many worker processes exist.
    // If the master is alone then abort the process
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    if ( worldSize == 1 ) {
        PROTECT(result = NEW_INTEGER(1));
        INTEGER(result)[0] = -2;
        UNPROTECT(1);

        return result;
    }

    // Perform checks on the input data
    if (!IS_VECTOR(data)) {
        ERR("\npcor.c accepts only matrices\n");
        result = NEW_INTEGER(1);
        INTEGER(result)[0] = -1;
        return result;
    }

    if (!IS_NUMERIC(data) && !IS_INTEGER(data)) {
        ERR("\npcor.c accepts only numeric matrices\n");
        result = NEW_INTEGER(1);
        INTEGER(result)[0] = -1;
        return result;
    }

    // Get number of genes and samples
    // Also get the name of the input and output files
    dataSize = GET_DIM(data);
    width = INTEGER_POINTER(dataSize)[0];
    height = INTEGER_POINTER(dataSize)[1];
    file_out = (char *)CHAR(STRING_ELT( out_file, 0 ));

    // Broadcast command to other processors
    commandCode = PCOR;
    MPI_Bcast(&commandCode, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

    // Call the correlation function
    if ( LOGICAL_POINTER(distance)[0] == TRUE )
        response = correlation(5, NUMERIC_POINTER(AS_NUMERIC(data)), width, height, file_out, 1);
    else
        response = correlation(5, NUMERIC_POINTER(AS_NUMERIC(data)), width, height, file_out, 0);

    PROTECT(result = NEW_INTEGER(1));
    INTEGER(result)[0] = response;
    UNPROTECT(1);

    return result;
}

