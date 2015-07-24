/* MEX file for random seed setting for other mex files
 *
 * Call: mexSeed(<seed>); // seed parameter is optional
 */

#include <stdlib.h>
#include "mex.h"

void mexFunction(int nlhs, mxArray *plhs[],
    int nrhs, const mxArray *prhs[]) {
    
    srand(time(NULL));
    if(nrhs != 0){
        srand(mxGetScalar(prhs[0]));
    }
}