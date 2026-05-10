Simple string assembly for C
############################

:date: 2025-08-29
:tags: C programming, string building, public domain
:author: Roland Smith

.. Last modified: 2026-05-10T13:23:46+0200
.. vim:spelllang=en

Introduction
============

Assembling strings for printing is something that is often needed. Usually this is done ad-hoc.
For that purpose I wrote this simple library, which fills most of my use cases.
Header and implementation are less than 100 lines of code in total.


No AI policy
============

This code is by a human and for humans.
"AI" / LLM-generated rewrites and additions are not welcome.


Implementation
==============

A static buffer inside the ``Sbuf`` structure is used.
The size of the buffer is defined as ``SBUF_SIZE`` which is set to ``PATH_MAX`` by default.

An empty initialized ``Sbuf`` is valid::

    Sbuf buf = {0};

This implementation can append strings to the buffer and format data into the
buffer using ``vsnprintf``.
It can output the contents of a buffer to any ``FILE*``.

The header file also serves as the documentation. :)


Usage
=====

Just copy the source files (``sbuf.h`` and ``sbuf.c``) in to
your project, and hook them up to the build.

Alternatively, you can use this project as a *single header library*.
This variant can be found in the subdirectory ``single_header/sbuf.h``.
To use the single header library, copy the file ``sbuf.h`` from
``single_header/`` into your project.
In **one** of the C-files you use the single header library in, you should
define ``SBUF_IMPLEMENTATION`` before including the library::

    #define SBUF_IMPLEMENTATION
    #include "sbuf.h"

