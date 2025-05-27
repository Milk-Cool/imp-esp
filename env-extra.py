# https://stackoverflow.com/a/66748991

from os.path import isfile
Import("env")
assert isfile(".env")
try:
  f = open(".env", "r")
  lines = f.readlines()
  envs = []
  for line in lines:
    if line[0] == "#":
      continue
    envs.append("-D{}".format(line.strip()))
  env.Append(BUILD_FLAGS=envs)
except IOError:
  print("File .env not accessible",)
finally:
  f.close()