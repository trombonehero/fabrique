#!/bin/sh

fab --format=gmake -D 'debug=false' -D 'asserts=false' ../../fabfile || exit 1
./strip-make-paths.sh GNUmakefile || exit 1
