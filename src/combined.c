#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_splinalg.h>
#include <time.h>
#include <unistd.h>

int type;
int dimension;
int num_iterations;
float matrix[1000][1000];
FILE *f;

int readboundary(double tgt[], int boundaryflag[], int dim);
int solve(int dimension, double boundary[], int boundaryflag[]);
int sparse_solve(int dimension, double boundary[], int boundaryflag[], int itermax);
int jacobi(int dimension, double boundary[], int boundaryflag[], int num_iterations);
int calc_handle(int dimension, int num_iterations);

typedef struct {
  GtkWidget *w_comboboxtext_options;
  GtkWidget *w_lbl_sel_text;
  GtkWidget *w_sbtn_quantity;
  GtkWidget *w_sbtn_iterations;
  GtkWidget *w_btn_matrix;
  GtkWidget *w_lbl_user1;
  GtkWidget *w_lbl_user2;
  GtkWidget *w_lbl_user3;
  GtkWidget *w_lbl_user4;
  GtkWidget *w_lbl_user5;
  GtkWidget *w_lbl_user6;
  GtkWidget *w_sbtn_user1;
  GtkWidget *w_sbtn_user2;
  GtkWidget *w_sbtn_user3;
  GtkWidget *w_sbtn_user4;
  GtkWidget *w_sbtn_user5;
  GtkWidget *w_tbtn_user;
  GtkWidget *w_btn_add_object;
  GtkWidget *w_btn_generate;
  GtkWidget *w_image_schematic;
} app_widgets;

int main(int argc, char *argv[])
{
    
    GtkBuilder      *builder; 
    GtkWidget       *window;
    app_widgets     *widgets = g_slice_new(app_widgets);

    //initialise GTK builder and window
    gtk_init(&argc, &argv); 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window_main.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

    //initialise widgets
    widgets->w_comboboxtext_options = GTK_WIDGET(gtk_builder_get_object(builder, "comboboxtext_options"));
    widgets->w_lbl_sel_text = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_sel_text"));
    widgets->w_sbtn_quantity = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_quantity"));
    widgets->w_sbtn_iterations = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_iterations"));
    widgets->w_btn_matrix = GTK_WIDGET(gtk_builder_get_object(builder, "btn_matrix"));
    widgets->w_lbl_user1 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user1"));
    widgets->w_lbl_user2 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user2"));
    widgets->w_lbl_user3 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user3"));
    widgets->w_lbl_user4 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user4"));
    widgets->w_lbl_user5 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user5"));
    widgets->w_lbl_user6 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user6"));
    widgets->w_sbtn_user1 = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_user1"));
    widgets->w_sbtn_user2 = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_user2"));
    widgets->w_sbtn_user3 = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_user3"));
    widgets->w_sbtn_user4 = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_user4"));
    widgets->w_sbtn_user5 = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_user5"));
    widgets->w_tbtn_user = GTK_WIDGET(gtk_builder_get_object(builder, "tbtn_user"));
    widgets->w_btn_add_object = GTK_WIDGET(gtk_builder_get_object(builder, "btn_add_object"));
    widgets->w_btn_generate = GTK_WIDGET(gtk_builder_get_object(builder, "btn_generate"));
    widgets->w_image_schematic = GTK_WIDGET(gtk_builder_get_object(builder, "image_schematic"));

    //connect builder and signals
    gtk_builder_connect_signals(builder, widgets);
    g_object_unref(builder);
    gtk_widget_show(window);                
    gtk_main();
    g_slice_free(app_widgets, widgets);
 
    return 0;
}
 
// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}

//Show Option button clicked by user
void on_btn_show_option_clicked(GtkButton *button, app_widgets *app_wdgts) {

  gchar *item_text = 0; //selected item text from text combo box
  int object_type;
 
  //get selected item text from GtkComboBoxText object
  item_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_wdgts->w_comboboxtext_options));
  if (item_text == NULL) {
    //return if no text found in combo box
    return;
  }
  
  gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_sel_text), item_text);

  //Determines what type of object user is inputting
  if (strcmp(item_text, "Point") == 0) {object_type=1;}
  if (strcmp(item_text, "Rectangle") == 0) {object_type=2;}
  if (strcmp(item_text, "Circle") == 0) {object_type=3;}

  //Requirements for every type of entry

   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user5), "Enter magnitude (<100,000):");
  
  //Requests the user for inputs corresponding to object types
  switch(object_type) {
  case 1 ://Point
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user1), "Enter x-coordinate:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user2), "Enter y-coordinate:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user3), "N/A");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user4), "N/A:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user6), "N/A");
   break;
  case 2 : //Rectangle
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user1), "Enter x-coordinate of bottom-left corner:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user2), "Enter y-coordinate of bottom-left corner:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user3), "Enter length:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user4), "Enter height");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user6), "(Optional) Fill the Rectangle?");
   break;
  case 3 : //Circle
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user1), "Enter x-coordinate of center:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user2), "Enter y-coordinate of center:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user3), "Enter radius");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user4), "N/A");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user6), "(Optional) Fill the Circle?");
   break;
  } 
  //free up dynamically allocated memory
  g_free(item_text);
  type = object_type;
}

void on_btn_add_object_clicked(GtkButton *button, app_widgets *app_wdgts) {

  int x1,y1,length,height,i,j,filled;
  float radius,magnitude;
  f = fopen("matrix.txt","w");

    filled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app_wdgts->w_tbtn_user));
    x1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user1));
    y1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user2));
    magnitude = gtk_spin_button_get_value(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user5));
    
  // Matrix values altered according to type of shape
  switch(type) {
  case 1 : //Point
    //printf("Point entered: (%d,%d) with magnitude %f\n",x1,y1,magnitude);
    //fprintf(f,"Point entered: (%d,%d) with magnitude %f\n",x1,y1,magnitude);
    matrix[y1][x1]=magnitude;
    break;
  
  case 2 : //Rectangle
    length = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user3));
    height = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user4));
    //printf("Rectangle entered from (%d,%d), with length %d, height %d and magnitude %f\n",x1,y1,length,height,magnitude);
    //fprintf(f,"Rectangle entered from (%d,%d), with length %d, height %d and magnitude %f\n",x1,y1,length,height,magnitude);
    for (i=x1; i < x1+length; i++) {
      for (j=y1; j < y1+height; j++) {
	if (i==x1 || i==x1+length-1 || j==y1 || j==y1+height-1) {
	  matrix[j][i]=magnitude;
	  //add if element is on rectangle boundary
	}
	if (filled) {
	  matrix[j][i]=magnitude;
	  //add if within boundary and is filled
	}
      }
    }
   break;
    
  case 3 : //Circle
    radius = gtk_spin_button_get_value(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user3));
    //printf("Circle entered with center (%d,%d), radius %f and magnitude %f\n",x1,y1,radius,magnitude);
    //fprintf(f,"Circle entered with center (%d,%d), radius %f and magnitude %f\n",x1,y1,radius,magnitude);
 
     for (i=0; i < dimension; i++) { 
       for (j=0; j < dimension; j++) { 
     	double distance = sqrt((double)(i-y1)*(i-y1)+(j-x1)*(j-x1)); 
     	if (distance>radius-0.5 && distance<radius+0.5) { 
     	      matrix[i][j]=magnitude; 
     	    } else if (distance<radius+0.5 && filled) { 
     	      matrix[i][j]=magnitude; 
     	} 
       } 
     }	    
    break;
  }

  for (int i=0; i < dimension; i++) {
    for (int j=0; j < dimension; j++) {
      if (matrix[i][j]==0.000001) {
	//printf("0.000001 ");
	fprintf(f,"0.000001");
      } else {
	//printf("%f ", matrix[i][j]);
	fprintf(f,"%f ", matrix[i][j]);
      }
    }
    //printf("\n");
    fprintf(f," ");
  }
fclose(f);
system("python plot_schematic.py > /dev/null");
 
//Update the image
 GList *children, *iter;
 children = gtk_container_get_children(GTK_CONTAINER(app_wdgts->w_image_schematic));
 for (iter = children; iter != NULL; iter = g_list_next(iter)){
   gtk_widget_destroy(GTK_WIDGET(iter->data));
  }
					 				       
 GtkWidget *image = gtk_image_new_from_file("schematic.png");
 gtk_container_add(GTK_CONTAINER(app_wdgts->w_image_schematic),image);
 gtk_widget_show(image);
 
}

//Takes dimension of matrix and produces square matrix on text file
void on_btn_matrix_clicked(GtkButton *button, app_widgets *app_wdgts) {

  //stores integer read from spin button widget

  num_iterations = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_iterations));
  
  //outputs dimension and initialises matrix elements
  //printf("Matrix dimension is %d\n", dimension);
    for (int i=0; i < dimension; i++) {
      for (int j=0; j < dimension; j++) {
	matrix[i][j]=0.000001; //0.000001 denotes unknown value (outwith acceptable potential range)
      }
   }
    f=fopen("log.txt","w");

  FILE *fp;
  fp = fopen("matrix.txt","w");
  for (int i=0; i < dimension; i++) {
    for (int j=0; j < dimension; j++) {
      if (matrix[i][j] == 0.000001) {
	    fprintf(fp, "0.000001 ");
      } else {
        fprintf(fp, "%f ", matrix[i][j]);
      }
    }
  }
  fclose(fp);
}

void on_btn_generate_clicked (GtkButton *button, app_widgets *app_wdgts) {
  f = fopen("log.txt","a");

  printf("Matrix successfully outputted to matrix.txt!\n");
  fprintf(f,"Matrix successfully outputted to matrix.txt!\n");
  fclose(f);

  printf("Passing Matrix to calculation handler\n");
  dimension = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_quantity));
  num_iterations = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_iterations));
  calc_handle(dimension, num_iterations);// Take tolerance from a button soon
  gtk_main_quit();
}


int calc_handle(int dimension, int num_iterations){

  // Create and fill matrix, we use the naming convention for a normal
  // matrix problem i.e. Ax=b
  double b[dimension*dimension];

  // Secondary matrix to hold information on wether or not each entry
  // is a boundary condition and should be skipped in calulation
  int boundaryflag[dimension*dimension];

  // Reads in the file containing the problem
  readboundary(b, boundaryflag, dimension);

  // Passes the matrix system (Ax=b) to the solver
  //solve(dimension, b, boundaryflag);
  //sparse_solve(dimension,b , boundaryflag, 2);

  clock_t start = clock();
  jacobi(dimension,b ,boundaryflag, num_iterations);
  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;
  printf("It took %.6f seconds\n", seconds);

  // avoids race condition with the system call below - i.e. prints before calling python exec
  sleep(0.1);

  system("python Plotter.py > /dev/null");

  exit(0);

  return 0;
}



int readboundary(double b[], int boundaryflag[], int dim){

  // b is the matrix (Ax=b) that holds information about the boundary conditions
  // src is the string holding file location


  // Open src and check for bad file
  FILE *fp;
  fp = fopen("matrix.txt", "r");
  if ( fp == NULL )
    {
        printf("Could not open file\n");
    }


    // Read the file's characters into the tgt array, skipping newlines
    // tgt size must be sufficient (defined outsie this function)
    
    

    for(long i = 0; i<dim*dim; i++){
      float c;
      fscanf(fp,"%f ", &c);

      // Remember which zeros and numbers are boundary conditions using a second vector
      // called boundaryflag
      if(c<0.001 && c>=0){
        b[i]=0;
        boundaryflag[i]=0;
      }

      // Shift ASCII table to turn che character for a number into the number itself
      else{
        b[i] = c;
        boundaryflag[i] = 1;
      }

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
  gsl_vector *b = gsl_vector_calloc(dim*dim);

  // Convert 1D to 2D
  for(long i =0; i<dim*dim;i++){
    gsl_vector_set(b, i, bpass[i]);
  }


  // Create empty "x" vector
  gsl_vector *x = gsl_vector_alloc(dim*dim);


  // Now need to create the correct forms of the matrices to solve the problem
  // as the current matrices are empty/garbage data. See p1029 of numerical
  // recipies for what the matrix should look like (tri-diagonal with fringes)
  for(long i = 0; i < (dim*dim); i++){

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

  printf("\n\n%d\n\n",dim);

  gsl_permutation * p = gsl_permutation_alloc (dim*dim);

  gsl_linalg_LU_decomp (A, p, &s);
  gsl_linalg_LU_solve (A, p, b, x);

  FILE *fp;
  fp = fopen("output.txt", "w");
  gsl_vector_fprintf (fp, x, "%g");
  fclose(fp);

  gsl_permutation_free (p);
  gsl_vector_free (x);
  return 0;
 
}



int sparse_solve(int dim, double bpass[], int boundaryflag[], int itermax){

  // Create/Convert arrays into gsl matrix types
  // Create coefficients array and turn it into a matrix
  gsl_spmatrix *A = gsl_spmatrix_alloc(dim*dim,dim*dim);

  // Create b vector from passed array (boundary conditions)
  gsl_vector *b = gsl_vector_alloc(dim*dim);

  // Convert 1D to 2D
  for(long i =0; i<dim*dim;i++){
    gsl_vector_set(b, i, bpass[i]);
  }
  // Create empty "x" vector
  gsl_vector *x = gsl_vector_calloc(dim*dim);



  // Now need to create the correct forms of the matrices to solve the problem
  // as the current matrices are empty/garbage data. See p1029 of numerical
  // recipies for what the matrix should look like (tri-diagonal with fringes)
  for(long i = 0; i < (dim*dim); i++){

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

 printf("\n\n%d\n\n",dim);

  // Now solve the sparse linear system using iterativ methods
  gsl_splinalg_itersolve *solv_space=gsl_splinalg_itersolve_alloc(gsl_splinalg_itersolve_gmres, dim*dim, 0);

  gsl_splinalg_itersolve_iterate(A, b, 0.1, x, solv_space);
  
  gsl_splinalg_itersolve_free(solv_space);
  gsl_vector_fprintf(stdout, x, "%g");

  /*for(int county=0; county<dim;county++){
    for(int countx=0;countx<dim;countx++){
      printf("%d      %d      %g\n",countx+1,county+1,gsl_vector_get(x,county*dim +countx));
    }
    
  }*/
    return 0;
 
}

int jacobi(int dim, double b[], int boundaryflag[], int num_iterations){
  
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
  for(int k = 0; k<num_iterations; k++){
    err = 0;

    for(int i=0;i<dim;i++){
      for(int j=0;j<dim;j++){

        // Check for Boundary
        if(boundary[i][j]==1){x_1[i][j]=x[i][j];continue;}

        // Top Left
        if(i==0 && j==0){x_1[i][j] = 0.5*(x[i+1][j]+x[i][j+1]);continue;}


        // Bottom Right
        if(i==dim-1 && j==dim-1){x_1[i][j] = 0.5*(x[i-1][j]+x[i][j-1]);continue;}


        // Bottom Left
        if(i==0 && j==dim-1){x_1[i][j] = 0.5*(x[i][j-1]+x[i+1][j]);continue;}

        // Top Right
        if(i==dim-1 && j==0){x_1[i][j] = 0.5*(x[i-1][j]+x[i][j+1]);continue;}


        // Top Edge
        if(j==0){x_1[i][j] = 0.3333333*(x[i+1][j] + x[i-1][j] + x[i][j+1]);continue;}

        // Bottom Edge
        if(j==dim-1){x_1[i][j] = 0.333333*(x[i-1][j] + x[i+1][j] + x[i][j-1]);continue;}

        // Left Edge
        if(i==0){x_1[i][j] = 0.3333333*(x[i+1][j] + x[i][j+1] + x[i][j-1]);continue;}


        // Right Edge
        if(i==dim-1){x_1[i][j] = 0.3333333*(x[i-1][j] + x[i][j+1] + x[i][j-1]);continue;}


        x_1[i][j] = 0.25*(x[i+1][j] + x[i-1][j] + x[i][j+1] + x[i][j-1]);

        err += abs(x_1[i][j]) - abs(x[i][j]);
      }
    }

    memcpy(x,x_1,dim*dim*8);
    err = err/(dim*dim);
    //if(err < tolerance){break;}
  }

  FILE *fp;
  fp = fopen("output.txt", "w");

  

for(int i=0;i<dim;i++){
    for(int j=0;j<dim;j++){
      fprintf(fp,"%f\n",x_1[i][j]);
    }
    //printf("\n");
  }
  fclose(fp);
}
