This project is a collaboration between Sarah Hemler, Paul Karhnak, Casey Ladd,
and John Berberian.

# Design and Concept

## Core Game

Our product will be a variation on the proposed "cube game" that aims to act as
a sort of mini-FPS (first person shooter). We intend to accomplish this by
implementing a "point-and-click" reticle that eliminates targets when lined up
with them and a button pressed. We also intend to add another layer of
complexity to this gameplay by adding in an ammunition supply that the player
will need to manage. During demo gameplay of the "cube game," we noticed that
it was difficult to judge which cubes were the closest to the end of their
life, making prioritization of targets more difficult. To remedy this, we plan
to add visual indicators (alternative sprites) for when "cubes" are near the
end of their lives.

## Expanded Functionality

In addition to these features, we want to use the high color and audio
capabilities of the mounted BSPMKII expansion board to create a richer
audiovisual experience with complex sprites and background music. Sprites will
be flashed to the Tiva board's ROM similarly to the flashing process for the
RTOS and game code. Background music will be produced on the mounted BSP MKII
expansion board buzzer module using the Tiva board's onboard pulse width
modulation (PWM) capabilities.

# Team Communication, Membership, and Responsibilities

We are coordinating our efforts through GitHub, in person, and on the chat
platform Discord.

**Sarah Hemler** is currently assuming responsibility of project manager and art
director. The associated responsibilities include driving frequent group
meetings and engagement, ensuring all parties understand the schedule and task
breakdown, creating flow charts to communicate the logic of the program, and
development of a sprite (bitmap image) array that the code will be able to pull
from.

**Paul Karhnak** is currently implementing and testing an xorshift-based
pseudorandom number generator (PRNG) to fulfill one of the core project
requirements. Furthermore, Karhnak plans to implement deadlock prevention
features in the final game, though this will first require implementing the
necessary synchronization see where in thread interactions deadlock should be
targeted to prevent.

**John Berberian** is currently assuming responsibility for writing the compositor
and display threads. This involves writing an algorithm to process and render events
from a synchronized FIFO. Depending on the complexity of implementation we decide
on, this may involve maintaining a parallel state and periodically rendering it at
20Hz.

**Casey Ladd** is currently assuming responsibility for research of the audio and
video functions. The associated responsibilities include determining how to
allow our program to read and display graphics from our array, and determining
how to allow our board to play music.

# Week 2 Progress

The team is continuing to work on the bitmap images, sound, and deadlock
prevention required to meet the project specifications. In particular, Sarah
has taken the lead on developing bitmap image graphics that use a 16-bit
high color palette, which is compatible with the LaunchPad's display capabilities. 
We are investigating several options for useful external libraries, now that we're
aware that we're able to use them. As an example, we're looking into using the tone() 
function and its related libraries present in Energia, as recommended in some user maunals for
the LaunchPad. We've also created a skeleton code in a source file for game-specific
mechanics, named gameplay.c, which will be filled out by the group as we connect the elements
of our program together. Additionally, we've created video.c and sound.c, which will handle our bitmap
arrays and our PWM buzzer functions, respectively.

The group is coordinating to divide up tasks whenever possible.
Our hope in the upcoming week is to flesh out the skeleton code, to complete our creation
of audio and visual resources, and to work out the best order for the creation and execution
of different threads.
