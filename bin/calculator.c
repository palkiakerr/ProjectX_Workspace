#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<gsl/gsl_linalg.h>
#include<gsl/gsl_splinalg.h>
////////// Function Prototypes //////////

int readboundary(double tgt[], char src[50], int boundaryflag[]);

int solve(int dimension, double boundary[], int boundaryflag[]);
int sparse_solve(int dimension, double boundary[], int boundaryflag[], int threshold);
int jacobi(int dimension, double boundary[], int boundaryflag[], float tolerance);

/////////////////////////////////////////


int main(int argc, char *argv[]){
	

	// Basic inputs from arguments, matrix size and filename
	int dimension;
	char filename[50];
	int maxiter;
	float tolerance;
	sscanf(argv[1], "%d", &dimension);
	sscanf(argv[2], "%s", filename);
	sscanf(argv[3], "%f", &tolerance);



	// Create and fill matrix, we use the naming convention for a normal
	// matrix problem i.e. Ax=b
	double b[dimension*dimension];

	// Secondary matrix to hold information on wether or not each entry
	// is a boundary condition and should be skipped in calulation
	int boundaryflag[dimension*dimension];

	// Reads in the file containing the problem
	readboundary(b, filename, boundaryflag);

	// Passes the matrix system (Ax=b) to the solver
	//solve(dimension, b, boundaryflag);
	//sparse_solve(dimension,b , boundaryflag, maxiter);
	jacobi(dimension,b , boundaryflag, tolerance);

	return 0;
}



int readboundary(double b[], char src[50], int boundaryflag[]){

	// b is the matrix (Ax=b) that holds information about the boundary conditions
	// src is the string holding file location


	// Open src and check for bad file
	FILE *fp;
	fp = fopen(src, "r");
	if ( fp == NULL )
    {
        printf("Could not open file\n");
    }


    // Read the file's characters into the tgt array, skipping newlines
    // tgt size must be sufficient (defined outsie this function)
   	char c;
   	int count = 0;
   	

    while(1){

    	// Read in the problem once character at a time
    	c = fgetc(fp);
    	
    	// Ignore newlines and stop ant the end of the file
    	if(c==EOF){break;}
    	if(c=='\n'){continue;}

    	// Remember which zeros and numbers are boundary conditions using a second vector
    	// called boundaryflag
    	if(c=='.'){
    		b[count]=0;
    		boundaryflag[count]=0;
    	}

    	// Shift ASCII table to turn che character for a number into the number itself
    	else{
    		b[count] = c-48;
    		boundaryflag[count] = 1;
    	}

    	count++;

    }

    fclose(fp);

   	// Return sucess
   	return 0;

}


int solve(int dim, double bpass[], int boundaryflag[]){

	// Create/Convert arrays into gsl matrix types
	// Create coefficients array and turn it into a matrix
	gsl_matrix *A = gsl_matrix_alloc(dim*dim,dim*dim);

	// Create b vector from passed array (boundary conditions)
	gsl_vector_view b = gsl_vector_view_array(bpass, dim*dim);

	// Create empty "x" vector
	gsl_vector *x = gsl_vector_alloc(dim*dim);


	// Now need to create the correct forms of the matrices to solve the problem
	// as the current matrices are empty/garbage data. See p1029 of numerical
	// recipies for what the matrix should look like (tri-diagonal with fringes)
	for(int i = 0; i < (dim*dim); i++){

		if(boundaryflag[i]==1){
			gsl_matrix_set(A,i,i,1);
			continue;
		}
	
		gsl_matrix_set(A,i,i,4);
	
		
		if(i%dim != 0){
			gsl_matrix_set(A,i,i+1,-1);
		}

		if(i%dim != 1){
			gsl_matrix_set(A,i,i-1,-1);
		}

		if(i>=dim+1){
			gsl_matrix_set(A,i,i-dim,-1);
		}

		if(i<=(dim*dim -dim-1)){
			gsl_matrix_set(A,i,i+dim,-1);
		}
	}




	// Now solve the matrix sysstem Ax=b using LU Decomposition (the
	// direct matrix method)
	int s;

    gsl_permutation * p = gsl_permutation_alloc (dim*dim);

    gsl_linalg_LU_decomp (A, p, &s);

    gsl_linalg_LU_solve (A, p, &b.vector, x);

    gsl_vector_fprintf (stdout, x, "%g");

    gsl_permutation_free (p);
    gsl_vector_free (x);
    return 0;
 
}



int sparse_solve(int dim, double bpass[], int boundaryflag[], int threshold){

	// Create/Convert arrays into gsl matrix types
	// Create coefficients array and turn it into a matrix
	gsl_spmatrix *A = gsl_spmatrix_alloc(dim*dim,dim*dim);

	// Create b vector from passed array (boundary conditions)
	gsl_vector_view b = gsl_vector_view_array(bpass, dim*dim);

	// Create empty "x" vector
	gsl_vector *x = gsl_vector_calloc(dim*dim);


	// Now need to create the correct forms of the matrices to solve the problem
	// as the current matrices are empty/garbage data. See p1029 of numerical
	// recipies for what the matrix should look like (tri-diagonal with fringes)
	for(int i = 0; i < (dim*dim); i++){

		if(boundaryflag[i]==1){
			gsl_spmatrix_set(A,i,i,1);
			continue;
		}
	
		gsl_spmatrix_set(A,i,i,4);
	
		
		if(i%dim != 0){
			gsl_spmatrix_set(A,i,i+1,-1);
		}

		if(i%dim != 1){
			gsl_spmatrix_set(A,i,i-1,-1);
		}

		if(i>=dim+1){
			gsl_spmatrix_set(A,i,i-dim,-1);
		}

		if(i<=(dim*dim -dim-1)){
			gsl_spmatrix_set(A,i,i+dim,-1);
		}
	}




	// Now solve the sparse linear system using iterativ methods
	gsl_splinalg_itersolve *solv_space=gsl_splinalg_itersolve_alloc(gsl_splinalg_itersolve_gmres, dim*dim, dim*dim);

	gsl_splinalg_itersolve_iterate(A, &b.vector, 0, x, solv_space);
	
	gsl_splinalg_itersolve_free(solv_space);
	gsl_vector_fprintf(stdout, x, "%g");

	/*for(int county=0; county<dim;county++){
		for(int countx=0;countx<dim;countx++){
			printf("%d      %d      %g\n",countx+1,county+1,gsl_vector_get(x,county*dim +countx));
		}
		
	}*/
    return 0;
 
}

int jacobi(int dim, double b[], int boundaryflag[], float tolerance){
	
	// Initalist calcu9lation arrays
	int boundary[dim][dim];
	double x_1[dim][dim];
	double x[dim][dim];
	float err;

	// Convert 1D to 2D
	for(int i=0;i<dim;i++){
		for(int j=0;j<dim;j++){
			x[i][j]=b[i*dim+j];
			boundary[i][j]=boundaryflag[i*dim+j];
		}
	}


	// Jacobi Relaxation Method
	for(int iters = 0; iters < 500000; iters++){
		err = 0;

		for(int i=0;i<dim;i++){
			for(int j=0;j<dim;j++){

				// Check for Boundary
				if(boundary[i][j]==1){x_1[i][j]=x[i][j];continue;}

				// Top Left
				if(i==0 && j==0){x_1[i][j] = 0.5*(x[i+1][j]+x[i][j+1]);continue;}


				// Bottom Right
				if(i==dim-1 && j==dim-1){x_1[i-1][j] = 0.5*(x[i+1][j]+x[i][j-1]);continue;}


				// Bottom Left
				if(i==dim-1 && j==0){x_1[i][j] = 0.5*(x[i-1][j]+x[i][j+1]);continue;}

				// Bottom Right
				if(i==0 && j==dim-1){x_1[i+1][j] = 0.5*(x[i][j]+x[i][j-1]);continue;}


				// Top Edge
				if(i==0){x_1[i][j] = 0.333333333333*(x[i+1][j] + x[i][j-1] + x[i][j+1]);continue;}

				// Bottom Edge
				if(i==dim-1){x_1[i][j] = 0.333333333333*(x[i-1][j] + x[i][j-1] + x[i][j+1]);continue;}

				// Left Edge
				if(j==0){x_1[i][j] = 0.333333333333*(x[i+1][j] + x[i][j+1] + x[i-1][j]);continue;}


				// Right Edge
				if(i==dim-1){x_1[i][j] = 0.333333333333*(x[i+1][j] + x[i-1][j] + x[i][j-1]);continue;}


				x_1[i][j] = 0.25*(x[i+1][j] + x[i-1][j] + x[i][j+1] + x[i][j-1]);

				err += x_1[i][j] - x[i][j];
			}
		}

		memcpy(x,x_1,dim*dim*8);

		if(err < tolerance){break;}
	}


for(int i=0;i<dim;i++){
		for(int j=0;j<dim;j++){
			printf("%f\n",x[i][j]);
		}
		//printf("\n");
	}

}