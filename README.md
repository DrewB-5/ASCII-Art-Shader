cat << 'EOF' > README.md
# ASCII Image to Text Renderer

| Original Image | ASCII Output |
|---------------|--------------|
| ![Before](docs/before.jpg) | ![After](docs/after.jpg) |

This project converts an input image into high-resolution **ASCII art** using image processing techniques implemented in **C++**, and displays the result using a **Python Tkinter viewer**. Additionally, it now supports **real-time screen capture and video-to-ASCII rendering**.

At its core, the program performs **edge detection, contrast enhancement, and brightness-based character mapping** to produce visually sharp ASCII output suitable for fullscreen display.

---

## Features

- Grayscale image loading via `stb_image`
- Gaussian blur for noise reduction
- Sobel gradient edge detection
- Non-maximum suppression (Canny-style thinning)
- Hysteresis thresholding for clean edges
- Directional edge characters (`|`, `-`, `/`, `\\`)
- Adaptive gamma correction for improved contrast
- Brightness-based ASCII character mapping
- Binary output stream for fast IPC
- **Real-time screen/video capture**
- **FPS display** for performance monitoring

---

## Project Structure

.
├── ascii.cpp           # C++ image → ASCII converter
├── ascii.exe           # Compiled image binary
├── viewer.py           # Python fullscreen ASCII viewer
├── stb_image.h         # Image loading library
├── image.png           # Input image
└── video_capture.cpp   # C++ real-time screen/video ASCII renderer

---

## How It Works (High Level)

### Static Image Rendering

Image  
↓  
Grayscale  
↓  
Gaussian Blur  
↓  
Sobel Gradients  
↓  
Non-Maximum Suppression  
↓  
Hysteresis Thresholding  
↓  
Directional Edge Mapping  
↓  
Gamma Correction  
↓  
ASCII Mapping  
↓  
Binary Output  

### Real-Time Screen/Video Rendering

Screen Capture / Video Frame  
↓  
Downscale & Grayscale  
↓  
Approximate Sobel Edge Detection  
↓  
ASCII Mapping (edges override brightness)  
↓  
Render to Window via Win32 GDI  
↓  
FPS Display Overlay  

The real-time renderer captures the screen at ~30 FPS, downsamples it, applies a Sobel-style edge filter, maps pixel intensity and edges to ASCII characters, and renders the result directly in a Win32 window.

---

## Compilation (C++)

### Static Image Renderer

g++ -O3 -march=native -flto ascii.cpp -o ascii

### Real-Time Screen Capture Renderer (Windows Only)

g++ -O3 -march=native -flto video_capture.cpp -o video_ascii.exe -lgdi32 -luser32

### Optimization Flags

- -O3 – Maximum compiler optimizations  
- -march=native – Enables CPU-specific SIMD instructions  
- -flto – Link-time optimization for additional speed  

Both builds are optimized for performance.

---

## Usage

### 1. Run the C++ ASCII generator directly (image mode)

./ascii image.png > output.bin

This writes a binary ASCII frame to stdout.

---

### 2. View the ASCII output (recommended for images)

python viewer.py image.png

#### Controls

- Mouse click – Exit  
- Escape key – Exit  

---

### 3. Run Real-Time Screen ASCII Renderer (Windows)

./video_ascii.exe

#### Features

- Captures the desktop screen in real time  
- Converts frames to ASCII with edge enhancement  
- Displays live FPS in the top corner  
- Uses Win32 GDI for rendering  
- Close window or press Escape to exit  

---

## Output Format (Binary Protocol – Image Renderer Only)

The static image renderer writes:

1. uint32_t – ASCII width  
2. uint32_t – ASCII height  
3. For each cell:
   - uint8_t – UTF-8 byte length  
   - N bytes – UTF-8 encoded character  

The real-time renderer draws directly to a window and does not output a binary stream.

---

## ASCII Character Set

" " . : c o P 0 ? @ ■

For real-time rendering, detected edges are displayed as:

#

---

## Requirements

### C++

- C++17 or newer  
- stb_image.h (image renderer only)  
- GCC / Clang / MSVC  
- Windows (required for real-time screen capture)  

### Python (Image Viewer Only)

- Python 3.8+  
- Tkinter (included with most Python installs)  

---

## Notes

- Supports static image rendering and real-time screen capture  
- Optimized for fullscreen ASCII display  
- Deterministic output for identical image input  
- Unicode output requires UTF-8 capable terminal/viewer  
- Real-time renderer defaults to ~30 FPS  

---

## Future Improvements

- Real-time webcam support  
- Multi-frame animation streaming  
- Adjustable ASCII resolution  
- GPU acceleration  
- Cross-platform console renderer  
- Advanced edge detection for video frames  

---

## License

This project is provided as-is for educational and experimental purposes.
EOF
