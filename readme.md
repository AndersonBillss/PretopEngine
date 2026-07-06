# Pretop Engine

This is my attempt at a C++ engine made with Google's webGPU implementation Dawn

## Why Dawn / WebGPU?

This project uses WebGPU as its graphics API. WebGPU is a modern GPU API designed to run in both web browsers and native environments. By using Dawn, Google’s native WebGPU implementation, the renderer can target desktop platforms while sharing the same API model as the web, making it easier to develop fast native applications that can also be deployed to the browser.

## Documentation / References

- WebGPU C API specification
  https://webgpu-native.github.io/webgpu-headers/

- W3C Draft
  https://www.w3.org/TR/webgpu/

- Dawn Documentation
  https://dawn.googlesource.com/dawn/

- Learn WebGPU
  https://eliemichel.github.io/LearnWebGPU/
