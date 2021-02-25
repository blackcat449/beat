# Beat
A simple framework for making games with low-level control

# What exactly is this?
This is a C framework that is meant to be run on the Windows operating system. It provides a window with a pixelated screen and audio, both of which can be written to directly. It is designed so that programmers who are interested in doing everything themselves can make their own methods for drawing frames or playing audio.

# What is this NOT?
 * This is not a library for interacting with the Windows operating system. If you want to open up more windows or use other features of Windows, you'll just have to modify the code that's there.
 * This is not a library for drawing pixelated graphics. It is a framework meant to provide (arguably) easier ways to do that yourself. (I guess people might one day decide to make and release libraries that can do that, possibly including myself. But in that case, that code is the library, and this is just the framework.)

# How can I use it?
If you look in the header file 'beat.h', you'll find a few variables and some functions that you need to implement. Here's a description of each of them:
 * `viewSize`: The number of pixels wide and high to make the screen. If the window gets resized, the screen will automatically scale to a size where all of the pixels look the same size, so you won't have any problems where the pixels look weird.
 * `bgColor`: The background color (more specifically, the color that surrounds the screen)
 * `winTitle`: A null-terminated string to be used as the window's title. At the time of writing, you can't change this mid-game, unfortunately.
 * `init()`: Called once after the window has been created.
 * `update(t, dt)`: Called as often as possible, specifically when not processing other window messages, drawing, or playing audio.
 * `draw(view)`: Called (about) every 1/60 seconds. Draw to `view.pixels`. To set a pixel at the location (x,y), write to `view.pixels[x+y*view.width]`. Note that view also has `width` and `height` members; this is just in case I ever add support for having separate values for the width and height of the screen. So if you're going to make a library, you probably shouldn't get lazy and use `viewSize` instead. Keep in mind that the screen is uninitialized, so if you don't write to a pixel, there's no guarantee what color it will be.
 * `play(audio, missed)`: Called (about) every 1/100 seconds. `audio` is an array with 441 samples in the form of short integers. Again, keep in mind that the audio frame is uninitialized, so if you don't write to a frame, there's no guarantee what amplitude it will be. So if you're trying it for the first time, you should probably make sure to fill all of the samples with 0's, because otherwise you'll probably hear a noise like static. Also note the `missed` variable. This is to tell you how many frames of audio went by without any audio being played. You should probably use this because there's no way to know where the audio is aligned without tracking how many have already passed, and sometimes frames of audio get missed, especially when Windows decides to keep the window busy with messages, particularly when moving or resizing the window. If you don't use it, you may end up writing every frame of audio that gets missed, and then it's just going to get behind since all audio frames always play in order.
 * `cleanup()`: Called when the program is about to close. This is just to give the program the chance to free any memory or close anything it needs to before the program closes. Nothing needs to go here.

There are also variables that hold input which is meant to be accessed in the update function. (I guess you can access most of them fine in the other functions... but why would you do that?? Well, other than for the mouse...). The arrays `keys` and `pkeys` tell you whether each key is / was pressed at the moment; use the virtual keycodes provided in `<winuser.h>` to acces them. `kchars` tells you which characters are being typed on the keyboard; use the ASCII character constants to access these. (At least, I think that works for all of them...) `mouseLButton` and `mouseRButton` tells you whether the left and right mouse buttons are being pressed. `mouseX` and `mouseY` tells you what pixel the mouse is at on the screen (the game screen, not the monitor screen), and `mouseInScreen` tells you whether the mouse is actually in the window (I guess you don't really NEED that since you can figure that out yourself, but it's a freebie for you). Currently, there's no support for the mouse wheel (Why not? I got lazy.), for game controllers, or pretty much anything else. But if I update this ever, I'll probably at least try to add support for those two things.

Final things to note: As expected, you should `#include "beat.h"`, and compile it with `beat.c`. You'll also need to link the program to `WinMM` and `Gdi32`. (Old, I know, but they work perfectly fine)

# Will I add to this?
Being honest, I probably won't. At least, not to the framework. But I am actually using this for my own projects, and if I feel up for it, I might package some of my code into a nice library for other people to use with my framework.

# Why is it called 'Beat'?
Good question. Not for any particular reason. Maybe because I had a different idea in mind when I started making it, and I never came up with a more appropriate name to replace it.
