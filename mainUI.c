#include <gtk/gtk.h>
#include <gtk/gtkx.h>

#include "include/messages.h"
#include "include/memory.h"
#include "include/cache.h"
#include "include/random_instruction.h"
#include "include/cpu.h"
#include "include/bus.h"

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

int startProgramCondition;
GtkBuilder *builder;
GtkWidget *window, *grid, *start,
    *mainGrid,
    *cpu2,
    *cpu2Grid0,
    *pe2,
    *labelPE2,
    *nextPE2,
    *currentPE2,
    *cpu2Grid1,
    *dataPE2,*stateNomenclature, *mainMemory,
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
    *address7,
    *data8,
    *data9,
    *data10,
    *data11,
    *data12,
    *data13,
    *data14,
    *data15,
    *currentPE3data0,
    *currentPE3data1,
    *currentPE3data2,
    *currentPE1data0,
    *currentPE1data1,
    *currentPE1data2,
    *currentPE2data0,
    *currentPE2data1,
    *currentPE2data2;

GtkWidget *label0, *label1;
GtkWidget *event_box;
GtkWidget *box;


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
    int *text;
    const char *color;
} LabelDataInt;



int counter = 100;
struct bus_thread_args bus_t_args;
struct cpu_thread_args cpu_thread_args_array[N_CPU];
volatile int stop_thread = 0;
pthread_t cpu_threads[N_CPU];
pthread_t bus_t;


void *cpu_thread(void *args)
{
    struct cpu_thread_args *t_args = (struct cpu_thread_args *)args;
    struct Instruction new_instruction;
    struct CPU *cpu = t_args->cpu;
    mqd_t mq = t_args->mq;
    
    
    while (!stop_thread) {
        get_random_instruction(&new_instruction);

        cpu->instruction = new_instruction;

        printf("Executing instruction %s from core %d \n", new_instruction.op, cpu->id);
        printf("%s , %s , %d \n", new_instruction.op,new_instruction.address,new_instruction.data);
        execute_instruction(cpu, &new_instruction, mq);

        printf("******CPU: %d ******INV: %d , READ: %d, WRITE: %d ************ \n", cpu->id, cpu->stats.INV, cpu->stats.READ_REQ_RESP, cpu->stats.WRITE_REQ_RESP);
    }
    cpu->stats.avg_exec = (cpu->stats.avg_exec) / (cpu->stats.READ_REQ_RESP + cpu->stats.WRITE_REQ_RESP + cpu->stats.INCR_REQ_RESP);
    printf("<<<<<<<<<<CPU - %d AVG EXECUTION TIME %3.3f seconds >>>>>>>>>>>>> \n",cpu->id, cpu->stats.avg_exec);
    return NULL;
}

void *cpu_thread_step(void *args)
{
    struct cpu_thread_args *t_args = (struct cpu_thread_args *)args;
    struct Instruction new_instruction;
    struct CPU *cpu = t_args->cpu;
    mqd_t mq = t_args->mq;

    get_random_instruction(&new_instruction);

    cpu->instruction = new_instruction;

    printf("Executing instruction %s from core %d \n", new_instruction.op, cpu->id);
    printf("%s , %s , %d \n", new_instruction.op,new_instruction.address,new_instruction.data);
    execute_instruction(cpu, &new_instruction, mq);

    printf("******CPU: %d ******INV: %d , READ: %d, WRITE: %d ************ \n", cpu->id, cpu->stats.INV, cpu->stats.READ_REQ_RESP, cpu->stats.WRITE_REQ_RESP);
    return NULL;
}

void *bus_thread(void *args)
{
    struct bus_thread_args *t_args = (struct bus_thread_args *)args;
    struct bus *bus = t_args->bus;
    mqd_t mq = t_args->mq;
    int isBusActive = t_args->isBusActive;
    int proto = t_args->protocolo;
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        printf("77777777---Cache x Tag -> %d Address -> %s State -> %d Value -> %d \n", bus->cpus[0].cache.blocks[i].tag, bus->cpus[0].cache.blocks[i].address, bus->cpus[0].cache.blocks[i].state, bus->cpus[0].cache.blocks[i].data);
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
    return changeLabelColorInt(data->label, (unsigned int)*(data->text), data->color);
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

gboolean changeLabelColorInt(GtkWidget *label, unsigned int text, const char *color)
{
    gchar *display;
    display = g_strdup_printf("%d", text);
    static gboolean colorFlag = FALSE;
    counter = counter - 1;
    colorFlag = !colorFlag;

    // const char *backgroundColor = colorFlag ? color : color;
    // const char *markup = g_markup_printf_escaped("<span background=\"%s\">%d</span>", backgroundColor, text);
    // char *markup = g_markup_printf_escaped("<span background=\"%s\">%d</span>", backgroundColor, text);
    // gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_label_set_text(GTK_LABEL(label), display);
    g_free(display);

    return TRUE;
}

void button_clicked(GtkWidget *widget, gpointer data)
{
    stop_thread = 0;

    // Crea e inicia los hilos CPU y el hilo de bus.
    for (int i = 0; i < N_CPU; i++)
    {
        pthread_create(&cpu_threads[i], NULL, cpu_thread, (void *)&cpu_thread_args_array[i]);
    }
    pthread_create(&bus_t, NULL, bus_thread, (void *)&bus_t_args);

    g_print("Button clicked!\n");
}

void button_stop_clicked(GtkWidget *widget, gpointer data)
{
    stop_thread = 1;

    g_print("Stop!\n");
}

void button_step_clicked(GtkWidget *widget, gpointer data)
{
    // Crea e inicia los hilos CPU y el hilo de bus.
    for (int i = 0; i < N_CPU; i++)
    {
        pthread_create(&cpu_threads[i], NULL, cpu_thread_step, (void *)&cpu_thread_args_array[i]);
    }
    pthread_create(&bus_t, NULL, bus_thread, (void *)&bus_t_args);
    g_print("Step!\n");
}

int main(int argc, char *argv[])
{
    startProgramCondition=0;
    gtk_init(&argc, &argv); ////NO PONER NADA FUERA DE ACA

    struct memory main_memory;
    struct bus my_bus;
    
    mqd_t mq;

    // Bus ops testing
    memory_init(&main_memory);
    my_bus.main_memory = main_memory;

    

    mq = create_message_queue();
    bus_t_args.bus = &my_bus; // Estructura bus
    //   bus_t_args.bus->channel.access
    bus_t_args.isBusActive = 1;
    bus_t_args.protocolo = MESI;
    bus_t_args.mq = mq;

    
    // Inicializar y lanzar las threads de CPU
        for (int i = 0; i < N_CPU; i++)
        {
            my_bus.cpus[i].id = i;
            initializeCache(&my_bus.cpus[i].cache);

            my_bus.cpus[i].stats.INV = 0;
            my_bus.cpus[i].stats.READ_REQ_RESP = 0;
            my_bus.cpus[i].stats.WRITE_REQ_RESP = 0;
            my_bus.cpus[i].stats.INCR_REQ_RESP = 0;

            cpu_thread_args_array[i].cpu = &my_bus.cpus[i];
            cpu_thread_args_array[i].mq = mq;
            //pthread_create(&cpu_threads[i], NULL, cpu_thread, (void *)&cpu_thread_args_array[i]);
        }
        //pthread_create(&bus_t, NULL, bus_thread, (void *)&bus_t_args);

    //g_signal_connect(G_OBJECT(start), "clicked", G_CALLBACK(button_clicked), NULL);

    // char *currentPE1 = my_bus.;

    // Join threads and its args data structures



    char *currentPE_0_op = my_bus.cpus[0].instruction.op; 
    char *currentPE_0_add = my_bus.cpus[0].instruction.address; 
    int *currentPE_0_dat = &(my_bus.cpus[0].instruction.data); 

    char *currentPE_1_op = my_bus.cpus[1].instruction.op; 
    char *currentPE_1_add = my_bus.cpus[1].instruction.address; 
    int *currentPE_1_dat = &(my_bus.cpus[1].instruction.data); 

    char *currentPE_2_op = my_bus.cpus[2].instruction.op; 
    char *currentPE_2_add = my_bus.cpus[2].instruction.address; 
    int *currentPE_2_dat = &(my_bus.cpus[2].instruction.data); 

    /*==================================PE0=======================================*/
    char *textA0Address = my_bus.cpus[0].cache.blocks[0].address;
    char *textA1Address = my_bus.cpus[0].cache.blocks[1].address;
    char *textA2Address = my_bus.cpus[0].cache.blocks[2].address;
    char *textA3Address = my_bus.cpus[0].cache.blocks[3].address;

    // char textA0Data[4];
    // sprintf(textA0Data,"%d",my_bus.cpus[0].cache.blocks[0].data);
    int *textA0Data = &(my_bus.cpus[0].cache.blocks[0].data);
    int *textA1Data = &(my_bus.cpus[0].cache.blocks[1].data);
    int *textA2Data = &(my_bus.cpus[0].cache.blocks[2].data);
    int *textA3Data = &(my_bus.cpus[0].cache.blocks[3].data);

    int *textA0State = &(my_bus.cpus[0].cache.blocks[0].state);
    int *textA1State = &(my_bus.cpus[0].cache.blocks[1].state);
    int *textA2State = &(my_bus.cpus[0].cache.blocks[2].state);
    int *textA3State = &(my_bus.cpus[0].cache.blocks[3].state);
    /*==================================PE1=======================================*/
    char *textB0Address = my_bus.cpus[1].cache.blocks[0].address;
    char *textB1Address = my_bus.cpus[1].cache.blocks[1].address;
    char *textB2Address = my_bus.cpus[1].cache.blocks[2].address;
    char *textB3Address = my_bus.cpus[1].cache.blocks[3].address;

    // char *textB0Data = (char *)(intptr_t)my_bus.cpus[1].cache.blocks[0].data;
    // char textB0Data[4];
    // printf("The integer is: %d\n",my_bus.cpus[0].cache.blocks[0].data);

    int *textB0Data = &(my_bus.cpus[1].cache.blocks[0].data);
    int *textB1Data = &(my_bus.cpus[1].cache.blocks[1].data);
    int *textB2Data = &(my_bus.cpus[1].cache.blocks[2].data);
    int *textB3Data = &(my_bus.cpus[1].cache.blocks[3].data);

    int *textB0State = &(my_bus.cpus[1].cache.blocks[0].state);
    int *textB1State = &(my_bus.cpus[1].cache.blocks[1].state);
    int *textB2State = &(my_bus.cpus[1].cache.blocks[2].state);
    int *textB3State = &(my_bus.cpus[1].cache.blocks[3].state);

    /*=================================PE2=========================================*/
    char *textC0Address = my_bus.cpus[2].cache.blocks[0].address;
    char *textC1Address = my_bus.cpus[2].cache.blocks[1].address;
    char *textC2Address = my_bus.cpus[2].cache.blocks[2].address;
    char *textC3Address = my_bus.cpus[2].cache.blocks[3].address;

    int *textC0Data = &(my_bus.cpus[2].cache.blocks[0].data);
    int *textC1Data = &(my_bus.cpus[2].cache.blocks[1].data);
    int *textC2Data = &(my_bus.cpus[2].cache.blocks[2].data);
    int *textC3Data = &(my_bus.cpus[2].cache.blocks[3].data);

    int *textC0State = &(my_bus.cpus[2].cache.blocks[0].state);
    int *textC1State = &(my_bus.cpus[2].cache.blocks[1].state);
    int *textC2State = &(my_bus.cpus[2].cache.blocks[2].state);
    int *textC3State = &(my_bus.cpus[2].cache.blocks[3].state);

    /*=================================Memoria Principal=========================================*/

    int *memoryData0 = &(my_bus.main_memory.entries[0].data);
    int *memoryData1 = &(my_bus.main_memory.entries[1].data);
    int *memoryData2 = &(my_bus.main_memory.entries[2].data);
    int *memoryData3 = &(my_bus.main_memory.entries[3].data);
    int *memoryData4 = &(my_bus.main_memory.entries[4].data);
    int *memoryData5 = &(my_bus.main_memory.entries[5].data);
    int *memoryData6 = &(my_bus.main_memory.entries[6].data);
    int *memoryData7 = &(my_bus.main_memory.entries[7].data);
    int *memoryData8 = &(my_bus.main_memory.entries[8].data);
    int *memoryData9 = &(my_bus.main_memory.entries[9].data);
    int *memoryData10 = &(my_bus.main_memory.entries[10].data);
    int *memoryData11 = &(my_bus.main_memory.entries[11].data);
    int *memoryData12 = &(my_bus.main_memory.entries[12].data);
    int *memoryData13 = &(my_bus.main_memory.entries[13].data);
    int *memoryData14 = &(my_bus.main_memory.entries[14].data);
    int *memoryData15 = &(my_bus.main_memory.entries[15].data);

    /*==========================================================================*/

    guint timer_id;
    LabelData data01,
        cpuDataAddress0, cpuDataAddress1, cpuDataAddress2, cpuDataAddress3,
        cpuB0DataAddress0, cpuB1DataAddress1, cpuB2DataAddress2, cpuB3DataAddress3,
        cpuC0DataAddress0, cpuC1DataAddress1, cpuC2DataAddress2, cpuC3DataAddress3;
    LabelDataInt pe0Data0, pe0Data1, pe0Data2, pe0Data3;
    LabelDataInt pe0State0, pe0State1, pe0State2, pe0State3;
    LabelDataInt pe1Data0, pe1Data1, pe1Data2, pe1Data3;
    LabelDataInt pe1State0, pe1State1, pe1State2, pe1State3;
    LabelDataInt pe2Data0, pe2Data1, pe2Data2, pe2Data3;
    LabelDataInt pe2State0, pe2State1, pe2State2, pe2State3;
    LabelDataInt MD0, MD1, MD2, MD3, MD4, MD5, MD6, MD7, MD8, MD9, MD10, MD11, MD12, MD13, MD14, MD15;
    LabelData curr_0_add,curr_1_add,curr_2_add, curr_0_op,curr_1_op,curr_2_op;
    LabelDataInt curr_0_dat,curr_1_dat,curr_2_dat;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "memoryCoherence.ui", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    grid = GTK_WIDGET(gtk_builder_get_object(builder, "mainGrid")); // Replace "my_grid" with your GtkGrid ID

    /*=================================Botones=====================================*/
    start = GTK_WIDGET(gtk_builder_get_object(builder, "start"));
    g_signal_connect(G_OBJECT(start), "clicked", G_CALLBACK(button_clicked), NULL);

    stop = GTK_WIDGET(gtk_builder_get_object(builder, "stop"));
    g_signal_connect(G_OBJECT(stop), "clicked", G_CALLBACK(button_stop_clicked), NULL);

    step = GTK_WIDGET(gtk_builder_get_object(builder, "step"));
    g_signal_connect(G_OBJECT(step), "clicked", G_CALLBACK(button_step_clicked), NULL);
    /*=================================Botones=====================================*/

    a0Address = GTK_WIDGET(gtk_builder_get_object(builder, "a0Address"));
    a1Address = GTK_WIDGET(gtk_builder_get_object(builder, "a1Address"));
    a2Address = GTK_WIDGET(gtk_builder_get_object(builder, "a2Address"));
    a3Address = GTK_WIDGET(gtk_builder_get_object(builder, "a3Address"));

    a0Data = GTK_WIDGET(gtk_builder_get_object(builder, "a0Data"));
    a1Data = GTK_WIDGET(gtk_builder_get_object(builder, "a1Data"));
    a2Data = GTK_WIDGET(gtk_builder_get_object(builder, "a2Data"));
    a3Data = GTK_WIDGET(gtk_builder_get_object(builder, "a3Data"));

    a0state = GTK_WIDGET(gtk_builder_get_object(builder, "a0state"));
    a1state = GTK_WIDGET(gtk_builder_get_object(builder, "a1state"));
    a2state = GTK_WIDGET(gtk_builder_get_object(builder, "a2state"));
    a3state = GTK_WIDGET(gtk_builder_get_object(builder, "a3state"));

    b0Address = GTK_WIDGET(gtk_builder_get_object(builder, "b0Address"));
    b1Address = GTK_WIDGET(gtk_builder_get_object(builder, "b1Address"));
    b2Address = GTK_WIDGET(gtk_builder_get_object(builder, "b2Address"));
    b3Address = GTK_WIDGET(gtk_builder_get_object(builder, "b3Address"));

    b0data = GTK_WIDGET(gtk_builder_get_object(builder, "b0data"));
    b1data = GTK_WIDGET(gtk_builder_get_object(builder, "b1data"));
    b2data = GTK_WIDGET(gtk_builder_get_object(builder, "b2data"));
    b3data = GTK_WIDGET(gtk_builder_get_object(builder, "b3data"));

    b0state = GTK_WIDGET(gtk_builder_get_object(builder, "b0state"));
    b1state = GTK_WIDGET(gtk_builder_get_object(builder, "b1state"));
    b2state = GTK_WIDGET(gtk_builder_get_object(builder, "b2state"));
    b3state = GTK_WIDGET(gtk_builder_get_object(builder, "b3state"));

    c0Address = GTK_WIDGET(gtk_builder_get_object(builder, "c0Address"));
    c1Address = GTK_WIDGET(gtk_builder_get_object(builder, "c1Address"));
    c2Address = GTK_WIDGET(gtk_builder_get_object(builder, "c2Address"));
    c3Address = GTK_WIDGET(gtk_builder_get_object(builder, "c3Address"));

    c0data = GTK_WIDGET(gtk_builder_get_object(builder, "c0data"));
    c1data = GTK_WIDGET(gtk_builder_get_object(builder, "c1data"));
    c2data = GTK_WIDGET(gtk_builder_get_object(builder, "c2data"));
    c3data = GTK_WIDGET(gtk_builder_get_object(builder, "c3data"));

    c0state = GTK_WIDGET(gtk_builder_get_object(builder, "c0state"));
    c1state = GTK_WIDGET(gtk_builder_get_object(builder, "c1state"));
    c2state = GTK_WIDGET(gtk_builder_get_object(builder, "c2state"));
    c3state = GTK_WIDGET(gtk_builder_get_object(builder, "c3state"));

    data0 = GTK_WIDGET(gtk_builder_get_object(builder, "data0"));
    data1 = GTK_WIDGET(gtk_builder_get_object(builder, "data1"));
    data2 = GTK_WIDGET(gtk_builder_get_object(builder, "data2"));
    data3 = GTK_WIDGET(gtk_builder_get_object(builder, "data3"));
    data4 = GTK_WIDGET(gtk_builder_get_object(builder, "data4"));
    data5 = GTK_WIDGET(gtk_builder_get_object(builder, "data5"));
    data6 = GTK_WIDGET(gtk_builder_get_object(builder, "data6"));
    data7 = GTK_WIDGET(gtk_builder_get_object(builder, "data7"));
    data8 = GTK_WIDGET(gtk_builder_get_object(builder, "data8"));
    data9 = GTK_WIDGET(gtk_builder_get_object(builder, "data9"));
    data10 = GTK_WIDGET(gtk_builder_get_object(builder, "data10"));
    data11 = GTK_WIDGET(gtk_builder_get_object(builder, "data11"));
    data12 = GTK_WIDGET(gtk_builder_get_object(builder, "data12"));
    data13 = GTK_WIDGET(gtk_builder_get_object(builder, "data13"));
    data14 = GTK_WIDGET(gtk_builder_get_object(builder, "data14"));
    data15 = GTK_WIDGET(gtk_builder_get_object(builder, "data15"));

    addressPE1 = GTK_WIDGET(gtk_builder_get_object(builder, "addressPE1"));
    addressPE2 = GTK_WIDGET(gtk_builder_get_object(builder, "addressPE2"));
    addressPE3 = GTK_WIDGET(gtk_builder_get_object(builder, "addressPE3"));

    a0 = GTK_WIDGET(gtk_builder_get_object(builder, "a0"));
    a1 = GTK_WIDGET(gtk_builder_get_object(builder, "a1"));
    a2 = GTK_WIDGET(gtk_builder_get_object(builder, "a2"));
    a3 = GTK_WIDGET(gtk_builder_get_object(builder, "a3"));

    currentPE1 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE1"));
    currentPE2 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE2"));
    currentPE3 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE3"));


    stateNomenclature = GTK_WIDGET(gtk_builder_get_object(builder, "stateNomenclature"));
    mainMemory = GTK_WIDGET(gtk_builder_get_object(builder, "mainMemory"));
    

    b0 = GTK_WIDGET(gtk_builder_get_object(builder, "b0"));
    b1 = GTK_WIDGET(gtk_builder_get_object(builder, "b1"));
    b2 = GTK_WIDGET(gtk_builder_get_object(builder, "b2"));
    b3 = GTK_WIDGET(gtk_builder_get_object(builder, "b3"));

    label0 = GTK_WIDGET(gtk_builder_get_object(builder, "pe2"));
    label1 = GTK_WIDGET(gtk_builder_get_object(builder, "nextPE2"));


    currentPE1data0 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE1data0"));
    currentPE1data1 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE1data1"));
    currentPE1data2 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE1data2"));
    currentPE2data0 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE2data0"));
    currentPE2data1 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE2data1"));
    currentPE2data2 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE2data2"));
    currentPE3data0 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE3data0"));
    currentPE3data1 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE3data1"));
    currentPE3data2 = GTK_WIDGET(gtk_builder_get_object(builder, "currentPE3data2"));

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    GtkStyleContext *styleContext,
        *styleContext1,
        *styleContext2,
        *styleContext3,
        *styleContext4,
        *styleContext5,
        *styleContext6,
        *styleContext7,
        *styleContext8,
        *styleContext9,
        *styleContext10,
        *styleContext11,
        *styleContext12,
        *styleContext13,
        *styleContext14,
        *styleContext15,
        *styleContext16,
        *styleContext17,
        *styleContext18,
        *styleContext19,
        *styleContext20,
        *styleContext21,
        *styleContext22,
        *styleContext23,
        *styleContext24,
        *styleContext25,
        *styleContext26,
        *styleContext27,
        *styleContext28;

    GError *error = NULL;
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(cssProvider), "style.css", &error);

    styleContext1 = gtk_widget_get_style_context(addressPE2);
    styleContext2 = gtk_widget_get_style_context(dataPE1);
    styleContext7 = gtk_widget_get_style_context(addressPE1);
    styleContext3 = gtk_widget_get_style_context(a0);
    styleContext4 = gtk_widget_get_style_context(a1);
    styleContext5 = gtk_widget_get_style_context(a2);
    styleContext6 = gtk_widget_get_style_context(a3);
    styleContext8 = gtk_widget_get_style_context(c0);
    styleContext9 = gtk_widget_get_style_context(c1);
    styleContext10 = gtk_widget_get_style_context(c2);
    styleContext11 = gtk_widget_get_style_context(c3);
    styleContext12 = gtk_widget_get_style_context(b0);
    styleContext13 = gtk_widget_get_style_context(b1);
    styleContext14 = gtk_widget_get_style_context(b2);
    styleContext15 = gtk_widget_get_style_context(b3);
    

    styleContext16 = gtk_widget_get_style_context(stateNomenclature);
    styleContext17 = gtk_widget_get_style_context(mainMemory);
   // styleContext18 = gtk_widget_get_style_context(start);
    

    gtk_style_context_add_class(styleContext1, "mainLabel");
    gtk_style_context_add_class(styleContext2, "mainLabel");
    gtk_style_context_add_class(styleContext3, "mainLabel");
    gtk_style_context_add_class(styleContext4, "mainLabel");
    gtk_style_context_add_class(styleContext5, "mainLabel");
    gtk_style_context_add_class(styleContext6, "mainLabel");
    gtk_style_context_add_class(styleContext7, "mainLabel");
    gtk_style_context_add_class(styleContext8, "mainLabel");
    gtk_style_context_add_class(styleContext9, "mainLabel");
    gtk_style_context_add_class(styleContext10, "mainLabel");
    gtk_style_context_add_class(styleContext11, "mainLabel");
    gtk_style_context_add_class(styleContext12, "mainLabel");
    gtk_style_context_add_class(styleContext13, "mainLabel");
    gtk_style_context_add_class(styleContext14, "mainLabel");
    gtk_style_context_add_class(styleContext15, "mainLabel");

    gtk_style_context_add_class(styleContext16, "principalLabel");
    gtk_style_context_add_class(styleContext17, "principalLabel");
   // gtk_style_context_add_class(styleContext18, "button");

    
   

    gtk_style_context_add_provider(styleContext1, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext2, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext3, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext4, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext5, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext6, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext7, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext8, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext9, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext10, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext11, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext12, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext13, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext14, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext15, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_style_context_add_provider(styleContext16, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(styleContext17, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
   // gtk_style_context_add_provider(styleContext18, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    /* gtk_style_context_add_provider(styleContext2, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
     gtk_style_context_add_provider(styleContext3, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
     gtk_style_context_add_provider(styleContext4, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
     gtk_style_context_add_provider(styleContext5, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
     gtk_style_context_add_provider(styleContext6, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
     gtk_style_context_add_provider(styleContext7, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
     gtk_style_context_add_provider(styleContext8, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
     gtk_style_context_add_provider(styleContext9, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);*/

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

    const char *text = "PE1";
    const char *text1 = "PE2";

    data01.label = label0;
    data01.text = text1;
    data01.color = "white";

    /*=====================CPU 0 ==========================*/

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

    pe0State0.label = a0state;
    pe0State0.text = textA0State;
    pe0State0.color = "yellow";

    pe0State1.label = a1state;
    pe0State1.text = textA1State;
    pe0State1.color = "yellow";

    pe0State2.label = a2state;
    pe0State2.text = textA2State;
    pe0State2.color = "yellow";

    pe0State3.label = a3state;
    pe0State3.text = textA3State;
    pe0State3.color = "yellow";
    /*=======================CPU 1========================*/
    cpuB0DataAddress0.label = b0Address;
    cpuB0DataAddress0.text = textB0Address;
    cpuB0DataAddress0.color = "yellow";

    cpuB1DataAddress1.label = b1Address;
    cpuB1DataAddress1.text = textB1Address;
    cpuB1DataAddress1.color = "yellow";

    cpuB2DataAddress2.label = b2Address;
    cpuB2DataAddress2.text = textB2Address;
    cpuB2DataAddress2.color = "yellow";

    cpuB3DataAddress3.label = b3Address;
    cpuB3DataAddress3.text = textB3Address;
    cpuB3DataAddress3.color = "yellow";

    pe1Data0.label = b0data;
    pe1Data0.text = textB0Data;
    pe1Data0.color = "yellow";

    pe1Data1.label = b1data;
    pe1Data1.text = textB1Data;
    pe1Data1.color = "yellow";

    pe1Data2.label = b2data;
    pe1Data2.text = textB2Data;
    pe1Data2.color = "yellow";

    pe1Data3.label = b3data;
    pe1Data3.text = textB3Data;
    pe1Data3.color = "yellow";

    pe1State0.label = b0state;
    pe1State0.text = textB0State;
    pe1State0.color = "yellow";

    pe1State1.label = b1state;
    pe1State1.text = textB1State;
    pe1State1.color = "yellow";

    pe1State2.label = b2state;
    pe1State2.text = textB2State;
    pe1State2.color = "yellow";

    pe1State3.label = b3state;
    pe1State3.text = textB3State;
    pe1State3.color = "yellow";

    /*=======================CPU 2========================*/
    cpuC0DataAddress0.label = c0Address;
    cpuC0DataAddress0.text = textC0Address;
    cpuC0DataAddress0.color = "yellow";

    cpuC1DataAddress1.label = c1Address;
    cpuC1DataAddress1.text = textC1Address;
    cpuC1DataAddress1.color = "yellow";

    cpuC2DataAddress2.label = c2Address;
    cpuC2DataAddress2.text = textC2Address;
    cpuC2DataAddress2.color = "yellow";

    cpuC3DataAddress3.label = c3Address;
    cpuC3DataAddress3.text = textC3Address;
    cpuC3DataAddress3.color = "yellow";

    pe2Data0.label = c0data;
    pe2Data0.text = textC0Data;
    pe2Data0.color = "yellow";

    pe2Data1.label = c1data;
    pe2Data1.text = textC1Data;
    pe2Data1.color = "yellow";

    pe2Data2.label = c2data;
    pe2Data2.text = textC2Data;
    pe2Data2.color = "yellow";

    pe2Data3.label = c3data;
    pe2Data3.text = textC3Data;
    pe2Data3.color = "yellow";

    pe2State0.label = c0state;
    pe2State0.text = textC0State;
    pe2State0.color = "yellow";

    pe2State1.label = c1state;
    pe2State1.text = textC1State;
    pe2State1.color = "yellow";

    pe2State2.label = c2state;
    pe2State2.text = textC2State;
    pe2State2.color = "yellow";

    pe2State3.label = c3state;
    pe2State3.text = textC3State;
    pe2State3.color = "yellow";
    /*=====================Memoria Principal==========================*/
    MD0.label = data0;
    MD0.text = memoryData0;
    MD0.color = "yellow";

    MD1.label = data1;
    MD1.text = memoryData1;
    MD1.color = "yellow";

    MD2.label = data2;
    MD2.text = memoryData2;
    MD2.color = "yellow";

    MD3.label = data3;
    MD3.text = memoryData3;
    MD3.color = "yellow";

    MD4.label = data4;
    MD4.text = memoryData4;
    MD4.color = "yellow";

    MD5.label = data5;
    MD5.text = memoryData5;
    MD5.color = "yellow";

    MD6.label = data6;
    MD6.text = memoryData6;
    MD6.color = "yellow";

    MD7.label = data7;
    MD7.text = memoryData7;
    MD7.color = "yellow";

    MD8.label = data8;
    MD8.text = memoryData8;
    MD8.color = "yellow";

    MD9.label = data9;
    MD9.text = memoryData9;
    MD9.color = "yellow";

    MD10.label = data10;
    MD10.text = memoryData10;
    MD10.color = "yellow";

    MD11.label = data11;
    MD11.text = memoryData11;
    MD11.color = "yellow";

    MD12.label = data12;
    MD12.text = memoryData12;
    MD12.color = "yellow";

    MD13.label = data13;
    MD13.text = memoryData13;
    MD13.color = "yellow";

    MD14.label = data14;
    MD14.text = memoryData14;
    MD14.color = "yellow";

    MD15.label = data15;
    MD15.text = memoryData15;
    MD15.color = "yellow";

    /*=====================Instrucciones==========================*/
    curr_0_add.label = currentPE1data1;
    curr_0_add.text = currentPE_0_add;
    curr_0_add.color = "white";

    curr_1_add.label = currentPE2data1;
    curr_1_add.text = currentPE_1_add;
    curr_1_add.color = "white";

    curr_2_add.label = currentPE3data1;
    curr_2_add.text = currentPE_2_add;
    curr_2_add.color = "white";

    curr_0_op.label = currentPE1data0;
    curr_0_op.text = currentPE_0_op;
    curr_0_op.color = "white";

    curr_1_op.label = currentPE2data0;
    curr_1_op.text = currentPE_1_op;
    curr_1_op.color = "white";

    curr_2_op.label = currentPE3data0;
    curr_2_op.text = currentPE_2_op;
    curr_2_op.color = "white";

    curr_0_dat.label = currentPE1data2;
    curr_0_dat.text = currentPE_0_dat;
    curr_0_dat.color = "white";

    curr_1_dat.label = currentPE2data2;
    curr_1_dat.text = currentPE_1_dat;
    curr_1_dat.color = "white";

    curr_2_dat.label = currentPE3data2;
    curr_2_dat.text = currentPE_2_dat;
    curr_2_dat.color = "white";



    guint timer_id4 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuDataAddress0);
    guint timer_id1 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuDataAddress1);
    guint timer_id2 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuDataAddress2);
    guint timer_id3 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuDataAddress3);

    guint timer_id5 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe0Data0);
    guint timer_id6 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe0Data1);
    guint timer_id7 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe0Data2);
    guint timer_id8 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe0Data3);

    guint timer_id17 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe0State0);
    guint timer_id18 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe0State1);
    guint timer_id19 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe0State2);
    guint timer_id20 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe0State3);

    guint timer_id9 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuB0DataAddress0);
    guint timer_id10 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuB1DataAddress1);
    guint timer_id11 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuB2DataAddress2);
    guint timer_id12 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuB3DataAddress3);

    guint timer_id21 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe1Data0);
    guint timer_id22 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe1Data1);
    guint timer_id23 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe1Data2);
    guint timer_id24 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe1Data3);

    guint timer_id25 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe1State0);
    guint timer_id26 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe1State1);
    guint timer_id27 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe1State2);
    guint timer_id28 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe1State3);

    guint timer_id13 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuC0DataAddress0);
    guint timer_id14 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuC1DataAddress1);
    guint timer_id15 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuC2DataAddress2);
    guint timer_id16 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &cpuC3DataAddress3);

    guint timer_id29 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe2Data0);
    guint timer_id30 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe2Data1);
    guint timer_id31 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe2Data2);
    guint timer_id32 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe2Data3);

    guint timer_id33 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe2State0);
    guint timer_id34 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe2State1);
    guint timer_id35 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe2State2);
    guint timer_id36 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &pe2State3);

    guint timer_id37 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD0);
    guint timer_id38 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD1);
    guint timer_id39 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD2);
    guint timer_id40 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD3);
    guint timer_id41 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD4);
    guint timer_id42 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD5);
    guint timer_id43 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD6);
    guint timer_id44 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD7);
    guint timer_id45 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD8);
    guint timer_id46 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD9);
    guint timer_id47 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD10);
    guint timer_id48 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD11);
    guint timer_id49 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD12);
    guint timer_id50 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD13);
    guint timer_id51 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD14);
    guint timer_id52 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &MD15);

    guint timer_id53 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &curr_0_add);
    guint timer_id54 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &curr_1_add);
    guint timer_id55 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &curr_2_add);
    guint timer_id56 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &curr_0_op);
    guint timer_id57 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &curr_1_op);
    guint timer_id58 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapper, &curr_2_op);
    guint timer_id59 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &curr_0_dat);
    guint timer_id60 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &curr_1_dat);
    guint timer_id61 = g_timeout_add(1000, (GSourceFunc)changeLabelColorWrapperInt, &curr_2_dat);

    // NO PONER NADA POR ABAJO DE ACA
    gtk_widget_show_all(window);

    gtk_main();

    for (int i = 0; i < N_CPU; i++)
    {
        pthread_join(cpu_threads[i], NULL);
    }
    pthread_join(bus_t,NULL);

    bus_t_args.isBusActive=0;

    return 0;
} // gcc -o mainUI mainUI.c `pkg-config --cflags --libs gtk+-3.0`
// ./mainUI
// gtk-builder-convert mockUp.glade memoryCoherence.ui
