<p align="center">
  <img src="https://github.com/zbaylin/ReQuests/blob/master/assets/logo.svg?raw=true" style="max-width: 60%" />
  <p align="center">A web requests library for Reason(ML), based on cURL and libuv.</p>
</p>

🚧 **Warning**: This library is a WIP and the API may change frequently 🚧

## Why ReQuests?
ReQuests was created to serve as a lightweight & cross-platform web requests library for [Onivim2](https://github.com/onivim/oni2). The benefit of ReQuests is that:
- **It uses `libcurl`**, one of the most ubiquitous native libraries in existence, created and supported by some of the largest tech companies. Using `libcurl` on it's own provides numerous benefits:
  - **All the "hard stuff" is already done.** While writing an HTTP/2 implementation from scratch in OCaml sounds interesting, it would severely slow down the development velocity of the project. cURL has HTTP/2 support by default (at least in recent versions), and that's only one facet of what you need from a requests library!
  - **It works cross-platform.** Many brave souls have tried to write request libraries for Reason, including [@lessp](https://github.com/lessp)'s [fetch](https://github.com/lessp/fetch). The Achilles' heel of these attempts has always been Windows. Having that working by default (well, [almost](https://github.com/zbaylin/esy-ocurl)) is incredibly beneficial.
- **It uses `libuv`**, a robust event-based library that's used by Onivim2 and Node. It's lightweight, and integrates well with Oni and Revery.
