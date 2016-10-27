The project was built with Qt version 5.5

* https://www.qt.io/

A Qt license is actually necessary for commercial deployment but it can be evaluated for 30 days.

The Qt Creator is an IDE that comes bundled with a Qt installation and is necessary for editing the UI files, and recommended for building the project.

h2. Project Setup, Requirements, Repository location, Files

* Load whatchado_diy.pro in Qt
* In the opened project in Qt, select "Projects" on the left, and then press "Manage Kits..."
* Resolve the issues in that dialog
* Then building should work

h3. Initial Build on Mac OS X

# Download and install Qt 5.5.1 (http://download.qt.io/archive/qt/5.5/5.5.1/qt-opensource-mac-x64-clang-5.5.1.dmg or RackStation folder)
# Install ffmpeg libraries (see below)
# Launch Qt Creator and open whatchado_diy.pro located at the root folder of the storyrecorder project
# In video_image.h delete the 6th line "#define snprintf _snprintf" (this is probably required for builds on Windows)
# In whatchado_diy.pro comment out line 21 'LIBS += -L"C:\ffmpeg\lib"'(probably required for Windows) and remove the linker flag -L/opt/local/lib in line 23 (not the other one)
# On the left sidebar, select either Debug or Release as build configuration and then press the build button below. The output directory is defined in "Projects"->"Build & Run" settings.

Qt version 5.5 is the latest version that still includes the Qt webkit module.

h4. Installing ffmpeg dev libs

Install Homebrew: http://brew.sh/
Then in the terminal do:

<pre>
brew install automake fdk-aac git lame libass libtool libvorbis libvpx opus sdl shtool texi2html theora wget x264 xvid yasm
</pre>

Make that the Xcode command line tools are installed (requires Xcode itself):

<pre>
xcode-select --install
</pre>

Goto a directory you want to install ffmpeg source to (can be temporary of course):

<pre>
git clone http://source.ffmpeg.org/git/ffmpeg.git ffmpeg
cd ffmpeg
./configure  --prefix=/usr/local --enable-gpl --enable-nonfree --enable-libass --enable-libfdk-aac --enable-libfreetype --enable-libmp3lame --enable-libopus --enable-libtheora --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libxvid --disable-static --enable-shared
make && sudo make install
</pre>

h2. Building

h3. Windows

* Some runtime is needed (windows)
* Then hit build -> Debug or release
* Folder is generated in project directory
* All necessary Qt .dll files must be copied to the project directory to be able to start the application. They can be found in "$QtInstallationDirectory/$QtVersion/msvc20XX_64/bin"
* Also the "question video files" must be copied into the project directory. They can be found in in the repo in "storyrecorder/resources/cutscenes"
* It is possible that some windows .dll files are missing. Resolve by googling .dll file name (displayed in error message when starting app) and download it into project directory.

h3. Mac OS

See the section +Initial build on Mac OS X+ above.

h2. Additional links and resources

* http://doc.qt.io/qtcreator/creator-debugger-engines.html    //how to Setting Up Debugger & Setup the QT environment ,
* http://doc.qt.io/qtcreator/creator-build-settings.html      //Specifying Build Settings
* http://doc.qt.io/qtcreator/creator-run-settings.html        //Specifying Run Settings
* http://doc.qt.io/qtcreator/creator-editor-settings.html     //Specifying Editor Settings
* http://doc.qt.io/qtcreator/creator-code-style-settings.html //Specifying Code Style Settings
