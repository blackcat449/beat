/**
 * @file beat.h
 * @author Nathan Steimle (nathanosteimle@gmail.com)
 * @brief A framework for building games and simulations from the ground up. Allows for directly setting screen pixels and audio samples.
 * @version 1.0
 * @date 2021-02-21
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef _BEAT_H
#define _BEAT_H
#ifndef _WINUSER_H
#include <winuser.h>
#endif
#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef unsigned long long u_longlong;
typedef struct color {
    u_char blue;
    u_char green;
    u_char red;
    u_char alpha;
} color;
typedef union pixel {
    color color;
    u_int value;
} pixel;
typedef struct screen {
    u_int width;
    u_int height;
    pixel *pixels;
} screen;
typedef short (*audioframe)[441];

/**
 * @brief Indicates whether each key is currently being pressed.
 */
bool keys[256];
/**
 * @brief Indicates whether each key was pressed during the last call to update.
 */
bool pkeys[256];
/**
 * @brief Indicates if a character has been typed. Automatically reset to false after each call to update.
 */
bool kchars[256];
/**
 * @brief Indicates whether the left mouse button is currently being pressed.
 */
bool mouseLButton;
/**
 * @brief Indicates whether the right mouse button is currently being pressed.
 */
bool mouseRButton;
/**
 * @brief Indicates whether the mouse is currently in the screen (the part of the screen that the program can draw to).
 */
bool mouseInScreen;
/**
 * @brief Indicates the position of the mouse on screen
 */
int mouseX, mouseY;

// Program-defined

/**
 * @brief The program should define this to be the desired width and height of the screen. The program may change this during runtime.
 */
extern int viewSize;
/**
 * @brief The program should define this to be the desired color of the border around the drawable screen. Changes made during runtime will not be reflected.
 */
extern pixel bgColor;
/**
 * @brief The program should define this to be the desired title of the window. Changes made during runtime will not be reflected.
 */
extern char *winTitle;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function is called at the start of the program, after the window has been created.
 */
void init ();
/**
 * @brief This function is called whenever window messages are not being processed.
 * 
 * @param t The time the function is being called at, relative to the start of the program.
 * @param dt The difference in time between this call to the function and the last call to the function.
 */
void update (double t, double dt);
/**
 * @brief This function is called to allow the program to draw to the screen. 
 * 
 * @param view The screen to draw to.
 */
void draw (screen view);
/**
 * @brief This function is called to allow the program to play audio.
 * 
 * @param audio The samples to write to.
 * @param missed The number of audio frames that have been missed since the last call to play. Audio frames may be missed if the program lags or if window messages hold the program for too long.
 */
void play (audioframe audio, int missed);
/**
 * @brief Called when the program is about to close. This is to allow the program to deallocate any memory it allocated, as well as any other tasks it needs to do before closing.
 */
void cleanup ();

#ifdef __cplusplus
}
#endif

#endif // _BEAT_H