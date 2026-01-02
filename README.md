# ASCII Image to Text Renderer

![Original Image](docs/before.png)
![ASCII Output](docs/after.png)

This project converts an input image into high‑resolution **ASCII art** using image processing techniques implemented in **C++**, and displays the result using a **Python Tkinter viewer**.

At its core, the program performs **edge detection, contrast enhancement, and brightness-based character mapping** to produce visually sharp ASCII output suitable for fullscreen display.

---

## Features

* Grayscale image loading via `stb_image`
* Gaussian blur for noise reduction
* Sobel gradient edge detection
* Non‑maximum suppression (Canny‑style thinning)
* Hysteresis thresholding for clean edges
* Directional edge characters (`|`, `-`, `/`, `\\`)
* Adaptive gamma correction for improved contrast
* Brightness‑based ASCII character mapping
* Binary output stream for fast IPC

---

## Project Structure

```
.
├── ascii.cpp        # C++ image → ASCII converter (this project)
├── ascii.exe        # Compiled binary
├── viewer.py        # Python fullscreen ASCII viewer
├── stb_image.h      # Image loading library
└── image.png        # Input image
```

---

## How It Works (High Level)

```
Image
 ↓
Grayscale
 ↓
Gaussian Blur
 ↓
Sobel Gradients
 ↓
Non‑Maximum Suppression
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
```

The C++ program outputs a **binary stream** describing the ASCII frame, which the Python viewer reads and renders in real time.

---

## Compilation (C++)

Compile the ASCII renderer with full optimization:

```bash
g++ -O3 -march=native -flto ascii.cpp -o ascii
```

### Explanation

* `-O3` – Maximum compiler optimizations
* `-march=native` – Enables CPU‑specific SIMD instructions
* `-flto` – Link‑time optimization for additional speed

> This build is optimized for performance.

---

## Usage

### 1. Run the C++ ASCII generator directly

```bash
./ascii image.png > output.bin
```

This writes a binary ASCII frame to `stdout`.

---

### 2. View the ASCII output (recommended)

Use the Python viewer to display the ASCII art fullscreen:

```bash
python renderer.py image.png
```

#### Controls

* **Mouse click** – Exit
* **Escape key** – Exit

---

## Output Format (Binary Protocol)

The C++ program writes data in the following format:

1. `uint32_t` – ASCII width
2. `uint32_t` – ASCII height
3. For each cell:

   * `uint8_t` – UTF‑8 byte length
   * `N bytes` – UTF‑8 encoded character

This allows:

* Unicode characters
* Fast streaming
* Language‑agnostic parsing

---

## ASCII Character Set

Characters are ordered by visual density:

```
" " . : c o P 0 ? @ ■
```

Edges override brightness mapping using directional symbols.

---

## Requirements

### C++

* C++17 or newer
* [`stb_image.h`](https://github.com/nothings/stb)
* GCC / Clang recommended

### Python

* Python 3.8+
* Tkinter (included with most Python installs)

---

## Notes

* Designed for **static images** (not video)
* Optimized for **fullscreen ASCII rendering**
* Output is deterministic for a given image
* Unicode output requires UTF‑8 capable terminal/viewer

---

## Future Improvements

* Real‑time video / webcam support
* Multi‑frame animation streaming
* Adjustable ASCII resolution
* GPU acceleration
* Cross‑platform console renderer

---

## License

This project is provided as‑is for educational and experimental purposes.
