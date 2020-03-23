# QtOpenGuidance

A software to do a GPS-based guidance of farming equipment. It uses a principle called "dataflow programming": basicaly this
means every module of the software receives, modifies and then sends data. These modules are then connected in a meaningfull 
way, so a calculation network does the job of transforming the input data (GPS, wheelangle, roll, heading...) to a 3D-view on
the screen and finally outputs commmands for the equipment (steering commands, hitch, section control, etc).

It is pre-alpha now and the development is quite fast. I encourage other devs to add functionality to QtOpenGuidance, but
please contact me first (@eringerli on Telegram) for coordinating the effort. There's no sense in doing work twice.

## Word to the Wise
**Read this readme through! Multiple times if something is not clear!** There's a reason why this is called "Readme": RTFM!

## Warning
As clearly in the file LICENSE stated (especialy Section 15 through 17), there are no guaranties of any kind. You're responsible to
use the software responsibly. If you think, this software is fit to drive expensive equipment with potentialy deadly consequenses,
that's your thing.

## [TL;DR](https://www.urbandictionary.com/define.php?term=tl%3Bdr): Installing
Install and run [Manjaro Linux](https://manjaro.org/) on your machine, then enter in a console:
```
# update the system and install various needed packages
sudo pacman -Suy --needed base-devel bash-completion qt5 qtcreator gmp mpfr git
# delete the downloaded packages again, helps on small SSDs/HDs
sudo pacman -Scc --noconfirm

# download, build and install KDDockWidgets
cd
git clone https://github.com/eringerli/KDDockWidgets.git
mkdir build-KDDockWidgets ; cd build-KDDockWidgets
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/usr ../KDDockWidgets
make -j4
sudo make install

# get QtOpenGuidance
cd
git clone https://github.com/eringerli/QtOpenGuidance.git --recursive

# get CGAL
cd QtOpenGuidance/lib/
wget https://github.com/CGAL/cgal/releases/download/releases%2FCGAL-5.0.2/CGAL-5.0.2.tar.xz
tar xf CGAL-5.0.2.tar.xz

# build QtOpenGuidance
cd
mkdir build-QtOpenGuidance ; cd build-QtOpenGuidance
qmake -makefile -Wall ../QtOpenGuidance/QtOpenGuidance.pro -after CONFIG-=ccache KDDOCKWIDGET_LIBPATH=/usr
make -j4

# add a symlink to the desktop, replace "Schreibtisch" with the equivalent in your language
cd ~/Schreibtisch
ln -s ../build-QtOpenGuidance/QtOpenGuidance
```
Your set: double-click on the icon on the desktop and load a config. `config/minimal.json` in the QtOpenGuidance-repository 
should provide a good start. Don't forget to save the config as default in the tab "General". Enable the checkbox for the
simulator and start playing around.

## Dependecies

### General advice about Windows
[TL;DR](https://www.urbandictionary.com/define.php?term=tl%3Bdr): Install linux, especially if you want a stable,
fast and dataplan-saving system without required and unconvenient maintenance and general frickle-ness.

_Seriously, install Linux_. You have nothing to loose: it takes about 30 minutes on a resonably fast computer, from which
about 20 you can do other stuff. It takes on an average system (without Office) less than 20GB (_for everything_, not just
the bare system). [Manjaro](https://manjaro.org/) is a good distribution to start, also for Linux-beginners. Ubuntu is not
a good choice, as it differs too much from eringerlis system (Arch Linux), has generally too old versions installed and is
not overly development friendly: Manjaro installs the files for linking with each library (headers and debug-info) per default. 
Also, as we generally use weaker systems in our tractors (like the Dell 7130/7139/7140 or even slower tablets), the 
performance boost of having a lean system without all the Microsoft/Windows/Manufacturer clutter and background processes 
is quite noticable: the fan is not running anymore all time and the battery lasts forever. You never have to worry 
about automatic background downloads/updates and generally no control over your system (like no option to cancel a
reboot or update) ever again. Bricking your tablet by interrupting a overly long/hanging update before a shutdown
is also not possible.

### QT
#### Linux
Use your distributions packet management system. Make sure to install a git, a compiler, QT5 (QT itself, Qt3D, qtserialport, etc...) and qtcreator. Install the development and debug packages too (`-dev`, `-dbg`), if your distribution splits them into different packages. If you get an error about not found components, first make sure you have them installed.

#### Ubuntu
Use Ubuntu 19.04, as 18.04 LTS has an ancient version of QT (QT5.9.3), which does't work with QtOpenGuidance.

To install the dependencies, open a terminal and execute the following commands:
```sudo apt-get install build-essential qt3d5-dev qt3d5-dev-tools qt3d5-examples qtcreator qt5-default qt5-qmake libqt5serialport5 libqt5serialport5-dev git gitk```

#### If it doesn't work
Install the binaries from https://qt.io on linux and use the bundled QtCreator. This ensures, that the right version of QT is used.

#### Windows
Use the normal installer of QT from this [link](https://www.qt.io/download-qt-installer). As were are installing Qt and all other dependencies with MSYS2, basicaly all you need from this installer is QtCreator, which is automatically selected.

### Git
#### Linux
Most likely, git is already installed. If not, search for it in the package repositories of your system.
#### Windows
Install git from https://git-scm.com.

### CGAL
#### Linux & Windows
Download CGAL (the zip, not the setup) and extract it in `lib/`. Use the Version `5.0.2` from https://github.com/CGAL/cgal/releases

### MSYS2
#### Linux
Not needed.
#### Windows
On Windows, it is much simpler to use MSYS2 instead of the bundled MinGW in Qt. This takes also care of GMP/MPFR (needed by CGAL), Boost (also needed by CGAL) and QT itself. Also you get a recent compiler and everything works pretty much out of the box.
To install all the required packages, follow the instructions below.

1. Download from https://www.msys2.org/. Make sure to use the 64bit-version.
1. Run the installer
1. Execute the following commands (change the path to your QT path) in an MSYS2-console:
```
pacman -Syu
```
1. close the window
1. open a new MSYS2-console and execute the following commands:
```
pacman -Syu
pacman -S base-devel mingw-w64-x86_64-qt5 git mingw-w64-x86_64-clang mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make mingw-w64-x86_64-gdb mingw-w64-x86_64-gmp mingw-w64-x86_64-mpfr mingw-w64-x86_64-boost
```

After that, you have to register the newly installed toolchain in QtCreator (save the changes everytime before you change the tab with a click in `Apply`). Choose wise names to find the stuff later on.
1. Open `Tools->Options` and go to the tab `Qt Versions`. Add the toolchain by clicking on `Add` and selecting the program
   `qmake.exe`. If you installed everything in the default localtions, you find it in `C:\msys64\mingw64\bin`.
1. Go to the tab `Compiler`. Here you add a new MinGW-C++-Compiler and set the path to `C:\msys64\mingw64\bin\g++.exe`. Do the same for a new MinGW-C-Compiler, but choose `C:\msys64\mingw64\bin\gcc.exe`
1. Go to the tab `Debuger` and add new one. Select `C:\msys64\mingw64\bin\gdb.exe`
1. Go to the tab `CMake` and add new one. Select `C:\msys64\mingw64\bin\cmake.exe`
1. Finnaly go to the tab `Kits`, make a new one and select all the newly added stuff.
1. Change `CMake Generator` to `MinGW Makefiles` (scroll all the way down and click in `Change`).
1. Close the dialog

**Attention:** to actually use the new kit, you have add new build-settings by clicking first on `Projects` and then on the name of the newly added kit.

### KDAB KDockWidgets
#### Linux
Open a terminal and clone KDDockWidgets:
```git clone https://github.com/eringerli/KDDockWidgets.git```
And build/install:
```
mkdir KDDockWidgets.build && cd KDDockWidgets.build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/opt/KDAB ../KDDockWidgets
make
sudo make install
```

#### Windows
Open a msys2-mingw64-terminal and clone KDDockWidgets:
`git clone https://github.com/eringerli/KDDockWidgets.git`
And build/install:
```
mkdir KDDockWidgets.build && cd KDDockWidgets.build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/c/msys64/mingw64/ ../KDDockWidgets
/c/msys64/mingw64/bin/mingw32-make.exe
/c/msys64/mingw64/bin/mingw32-make.exe install
```

### Android
It works kind of stable, but you have to jump through many hoops. Some hints:
* install all the SDK/NDK-stuff
* compile QT for android
* compile KDockWidget for Android
* you have to install Boost. The easiest way is to use the script in https://github.com/moritz-wundke/Boost-for-Android. Put the compiled stuff in `android/` and edit `QtOpenGuidance.pro` to include and link with the right architecture.
* install GMP and MPFR (search for "install CGAL on android")
* dance around a fire on a new moon on midnight and hope, that it works...

## Cloning
The easiest way is to use QtCreator: go to welcome, the to project and hit "New Project". Choose "Import Project" and in the list "Git Clone". Configure the repository-URL and **make sure to enable "recursive"**. Hit "Import". qtcreator should help you from here on out.

For the diehards out there who use git bash (like me), you have to `git submodule init` and then `git submodule update`.

## Compiling
Open the project in qtcreator, change the buildtype to "Release" (the small screen-icon on the lower left), compile and run it (the green triangle). If the buttons are grayed out, do as suggested by qtcreator.

It is developed on linux, but should work on any platform supported by QT and Qt3D.

## Running
To make something useful with the software and to test its functions, open the setup dialog and load a configuration out of the `config/` folder. `minimal.json` should work everytime, the others should too, but are sometimes not kept up to date with the development. Click on the checkbox for the simulator and you can steer the GPS-source.

You get the structure of the configuration quite fast if you look at the different configurations. Don't try to edit them by hand, as it is possible, but not at all comfortable. You can do all the editing in the setup dialog. Deleting works by either hit the `del`-key or the button. Zooming works with the mousewheel or the buttons, paning by right-click and drag.

To add new connections drag from a red circle to another. If the two ports are compatible, the connection is made instantly and data is transfered. To alter a value of a block, you can edit that in the next tab by double-clicking on a cell in the table view.

If it crashes on start, try to use another build type.

## Keeping it up to date
1. **Don't fork it**
1. open a Git Bash Console
1. `git pull`
1. `git submodule init`
1. `git submodule update`

## Issues and Bugs
If you find an issue or a bug, report them on github.

## Forking
It is possible to run this software without forking. So if you're not interested in making changes yourself, don't. To just 
remember the repository, there's a small star.

A normal clone (not with the function "download ZIP") can be easily kept up-to-date. To keep a fork synchronised with the
forked from repository, more work is needed. A downloaded ZIP lacks the nessesary data for git.

## Contributing

If you want to contribute to the project, there are some rules:

- Fork it on github, send pull requests with a clear and compelling description
- Good and complete commit descriptions, no "WIP"-commits without any hint what is changed
- Have a plan what to change and what to achieve with it. Read and comprehend the existing code.
- Coding style rules:
   - 2 spaces indentation, no tabs
   - Blocks are opened on the same line (pe. `if(...) {`)
   - spaces around operators and on the inside but not outside of parentheses
- Meaningful and descriptive names. If you alter the meaning of a temporary variable, also change its name.
- Generaly keep a dataflow-paradigm in the code: data comes in, gets altered/calculated and then gets sent to the next block
- Use meaningful datatypes to exchange data; steerangle alone is ok, also `Tile*, Vector3D, QQuaterion` for the position+orientation (aka pose), as they are alone meaningless. But don't mash together logicaly different things like velocity + roll. Use seperate signals/slots if it makes sense, instead of one with all the data. The same goes for data streams like UDP-sockets or serial communications.
- Use SI-units: seconds, meters. Convert the data once on entry and once on exit. Use degrees in external data exchange (like with [esp32-aog](https://github.com/eringerli/esp32-aog)), but quaternions or rad internaly.
- If you want to show different units in the GUI, make the conversion in one place and as the last step before showing.
- As we develop for modern machines, floating point calculations are as efficient as integer math (and we use modern compilers, so much of the expression if known at compile time gets optimized out). Don't do divisions as multiplications or other such hard to comprehend math. Use `constexpr` as much as possible. If you optimize an algorythm, document the simplest version of it, so it can be understood with little effort.
- No global variables to exchange data, all data and config gets exchanged with signals and slots.
- Keep things as local as possible, make as few globals as needed. Local scoping (`{}` without a `for, while(), if()` etc) are encouraged.
- When adding reused objects/variables, use the factories to pass a pointer to the blocks. Keep the factories in the same file as the block itself.
- Use as much of QT as possible, especialy Qt3D, the `QVector3D`/`QQuaternion`-based math, the `QObject`-model, `QString`, `QNetwork` and the classes for XML, JSON etc. Don't homegrow something if there is already an implementation. The chance is great, that QTs version is not only more robust and works in more cornercases but is also more performant.
- You can add libraries, but it has to be as a git submodule and built together with QtOpenGuidance. If there is a compelling reason to do otherwise, clearly state it in the commit message.
- Don't introduce platform-specific code, use the avaible QT-abstractions
- QT and Qt3D are pretty performant, so no "optimized but unreadable" code (aka write-only code). Qtcreater has builtin support for profiling; implement new features clean and straightforward, then profile and only optimize if needed. Saving RAM and storage space is pretty pointless on modern machines, saving cycles most of the time too. So generally: better implement new features instead of getting +1% more performance on codepaths which are not critical.
- This isn't a collection of proof of concepts, but a maintainable, usable and readable codebase for a RTK-based guidance. So keep your implementations readable, documented and clean of premature optimitions.
- Be as compatible to standards as possible: if there is already an established format like json, J1939 or GPX, use that. Also be as compatible as possible to [AgOpenGPS](https://github.com/farmerbriantee/AgOpenGPS), especialy with the I/O for the motor and sensors. Use the possibilities of QT to read and write them. Normaly, data exchange is not performance critical, so no super optimized code there (see also the points above). No problem if the user waits 0.1s longer to read the previous field with QT-classes instead of a homegrown implementation.
- Experiment in your own branches, don't break the master-branch or configfiles without a really compelling reason.
- If you are still with me; these rules ensure that we create a maintainable codebase which can attract new devs. And this is the most important thing there is for such a project. Not only that it runs on virtualy every kind of machine out there (incl. Raspberry Pis and other SBC), but also that it is fun to implement new features in a way that helps the whole project. So if you are interested in helping me with this project (even by reporting a bug or do a translation), don't hesitate, fork the repository and send me a pull request.
