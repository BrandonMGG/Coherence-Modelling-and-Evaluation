#include <gtk/gtk.h>
#include <gtk/gtkx.h>

GtkBuilder *builder;
GtkWidget *window, *grid,
    *mainGrid,
    *cpu2,
    *cpu2Grid0,
    *pe2,
    *labelPE2,
    *nextPE2,
    *currentPE2,
    *cpu2Grid1,
    *dataPE2,
    *relleno2,
    *b0,
    *b1,
    *b2,
    *b3,
    *b3state,
    *b1state,
    *b0state,
    *b2Address,
    *b0Address,
    *b1data,
    *b0data,
    *b1Address,
    *b2state,
    *b3data,
    *b2data,
    *b3Address,
    *statePE2,
    *addressPE2parent,
    *addressPE2,
    *pe4,
    *labelPE4,
    *nextPE4,
    *currentPE4,
    *addressPE4,
    *dataPE4,
    *relleno4,
    *d0,
    *d1,
    *d2,
    *d3,
    *d3state,
    *d1state,
    *d0state,
    *d2Address,
    *d0Address,
    *d1data,
    *d0data,
    *d1Address,
    *d2state,
    *d3data,
    *d2data,
    *d3Address,
    *statePE4,
    *cyclesLabel,
    *cyclesNumber,
    *selectPE,
    *addressDropdown,
    *dataInput,
    *add,
    *start,
    *stop,
    *step,
    *run,
    *operation,
    *pe3,
    *labelPE3,
    *nextPE3,
    *currentPE3,
    *addressPE3,
    *dataPE3,
    *relleno3,
    *c0,
    *c1,
    *c2,
    *c3,
    *c3state,
    *c1state,
    *c0state,
    *c2Address,
    *c0Address,
    *c1data,
    *c0data,
    *c1Address,
    *c2state,
    *c3data,
    *c2data,
    *c3Address,
    *statePE3,
    *pe1,
    *nextPE1,
    *currentPE1,
    *labelPE1,
    *addressPE1,
    *dataPE1,
    *relleno1,
    *a0,
    *a1,
    *a2,
    *a3,
*a3state,
*a1state,
*a0state,
*a2Address,
*a0Address,
*a1Data,
*a0Data,
*a1Address,
*a2state,
*a3Data,
*a2Data,
*a3Address,
*statePE1,
*addressMemoryLabel,
*data0,
*dataMemoryLabel,
*address0,
*address1,
*data2,
*address2,
*data1,
*data3,
*data5,
*address3,
*data4,
*data6,
*data7,
*address6,
*address5,
*address4,
*address7;
GtkWidget *addressPE2;

GtkWidget *label0, *label1;
GtkWidget *event_box;
GtkWidget *box;
int counter = 100;

typedef struct
{
    GtkWidget *label;
    const char *text;
    const char *color;
} LabelData;

gboolean changeLabelColorWrapper(gpointer user_data)
{
    LabelData *data = (LabelData *)user_data;
    return changeLabelColor(data->label, data->text, data->color);
}

gboolean changeLabelColor(GtkWidget *label, const char *text, const char *color)
{
    static gboolean colorFlag = FALSE;
    counter = counter - 1;

    // printf("Counter %s\n", text);
    //  Toggle the colorFlag to alternate between colors
    colorFlag = !colorFlag;

    // Create a markup string with the desired background color and the provided text
    const char *backgroundColor = colorFlag ? color : "red";
    const char *markup = g_markup_printf_escaped("<span background=\"%s\">%s</span>", backgroundColor, text);

    // Update the label's text with the new markup
    gtk_label_set_markup(GTK_LABEL(label), markup);

    // Free the markup string to avoid memory leaks
    g_free((char *)markup);

    // Returning TRUE means the timer will continue running
    return TRUE;
}
int main(int argc, char *argv[])
{

    gtk_init(&argc, &argv);
    LabelData data;
    guint timer_id;
    LabelData data1;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "memoryCoherence.ui", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    grid = GTK_WIDGET(gtk_builder_get_object(builder, "mainGrid")); // Replace "my_grid" with your GtkGrid ID

    addressPE2 = GTK_WIDGET(gtk_builder_get_object(builder, "addressPE2"));

    label0 = GTK_WIDGET(gtk_builder_get_object(builder, "pe2"));

    label1 = GTK_WIDGET(gtk_builder_get_object(builder, "nextPE2"));

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    GtkStyleContext *styleContext;
    GError *error = NULL;
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(cssProvider), "style.css", &error);

    styleContext = gtk_widget_get_style_context(label1);
    gtk_style_context_add_class(styleContext, "label0");

    styleContext = gtk_widget_get_style_context(addressPE2);
    gtk_style_context_add_class(styleContext, "label1");
    gtk_style_context_add_provider(styleContext, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

   
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

    const char *text = "PE1";
    const char *text1 = "PE2";

    data.label = addressPE2;
    data.text = "Call an ambulance";
    data.color = "blue";

    data1.label = label0;
    data1.text = "but not for me";
    data1.color = "black";

    // Start the timer to call changeLabelColorWrapper every 1000 ms (1 second)
    // timer_id = g_timeout_add(1000, changeLabelColorWrapper, &data);

    timer_id = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &data);

    guint timer_id1 = g_timeout_add(1500, (GSourceFunc)changeLabelColorWrapper, &data1);

    //  counter = counter-1;
    //  printf("Counter %d\n", counter);

    // gtk_container_set_border_width(GTK_CONTAINER(window),20);
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
} // gcc -o mainUI mainUI.c `pkg-config --cflags --libs gtk+-3.0`
// ./mainUI