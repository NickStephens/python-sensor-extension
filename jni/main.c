#include <python2.7/Python.h>

#include <string.h>
#include <jni.h>

#include <android/log.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "PythonExt", __VA_ARGS__))

/* globals for sensors */
ASensorManager *sensorManager;
ASensorEventQueue *sensorEventQueue;
ASensor *sensor;

ASensorEvent event;

/* sensor event callback */
int get_sensor_events(int fd, int events, void *data)
{
  while(ASensorEventQueue_getEvents(sensorEventQueue, &event, 1)>0)
  {
    if (event.type == ASENSOR_TYPE_ACCELEROMETER) {
          LOGI("accelerometer: x=%f y=%f z=%f", 
              event.acceleration.x, event.acceleration.y,
              event.acceleration.z);
    }
  }
}

/* setup an accelerometer sensor */
void setup_sensors(void)
{
  const char *retstr;

  LOGI("[setup_sensors] initializing sensor manager from NDK");
  ALooper *looper = ALooper_forThread();
  if (looper == NULL)
  {
    LOGI("[setup_sensors] ALooper_forThread() failed");

    /* might need to remove the flag associated with this */
    //looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    looper = ALooper_prepare(0);
  }
  LOGI("[setup_sensors] looper: 0x%x", looper);

  sensorManager = ASensorManager_getInstance();
  LOGI("[setup_sensors] sensorManager: 0x%x", sensorManager);

  LOGI("[setup_sensors] requesting sensor: %d", ASENSOR_TYPE_ACCELEROMETER);
  sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
  LOGI("[setup_sensors] sensor: 0x%x", sensor);

  int type = ASensor_getType(sensor);
  LOGI("[setup_sensors] type id: %d", type);

  retstr = ASensor_getName(sensor);
  LOGI("[setup_sensors] name: (0x%x) %s", retstr);

  retstr = ASensor_getVendor(sensor);
  LOGI("[setup_sensors] vendor: (0x%x) %s", retstr);

  sensorEventQueue = ASensorManager_createEventQueue(sensorManager, looper, LOOPER_ID_USER, get_sensor_events, NULL);
  LOGI("[setup_sensors] sensorEventQueue: 0x%x", sensorEventQueue);

  int ret = ASensorEventQueue_enableSensor(sensorEventQueue, sensor);
  LOGI("[setup_sensors] ASensorEventQueue_enableSensor returned %d", ret);
}

static PyObject *callme(PyObject *self, PyObject *args)
{

  setup_sensors();
  LOGI("[callme] calling ALooper_pollOnce");
  /* blocks until a new event arrived */
  ALooper_pollOnce(-1, NULL, NULL, NULL);

  return Py_BuildValue("(f,f,f)", event.acceleration.x, event.acceleration.y,
    event.acceleration.z);

}

static PyMethodDef ExtensionMethods[] = {
  {"callme", callme, METH_VARARGS,
   "Call this function for a test."},
  {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initmy_extension(void)
{
  //setup_sensors();
  LOGI("[initextension] initializing...");
  (void) Py_InitModule("my_extension", ExtensionMethods);
}
