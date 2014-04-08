sgl
===

This is my own C++ standard library. MIT licensed. Things may explode.

Feature list
------------
* `Maybe<T>`, my small nod to haskell's sexy type system.
* `Array<T>` Stretchy array (substitute for std::vector)
* `ScopedPtr` and `ScopedArray` Smartish pointers (substitute for std::unique_ptr)
* `String` class. Doesn't do much yet!
* `Dict<T>` Dictionary (Map strings to keys of any type.)

### Planned features:

* Modern OpenGL helpers (It would be nice to draw a triangle to the screen using less than 1000 lines)
* Refcounted pointer

Purposes
--------
1. Avoid the stl.
2. Provide common functionality for the kind of coding that I do.

Motivations
-----------
1. I enjoy reinventing wheels.
2. My eyes bleed when looking at stl implementations.
3. I enjoy reinventing wheels.
