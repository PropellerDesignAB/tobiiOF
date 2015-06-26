A wrapper of Tobii EyeX SDK 1.5 (C++) for OpenFrameworks 0.8.6
(Windows only!)

Some use instructions:

Don't forget to link dependencies "includes" and "bin" in VS 
and the "Tobii.EyeX.Client.dll" in the executable folder.

---

// ofApp.h
#include "../stdafx.h"
#include "eyex/EyeX.h"
#include "../eyeXHost.h"

// in class methods add:
eyeXHost eyex;

---

// ofApp.cpp
// in the setup() method add:
eyex.init();

// #1 in the update() method add (if you'd like raw data)
posX = eyex.gazePoint.x;
posY = eyex.gazePoint.y;

// 2# (if you'd like slightly averaged data)
posX = eyex.gazePoint.x*0.25 + posX*0.75;
posY = eyex.gazePoint.y*0.25 + posY*0.75;

// use posX and posY as you please.
