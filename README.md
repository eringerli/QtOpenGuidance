# QtOpenGuidance

A software for GPS-based guidance of farming equipment. It uses a principle called [dataflow programming](https://en.wikipedia.org/wiki/Dataflow_programming): basicaly this
means every module of the software receives, modifies and then sends data. These modules are then connected in a meaningfull 
way, so a calculation network does the job of transforming the input data (GPS, wheelangle, roll, heading...) to a 3D-view on the screen and finally into commmands for the equipment (steering commands, hitch, section control, etc).

It is pre-alpha now and the development is quite fast. I encourage other devs to add functionality to QtOpenGuidance, but
please contact me first (@eringerli on Telegram) for coordinating the effort. There's no sense in doing work twice. This also means that the (meager) documentation there is, can be obsolete quite fast.

## Word to the Wise
**Read this readme through! Multiple times if something is not clear!** There's a reason why this is called "Readme": RTFM!

## Warning
The GPLv3 states clearly in Section 15 through 17 (the whole text is in the file LICENSE), there are no guaranties of any kind. You're responsible to
use the software responsibly. If you think, this software is fit to drive expensive equipment with potentialy deadly consequenses,
that's your thing. Test it thorously before using it on actual equipment, *and then test it on the equipment itself too*. Never leave it unsupervised, pe. by leaving the drivers seat or sleeping behind the wheel (that would be basic common sense).

## [TL;DR](https://www.urbandictionary.com/define.php?term=tl%3Bdr): Installing on Linux
Install and run [Manjaro Linux](https://manjaro.org/), either natively or in a virtual machine (instructions [here](https://manjaro.org/support/firststeps/)). This also works on [Arch Linux](https://www.archlinux.org), but Arch is definitively not for Linux beginners.

> It won't work with most Debian based Linux (this includes Ubuntu and its derivates like Kubuntu), as they use too old versions of critical libraries, especially Qt. And no, there isn't an easy way to fix that, except compiling and installing the newest versions yourself or adding more (most of the time only half maintained) package repositories to the system (PPAs), which either takes a lot of knowledge and time, is boring and tedious and/or leaves the system in a very brittle state. So please don't open an issue for it or ask for support, just install Manjaro.
>
>Newer version *can* work, but the focus of development isn't in keeping it compatible with the most ancient versions of libraries available, but using the existing ones to the extent of their functionality and getting the latest bug fixes. This also means using a reasonably new C++ standard (as of now C++17), which in turn needs a recent compiler. *You have been hereby **strongly** discouraged of using old versions of distributions and/or keeping your system not up to date.*

Then enter in a console:
```
# update the system and install various needed packages
sudo pacman -Suy --needed base-devel bash-completion wget
# delete the downloaded packages again, helps on small SSDs/HDs
sudo pacman -Scc --noconfirm

# make some folders for the building of the packages
mkdir -p ~/qtopenguidance-build/{QtOpenGuidance,KDDockWidgets}

# download, build and install KDDockWidgets
cd ~/qtopenguidance-build/KDDockWidgets
wget https://raw.githubusercontent.com/eringerli/KDDockWidgets/pkgbuild/PKGBUILD -O PKGBUILD
makepkg -is

# download, build and install QtOpenGuidance
cd ~/qtopenguidance-build/QtOpenGuidance
wget https://raw.githubusercontent.com/eringerli/QtOpenGuidance/master/PKGBUILD -O PKGBUILD
makepkg -is
```
Your set: QtOpenGuidance was added to the systems start menu, so you can open it from there.

To update it, run the same commands again. If it doesn't work, you can delete the whole folder with `rm -rf ~/qtopenguidance-build` and try again. Should you ever want to remove the two packages, enter `pacman -R qtopenguidance-git kddockwidgets-eringerli-git`.

For the next steps, go to [Quickstart](README.md#quickstart).

## Windows
### General advice about Windows
TL;DR: Install linux, especially if you want a stable, fast and dataplan-saving system without required and unconvenient maintenance and general frickle-ness.

_Seriously, install Linux_. You have nothing to loose: it takes about 15-30 minutes on a resonably fast computer, from which
more than half you can do other stuff. It takes on an average system (without office applications) less than 20GB (_for everything_, not just
the bare system, the bare system itself takes less than 5GB). [Manjaro](https://manjaro.org/) is a good distribution to start, also for Linux-beginners, as it balances performance with simplicity to use. Ubuntu is not
a good choice (see intro above) and is not overly development friendly: Manjaro installs the files for linking with each library (headers and debug-info) per default.

Also, as we generally use weaker systems in our tractors (like the Dell 7130/7139/7140 or even slower tablets), the 
performance boost of having a lean system without all the Microsoft/Windows/Manufacturer clutter and background processes 
is quite noticable: the fan is not running all time anymore and the battery lasts forever. You never have to worry 
about automatic background downloads/updates (especially dowloads can be expensive if you use a limited, cheap connection for the RTK corrections) and generally no control over your system (like no option to cancel a reboot or updates) ever 
again. Bricking your tablet by interrupting a overly long/hanging update before a shutdown is also not possible, as all updates are triggered by the user. Additionally the building of the packages is automated and they can be removed again.

One important difference from Linux to Windows is the different aproaches when it comes to the shared libraries: Linux loads one of each library into the RAM, which then is used by all installed programs on the system. Windows loads all libs for
each installed packate, even if they are exactly the same. This lowers the used RAM on Linux tremenduously, which is 
especially noticeble on system with only a small amount of it. Also on Linux, you update the whole system, which includes
_all_ installed libraries and programs in one go.

### Installing with MSYS2
#### Prerequisites
* You're willing to invest some time searching for solutions on the internet. The procedure below is as streamlined as it can be, but sometimes, there are errors none the less.
* More than 4Gb of free space
* A working internet connection, the total volume downloaded is about 1.5GB
* The whole thing takes more than an hour (at least on my virtual machine, so [YMMV](https://www.urbandictionary.com/define.php?term=ymmv)), in this time you have a running Manjaro Linux *even with installing the whole system*

#### Installing
1. go to (https://www.msys2.org/) and download the installer for 64bit
1. install it according to the instructions; scroll a bit down from the download link. Close the terminal afterwards.
1. Open "MSYS2 MingW 64bit" (not "MSYS2 MSYS" or "MSYS2 MingW 32bit")
1. enter `pacman -S --needed base-devel mingw-w64-x86_64-gcc mingw-w64-x86_64-qt5 git mingw-w64-x86_64-cmake mingw-w64-x86_64-make mingw-w64-x86_64-gmp mingw-w64-x86_64-mpfr mingw-w64-x86_64-boost mingw-w64-x86_64-ninja`
1. enter 
```
# make a new folder in C: and change to it
mkdir /c/qtopenguidance
cd /c/qtopenguidance

# clone, build and install KDDockWidgets
git clone https://github.com/eringerli/KDDockWidgets.git
mkdir build-KDDockWidgets ; cd build-KDDockWidgets
cmake -G Ninja -DCMAKE_INSTALL_PREFIX=/usr ../KDDockWidgets
ninja
ninja install

# clone QtOpenGuidance with all the submodules
cd /c/qtopenguidance/
git clone --recursive https://github.com/eringerli/QtOpenGuidance

# get CGAL
cd QtOpenGuidance/lib/
wget https://github.com/CGAL/cgal/releases/download/v5.1.1/CGAL-5.1.1.tar.xz
tar xf CGAL-5.1.1.tar.xz

# build QtOpenGuidance
cd /c/qtopenguidance/
mkdir build-QtOpenGuidance ; cd build-QtOpenGuidance
qmake -makefile -Wall ../QtOpenGuidance/QtOpenGuidance.pro -after CONFIG-=ccache -after CONFIG-=precompile_header -after LIBS+=-L/c/msys64/usr/lib LIBS+=-L/mingw64/lib -after INCLUDEPATH+=/c/msys64/usr/include
make -j8
```

**Important:** Choose an appropriate amount of parallel processes in the last command: rule of thumb is one for every 800MB of free RAM.

To make a shortcut on the desktop, right-click on it, choose "New" and then "Shortcut". Enter in the text field for the location: `C:\msys64\usr\bin\mintty.exe -w hide /bin/env MSYSTEM=MINGW64 /bin/bash -lc /c/qtopenguidance/build-QtOpenGuidance/release/QtOpenGuidance.exe`

For the next steps, go to [Quickstart](README.md#quickstart).

## Rapsberry Pi
### Hardware
Use a Raspberry Pi4 with at least 2GB of RAM and a fast SD card (minimum Class 10, better UHS 1 or 3) with 16GB or more of storage. Use at least a HD/720p-display, better FullHD/1080p (testing is done with FullHD). Also make sure you have sufficient power for the RPI, 5V/3A is recomended. The RPI4 requires a lot of power, so make sure you use the supplied power brick *and a known good cable*. If you build your own supply for the tractor, keep these requirements in mind.

### Installing Manjaro Linux
There are numerous guides on the internet how to flash an image to an SD-card. Use one of those. But basically:
1. download an image from https://osdn.net/projects/manjaro-arm/storage/rpi4/xfce/
1. decompress and flash it to a SD-card according to a guide of your choosing
1. connect all the hardware to the pi, especially the screen, the keyboard and the mouse
1. boot it up
1. configure to you liking, but **use safe passwords!**
1. let it reboot
1. log in
1. some settings have to be changed from the default:
   1. change from the dark theme to a light one: open the settings, go to "Appearance" and choose one starting with "Matcha-light-". You can use a dark theme, but the development is done on a light one. Also the application honors the chosen icon set. Change it to Oxygen to get the best experience.
   1. if you want to use SSH, you have to install and start/enable the SSH-server with:
  ```
  sudo pacman -S openssh
  sudo systemctl enable sshd.service
  sudo systemctl start sshd.service
  ```

### Installing QtOpenguidance
It is exactly the same as for the X86_64 described [here](https://github.com/eringerli/QtOpenGuidance#tldr-installing), it just takes longer :wink:.

**Attention:** Don't select the "-es2"-versions of QT, choose wisely when asked. If you want to change it back, execute the following command: `sudo pacman -S qt5-base qt5-declarative qt5-multimedia qt5-wayland` and explicitly press `Y` to resolve the conflicts.

For the next steps, go to [Quickstart](README.md#quickstart).

### Troubleshooting
#### Resolution of the display
If the resolution is not set correctly for you display, first check if it is a configuration issue by opening the settings for the display and choosing the best resolution. If that is not enough, open the manual for your display and add the respective lines starting with "hdmi_" to the file `/boot/config.txt` with the `nano`-editor: `sudo nano /boot/config.txt`. Exit it with `Ctrl+X` and affirm the question about the accepting of the changes. Other lines are normally not required. Reboot and choose the right resolution in the settings panel.

#### Performance
If you experience a really slow system, normally it's caused by a too weak power supply. This in indicated by a randomly flashing red LED on the PCB and the yellow flash in the upper right corner of the display.

Another cause can be thermal throttling. In this case the system slows down if it gets too hot. Consider active cooling, some heat spreaders and/or better air circulation.

#### Linking errors
These are caused by the ES2-version of QT. Please follow the instructions exactly. You have been warned...

#### Doesn't start/crashes with a SEGFAULT
See [Issue #1](https://github.com/eringerli/QtOpenGuidance/issues/1). Let's hope, they fix it soon in ARM64 too...

## Android
You have to jump through many hoops. Some hints:
* install all the SDK/NDK-stuff
* compile QT for android (takes about 1-4 hours, depending on the performance of the CPU and the choosen modules)
* compile and install KDockWidget
* compile and install Boost. The easiest way is to use the script in https://github.com/moritz-wundke/Boost-for-Android. Put the compiled stuff in `android/` and edit `QtOpenGuidance.pro` to include and link with the right architecture.
* install GMP and MPFR (search for "install CGAL on android")
* dance around a fire on a new moon on midnight and hope that it works on the first try...

It *can* run on Android, but the GUI is in no way optimized for it: to actually set it up you have to connect a mouse anyway, a bluetooth connected one works best in this case. Just because it is doable, doesn't imply usefulness or added value over an RPI or a x86-tablet. Also there is the question of transmiting data to and from it. Android has a strict seperation between apps (so data exchange and opening downloaded files is harder to do), so it generally a big hassle to actually use it. If you have expierence in cross-compiling for Android, it is doable. If you don't have, better leave it be, your time is better spent in searching for a reasonably priced used x86-tablet.

## Developing
If you want to implement new stuff, you have to clone the repository and open it in QtCreator. Developing on Linux is much simpler than on Windows, but both are possible (you have to add a custom toolchain/kit to QtCreator).

### Dependencies
The simplest way is to install all the dependencies simply follow the guide for installing on Linux/Windows. Everything is then installed automatically. CGAL has to be installed seperately, follow the part about it in the instructions for MSYS2.

### Cloning
The easiest way is to clone it recursively, either by adding `--recursive` to `git clone` or ticking the box on importing a new project in QtCreator.

For the diehards out there who use Git Bash and have forgotten to add `--recursive`, or after pulling the newest commits in the repository, you have to execute `git submodule init` and then `git submodule update`.

### Keeping it up to date
**Don't fork it, until you are sure you need to!**

If you didn't fork:
1. open a Git Bash Console in the repository
1. `git pull`
1. `git submodule init`
1. `git submodule update`

If you do, consult a good guide about upstream merging.

### Compiling
Open the project in QtCreator, change the buildtype to "Release" (the small screen-icon on the lower left), compile and run it (the green triangle). If the buttons are grayed out, do as suggested by QtCreator.

#### If it doesn't compile
* Please update your repository to the most recent version. I sometimes do a force-push, `git reset --hard origin/master` is neccessary in this case. **This removes all local changes, so be carefull!**
* If some files are not found, it is most likely because a new submodule was added. To initialize that, open Git Bash inside the QtOpenGuidance-folder and execute `git submodule update` and `git submodule init`
* A clean build helps most of the times, especially after an update. Either enter `make clean` in the terminal or choose the option in the build menu of QtCreator.

## Issues and Bugs
If you find an issue or a bug, report them on github. Also, there's a Telegram group: https://t.me/QtOpenGuidance.

## Forking
It is possible to run this software without forking. So if you're not interested in making changes yourself, don't. To just 
remember the repository, there's a small star or the watch button.

A normal clone (not with the function "download ZIP") can be easily kept up-to-date. To keep a fork synchronised with the
forked from repository, more work is needed. A downloaded ZIP lacks the nessesary data for git.

## Quickstart
To make something useful with the software and to test its functions, open the settings dialog and load a configuration out of the `config/` folder in the repository or `/usr/share/QtOpenGuidance/config` if you have installed it systemwide. `minimal.json` should work most of the time, is generally kept up to date with the development and give a good overview on the implemented functionality.

### Appearance
Normally, you want to change from the dark theme to a light one: open the system settings, go to "Appearance" and choose one starting with "Matcha-light-". You can use a dark theme, but the development is done on a light one. Also the application honors the chosen icon set. Change it to Oxygen to get the best experience, but you can try others too: they even automatically update after pushing "Apply", so you can leave QtOpenGuidance open and see the result immidietly.

### Settings Dialog
You get the structure of the configuration quite fast if you look at the first tab in the settings tab. The blocks represent different kinds of logical parts in the software and are color coded, like green for calculations or red for docks. Deleting and zooming works as you would expect with the mousewheel or the `DEL`-key, paning by right-click and drag, right-click on a block deletes it. Almost every element in the GUI has it's counterpart in this view, also all the calculation and most of the input values are connected here. The connecting lines represent a path for data. You can try to connect different things, if a line appears (it checks if the source and sink are compatible), the data flows immidietly. It looks overwhelming at first, but by adding blocks and connections, almost all possible (and impossible) configurations can be implemented. It follows a data flow paradigm: data is received (left side), processed and sent (right side) by as simple as possible logical blocks. This makes it more complicated to look at, but makes development easier and also configurations possible, which aren't explicitly developed for. Loading works incremental, so you can also save parts of you configuration an load it multiple times. This also means that you have to "Clear All" if you want to load a complete config.

The second tab is for general configuration. You most likely want to enable the loading of the configuration and the positions of the docks on start. Here you can load and save the arrangement of the docks: for the config `config/minimal.json` the file `config/docks.json` contains a possible arrangement.

The third tab is for finer controll of the value block (the gold ones).

The fourth is for the visualisation of the data in the main window, like the displayed unit or the used font (the red ones).

The fifth is for the channel IDs for the communication with the esp32 for controlling the tractor (the light blue ones).

The sixth for configuring the implement.

### Arranging the Main Window
After you configured it to your linking and closed the setting dialog, you most likely want to arrange all the different docks in the main window. You can also hide/show the docks with a press on the button "Docks". If it somehow screwes up the layout (it's stressing the framework quite well, so sometime it has some hickups), try to resize the window a bit, the avaible space should be used better now. If your happy with the arrangement, don't forget to save it in the settings dialog. The config `minimal.json` is used in the development of the software, so it contains quite many docks. If you don't want them, either hide or delete them in the settings dialog.

### Simulator
Now you're set to use the simulator: enable it with a click in the checkbox and adjust the sliders for velocity or steer angle. If you have a space mouse and are on Linux, chances are you can use it too for this. Press A/B for creating a new guidance-line and the the play button to enable the automatic steering. The software is completely asynchronous, so without a data source like the simulator, the main view for the guidance is not drawn. With the camera toolbar you can adjust the view (the buttons auto-repeat when pressed longer), zooming is also possible with the mouse wheel. To automatically turn, press the big arrows with the number by it. With the small +/- you can adjust the skipping. You can adjust the slider "Frequency" for simulating different kinds of GPS sources or the performance of you hardware. On a desktop, even 100Hz shouldn't be much of a problem, on weaker processors the framerate normaly drops above 30-40Hz.

### Load/Save/New
Load/save/new works with a central toolbar: click on the icon and choose in the menu what to load/save/create new. The data format is GeoJSON, so it is compatible with most GIS-software without conversion, pe. with QGIS. All configuration files are in JSON and plain text. No built in chooser is provided, you organise the different A/B-lines, fields, configurations etc in the filesystem of your operating system. I suggest to add a couple of folders in a well known place like your home directory or the Desktop. You can use whatever backup and/or data exchange tool you want, like a cloud service, a simple USB stick or even a git repository. On Linux, you can add a shortcut by draging a folder to the places-tab in the open-dialog of the system.

## Contributing
If you want to contribute to the project, there are some rules:

- **Enter the [telegram group](https://t.me/QtOpenGuidance) and ask for open jobs. There's no sense in doing duplicate work without any coordination.**
- Fork it on github, send pull requests with a clear and compelling description
- Do small incremental commits with consise commit descriptions. No "WIP"-commits without any hint what is changed or big commits which touch many indipendent parts of the project. 
- Have a plan what to change and what to achieve with it. *Read and comprehend the existing code.*
- Coding style rules:
   - 2 spaces indentation, no tabs
   - Blocks are opened on the same line (pe. `if(...) {`)
   - spaces around operators and on the inside but not outside of parentheses
- Meaningful and descriptive names. If you alter the meaning of a temporary variable, also change its name. No indeciverable abreviations.
- Generaly keep a dataflow-paradigm in the code: data comes in, gets altered/calculated and then gets sent to the next block. Keep logically different parts seperated.
- Use meaningful datatypes to exchange data. Don't mash together logicaly different things like velocity + roll. Use seperate signals/slots if it makes sense, instead of one with all the data. The same goes for data streams like UDP-sockets or serial communications.
- Use SI-units: seconds, meters. Convert the data once on entry and once on exit. Use degrees in external data exchange (like with [esp32-aog](https://github.com/eringerli/esp32-aog)), but quaternions or rad internaly. Add "Degrees" or "Rad" to the variable name to indicate the unit.
- If you want to show different units in the GUI, make the conversion in one place and as the last step before showing. Don't transmit scaled values.
- As we develop for modern machines, floating point calculations are about as efficient as integer math (and we use modern compilers, so much of the expression if known at compile time gets optimized out). Don't do divisions as multiplications or other such hard to comprehend math. If you optimize an algorythm, document the simplest version of it, so it can be understood with little effort.
- Document your sources, especially for algorythms.
- Use `const` and `constexpr` as much as possible.
- No global variables to exchange data, all data and config gets exchanged with signals and slots.
- Keep things as local as possible, make as few globals as needed. Local scoping (`{}` without a `for, while(), if()` etc) are encouraged to keep the namespace clear and hint to the compiler the locality of the data.
- Generally keep all classes in their own file in the apropriate folder. Name the file after the class and also rename it when changing this name.
- General advice about configuration/input parameters: if it has to be changed regularily and on the fly, add a signal/slot to the block (pe. the maximal angle velocity to calculate the minimal turning radius from). If it is to be changed once, do it in the settings dialog (pe. the color of the grid lines).
- Find and set up sane defaults.
- When adding reused objects/variables, use the factories to pass a pointer to the blocks. Keep the factories in the same file as the block itself.
- Use as much of QT as possible, especialy Qt3D, the `QVector3D`/`Eigen::Quaterniond`-based math, the `QObject`-model, `QString`, `QNetwork` and the classes for XML, JSON, CBOR etc. Don't homegrow something if there is already an implementation. The chance is great, that QTs version is not only more robust and works in more cornercases but is also more performant.
- You can add libraries, but it has to be as a git submodule and built together with QtOpenGuidance. If there is a compelling reason to do otherwise, clearly state it in the commit message.
- Don't introduce platform-specific code, use the avaible QT-abstractions.
- QT and Qt3D are pretty performant, so no "optimized but unreadable" code (aka write-only code). Qtcreater has builtin support for profiling; implement new features clean and straightforward, then profile and only optimize if needed. Saving RAM and storage space is pretty pointless on modern machines, saving cycles most of the time too. So generally: better implement new features instead of getting +1% more performance on codepaths which are not critical.
- This isn't a collection of proof of concepts, but a maintainable, usable and readable codebase for a RTK-based guidance. So keep your implementations readable, documented and clean of premature optimitions.
- Be as compatible to standards as possible: if there is already an established format like JSON, CBOR, J1939 or GeoJSON, use that. Normaly, data exchange is not performance critical (it is done only a few times a second), so no super optimized code there (see also the points above). No problem if the user waits a couple of ms longer in this case.
- Experiment in your own branches, don't break the master-branch or configfiles without a really compelling reason.
- If you are still with me; these rules ensure that we create a maintainable codebase which can attract new devs. And this is the most important thing there is for such a project. Not only that it runs on virtualy every kind of machine out there (incl. Raspberry Pis and other SBC), but also that it is fun to implement new features in a way that helps the whole project. So if you are interested in helping me with this project (even by reporting a bug or do a translation), don't hesitate, fork the repository and send me a pull request.
