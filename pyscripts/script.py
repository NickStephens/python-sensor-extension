import sensors
import time

samples = []
while True:
  t1 = time.time()
  print "--samples:"
  for i in range(0, 10):
    print "\t", sensors.grab_accel()
  t2 = time.time()

  print 10/(t2 - t1), "samples a second"

