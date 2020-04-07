#!/bin/bash
ls $1 | grep -r -l $2 * | sort
