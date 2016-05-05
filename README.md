# NoteReader#

This Project was created for a fpga project where we had to create custom sheet music from real scores that we got from flat.io.

I tried to make the program as general as possible and tried out multiple algorithms i came up with, however after tweaking I came up with this program. 

It is not yet available on every platform, which is a goal that I am aiming at and does not yet support every type of symbol or sheet music as I only used my own algorithms to detect everything.

- - - - -

##How it works##

first measures the intensities of the note line in black and white seen below:

Horizontally
![alt tag](https://github.com/zedarider/NoteReader/blob/master/program_files/cut_hist_horiz.jpg)

and Vertically
![alt tag](https://github.com/zedarider/NoteReader/blob/master/program_files/cut_hist_vert.jpg)
