#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

static void event_handler_cb_main_encoder_arc(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_arc_get_value(ta);
            set_var_arc_value(value);
        }
    }
}

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 128, 160);
    {
        lv_obj_t *parent_obj = obj;
        {
            // HelloWorldLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.hello_world_label = obj;
            lv_obj_set_pos(obj, 20, 17);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Hello, world!");
        }
        {
            // EncoderArc
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.encoder_arc = obj;
            lv_obj_set_pos(obj, 32, 59);
            lv_obj_set_size(obj, 66, 72);
            lv_arc_set_bg_end_angle(obj, 60);
            lv_obj_add_event_cb(obj, action_change_arc_value, LV_EVENT_ROTARY, (void *)0);
            lv_obj_add_event_cb(obj, event_handler_cb_main_encoder_arc, LV_EVENT_ALL, 0);
        }
    }
}

void tick_screen_main() {
    {
        int32_t new_val = get_var_arc_value();
        int32_t cur_val = lv_arc_get_value(objects.encoder_arc);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.encoder_arc;
            lv_arc_set_value(objects.encoder_arc, new_val);
            tick_value_change_obj = NULL;
        }
    }
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
