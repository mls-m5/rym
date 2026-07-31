# Emscripten and WebGL considerations

## Frame-rate problem

The Emscripten build initially ran at a noticeably lower and less consistent
frame rate than the native desktop build.

The renderer wraps many OpenGL operations in the `glCall` macro:

```cpp
#define glCall(call) \
    call;            \
    checkGlError(#call)
```

`checkGlError()` originally called `glGetError()` after every wrapped OpenGL
operation. This is useful while debugging a native OpenGL application, but it
is particularly expensive in a WebGL build.

An Emscripten OpenGL call crosses from WebAssembly through JavaScript into the
browser's WebGL implementation. `glGetError()` is synchronous and may force
queued graphics work and browser-side state to be checked before it can return.
Doing this after hundreds of small rendering operations creates many
Wasm-to-JavaScript-to-WebGL round trips and disrupts frame pacing.

The effect is amplified by the current renderer because stars, comets, and
other objects are rendered using individual draw calls. A typical frame can
contain roughly 500 stars and 50 comets, with repeated shader bindings,
transform updates, draw calls, and error checks.

## Implemented workaround

For Emscripten builds, `checkGlError()` is now compiled as a no-op:

```cpp
#ifdef __EMSCRIPTEN__
    (void)op;
    (void)throwError;
    return 0;
#else
    // Existing native glGetError() diagnostics
#endif
```

This change is located in `src/glapi.cppm`.

Native builds are unaffected and retain the existing OpenGL error checking.
Only builds where the compiler defines `__EMSCRIPTEN__` omit the synchronous
checks.

This produced a substantial improvement in browser frame rate and consistency.

## Debugging tradeoff

The browser build will no longer report errors through `checkGlError()` after
each wrapped call. Shader compilation and program-linking errors are still
reported explicitly by `src/shaderprogram.cppm`.

If detailed WebGL error checking is needed again, it would be better to expose
it as a dedicated debug build option rather than enabling it in normal browser
builds. For example:

```cpp
#if defined(__EMSCRIPTEN__) && !defined(RYM_WEBGL_ERROR_CHECKS)
    return 0;
#else
    // Check glGetError()
#endif
```

The diagnostic version could then be enabled temporarily with:

```text
-DRYM_WEBGL_ERROR_CHECKS
```

Browser developer tools and WebGL debugging extensions can also provide useful
diagnostics without imposing the cost on every production frame.

## Further rendering improvements

Disabling `glGetError()` removes a major synchronization cost, but the renderer
still submits many small draw calls. Potential future improvements include:

1. Batch all stars into a single vertex buffer and draw call.
2. Use instanced rendering for stars and comets.
3. Avoid rebinding a shader or vertex array when it is already active.
4. Avoid resetting and uploading an object transform after every draw.
5. Continue collecting smoke vertices and uploading them once per frame.
6. Add an optional frame-time or FPS display to detect regressions.

These changes should reduce both native and WebGL CPU overhead, although WebGL
generally benefits more because every graphics API call crosses an additional
browser boundary.

