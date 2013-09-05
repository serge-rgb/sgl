sgl
===

This is my own C++ standard library. MIT licensed. Things may explode.

Feature list
------------
* NonCopyable base class.
* `Maybe<T>`, my small nod to haskell's sexy type system.
* Stretchy array (substitute for std::vector)
* Scoped pointer

### Planned features.
* String class.
* Modern OpenGL helpers (It would be nice to draw a triangle to the screen using less than 1000 lines)
* Refcounted pointer

Purposes
--------
1. Avoid the stl.
2. Provide common functionality for the kind of coding that I do (Graphics, at the moment).

Motivations
-----------
1. I enjoy reinventing wheels.
2. My eyes bleed when looking at stl implementations.
3. I enjoy reinventing wheels.

Manifesto
---------
I am inspired by Sean Barrett (aka nothings) and the style of this code is an emulation of his.
There are a lot of differences though, namely my lesser skill.

###Coding Style:

I mostly agree with Google's public C++ guidelines, with the following differences:

####Naming
```
ClassName
function_name()
variable_name
m_class_member
kConstantValueOrEnum
```
Research shows that `this_is_more_readable` `ThanThisAndIAgree` and using the `m_` prefix makes autocomplete nicer.

###Philosophy
In order of importance:

1. Efficiency. If need be, I will compromise any of the next points if my profiler suggests it.
2. Functional style. To use more fancy words: referential transparency whenever possible; I don't hate for loops. `const` is a good thing.
3. Avoid NULL whenever possible. Use `Maybe<T>`
4. I have modern C++ and I'm not afraid to use it (whithin reason).
5. RAII is the nicest thing that came out of C++. Use it for every resource that needs management.
6. Use reference counting for resources that *really* need management.
7. Data: Binary formats with text translators, or text format with compilation.
8. Don't be afraid to copy and paste from other MIT-like licensed codebases if I don't want to reinvent a particular wheel.
