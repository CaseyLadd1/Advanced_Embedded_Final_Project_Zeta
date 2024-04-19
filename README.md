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

**Casey Ladd** is currently assuming responsibility for research of the audio and
video functions. The associated responsibilities include determining how to
allow our program to read and display graphics from our array, and determining
how to allow our board to play music.
