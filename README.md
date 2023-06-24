
# This is an implementation of the "Manipulator interface" provided by Scienta for the SES program (for performing [ARPES](https://en.wikipedia.org/wiki/Angle-resolved_photoemission_spectroscopy) experiments).

a manipulator is a set of motors that controls the position and orientation of a sample/device.

### The idea behind this dll is to pass all positioning "commands" from the SES program to the manipulator program via a socket connection.
All of the heavy lifting (logic, boundries check and so on) is done by the manipulator python program.

Our manipulator (6 independent motors) is controled by a python script which runs a GUI using the [Eel](https://github.com/ChrisKnott/Eel) library.<br>
The messeges are handled by the manipulator program by running a thread that recives the messages from SES, and passes it to the main program thread.<br>
Included is a mock-up python script for testing purpose (socket_ex.py).<br>
A complete and working implementation of a class handeling the connection can be found [here](https://github.com/yuvalni/ARPESmotors/blob/main/Class/SESInterface.py).

> please note: I am not a professional programmer! The namings may be awkward, this might not be the most efficient or organized code - but it works (at your own risk).

## Automation
This functionality allows us to use automated sequences.<br>
**Essentialy**: the SES measurement program "thinks" it is in control of the motors, while the python code "intercepts" the commands and acts according to it's own logic.
This procedure let's us, for example, measure microns-sized samples where a positioning correction is required whenever one of the angles is changed.
