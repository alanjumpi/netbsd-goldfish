Nothing works at the moment, I'm learning as I go here. But if you really want to see some useless console messages, try:

```emulator-arm -avd ${YOUR_AVD_NAME} -show-kernel -kernel netbsd.gdb -qemu -s -S```

Most of it was "borrowed" from the Integrator port.
