
# This is an implementation of the "Manipulator interface" provided by Scienta for the SES program (for performing [ARPES](https://en.wikipedia.org/wiki/Angle-resolved_photoemission_spectroscopy) experiments).

### The idea behind this dll is to pass all "commands" from the SES program to the manipulator program via a socket.
All of the heavy lifting (logic, boundries check and so on) is done by the manipulator program.

Our manipulator is controled by a python script which runs a GUI using the [Eel](https://github.com/ChrisKnott/Eel) library.<br>
The messeges are handled by the manipulator program by running a thread that recives the messages from SES, and passes it to the main program thread.<br>
Included is a mock-up python script for checking purpose (socket_ex.py).<br>
A complete and working implementation of a class handeling the connection can be found [here](https://github.com/yuvalni/ARPESmotors/blob/main/Class/SESInterface.py).

> please note: I am not a professional programmer! The namings may be awkward, this might not be the most efficient or organized code - but it works.
