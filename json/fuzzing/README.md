## Fuzzing 
- To fuzz using `afl-fuzz`, using their compiler `afl-clang-lto`. 
A complete example assuming you managed to build the binary:
```
# Catch all crashes
sudo sh -c 'echo core >/proc/sys/kernel/core_pattern'

# Change CPU scaling
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

afl-fuzz -i json/fuzzing/inputs -o json/fuzzing/outputs build/json/fuzzing/json-fuzzing-Fuzzing -x json/fuzzing/json.dict
```

If it is difficult to find out what went wrong, you can use `casr-afl`:
```
mkdir json/fuzzing/casr
casr-afl -i json/fuzzing/outputs/ -o json/fuzzing/casr/
casr-afl xxx.rep
```

- To triage crashes.
A complete example assuming you managed to build the binary:
```
build/json/fuzzing/json-fuzzing-Debug path/to/crash
```

For more info on how to write a decent fuzzer:
- Inspect the source code of existing fuzzing binaries.
- Consult [AFL++](https://github.com/AFLplusplus/AFLplusplus). A good start is: 
https://github.com/AFLplusplus/AFLplusplus/blob/stable/instrumentation/README.persistent_mode.md


