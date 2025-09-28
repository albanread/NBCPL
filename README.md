### New BCPL

Not production ready (yet), under development, this has just started to work, and is now being tested and debugged.

Targets ARM 64 bit processors, and especially Apple Mx series, and eventually Linux also.

## I will keep this brief, this is the New BCPL programming language.

The language is inspired by classic BCPL with a few useful extensions, such as integrated floating point, Pairs, Lists and Objects.

I have kept the language additions simple.

This is not based on the elegant classic compiler, this is completely new and ugly, and not designed for portability, or constrained systems.

The addition of types without changing the feel of the language, drives the need for extensive type inference.

The addition of more heap allocated variables drives the need to add a simple memory management assistant called SAMM.

SAMM is absolutely not a garbage collector, but it takes care of memory on the heap, in a similar scoped way to local variables.

 
#### more to read

There is quite a lot of other documentation here.

#### Why?

Why - I enjoy simplicity in my programming languages, and I like to actually use the hardware features of the machines I own.


