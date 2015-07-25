Function Within Function, by Andrew Barrette

GENERAL
This program plots a 3D parametric function, using the function itself as the z-axis (and doing this recursively). As-provided, the parametric function is a helix with Recursions=1, so the plotted result is a helix that is bent into a helix. To change the parametric function, the program must be recompiled after editting foov (the function) and dfoov (the derivative of the function with respect to t).

CONTROLS
Controls are provided in the GUI:
Axial stretch 		- Scaling of the 0th-order z-axis.
Spatial scaling 	- Scaling of the >0th-order x-axis and y-axis.
Time scaling 		- Scaling of the >0th-order z-axis.
End time 		- 0th-order function is plotted from t=0 to t=tend.
Recursions 		- Depth of order; 0th order is no recursion.
Resolution 		- For Resolution=R, the time step is defined as 1/2^R.
Show Normals 		- Show local x and y (red and green) and z (blue) axes.
Line Width 		- Width of plotted line, in pixels.
Color Density 		- Number of color-spectrum cycles from t=0 to t=tend.
Z. Shade Dens. 		- For Density=D, plot brightness goes like exp(-z*D), where z is the 0th-order z-axis.

Other Controls:
Left mouse button 	- Click and drag for visual rotation.
Right mouse button 	- Click and drag for visual panning.
Scroll wheel forward/back OR '='/'-' keys - Zoom in/out.