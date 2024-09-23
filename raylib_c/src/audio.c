#include "audio.h"

Sound menu_item_switch_sound;

void audio_init(void)
{
    InitAudioDevice();

    menu_item_switch_sound = LoadSound("./assets/rollover2.ogg");
}

void audio_play_menu_item_switch_sound(void)
{
    PlaySound(menu_item_switch_sound);
}

void audio_destroy()
{
    UnloadSound(menu_item_switch_sound);

    CloseAudioDevice();
}