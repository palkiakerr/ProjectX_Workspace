#include <gtk/gtk.h>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cstdio>

int globe;

typedef struct {
  GtkWidget *w_comboboxtext_options;
  GtkWidget *w_lbl_sel_text;
  GtkWidget *w_sbtn_quantity;
  GtkWidget *w_btn_matrix;
  GtkWidget *w_lbl_user1;
  GtkWidget *w_lbl_user2;
  GtkWidget *w_lbl_user3;
  GtkWidget *w_lbl_user4;
  GtkWidget *w_lbl_user5;
  GtkWidget *w_sbtn_user1;
  GtkWidget *w_sbtn_user2;
  GtkWidget *w_sbtn_user3;
  GtkWidget *w_sbtn_user4;
  GtkWidget *w_sbtn_user5;
  GtkWidget *w_btn_add_object;
} app_widgets;


void generate_matrix(int quantity);

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;
    app_widgets     *widgets = g_slice_new(app_widgets);
 
    gtk_init(&argc, &argv);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window_main.glade", NULL);
 
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

    //get pointers to widgets
    widgets->w_comboboxtext_options = GTK_WIDGET(gtk_builder_get_object(builder, "comboboxtext_options"));
    widgets->w_lbl_sel_text = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_sel_text"));
    widgets->w_sbtn_quantity = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_quantity"));
    widgets->w_btn_matrix = GTK_WIDGET(gtk_builder_get_object(builder, "btn_matrix"));
    widgets->w_lbl_user1 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user1"));
    widgets->w_lbl_user2 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user2"));
    widgets->w_lbl_user3 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user3"));
    widgets->w_lbl_user4 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user4"));
    widgets->w_lbl_user5 = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_user5"));
    widgets->w_sbtn_user1 = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_user1"));
    widgets->w_sbtn_user2 = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_user2"));
    widgets->w_sbtn_user3 = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_user3"));
    widgets->w_sbtn_user4 = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_user4"));
    widgets->w_sbtn_user5 = GTK_WIDGET(gtk_builder_get_object(builder, "sbtn_user5"));
    widgets->w_btn_add_object = GTK_WIDGET(gtk_builder_get_object(builder, "btn_add_object"));
    
    gtk_builder_connect_signals(builder, widgets);
 
    g_object_unref(builder);
 
    gtk_widget_show(window);                
    gtk_main();
    g_slice_free(app_widgets, widgets);
 
    return 0;
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
  if (strcmp(item_text, "Line") == 0) {object_type=2;}
  if (strcmp(item_text, "Circle") == 0) {object_type=3;}


  //Requests the user for inputs corresponding to object types
  switch(object_type) {
  case 1 ://Point
    //printf("Point selected\n");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user1), "Enter x-coordinate:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user2), "Enter y-coordinate:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user3), "N/A");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user4), "N/A:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user5), "Enter magnitude of point:");
   break;
  case 2 : //Line
    // printf("Line selected\n");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user1), "Enter x1-coordinate:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user2), "Enter y1-coordinate:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user3), "Enter x2-coordinate:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user4), "Enter y2-coordinate:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user5), "Enter magnitude of line:");
   break;
  case 3 : //Circle
    //printf("Circle selected\n");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user1), "Enter x-coordinate of center:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user2), "Enter y-coordinate of center:");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user3), "Enter radius");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user4), "N/A");
   gtk_label_set_text(GTK_LABEL(app_wdgts->w_lbl_user5), "Enter magnitude of circle:");
   break;
  } 
  //free up dynamically allocated memory
  g_free(item_text);
  globe = object_type;
}

void on_btn_add_object_clicked(GtkButton *button, app_widgets *app_wdgts) {

  int x1,y1,x2,y2;
  float radius,magnitude;

  switch(globe) {
  case 1 : //Point
    x1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user1));
    y1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user2));
    magnitude = gtk_spin_button_get_value(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user5));
    printf("Point entered: (%d,%d) with magnitude %f\n",x1,y1,magnitude);
    break;

  case 2 : //Line
    x1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user1));
    y1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user2));
    x2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user3));
    y2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user4));
    magnitude = gtk_spin_button_get_value(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user5));
    printf("Line entered between (%d,%d) and (%d,%d) with magnitude %f\n",x1,y1,x2,y2,magnitude);
    break;
    
  case 3 : //Circle
    x1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user1));
    y1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user2));
    radius = gtk_spin_button_get_value(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user3));
    magnitude = gtk_spin_button_get_value(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_user5));
    printf("Circle entered with center (%d,%d), radius %f and magnitude %f\n",x1,y1,radius,magnitude);
    break;
  }
}

//Takes dimension of matrix and produces square matrix on text file
void on_btn_matrix_clicked(GtkButton *button, app_widgets *app_wdgts) {

   guint32 quantity = 0; //stores integer read from spin button widget

  quantity = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_sbtn_quantity));
  generate_matrix(quantity);
  printf("Matrx dimension is %d\n", quantity);

}
  
// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}

void generate_matrix(int quantity) {

  FILE *fp;
  fp=fopen("matrix.txt","w");
  
  for (int i = 0; i < quantity; i++) {
    for (int j = 0; j < quantity; j++) {

	fprintf(fp, "0");
      }
	fprintf(fp,"\n");
  }
  fclose(fp);
}
	

	

  