
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstring>
#include <unistd.h>

#include <lv2.h>

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include "lv2/lv2plug.in/ns/ext/atom/forge.h"
#include "lv2/lv2plug.in/ns/ext/patch/patch.h"

///////////////////////// MACRO SUPPORT ////////////////////////////////

#define PLUGIN_URI "https://github.com/brummer10/XKeyboard"

#ifndef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

typedef int PortIndex;

////////////////////////////// PLUG-IN CLASS ///////////////////////////

namespace xkeyboard {

#define XKeyboard__note_on          PLUGIN_URI "#note_on"
#define XKeyboard__note_off         PLUGIN_URI "#note_off"

class Xxkeyboard
{
private:
    LV2_URID midi_MidiEvent;
    LV2_URID_Map* map;
    const LV2_Atom_Sequence* midi_in;
    LV2_Atom_Sequence* midi_out;

    LV2_Atom midiatom; 
    LV2_Atom_Forge forge;
    LV2_Atom_Forge_Frame frame;
    uint8_t data[3];

    // private functions
    void send_midi_data(int count, uint8_t controller,
                             uint8_t note, uint8_t velocity);
    inline void run_dsp_(uint32_t n_samples);
    inline void connect_(uint32_t port,void* data);
    inline void init_dsp_(uint32_t rate);
    inline void connect_all__ports(uint32_t port, void* data);
    inline void activate_f();
    inline void clean_up();
    inline void deactivate_f();
public:
    // LV2 Descriptor
    static const LV2_Descriptor descriptor;
    // static wrapper to private functions
    static void deactivate(LV2_Handle instance);
    static void cleanup(LV2_Handle instance);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void activate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void* data);
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor,
                                double rate, const char* bundle_path,
                                const LV2_Feature* const* features);
    Xxkeyboard();
    ~Xxkeyboard();
};

// constructor
Xxkeyboard::Xxkeyboard() :

    midi_in(NULL),
    midi_out(NULL) {};

// destructor
Xxkeyboard::~Xxkeyboard() { };

///////////////////////// PRIVATE CLASS  FUNCTIONS /////////////////////

void Xxkeyboard::init_dsp_(uint32_t rate)
{
    // set values for internal ramping
}

// connect the Ports used by the plug-in class
void Xxkeyboard::connect_(uint32_t port,void* data)
{
    switch ((PortIndex)port)
    {
        case 0:
            midi_in = (const LV2_Atom_Sequence*)data;
            break;
        case 1:
            midi_out = (LV2_Atom_Sequence*)data;
            break;
        default:
            break;
    }
}

void Xxkeyboard::activate_f()
{
    // allocate the internal DSP mem
}

void Xxkeyboard::clean_up()
{
    // delete the internal DSP mem
}

void Xxkeyboard::deactivate_f()
{
    // delete the internal DSP mem
}

// send midi data to the midi output port 
void Xxkeyboard::send_midi_data(int count, uint8_t controller,
                             uint8_t note, uint8_t velocity)
{
    if(!midi_out) return;
    data[0] = controller;
    data[1] = note;
    data[2] = velocity; 
    lv2_atom_forge_frame_time(&forge,count);
    lv2_atom_forge_raw(&forge,&midiatom,sizeof(LV2_Atom));
    lv2_atom_forge_raw(&forge,data, sizeof(data));
    lv2_atom_forge_pad(&forge,sizeof(data)+sizeof(LV2_Atom)); 
}

// run the event loop
void Xxkeyboard::run_dsp_(uint32_t n_samples)
{
    if(n_samples<1) return;

    lv2_atom_forge_set_buffer(&forge,(uint8_t*)midi_out, midi_out->atom.size);
    lv2_atom_forge_sequence_head(&forge, &frame, 0);
    // get controller values
    LV2_ATOM_SEQUENCE_FOREACH(midi_in, ev) {
        if (ev->body.type == midi_MidiEvent) {
            const uint8_t* const msg = (const uint8_t*)(ev + 1);
            send_midi_data(0, msg[0], msg[1], msg[2]);
        }
    }
}

// connect ports needed in the plugin
void Xxkeyboard::connect_all__ports(uint32_t port, void* data)
{
    // connect the Ports used by the plug-in class
    connect_(port,data); 
}

////////////////////// STATIC CLASS  FUNCTIONS  ////////////////////////

// instantiate the plugin class, init the needed URI's
LV2_Handle 
Xxkeyboard::instantiate(const LV2_Descriptor* descriptor,
                            double rate, const char* bundle_path,
                            const LV2_Feature* const* features)
{
    LV2_URID_Map* map = NULL;
    for (int i = 0; features[i]; ++i) {
        if (!strcmp(features[i]->URI, LV2_URID__map)) {
            map = (LV2_URID_Map*)features[i]->data;
            break;
        }
    }
    if (!map) {
        return NULL;
    }
    // init the plug-in class
    Xxkeyboard *self = new Xxkeyboard();
    if (!self) {
        return NULL;
    }
    self->map = map;
    lv2_atom_forge_init(&self->forge,self->map);
    self->midi_MidiEvent = map->map(map->handle, LV2_MIDI__MidiEvent);
    self->midiatom.type  = self->midi_MidiEvent;
    self->midiatom.size  = sizeof(self->data);
    self->init_dsp_((uint32_t)rate);
    return (LV2_Handle)self;
}

void Xxkeyboard::connect_port(LV2_Handle instance, 
                                   uint32_t port, void* data)
{
    // connect all ports
    static_cast<Xxkeyboard*>(instance)->connect_all__ports(port, data);
}

void Xxkeyboard::activate(LV2_Handle instance)
{
    // allocate needed mem
    static_cast<Xxkeyboard*>(instance)->activate_f();
}

void Xxkeyboard::run(LV2_Handle instance, uint32_t n_samples)
{
    // run dsp
    static_cast<Xxkeyboard*>(instance)->run_dsp_(n_samples);
}

void Xxkeyboard::deactivate(LV2_Handle instance)
{
    // free allocated mem
    static_cast<Xxkeyboard*>(instance)->deactivate_f();
}

void Xxkeyboard::cleanup(LV2_Handle instance)
{
    // well, clean up after us
    Xxkeyboard* self = static_cast<Xxkeyboard*>(instance);
    self->clean_up();
    delete self;
}

const LV2_Descriptor Xxkeyboard::descriptor =
{
    PLUGIN_URI ,
    Xxkeyboard::instantiate,
    Xxkeyboard::connect_port,
    Xxkeyboard::activate,
    Xxkeyboard::run,
    Xxkeyboard::deactivate,
    Xxkeyboard::cleanup,
    NULL
};

} // end namespace xkeyboard

////////////////////////// LV2 SYMBOL EXPORT ///////////////////////////

extern "C"
LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
    switch (index)
    {
        case 0:
            return &xkeyboard::Xxkeyboard::descriptor;
        default:
            return NULL;
    }
}

///////////////////////////// FIN //////////////////////////////////////
