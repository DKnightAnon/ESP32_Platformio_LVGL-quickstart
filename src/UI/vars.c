#include <ui/vars.h>

int32_t arc_value = 0;

int32_t get_var_arc_value() {
    return arc_value;
}

void set_var_arc_value(int32_t value) {
    // if ((arc_value - value )< 0){

    // } else if ((arc_value + value) > 100) {

    // } else{
        arc_value = value;
    // }
    
}