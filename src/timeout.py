#!/usr/bin/env python3

import argparse
import platform
import sys
import time

WINDOWS = platform.system() == 'Windows'

if WINDOWS:
  from winpexpect import winspawn as spawn
  import winpexpect as pexpect
  import win32gui, win32process
  WINDOWS = True
else:
  from pexpect import spawn
  import pexpect
args = sys.argv[1:]

parser = argparse.ArgumentParser(description='Run PROGRAM for limited amount of time')
parser.add_argument('timeout', metavar='TIMEOUT', type=int, help='Timeout in seconds')
parser.add_argument('program', metavar='PROGRAM', type=str, help='Program to execute')
parser.add_argument('-f', dest='final', metavar='FINAL_STRING', type=str, help='Final string')
parser.add_argument('arguments', metavar='ARGS', type=str, nargs='*', help='Arguments for program')

args = parser.parse_args()

print('Running "{0}" for a maximum time of {1} seconds.'.format(args.program, args.timeout))
p = spawn(args.program, args.arguments)
start = time.time()
timeout = False
finalseen = False
try:
  while True:
    p.expect('\n', start + args.timeout - time.time())
    data = p.before
    if isinstance(data, bytes):
      data = data.decode('string_escape' if sys.version_info < (3, 0) else 'unicode_escape')
    print(data)
    if args.final and args.final in data:
      finalseen = True
except pexpect.EOF:
  pass
except pexpect.TIMEOUT:
  timeout = True

if WINDOWS:
  def WindowIsVisible(pid):
    def enumHandler(hwnd, data):
      if (win32process.GetWindowThreadProcessId(hwnd)[1] == data[0] and
            win32gui.IsWindowVisible(hwnd)):
        data[1] = True

    data = [pid, False]
    win32gui.EnumWindows(enumHandler, data)
    return data[1]

  if WindowIsVisible(p.child_handle):
    print('The program created a window!')

if timeout:
  print('{0}: timeout hit ({1}s)'.format(args.program, args.timeout))
  p.terminate()
  if finalseen:
    sys.exit(1)

print('Program finished')
p.terminate()
