#include <stdio.h>
#include <stdlib.h>

void process_readmiss(){
    /*
    if (cpu->state == MODIFIED || cpu->state == OWNED ){
        //WRITE MEMORIA preform_wb wb bus.py
    }
    if //Revisar si las otras caches tienen el address en su cache 
        //Si no hay colocar estado en SHARED
    else revisar si el address en otra cache tienen estado EXCLUSIVE o SHARED
        if si leer en memoria 
        else set estado SHARED
    */
}

void process_writemiss(){
    /*
    if state == MODIFIED || state == OWNED write en memoria perform_wb en bus.py
    State O guardar dato en cache y camnbiar estado a MODIFIED
    Invalidad address a todas las caches
    
    */
}