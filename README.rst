=============
vmod_interval
=============

-----------------------
Varnish Interval Module
-----------------------

:Author: Tommy Allen
:Date: 2014-03-11
:Version: 1.0
:Manual section: 3

SYNOPSIS
========

import interval;


DESCRIPTION
===========

Set TTL that causes an object to expire at certain times or intervals.


FUNCTIONS
=========

interval
--------

Prototype
    ::

        every(INT hours, INT minutes, INT seconds)
Return value
    DURATION
Description
    Returns the number of seconds until the next interval


Prototype
    ::

        at(INT hours, INT minutes, INT seconds)
Return value
    DURATION
Description
    Returns the number of seconds until the specified time


Prototype
    ::

        is_before(INT hours, INT minutes, INT seconds)
Return value
    BOOL
Description
    Returns true if the specified time is before the current time.


Prototype
    ::

        is_before(INT hours, INT minutes, INT seconds)
Return value
    BOOL
Description
    Returns true if the specified time is after the current time.


INSTALLATION
============

The source tree is based on autotools to configure the building, and
does also have the necessary bits in place to do functional unit tests
using the varnishtest tool.

autoconf looks for a timezone in /etc/timezone.  If it's not found, GMT
is used.

Usage::

    ./autogen.sh
    ./configure VARNISHSRC=DIR [VMODDIR=DIR]

`VARNISHSRC` is the directory of the Varnish source tree for which to
compile your vmod. Both the `VARNISHSRC` and `VARNISHSRC/include`
will be added to the include search paths for your module.

Optionally you can also set the vmod install directory by adding
`VMODDIR=DIR` (defaults to the pkg-config discovered directory from your
Varnish installation).

Make targets:

* make - builds the vmod
* make install - installs the vmod in `VMODDIR`
* make check - runs the unit tests in ``src/tests/*.vtc``
    * The test will fail since this module is based on your system's time

In your VCL you could then use this vmod along the following lines::

    import interval;

    sub vcl_fetch {
        if (req.url == "/")
        {
            # Set the TTL to expire every 15 minutes (00:00, 00:15, 00:30, 00:45)
            set beresp.ttl = interval.every(0, 15, 0);
        }
        else if (req.url == "/test")
        {
            if (interval.is_after(9, 0, 0) && interval.is_before(11, 30, 0))
            {
                // Refresh every 5 minutes between 9:00AM and 11:30AM
                set beresp.ttl = interval.every(0, 5, 0);
            }
            else
            {
                // Keep cached until the next day at 8:59AM.
                set beresp.ttl = interval.at(8, 59, 0);
            }
        }
    }
