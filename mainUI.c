#include <gtk/gtk.h>
#include <gtk/gtkx.h>

#include "include/memory.h"
#include "include/cache.h"
#include "include/random_instruction.h"
#include "include/cpu.h"
#include "include/messages.h"
#include "include/bus.h"

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <mqueue.h>


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

struct cpu_thread_args
{
   struct CPU *cpu; 
   mqd_t mq;
};

struct bus_thread_args
{
   struct bus *bus; 
   int isBusActive;
   int protocolo;
   mqd_t mq;
};

typedef struct
{
    GtkWidget *label;
    const char *text;
    const char *color;
} LabelData;

typedef struct
{
    GtkWidget *label;
    const int *text;
    const char *color;
} LabelDataInt;

void* cpu_thread (void *args){
    struct cpu_thread_args* t_args = (struct cpu_thread_args*)args; 
    struct Instruction new_instruction;
    struct CPU *cpu = t_args->cpu;
    mqd_t mq = t_args->mq;
    
    for(int i=0 ; i < 10 ; i++){
        get_random_instruction(&new_instruction);
        
        printf("Executing instruction %s from core %d \n", new_instruction.op, cpu->id);

        execute_instruction(cpu, &new_instruction, mq);

        printf("******CPU: %d ******INV: %d , READ: %d, WRITE: %d ************ \n",cpu->id ,cpu->stats.INV, cpu->stats.READ_REQ_RESP, cpu->stats.WRITE_REQ_RESP);
    }
    return NULL;
}

void* bus_thread (void *args){
    struct bus_thread_args* t_args = (struct bus_thread_args*) args;
    struct bus *bus = t_args->bus;
    mqd_t mq = t_args->mq;
    int isBusActive = t_args->isBusActive;
    int proto = t_args->protocolo;
    for(int i =0; i < CACHE_SIZE ; i++){
            printf("77777777---Cache x Tag -> %d Address -> %s State -> %d Value -> %d \n",bus->cpus[0].cache.blocks[i].tag ,bus->cpus[0].cache.blocks[i].address ,bus->cpus[0].cache.blocks[i].state ,bus->cpus[0].cache.blocks[i].data);
    }
    
    process_tasks(bus, mq, isBusActive, proto);

    return NULL;
}



gboolean changeLabelColorWrapper(gpointer user_data)
{
    LabelData *data = (LabelData *)user_data;
    return changeLabelColor(data->label, data->text, data->color);
}
gboolean changeLabelColorWrapperInt(gpointer user_data)
{
    LabelData *data = (LabelData *)user_data;
    return changeLabelColorInt(data->label, data->text, data->color);
}

gboolean changeLabelColor(GtkWidget *label, const char *text, const char *color)
{
    static gboolean colorFlag = FALSE;
    counter = counter - 1;
    colorFlag = !colorFlag;

    const char *backgroundColor = colorFlag ? color : color;
    const char *markup = g_markup_printf_escaped("<span background=\"%s\">%s</span>", backgroundColor, text);

    gtk_label_set_markup(GTK_LABEL(label), markup);

    g_free((char *)markup);

    return TRUE;
}

gboolean changeLabelColorInt(GtkWidget *label, int text, const char *color)
{
    static gboolean colorFlag = FALSE;
    counter = counter - 1;
    colorFlag = !colorFlag;

    const char *backgroundColor = colorFlag ? color : color;
    const char *markup = g_markup_printf_escaped("<span background=\"%s\">%d</span>", backgroundColor, text);

    gtk_label_set_markup(GTK_LABEL(label), markup);

    g_free((char *)markup);

    return TRUE;
}

int main(int argc, char *argv[])
{
    
    gtk_init(&argc, &argv); ////NO PONER NADA FUERA DE ACA

    struct memory main_memory;
    struct bus my_bus;
    pthread_t cpu_threads[N_CPU];
    pthread_t bus_t;
    mqd_t mq;

    //Bus ops testing
    memory_init(&main_memory);
    my_bus.main_memory = main_memory; 

    
    struct bus_thread_args bus_t_args;

    mq = create_message_queue();
    bus_t_args.bus = &my_bus; //Estructura bus
 //   bus_t_args.bus->channel.access
    bus_t_args.isBusActive = 1;
    bus_t_args.protocolo = MESI;
    bus_t_args.mq = mq;

    struct cpu_thread_args cpu_thread_args_array[N_CPU];
 // Inicializar y lanzar las threads de CPU
    for (int i = 0; i < N_CPU; i++)
    {
        my_bus.cpus[i].id = i;
        initializeCache(&my_bus.cpus[i].cache);

        my_bus.cpus[i].stats.INV= 0;
        my_bus.cpus[i].stats.READ_REQ_RESP =0;
        my_bus.cpus[i].stats.WRITE_REQ_RESP= 0;
        

        cpu_thread_args_array[i].cpu = &my_bus.cpus[i];
        cpu_thread_args_array[i].mq = mq;
        pthread_create(&cpu_threads[i], NULL, cpu_thread, (void*)&cpu_thread_args_array[i]);
    }
    pthread_create(&bus_t, NULL, bus_thread, (void *) & bus_t_args);
    // Join threads and its args data structures
    /*==================================PE0=======================================*/
    char *textA0Address = my_bus.cpus[0].cache.blocks[0].address;
    char *textA1Address = my_bus.cpus[0].cache.blocks[1].address;
    char *textA2Address = my_bus.cpus[0].cache.blocks[2].address;
    char *textA3Address = my_bus.cpus[0].cache.blocks[3].address;

    //char textA0Data[4];
   // sprintf(textA0Data,"%d",my_bus.cpus[0].cache.blocks[0].data); 
    int textA0Data =my_bus.cpus[0].cache.blocks[0].data;
    
    char textA1Data[4];
    sprintf(textA1Data,"%d",my_bus.cpus[0].cache.blocks[1].data);   
    
  
    char textA2Data[4];
    sprintf(textA2Data,"%d",my_bus.cpus[0].cache.blocks[2].data);   
    
    
    char textA3Data[4];
    sprintf(textA3Data,"%d",my_bus.cpus[0].cache.blocks[3].data);   
    
   

    char *textA0State = (char*)my_bus.cpus[0].cache.blocks[0].state;
    char *textA1State = (char*)my_bus.cpus[0].cache.blocks[1].state;
    char *textA2State = (char*)my_bus.cpus[0].cache.blocks[2].state;
    char *textA3State = (char*)my_bus.cpus[0].cache.blocks[3].state;
    /*==================================PE1=======================================*/
    char *textB0Address = my_bus.cpus[1].cache.blocks[0].address;
    char *textB1Address = my_bus.cpus[1].cache.blocks[1].address;
    char *textB2Address = my_bus.cpus[1].cache.blocks[2].address;
    char *textB3Address = my_bus.cpus[1].cache.blocks[3].address;

    //char *textB0Data = (char *)(intptr_t)my_bus.cpus[1].cache.blocks[0].data;
    char textB0Data[4];
    printf("The integer is: %d\n",(char)my_bus.cpus[0].cache.blocks[0].data,textB0Data);
   
    char *textB1Data = (char)my_bus.cpus[1].cache.blocks[1].data;
    char *textB2Data = (char)my_bus.cpus[1].cache.blocks[2].data;
    char *textB3Data = (char)my_bus.cpus[1].cache.blocks[3].data;

    char *textB0State = (char*)my_bus.cpus[1].cache.blocks[0].state;
    char *textB1State = (char*)my_bus.cpus[1].cache.blocks[1].state;
    char *textB2State = (char*)my_bus.cpus[1].cache.blocks[2].state;
    char *textB3State = (char*)my_bus.cpus[1].cache.blocks[3].state;





    guint timer_id;
    LabelData data1, cpuDataAddress0,cpuDataAddress1,cpuDataAddress2,cpuDataAddress3;
    LabelData pe0Data1,pe0Data2,pe0Data3;
    LabelDataInt pe0Data0;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "memoryCoherence.ui", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    grid = GTK_WIDGET(gtk_builder_get_object(builder, "mainGrid")); // Replace "my_grid" with your GtkGrid ID

    a0state =GTK_WIDGET(gtk_builder_get_object(builder, "a0state"));
    a1state =GTK_WIDGET(gtk_builder_get_object(builder, "a1state"));
    a2state =GTK_WIDGET(gtk_builder_get_object(builder, "a2state"));
    a3state =GTK_WIDGET(gtk_builder_get_object(builder, "a3state"));


    a0Address =GTK_WIDGET(gtk_builder_get_object(builder, "a0Address"));
    a1Address=GTK_WIDGET(gtk_builder_get_object(builder, "a1Address"));
    a2Address =GTK_WIDGET(gtk_builder_get_object(builder, "a2Address"));
    a3Address=GTK_WIDGET(gtk_builder_get_object(builder, "a3Address"));

    a0Data = GTK_WIDGET(gtk_builder_get_object(builder, "a0Data"));
    a1Data = GTK_WIDGET(gtk_builder_get_object(builder, "a1Data"));
    a2Data = GTK_WIDGET(gtk_builder_get_object(builder, "a2Data"));
    a3Data = GTK_WIDGET(gtk_builder_get_object(builder, "a3Data"));

    b0Address =GTK_WIDGET(gtk_builder_get_object(builder, "a0Address"));
    b1Address=GTK_WIDGET(gtk_builder_get_object(builder, "a1Address"));
    b2Address =GTK_WIDGET(gtk_builder_get_object(builder, "20Address"));
    b3Address=GTK_WIDGET(gtk_builder_get_object(builder, "a3Address"));

   

    addressPE2 = GTK_WIDGET(gtk_builder_get_object(builder, "addressPE2"));
    label0 = GTK_WIDGET(gtk_builder_get_object(builder, "pe2"));
    label1 = GTK_WIDGET(gtk_builder_get_object(builder, "nextPE2"));

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    GtkStyleContext *styleContext;
    GError *error = NULL;
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(cssProvider), "style.css", &error);

    styleContext = gtk_widget_get_style_context(label0);
    gtk_style_context_add_class(styleContext, "mainLabel");

    styleContext = gtk_widget_get_style_context(addressPE2);
    gtk_style_context_add_class(styleContext, "label1");
    gtk_style_context_add_provider(styleContext, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

   
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

    const char *text = "PE1";
    const char *text1 = "PE2";

   

    data1.label = label0;
    data1.text = text1;
    data1.color = "white";

    /*===============================================*/

    cpuDataAddress0.label = a0Address;
    cpuDataAddress0.text = textA0Address;
    cpuDataAddress0.color = "yellow";

    cpuDataAddress1.label = a1Address;
    cpuDataAddress1.text = textA1Address;
    cpuDataAddress1.color = "yellow";
    
    cpuDataAddress2.label = a2Address;
    cpuDataAddress2.text = textA2Address;
    cpuDataAddress2.color = "yellow";
    
    cpuDataAddress3.label = a3Address;
    cpuDataAddress3.text = textA3Address;
    cpuDataAddress3.color = "yellow";

    /*===============================================*/
    pe0Data0.label = a0Data;
    pe0Data0.text = textA0Data;
    pe0Data0.color = "yellow";

    pe0Data1.label = a1Data;
    pe0Data1.text = textA1Data;
    pe0Data1.color = "yellow";
    
    pe0Data2.label = a2Data;
    pe0Data2.text = textA2Data;
    pe0Data2.color = "yellow";
    
    pe0Data3.label = a3Data;
    pe0Data3.text = textA3Data;
    pe0Data3.color = "yellow";
    
    //timer_id = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuDataAddress0);
    
  //  g_idle_add(changeLabelColorWrapper, &cpuDataAddress0);
  //  g_idle_add(changeLabelColorWrapper, &cpuDataAddress1);
   // g_idle_add(changeLabelColorWrapper, &cpuDataAddress2);
 //   g_idle_add(changeLabelColorWrapper, &cpuDataAddress3);
    guint timer_id4 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuDataAddress0);
    guint timer_id1 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuDataAddress1);
    guint timer_id2 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuDataAddress2);
    guint timer_id3 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuDataAddress3);

    guint timer_id5 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe0Data0);
    guint timer_id6 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &pe0Data1);
    guint timer_id7 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &pe0Data2);
    guint timer_id8 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &pe0Data3);
    
    
    /*
    guint timer_id5 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &data1);
    guint timer_id6 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &data1);
    guint timer_id7 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &data1);

    guint timer_id8 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &data1);
    guint timer_id9 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &data1);
    guint timer_id10 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &data1);
    guint timer_id11 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &data1);
*/
    
  

    //NO PONER NADA POR ABAJO DE ACA
    gtk_widget_show_all(window);
    

    gtk_main();

    for (int i = 0; i < N_CPU; i++)
    {  
        pthread_join(cpu_threads[i], NULL);
    }


    return 0;
} // gcc -o mainUI mainUI.c `pkg-config --cflags --libs gtk+-3.0`
// ./mainUI
//gtk-builder-convert mockUp.glade memoryCoherence.ui
