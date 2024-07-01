#!/usr/bin/python3
# 

numlocations = 512

for i in range(numlocations):
  print(f"w {4*i:#06x} {1000+i:#010x}")
for i in range(numlocations):
  print(f"r {4*i:#06x} {1000+i:#010x}")

