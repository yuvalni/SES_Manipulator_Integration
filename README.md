This is an implementation of "Manipulator interface" provided by Scienta for the SES program.

The idea behind this dll is to pass all "commands" from the SES program to the manipulator program via a socket.
All of the heavy lifting (logic, boundries check and so on) is done by the manipulator program.

Our manipulator is controled by a python script which runs a GUI using the [eel](https://github.com/ChrisKnott/Eel) library.
