
# This is an implementation of "Manipulator interface" provided by Scienta for the SES program.

> The idea behind this dll is to pass all "commands" from the SES program to the manipulator program via a socket.
All of the heavy lifting (logic, boundries check and so on) is done by the manipulator program.

Our manipulator is controled by a python script which runs a GUI using the [Eel](https://github.com/ChrisKnott/Eel) library.
The messeges are handled by the manipulator program by running a thread that recives the messages from SES, and passes it to the main program thread.
Included is a mock-up python script for checking purpose (socket_ex.py).
A complete and working implementation of a class handeling the connection can be found [here](https://github.com/yuvalni/ARPESmotors/blob/main/Class/SESInterface.py).


