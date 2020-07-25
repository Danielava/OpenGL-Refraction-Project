# OpenGL-Refraction-Project

Implementation of a refraction algorithm in OpenGL using an image-space approach.

Performs refraction upon entering and leaving the object, making it more realistic than the usual
approach in a rasterizer where the refraction only happens once, when entering.
This is achieved using data about the objects front and back side, which are stored in separate framebuffers.

Based on Chris Wyman's paper http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.104.9755&rep=rep1&type=pdf
