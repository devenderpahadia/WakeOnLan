# WakeOnLan
The Project Contains Two Folders:

1.WakeOnLan.xcodeproj-This Contains The Workspace files Of xcode Project

2.WakeOnLan - This contians the header file and c file of Wake On Lan.

This contains a c program named “main.c”and a header file named ”wakeOnLan.h”.This Program is designed to send Magic Packet to the Targeted Pc to turn it on.

Prerequisite:

Must turn on the wake on lan by going into the bios settings.

A lan wire must be connected to the target pc.

Mac Address and IP address are needed and can be found by going into the modem page, like 192.168.1.1/192.168.2.1,etc

How To Complie:

Open Terminal/cmd on the folder of the project.

gcc -Wall -o wol WakeOnLan/main.c

How To run:

Usage: ./main.c [-r remoteaddress] [-f filename1, ...|mac1, ...]

You can either pass some MAC addresses or you can pass with the option f some filenames contains a MAC address on each line to wake up.
With the option r you can specify the remote ip address. The default value is 255.255.255.255 for a broadcast call.

Examples:

    $ ./wol 00:0B:CD:39:2D:E9 0E:FD:FA:33:5D:A6
    $ ./wol -r 192.168.1.36 00:0B:CD:39:2D:E9 0E:FD:FA:33:5D:A6 //Preferable
    $ ./wol -f macaddresses
    $ ./wol -r 192.168.1.36 -f macaddresses. 
    $ ./wol -f macaddresses macaddresses2

MAC address File Syntax:

The mac address syntax is very simple. It expects one mac address per line.
Lines starting with `#` are comment lines and will be ignored.

Valid Syntax:

    # Mac address of host 192.168.1.2
    00:0B:CD:39:2D:E9

    # Brothers mac address
    00:05:FE:AB:3D:99

    # Servers mac address
    00:32:D5:35:EF:63

Screenshot : Successfully sent Magic Packet!!
  ![alt text](https://github.com/garvitchaudhary9/WakeOnLan/blob/master/ScreenshotOfWorkingOfWakeOnLan.png)  

Note that I and Devender Kumar Pahadia are the only contributors to the project when I was working on the project I accidently committed on git via my Brother's github profile on the local Repository,thats why in some of the files like main.c and WakeOnLan.h he is also visible as one of the contributor.    
    
    
