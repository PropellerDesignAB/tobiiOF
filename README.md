www.propeller.se

A wrapper of Tobii EyeX SDK 1.5 (C++) for OpenFrameworks 0.8.6
(Windows only!)

Some use instructions:

Don't forget to link dependencies "includes" and "bin" in VS 
and the "Tobii.EyeX.Client.dll" in the executable folder.

In file ofApp.h include:
```
#include "../stdafx.h"
#include "eyex/EyeX.h"
#include "../eyeXHost.h"
```

In class methods ofApp.h:
```
eyeXHost eyex;
```

In file ofApp.cpp setup() method add:
```
eyex.init();
```

In the update() method add these lines to use the raw data:
```
posX = eyex.gazePoint.x;
posY = eyex.gazePoint.y;
```

In the update() method add these lines to use the averaged data:
```
posX = eyex.gazePoint.x*0.25 + posX*0.75;
posY = eyex.gazePoint.y*0.25 + posY*0.75;
```

Use posX and posY as you please.
