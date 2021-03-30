
#define CONTROLS 5

#define GUI_ELEMENTS 0

#define TAB_ELEMENTS 0


#define PLUGIN_UI_URI "https://github.com/brummer10/XKeyboard_ui"

#define OBJ_BUF_SIZE 1024

#include "lv2_plugin.h"


#include "xkeyboard.c"

void set_costum_theme(Xputty *main) {
    main->color_scheme->normal = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        .fg =       { 0.850, 0.850, 0.850, 1.000},
        .bg =       { 0.100, 0.100, 0.100, 1.000},
        .base =     { 0.000, 0.000, 0.000, 1.000},
        .text =     { 0.900, 0.900, 0.900, 1.000},
        .shadow =   { 0.000, 0.000, 0.000, 0.200},
        .frame =    { 0.000, 0.000, 0.000, 1.000},
        .light =    { 0.100, 0.100, 0.100, 1.000}
    };

    main->color_scheme->prelight = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        .fg =       { 1.000, 0.000, 1.000, 1.000},
        .bg =       { 0.250, 0.250, 0.250, 1.000},
        .base =     { 0.300, 0.300, 0.300, 1.000},
        .text =     { 1.000, 1.000, 1.000, 1.000},
        .shadow =   { 0.100, 0.100, 0.100, 0.400},
        .frame =    { 0.300, 0.300, 0.300, 1.000},
        .light =    { 0.300, 0.300, 0.300, 1.000}
    };

    main->color_scheme->selected = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        .fg =       { 0.900, 0.900, 0.900, 1.000},
        .bg =       { 0.200, 0.200, 0.200, 1.000},
        .base =     { 0.500, 0.180, 0.180, 1.000},
        .text =     { 1.000, 1.000, 1.000, 1.000},
        .shadow =   { 0.800, 0.180, 0.180, 0.200},
        .frame =    { 0.500, 0.180, 0.180, 1.000},
        .light =    { 0.500, 0.180, 0.180, 1.000}
    };

    main->color_scheme->active = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        .fg =       { 0.000, 1.000, 1.000, 1.000},
        .bg =       { 0.000, 0.000, 0.000, 1.000},
        .base =     { 0.180, 0.380, 0.380, 1.000},
        .text =     { 0.750, 0.750, 0.750, 1.000},
        .shadow =   { 0.180, 0.380, 0.380, 0.500},
        .frame =    { 0.180, 0.380, 0.380, 1.000},
        .light =    { 0.180, 0.380, 0.380, 1.000}
    };

    main->color_scheme->insensitive = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        .fg =       { 0.850, 0.850, 0.850, 0.500},
        .bg =       { 0.100, 0.100, 0.100, 0.500},
        .base =     { 0.000, 0.000, 0.000, 0.500},
        .text =     { 0.900, 0.900, 0.900, 0.500},
        .shadow =   { 0.000, 0.000, 0.000, 0.100},
        .frame =    { 0.000, 0.000, 0.000, 0.500},
        .light =    { 0.100, 0.100, 0.100, 0.500}
    };
}

#include "lv2_plugin.cc"

void send_vec(Widget_t *w, const int *key, const bool on_off) {
    X11_UI *ui = (X11_UI*) w->parent_struct;
    uint8_t obj_buf[OBJ_BUF_SIZE];
    int vec[4];
    vec[0] = (*key);
    vec[1] = (int)adj_get_value(ui->widget[2]->adj);
    vec[2] = (int)on_off;
    vec[3] = (int)adj_get_value(ui->widget[1]->adj);
    lv2_atom_forge_set_buffer(&ui->forge, obj_buf, OBJ_BUF_SIZE);
    LV2_Atom_Forge_Frame frame;

    lv2_atom_forge_frame_time(&ui->forge, 0);
    LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&ui->forge, &frame, 1, ui->atom_Int);
    lv2_atom_forge_property_head(&ui->forge, ui->atom_Vector,0);
    lv2_atom_forge_vector(&ui->forge, sizeof(int), ui->atom_Int, 4, (void*)vec);
    lv2_atom_forge_pop(&ui->forge, &frame);
   
    ui->write_function(ui->controller, 0, lv2_atom_total_size(msg),
                       ui->atom_eventTransfer, msg);
}

void octave_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    X11_UI *ui = (X11_UI*) w->parent_struct;
    MidiKeyboard *keys = (MidiKeyboard*)ui->widget[0]->parent_struct;
    keys->octave = (int)12*adj_get_value(w->adj);
    expose_widget(ui->widget[0]);
}

void layout_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    X11_UI *ui = (X11_UI*) w->parent_struct;
    MidiKeyboard *keys = (MidiKeyboard*)ui->widget[0]->parent_struct;
    keys->layout = (int)adj_get_value(w->adj);
}

void xkey_press(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    X11_UI *ui = (X11_UI*) w->parent_struct;
    ui->widget[0]->func.key_press_callback(ui->widget[0], key_, user_data);
}

void xkey_release(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    X11_UI *ui = (X11_UI*) w->parent_struct;
    ui->widget[0]->func.key_release_callback(ui->widget[0], key_, user_data);
}

void get_all_notes_off(Widget_t *w, const int *value) {
    
}

void plugin_value_changed(X11_UI *ui, Widget_t *w, PortIndex index) {
    // do special stuff when needed
}

void plugin_set_window_size(int *w,int *h,const char * plugin_uri) {
    (*w) = 616; //set initial width of main window
    (*h) = 180; //set initial height of main window
}

const char* plugin_set_name() {
    return "XKeyboard"; //set plugin name to display on UI
}

void plugin_create_controller_widgets(X11_UI *ui, const char * plugin_uri) {
    set_costum_theme(&ui->main);
    XSelectInput(ui->win->app->dpy, ui->win->widget,StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask|KeyReleaseMask
                    |ButtonPressMask|Button1MotionMask|PointerMotionMask);
    ui->win->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    ui->win->func.key_press_callback = xkey_press;
    ui->win->func.key_release_callback = xkey_release;

    ui->widget[0] = create_widget(ui->win->app, ui->win, 0, 80, 616, 100);
    ui->widget[0]->parent = ui->win;
    ui->widget[0]->flags &= ~USE_TRANSPARENCY;
    ui->widget[0]->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    ui->widget[0]->scale.gravity = SOUTHEAST;
    add_midi_keyboard(ui->widget[0], "", 0, 0, 616, 100);
    MidiKeyboard *keys = (MidiKeyboard*)ui->widget[0]->parent_struct;

    keys->mk_send_note = send_vec;
    keys->mk_send_all_sound_off = get_all_notes_off;
    keys->layout = 0;
    keys->octave = 24;
    keys->key_size = 21;
    keys->key_offset = 13;
    
    ui->widget[1] = add_lv2_knob(ui->widget[1], ui->win, -1, "Velocity", ui, 540, 0, 50, 70);
    ui->widget[1]->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    ui->widget[1]->parent_struct = ui;
    set_adjustment(ui->widget[1]->adj, 64.0, 64.0, 0.0, 127.0, 1.0, CL_CONTINUOS);
    ui->widget[1]->func.key_press_callback = xkey_press;
    ui->widget[1]->func.key_release_callback = xkey_release;

    add_label(ui->win, "Channel", 460, 5, 60, 20);
    ui->widget[2] =  add_combobox(ui->win, _("Channel"), 460, 30, 60, 30);
    ui->widget[2]->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    ui->widget[2]->childlist->childs[0]->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    ui->widget[2]->scale.gravity = ASPECT;
    ui->widget[2]->parent_struct = ui;
    combobox_add_numeric_entrys(ui->widget[2],1,16);
    combobox_set_active_entry(ui->widget[2], 0);
    set_adjustment(ui->widget[2]->adj,0.0, 0.0, 0.0, 16.0, 1.0, CL_ENUM);
    ui->widget[2]->func.key_press_callback = xkey_press;
    ui->widget[2]->func.key_release_callback = xkey_release;

    add_label(ui->win, "Keymap", 10, 5, 100, 20);
    ui->widget[3] =  add_combobox(ui->win, _("Mapping"), 10, 30, 100, 30);
    ui->widget[3]->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    ui->widget[3]->childlist->childs[0]->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    ui->widget[3]->scale.gravity = ASPECT;
    ui->widget[3]->parent_struct = ui;
    combobox_add_entry(ui->widget[3],_("qwertz"));
    combobox_add_entry(ui->widget[3],_("qwerty"));
    combobox_add_entry(ui->widget[3],_("azerty (fr)"));
    combobox_add_entry(ui->widget[3],_("azerty (be)"));
    combobox_set_active_entry(ui->widget[3], 0);
    ui->widget[3]->func.value_changed_callback = layout_callback;
    ui->widget[3]->func.key_press_callback = xkey_press;
    ui->widget[3]->func.key_release_callback = xkey_release;

    add_label(ui->win, "Octave", 120, 5, 60, 20);
    ui->widget[4] = add_combobox(ui->win, _("Octave"), 120, 30, 60, 30);
    ui->widget[4]->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    ui->widget[4]->childlist->childs[0]->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    ui->widget[4]->scale.gravity = ASPECT;
    ui->widget[4]->parent_struct = ui;
    combobox_add_entry(ui->widget[4],_("C 0"));
    combobox_add_entry(ui->widget[4],_("C 1"));
    combobox_add_entry(ui->widget[4],_("C 2"));
    combobox_add_entry(ui->widget[4],_("C 3"));
    combobox_add_entry(ui->widget[4],_("C 4"));
    combobox_set_active_entry(ui->widget[4], 2);
    ui->widget[4]->func.value_changed_callback = octave_callback;
    ui->widget[4]->func.key_press_callback = xkey_press;
    ui->widget[4]->func.key_release_callback = xkey_release;
}

void on_idle(LV2UI_Handle handle) {
    X11_UI* ui = (X11_UI*)handle;
    MidiKeyboard *keys = (MidiKeyboard*)ui->widget[0]->parent_struct;
    bool repeat = need_redraw(keys);
    if (repeat || ui->run_one_more) {
        expose_widget(ui->widget[0]);
        if (repeat)
            ui->run_one_more = 10;
    }
    ui->run_one_more = max(0,ui->run_one_more-1);
}

void plugin_cleanup(X11_UI *ui) {
    // clean up used sources when needed
}

void plugin_port_event(LV2UI_Handle handle, uint32_t port_index,
                        uint32_t buffer_size, uint32_t format,
                        const void * buffer) {
    X11_UI* ui = (X11_UI*)handle;
    if (format == ui->atom_eventTransfer) {
        MidiKeyboard *keys = (MidiKeyboard*)ui->widget[0]->parent_struct;
        const LV2_Atom* atom = (LV2_Atom*)buffer;
        if (atom->type == ui->atom_Object) {
            const LV2_Atom_Object* obj = (LV2_Atom_Object*)atom;
            if (obj->body.otype == ui->atom_Int) {
                const LV2_Atom* vector_data = NULL;
                const int n_props  = lv2_atom_object_get(obj,ui->atom_Vector, &vector_data, NULL);
                if (!n_props) return;
                const LV2_Atom_Vector* vec = (LV2_Atom_Vector*)LV2_ATOM_BODY(vector_data);
                if (vec->atom.type == ui->atom_Int) {
                    //int n_elem = (vector_data->size - sizeof(LV2_Atom_Vector_Body)) / vec->atom.size;
                    int* data;
                    data = (int*) LV2_ATOM_BODY(&vec->atom);
                    if ((data[2])) {
                        set_key_in_matrix(keys->in_key_matrix[data[1]], data[0], true);
                    } else {
                        set_key_in_matrix(keys->in_key_matrix[data[1]], data[0], false);
                    }
                }
            }
        }
    }
}

