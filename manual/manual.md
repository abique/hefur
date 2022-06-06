Introduction
============

Hefur is a standalone and lightweight BitTorrent tracker written in C++.
It does not depend on a database or anything complex to setup. Hefur
keeps everything in memory which makes it very easy to run.

It has not been designed to be an open tracker like opentracker.org, but
fits perfectly for a small setup like a Linux distribution sharing its
releases or a fansub team.

The code is very simple so a neophyte could understand it and modify it
for its specific needs.

Features
--------

-   UDP, HTTP and HTTPS announce and scrape.
-   Both IPv4 and IPv6 are supported.
-   Torrent white list based on directory tree: Hefur will recursively
    scan a directory and white list every torrents found.
-   Everything in memory, which leads to faster response time and easier
    setup.
-   Stat and Peer HTML pages.
-   Control interface. A control client: hefurctl is provided.

Setup your own server
=====================

Download and build Hefur
------------------------

Go to <https://github.com/abique/hefur/downloads> and download the last
version.

Then execute the following command:

``` {.sh}
      tar xvf hefur-$version.tar.xz &&
      cd hefur-$version &&
      mkdir build &&
      cd build &&
      cmake -DCMAKE_BUILD_TYPE=Release .. &&
      make &&
      make install
      
```

If you like to disable the control interface and so hefurctl, run cmake
with:

``` {.sh}
      cmake -DHEFUR_CONTROL_INTERFACE=OFF ..
      
```

Run the server
--------------

To start the server, simply run ./hefurd \--torrent-dir my-torrent-dir

Here is a detailed list of available options:

    ./hefurd -h
    Usage: ./hefurd [option]...
      -allow-proxy (allow the peer to specify its address (so usage of proxies))
      -announce-interval [=15] (the duration in minutes between two announces)
      -control-socket [=/var/run/hefur/control] (the path to the control socket)
      -disable-file-page (disable the HTTP page /file, which serve .torrent files)
      -disable-peers-page (disable the HTTP page /peers, which list torrent's peers)
      -disable-stat-page (disable the HTTP page /stat, which list served torrents)
      -http-cert [=] (the path to the certificate)
      -http-key [=] (the path to the key)
      -http-port [=6969] (the port to use, 0 to disable)
      -http-timeout [=2000] (the number of milliseconds to wait until timeout)
      -https-port [=6970] (the port to use, 0 to disable)
      -ipv6 (bind on ipv6 instead of ipv4)
      -max-numwant [=100] (the maximum peers can be replied per announce request)
      -max-peers [=30000] (the maximum number of peers per torrent)
      -max-scan-depth [=64] (the maximum depth while scanning torrent-dir)
      -max-scan-inodes [=131072] (the maximum number of inode to scan while scanning torrent-dir)
      -max-torrent-name [=64] (the maximum torrent name length to truncate to)
      -max-torrent-size [=10] (the maximum torrent size, in MiB)
      -min-numwant [=50] (the minimum peers can be replied per announce request)
      -scan-interval [=60] (the duration in seconds between two torrent-dir scans)
      -scrape-interval [=15] (the duration in minutes between two scrapes)
      -torrent-dir [=] (the directory containing the allowed torrents, if empty every torrents are allowed)
      -udp-port [=6969] (the port to use, 0 to disable)
      -version (display the software's version)
      -www-dir [=/usr/share/hefur/www] (the directory containing the web data files (html, css, img))

    [log]
      -log-color (enables colors in log)
      -log-compression [=xz] (selection the compression method: identity, gzip, xz)
      -log-file [=] (the path to the log file, it will be passed to strftime(3), ie: my-app-%Y-%m-%d.%H:%M:%S.log)
      -log-filter (set the log filtering string: origin:level,... +origin is equivalent to origin:debug, and -origin is equivalent to origin:critical.)
      -log-level [=warning] (set the minimum log level, possible values are debug, info, warning, error, critical and fatal)
      -log-origins (lists available origins)
      -log-rotation-interval [=0] (enables log file rotation based on interval in seconds)

    [security]
      -chroot [=] (the path to chroot)
      -gid [=] (the new gid)
      -uid [=] (the new uid)

    [tpl]
      -tpl-debug (helps debugging of template)
          

Options
=======

The white list
--------------

The white list is used defined the set of torrents which are tracked by
the tracker. The way it works is pretty simple: you specify a path, and
Hefur will periodically scan it recursively to find .torrent files and
add them to the tracker. When a torrent file is removed from the torrent
directory, then it will be removed from the white list.

The white list is configured by the following options:

-   -torrent-dir \<path\>
-   -scan-interval \<interval\> (interval is in minutes)

Please ensure that the torrent directory is a finite tree.

Resources limits
----------------

On most OS, the maximum number of opened files is 1024, which is
probably enough. But if your tracker is going under heavy load, you
might consider to increase its resources usage.

To do so, I recommend you to read ulimit\'s manual and edit
/etc/security/limits.conf.

chroot, setuid and setgid
-------------------------

If you\'re starting Hefur as root, you will see new options appears to
help you dropping privileges:

-   -chroot \<path\>
-   -uid \<uid\>\|\<user name\>
-   -gid \<gid\>\|\<group name\>

/stat and /peers pages
----------------------

These pages are not necessary for the smooth functioning of Hefur, they
are just convenient to the user and administrator.

The /stat page display collected statistics relative to the start time.
Which means that every statistics are reset if you restart Hefur.

There is no arm to let anyone access /stat and /peers, but if you want,
you can disable them with

-   -disable-peers-page
-   -disable-stat-page

Security considerations
=======================

The white list
--------------

As you can own the directory scanned by Hefur, this is not really a big
issue. But if you scan a network file system or simply a virtual file
system which you don\'t control, this file system can arm Hefur by
doing:

-   an infinite tree
-   an infinite .torrent file
-   a high number of .torrent with crafted info part to put the torrent
    index in its worst case
-   a high number of .torrent with crafted very long name to fill up
    memory

So if you scan an uncontrolled file system you want to:

-   set the maximum .torrent file size: -max-torrent-size \<size in
    MiB\>
-   truncate torrent\'s name longer than X: -max-torrent-name \<length\>
-   limit the scan depth: -max-scan-depth \<depth\>
-   limit the number of inodes scanned: -max-scan-inodes \<nb\>

The number of peers per torrent
-------------------------------

This problem is very simple: you have an index for all the tracked
peers, and you store peers. So it take memory. If you do not have an
limit on the number of peers, you can exceed the available memory.

Our solution is to limit the number of peers per torrent. If we exceed
this limit, we simply pop the oldest peer, to push the new one. In fact
it is safe as a tracker could only remember the 200 last peers, they\'ll
be able to reach enough peers to achieve good download performances, and
they\'ll all remain connected with PEX (Peer Exchange Protocol).

You can use the following option to change the maximum number of peers
per torrents:

-   -max-peers \<nb\>

The number of peers from the same IP
------------------------------------

As we index the IP address plus the port, a single machine could
register many times to the tracker which is fine. But we don\'t want to
let it registers too many times. We could try to use our index to do it,
but as with ipv6, users can have a large number of distinct IP address
it gets very complicated to find the right network mask. So the simple
solution is to do nothing and just rely on the option:

-   -max-peers \<nb\>

Anyway, if someone is going to DDOS your tracker, it may come from a lot
of different IPv4 and/or IPv6 address, so using network mask to protect
the server is not a final solution.

Proxy considerations
--------------------

Hefur is usable through a proxy, but the referenced peer will be the
proxy, not the peer itself. The peer will still be able to start
downloading because it will gets a list of peers and be able to connect
to them (as far as not every body is using a proxy).

Why do we choose this behavior? Because we use the peer address to index
peers and if we let a peer specify it, we let him spawn a lot of
announces and then fill up our index.

If you really want to allow peers to specify their address, you can use
the option:

-   -allow-proxy

I warn you to be sure that you set the option -max-peers to something
reasonable if you use -allow-proxy.

Timeouts
--------

Hefur is pretty aggressive on timeouts. By default the peers has only 2
seconds to send its request, which is more than enough for a program.
Through you can change this with the option:

-   -http-timeout \<ms timeout\>

If you discover a hole
----------------------

Please report the issue to Alexandre Bique <bique.alexandre@gmail.com>.
I\'ll do my best to fix it as soon as possible.

Compatibility notes
===================

HTTP, Chunked-Encoding and Keep-Alive
-------------------------------------

As there are some BitTorrent client with pretty simple HTTP client, for
/announce and /scrape, the Chunked-Encoding and Keep-Alive are disabled.
But it is enabled for /stat and /peers.

ip set to a DNS name in the HTTP announce request
-------------------------------------------------

When you use -allow-proxy, the BitTorrent specification says that the
peer can set ip to a DNS name in the HTTP announce request. We don\'t
support that, as it requires to send the DNS name back to other peers,
and a DNS name can be longuer than an ipv6 address. So to save memory we
will just accept IPv4 and IPv6 coded address.
