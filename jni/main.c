#include <python2.7/Python.h>

#include <string.h>
#include <jni.h>

#include <android/log.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "PythonExt", __VA_ARGS__))

/* globals for sensors */
ALooper *looper;
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

void setup_manager(void)
{
  LOGI("[setup_manager] initializing sensor manager from NDK");
  looper = ALooper_forThread();
  if (looper == NULL)
  {
    LOGI("[setup_manager] ALooper_forThread() failed");

    /* might need to remove the flag associated with this */
    //looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    looper = ALooper_prepare(0);
  }
  LOGI("[setup_manager] looper: 0x%x", looper);
  
  sensorManager = ASensorManager_getInstance(); 
  LOGI("[setup_manager] sensorManager: 0x%x", sensorManager);
}

/* setup an accelerometer sensor */
void setup_sensors(void)
{
  const char *retstr;

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

void destroy_sensors(void)
{
  LOGI("[destroy_sensors] disabling sensor from NDK");
  int ret = ASensorEventQueue_disableSensor(sensorEventQueue, sensor);
  LOGI("[destroy_sensors] ASensorEventQueue_disableSensor returned %d", ret);

  LOGI("[destroy_sensors] destroying EventQueue");
  ret = ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
  LOGI("[destroy_sensors] ASensorManager_destroyEventQueue returned %d", ret);
}

static PyObject *grab_accel(PyObject *self, PyObject *args)
{

  setup_sensors();
  LOGI("[callme] calling ALooper_pollOnce");
  /* blocks until a new event arrives */
  ALooper_pollOnce(-1, NULL, NULL, NULL);

  /* not sure why this has to be done. if we don't destroy the sensor eventqueue on every call, we'll block indefinitely on the next call. */
  destroy_sensors();
  return Py_BuildValue("(f,f,f)", event.acceleration.x, event.acceleration.y,
    event.acceleration.z);

}

static PyMethodDef ExtensionMethods[] = {
  {"grab_accel", grab_accel, METH_VARARGS,
   "Wait for an accelerometer sensor event and retrieve its values."},
  {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initsensors(void)
{
  
  LOGI("[initmy_extension] initializing...");
  LOGI("[initmy_extension] setting up ASensorManager");
  setup_manager();
  (void) Py_InitModule("sensors", ExtensionMethods);
}
