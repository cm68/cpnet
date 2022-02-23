#!/bin/bash
/usr/local/lib/cpnet 2>&1 >/usr/local/log/cpnet.log </dev/null &
disown $!
