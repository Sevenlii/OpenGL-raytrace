# OpenGL-raytrace
Ray tracing scene built with OpenGL

Requires OpenGL glut libraries to compile.

	*Recommended glut files: http://user.xmission.com/~nate/glut.html

Use Left/Right mouse to rotate around the scene.

Ray tracing calculations are performed on the CPU for each pixel in the view plane with a single thread, so animation updates may vary based on your hardware. Perspective is updated with rendering, and thus will only change when ray tracing calculations are completed for the next render.
