#!/bin/bash

ndk-build && ant debug && adb install -r bin/PythonExt-debug.apk
