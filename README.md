# QtOpenGuidance

A software to do a GPS-based guidance of farming equipment. It uses a principle called "dataflow programming": basicaly this means 
every module of the software receives, modifies and then sends data. These modules are then connected in a meaningful way, so a 
calculation network does the job of transforming the input data (GPS, wheelangle, roll, heading...) to a 3D-view on the screen and 
finally outputs commmands for the equipment (steering commands, hitch, section control, etc).

## Warning
As clearly in the file LICENSE stated (especialy Section 15 through 17), there are no guaranties of any kind. You're responsible to
use the software responsibly. If you think, this software is fit to drive expensive equipment with potentialy deadly consequenses,
that's you thing.

## Compiling/Running
This software uses QT5 and Qt3D. Install them on your machine along with qtcreator. Open the project in qtcreator, change the buildtype
to "Release" (the small screen-icon on the lower left), compile and run it (the green triangle). It is developed on linux, but should 
work on any platform supported by QT and Qt3D.

## Issues and Bugs
If you find an issue or a bug, report them on github.

## Forking
It is possible to run this software without forking. So if you're not interested in making changes yourself, don't. A normal clone (not
with the function "download ZIP") can be easily kept up-to-date with a periodical ```git pull```. To keep a fork synchronised with the
forked from repository, more work is needed. A downloaded ZIP lacks the nessesary data for git and cannot be actualised with ```git pull```.
