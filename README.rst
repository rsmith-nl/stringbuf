Simple string assembly for C
############################

:date: 2025-08-29
:tags: C programming, string building, public domain
:author: Roland Smith

.. Last modified: 2026-02-22T12:35:00+0100
.. vim:spelllang=en

Introduction
============

Assembling strings for printing is something that is often needed. Usually this is done ad-hoc.
For that purpose I wrote this simple library, which fills most of my use cases.
Header and implementation are less than 100 lines of code in total.

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

The included ``Makefile`` builds the code for testing and can build a test
program with ``make sbtest``.
