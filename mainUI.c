#include <gtk/gtk.h>
#include <gtk/gtkx.h>

GtkBuilder *builder;
GtkWidget *window;
GtkWidget *label;
GtkWidget *label0;
GtkWidget *event_box;
GtkWidget *box;
int counter=100;

typedef struct {
    GtkWidget *label;
    const char *text;
} LabelData;

gboolean changeLabelColorWrapper(gpointer user_data) {
    LabelData *data = (LabelData *)user_data;
    return changeLabelColor(data->label, data->text);
}

gboolean changeLabelColor(GtkWidget *label, const char *text) {
    static gboolean colorFlag = FALSE;
    counter = counter-1;
   
    printf("Counter %s\n", text);
    // Toggle the colorFlag to alternate between colors
    colorFlag = !colorFlag;

    // Create a markup string with the desired background color and the provided text
    const char *backgroundColor = colorFlag ? "lightgreen" : "red";
    const char *markup = g_markup_printf_escaped("<span background=\"%s\">%s</span>", backgroundColor, text);

    // Update the label's text with the new markup
    gtk_label_set_markup(GTK_LABEL(label), markup);

    // Free the markup string to avoid memory leaks
    g_free((char *)markup);

    // Returning TRUE means the timer will continue running
    return TRUE;
}
int main(int argc, char *argv[]) {
  

    gtk_init(&argc, &argv);
    LabelData data; guint timer_id;
    LabelData data1;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "memoryCoherence.ui", NULL);
    
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    label = GTK_WIDGET(gtk_builder_get_object(builder, "addressPE2"));

    label0 = GTK_WIDGET(gtk_builder_get_object(builder, "addressPE1"));

    

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_builder_connect_signals(builder,NULL);

    const char *text = "PE1";    
    const char *text1 = "PE2"; 


    data.label = label;
    data.text = "Call an ambulance";

    data1.label= label0;
    data1.text = "but not for me";

    // Start the timer to call changeLabelColorWrapper every 1000 ms (1 second)
    //timer_id = g_timeout_add(1000, changeLabelColorWrapper, &data);

   
    timer_id = g_timeout_add(500, (GSourceFunc)changeLabelColorWrapper, &data);
   
    guint timer_id1 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &data1);
    
    
  //  counter = counter-1;
    printf("Counter %d\n", counter);
   
   
    gtk_container_set_border_width(GTK_CONTAINER(window),20);
    gtk_widget_show_all(window);


    gtk_main();

    return 0;
}//gcc -o mainUI mainUI.c `pkg-config --cflags --libs gtk+-3.0`
// ./mainUI