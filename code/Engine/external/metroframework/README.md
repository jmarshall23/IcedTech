
MetroFramework
--------------------

Metro UI of Windows 8 for .NET Windows Forms applications.

NOTE
----

The only reason for the existence of this fork is that the original author insists on removing 
all documentation from the code and some other renaming and merging issues. 
Forked & relicensed under same license (see below).

Added Features
------------------

* Complete rewrite of Theme Manager to support global and cascading style changes
* Theme manager designer to for "cleaning" some of the serialization garbage left by the original code
* Style Extender to support theming of legacy controls
* Font Resolver to implement fall-back alternatives for Segoe UI fonts
* Implemented compliancy with .NET 4, Client Profile (i.e. no dependency on System.Design.dll)
* Fixed issues with maximizing and restoring windows
* CLS Compliancy
* Compatibility with partially-trusted callers
* Added true Aero Shadow
* Added lightweight System Drop shadow
* Multiple bug-fixes to shadows and improvements to flicker issues
* Improvements to display over Remote Desktop
* Form and Panel borders
* Multi-monitor support
* Centering forms to screen or parent
* Lot's of fixes to designer serialization
* Plus loads of other bug fixes throughout the library...

Availability on NuGet
------------------------

Simply add the "MetroFramework Installer" package to your project.

See http://nuget.org/packages/MetroFramework for details.


Supported platforms
-------------------
* Windows XP SP1/SP2/SP3 (without Aero Glass)
* Windows Vista
* Windows 7
* Windows 8

Requirements
------------
* .NET Framework 4.0 and above.

Controls supported
------------------
* Button
* ComboBox
* Checkbox
* RadioButton
* Toggle
* Label
* Link
* Panel
* ScrollBar
* MetroTile
* ProgressBar
* ProgressSpinner
* TabControl
* TrackBar
* Custom User Control

Components supported
------------------
* StyleExtender (Apply theme propertys to none framework controls)
* StyleManager (Auto inherit a default style to all used controls)
* ToolTip

Screenshots
----------
*Light*

![MetroFramework](http://i.imgur.com/8Yk1BiN.png)
![MetroFramework](http://i.imgur.com/qjwRg5z.png)
![MetroFramework](http://i.imgur.com/3S7NPLQ.png)
![MetroFramework](http://i.imgur.com/ULRej3C.png)

*Dark*

![MetroFramework](http://i.imgur.com/EddlvbX.png)
![MetroFramework](http://i.imgur.com/Djnjkti.png)
![MetroFramework](http://i.imgur.com/bI2c6kE.png)
![MetroFramework](http://i.imgur.com/7cxHl1Y.png)

License
-------

MetroFramework - Modern UI for WinForms

Copyright (c) 2013 Jens Thiel, http://thielj.github.com/MetroFramework

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation the rights to use, copy, 
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the 
following conditions:

The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Portions of this software are (c) 2011 Sven Walter, http://github.com/viperneo
