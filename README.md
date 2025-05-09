# Osu! Projection Project
A project that captures your Osu! gameplay and project it down onto a stream of your tablet. Why? Because I thought it would be cool and a fun exercise to make.

Video demo using non-ideal webcam and non-ideal Osu! skill (I am definitely not mrekk).

[![Video Demo](https://img.youtube.com/vi/C_So4IpWdVg/0.jpg)](https://www.youtube.com/watch?v=C_So4IpWdVg "Video Demo")

## How it works
With four known point correspondences between your camera stream and Osu! gameplay, we can calculate a perspective transform which is used to project the Osu! gameplay down ontop the tablet.

### Calibration
Make sure Osu! and your tablet driver (OpenTabletDriver) is running. You will still need to run a tablet driver if you are running Lazer.

You will need to calibrate first time you run the program. Calibration is saved to disk, but if you move the camera or the tablet, you will need to recalibrate the setup.

Open the program and wait until you see your camera streamed to the application.

- Press ESC key and press "Calibrate".
- Open Osu! again.
- Using your tablet, hover the cursor near each corner. For each corner press space.
- Go back to the application window, you should now see a static picture of your tablet area with. Left click on the pens tip. Repeat for remaining three images.
- Calibration done!

### Usage
After calibration, it should simply work. If you wanna stream it, simply target this application window instead of the original Osu! window (although you can probably do some cool transition between both windows using OBS).

## Compiling from source
Dependencies are solved using [vcpkg](https://vcpkg.io/en/).
Ensure that you have vcpkg installed and you will need to add `VCPKG_ROOT` to your environment variables.
You will also need to have the Windows 10 SDK V10.0.20348.0 or newer installed (needed for Windows' window capture API).

To build, simply run the following in base directory of the project:
```
cmake --preset=release
cmake --build --preset=release
```
