#include <ui/vars.h>

int32_t arc_value = 0;

int32_t get_var_arc_value() {
    return arc_value;
}

void set_var_arc_value(int32_t value) {
    if (value < 0){
        arc_value = 0;
    } else if (value > 100) {
        arc_value = 100;
    } else{
        arc_value = value;
    }
    
}