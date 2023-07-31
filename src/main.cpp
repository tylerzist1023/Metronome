#define _CRT_SECURE_NO_WARNINGS
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <windows.h>
#include "metronome.h"

// TODO: polyrhythms
// TODO: better UI (buttons, etc.)
// TODO: disable/enable notes
// TODO: dotted notes
// TODO: swing rhythm

struct Metronome
{
    int beats;
    NoteValue value;
    int cur_beat;
};

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 256;

int bpm = 120;
Metronome metronome = {0};

int left_pressed = 0;
int reset = 0;

bool muted = false;

LARGE_INTEGER perf_freq;
LARGE_INTEGER start; 
LARGE_INTEGER now;
__int64 start64;
__int64 now64;

void reset_metronome();

int WinMain(
 HINSTANCE hInstance,
 HINSTANCE hPrevInstance,
 LPSTR     lpCmdLine,
 int       nShowCmd
)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 128);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Metronome", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Mix_Chunk* click1 = Mix_LoadWAV("res/click1.wav");
    Mix_Chunk* click2 = Mix_LoadWAV("res/click2.wav");

    SDL_Surface* surf = IMG_Load("res/notes.png");
    SDL_Texture* notes = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    TTF_Font* font = TTF_OpenFont("res/Arial.ttf", 32);
    
    QueryPerformanceFrequency(&perf_freq);
    QueryPerformanceCounter(&start);
    start64 = start.QuadPart;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_RenderClear(renderer);

    reset_metronome();
    metronome.beats = 4;
    metronome.value = NOTE_QUARTER;

    bool running = true;
    while(running) 
    {
        SDL_Event e;
        while(SDL_PollEvent(&e)) 
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }
            else if(e.type == SDL_KEYDOWN)
            {
                if(e.key.keysym.sym == SDLK_m)
                {
                    muted ^= true;
                }
            }
        }

        reset = 0;

        const Uint32 mouse = SDL_GetMouseState(0, 0);
        const Uint8* keys = SDL_GetKeyboardState(0);

        if((mouse & SDL_BUTTON_LMASK || keys[SDL_SCANCODE_R]) && !left_pressed)
        {
            reset_metronome();
        }
        else if(!(mouse & SDL_BUTTON_LMASK || keys[SDL_SCANCODE_R]))
        {
            left_pressed = 0;
        }

        // TODO: remember last choice
        if(keys[SDL_SCANCODE_T])
        {
            // change tempo
            int result;
            if(show_tempo_dialog(bpm, &result))
            {
                bpm = result;
            }
            reset_metronome();
        }
        else if(keys[SDL_SCANCODE_S])
        {
            // change signature
            int beats;
            NoteValue value;
            if(show_signature_dialog(metronome.beats, metronome.value, &beats, &value))
            {
                metronome.beats = beats;
                metronome.value = value;
            }
            reset_metronome();
        }

        QueryPerformanceCounter(&now);
        now64 = now.QuadPart;
        if(((double((now64 - start64)*1000) / perf_freq.QuadPart) >= (60/double(bpm))*1000) || reset)
        {
            if(!muted)
            {
                Mix_PlayChannel(-1, click1, 0);
                if(metronome.cur_beat == 1) { Mix_PlayChannel(-1, click2, 0); }
            }

            //printf("beat %d/%d\n", beat, total_beats);

            reset = 0;
            QueryPerformanceFrequency(&perf_freq);
            QueryPerformanceCounter(&start);
            start64 = start.QuadPart;

            {
                SDL_RenderClear(renderer);

                // draw notes
                for(int i = 0; i < metronome.beats; i++)
                {
                    int y = 128;
                    if(i <= metronome.cur_beat-1) {y = 0;}

                    SDL_Rect src = {metronome.value*64, y, 64, 128};
                    SDL_Rect dest = {(int)(i*((double)(WINDOW_WIDTH-64)/metronome.beats)), (WINDOW_HEIGHT/2)-64, 64, 128};

                    SDL_RenderCopy(renderer, notes, &src, &dest);
                }

                // draw text
                {
                    char buf[32];
                    sprintf(buf, "Tempo: %s=%d", NOTE_NAMES[metronome.value], bpm);
                    SDL_Surface* bpm_text_surf = TTF_RenderText_Blended(font, buf, {0xff,0xff,0xff});
                    SDL_Texture* bpm_text = SDL_CreateTextureFromSurface(renderer, bpm_text_surf);
                    SDL_FreeSurface(bpm_text_surf);

                    Uint32 format;
                    int access;
                    int w;
                    int h;
                    SDL_QueryTexture(bpm_text, &format, &access, &w, &h);
                    SDL_Rect dest = {/*WINDOW_WIDTH/2-w*/0, WINDOW_HEIGHT-h, w, h};

                    SDL_RenderCopy(renderer, bpm_text, 0, &dest);

                    SDL_DestroyTexture(bpm_text);
                }

                SDL_RenderPresent(renderer);
            }

            metronome.cur_beat++;
            if(metronome.cur_beat > metronome.beats) metronome.cur_beat = 1;
        }

        SDL_Delay(1); // to reduce CPU load. higher = less power consumption, but too high and the timing accuracy would be reduced
    }

    Mix_CloseAudio();
    Mix_FreeChunk(click1);
    Mix_FreeChunk(click2);
    TTF_CloseFont(font);
    SDL_DestroyTexture(notes);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}

void reset_metronome()
{
    QueryPerformanceCounter(&start);
    start64 = start.QuadPart;
    metronome.cur_beat = 1;
    left_pressed = 1;
    reset = 1;
}