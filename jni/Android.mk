# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

PYTHONVER := 2.7
PYTHONLIB := ../$(LOCAL_PATH)/../python$(PYTHONVER)-libs
PYTHONLIBINC := $(LOCAL_PATH)/../python$(PYTHONVER)-libs


$(warning $(PYTHONLIB))
$(warning $(PYTHONLIBINC))
include $(CLEAR_VARS)
LOCAL_MODULE := python2.7
LOCAL_SRC_FILES := $(PYTHONLIB)/lib/libpython2.7.so
LOCAL_EXPORT_C_INCLUDES := $(PYTHONLIB)/include/
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(PYTHONLIBINC)/include/
COMPILED_LIBRARIES := python2.7
LOCAL_MODULE    := application
LOCAL_SRC_FILES := main.c #start.c #main.c hello.c
#LOCAL_SHARED_LIBRARIES := python2.7
#LOCAL_LDLIBS := -L$(PYTHONLIB)/lib/ -llog -landroid -lEGL -lGLESv1_CM -lpython$(PYTHONVER)
#APPLICATION_ADDITIONAL_LDFLAGS := -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions
LOCAL_LDLIBS := -Lpython2.7-libs/lib -landroid -llog -lpython$(PYTHONVER) -ldl -lz -lm
#LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv1_CM
LOCAL_STATIC_LIBRARIES := android_native_app_glue
include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
