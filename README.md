# SDump
A plugin for IDA to dump segments to disk.

To build this package, you'll need:

- Visual Studio (I'm using Visual Studio 2015)
- IDA SDK (most likely version 6 and above, this code has been compiled with IDASDK 6.9)

To properlly build and install the plugin, you must:

* Go to Project Properties -> General
  * Change Output Directory to your IDA plugin directory
* Go to Project PRoperties -> C++ -> General
  * Change Additional Include Directories to IDA SDK include directory
* Go to Project Properties -> Linker -> Input
  * Change Additional Dependencies to %IDASDK%\lib\x86_win_vc_32\ida.lib where %IDASDK% is the directory where you have your IDA SDK.

Based on [Segdump by Dennis Elser](http://www.openrce.org/downloads/details/51/SegDump)

Copy sdump.plw to %IDAPRO%\plugins\ directory.

In IDA press ALT+5 for a list of loaded segments to appear. Select the one you want and dump it by:
* pressing Enter
* pressing OK button
* right clicking on item and choosing 'Dump segment to disk'
* pressing CTRL+E

'Save as' window will show up to let you save the file and message confirming that the dump was successful or failed.
