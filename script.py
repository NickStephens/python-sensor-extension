import my_extension
import time

print "starting!"

samples = []
while True:
  t1 = time.time()
  for i in range(0, 10):
    my_extension.callme()
  t2 = time.time()

  print 10/(t2 - t1), "samples a second"

