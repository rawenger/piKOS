#!/bin/sh
#
# Linux environment setup for pi-kos development
TOOLCHAIN_HOME='/opt/arm-gnu-toolchain'

# only add to PATH if we aren't already there
[[ ":$PATH:" == *":$TOOLCHAIN_HOME/bin:"* ]] || export PATH="$TOOLCHAIN_HOME/bin:$PATH"

# ARM's GDB executable breaks without these settings. Conveniently, the system/host GDB breaks *with* them.
export PYTHONPATH='/usr/lib64/python38.zip:/usr/lib64/python3.8:/usr/lib64/python3.8/lib-dynload:/usr/lib64/python3.8/site-packages:/usr/lib/python3.8/site-packages'
export PYTHONHOME='/usr'
