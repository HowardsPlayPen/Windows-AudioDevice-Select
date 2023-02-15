# Windows-AudioDevice-Select
Simple C++ CLI to query the list of sound devices and to set / change the current default device

##
The background was that I wanted to programatically change the current / active Sound Device
I only wanted a simple command line app to integrate it with Macros for changing the device.

Once I wrote the code I found that other repositories exist for the same thing - e.g. https://github.com/yan0lovesha/AudioSwitch an implementation in Rust.

It was an interesting voyage though as this implementation uses an interface that is clearly a Windows supported one but must have been reverse engineered - hence the PolicyConfig.h file I downloaded from the internet. To me it is an example of the breathtaking craziness of Microsoft and why you need to jump through hoops to do such a simple task

To call this application you can merely :

audio-dev-enumerate --list

OR 

audio-dev-enumerate --device {}

where on my machine I can call:

audio-dev-enumerate --device {0.0.0.00000000}.{e774d123-a2b9-4445-b182-5b1d3632c0d8}

to change the device to a RealTek device present.


I integrated it with AutoHotKey (see https://www.autohotkey.com/) in order to bind it to macro keystrokes that I can press on my keyboard:

### Example AutoHotKey macro file - e.g. stored in a file called RealTek.ahk
    
    #Requires AutoHotkey v2.0
    ; RealTek
    ^!1::run "e:\bin\audio-dev-enumerate.exe --device {0.0.0.00000000}.{e774d123-a2b9-4445-b182-5b1d3632c0d8}"
    
    ;Benq
    ^!2::run "e:\bin\audio-dev-enumerate.exe --device {0.0.0.00000000}.{cf8907d5-be32-429b-abf4-fea8d2a6799a}"

