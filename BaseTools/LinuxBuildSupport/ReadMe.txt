This folder contains supported files for support building InsydeH2O 5.0 firmware source under Linux.
To setup the Linux build support environment, run 

. InstallLinuxBuildSupport $WORKSPACE

under the Linux terminal console, where $WORKSPACE is the path of the H2O 5.0 source.

The installation requires the user to be a "sudoer" so that required development tools and libraries 
can be installed onto the development system via Linux package installation tools such as "apt-get" 
or "yum", etc.

To build the source code, change to the project directory, for example, SharkBayMultiBordPkg, and run

make kernel

to build InsydeModulePkg source, or

make uefi64
or
make uefi64ddt

to build the project source in release or debug mode respectively.
