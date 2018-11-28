#!/bin/bash

rsync -n -av --delete-after bin boot etc lib opt sbin usr var /
