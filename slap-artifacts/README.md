# Proof-of-Concept Code Bundle for SLAP

This bundle contains the reverse engineering code for the LAP, as well as the prerequisites to run these experiments on macOS. We have tested this setup on macOS 14.5 build 23F79. The steps are as follows:

1. Install the Kernel Debug Kit (KDK) for macOS 14.5 build 23F79.
1. Follow the README in `pacmanpatcher` to create a patched version of the development kernel, which allows user code to count cycles.
1. Follow the README in `enable-dc-civac`, which is a kernel extension allowing cache flush instructions to run from user code. Here, we also report a bug with `kmutil` where it uses an incorrect path when looking for a custom kernelcache.
1. Follow the README in `slap` for the reverse engineering experiments.