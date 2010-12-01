#! /bin/bash

TEST_DIR=`dirname $0`

export LD_LIBRARY_PATH=$TEST_DIR/../libjp4
export DYLD_LIBRARY_PATH=$TEST_DIR/../libjp4

bzcat -f $TEST_DIR/test_image.pgm.template.bz2 > $TEST_DIR/test_image.pgm.template

$TEST_DIR/test_image

