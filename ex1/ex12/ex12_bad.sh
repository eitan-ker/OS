#!/bin/bash
ls $1 | grep .txt | sort | sed 's/.*/& is a file/'
ls $1 | grep -F -v "." | sort | sed 's/.*/& is a directory/'
