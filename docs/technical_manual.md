# DESIGN
A technical reference manual for development your own products with RTOS-SDK.

## Compile system
### Compile options
- `-mfloat-abi`

​	This option is used to specify how the compiler to build floating-point calculations. You can choose `soft`, `softfp` or `hard`. The `soft` means that the compiler will use software to simulate the floating-point calculation. The `softfp` means that the process will use hardware floating-point, but the floating-point parameters are passed by integer registers. If some old libraries only support software floating-point but the processor uses hardware floating-point, in this case, you can use the option. The `hard` means that the compiler will build source code with hardware floating-point, which requires all of the libraries to support the hardware floating-point and the processor also support hardware floating-point.

- `-mfpu`

​	This option is used to specify the floating-point type for the target processor. You can choose `soft`, `none`, `fpv4-sp-d16` , `neon` or others. The `soft` option means that the processor will only use software to simulate floating-point operations. The `none` option will instruct the compiler to disable floating-point. The `fpv4-sp-d16` option means that the compiler will use the ARMv7 Floating Point Unit 4 (fpv4), Single Precision (sp), 64 bit (d16 double word&times;16) floating-point calculations. The `neon` options is used to extensional floating-point calculations.