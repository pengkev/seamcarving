# Seam Carving in C

A content-aware image resizing algorithm implemented in C. Seam carving intelligently reduces image dimensions by removing the least noticeable vertical seams, preserving the most important visual content.

## 📸 Overview

This project implements the full pipeline of vertical seam carving:
1. **Energy Calculation** – Computes pixel energy using image gradients.
2. **Dynamic Programming** – Identifies the lowest-energy seam.
3. **Path Recovery** – Traces the optimal seam through the image.
4. **Seam Removal** – Produces a resized image by removing the seam.

This allows images to be resized with minimal perceptual distortion compared to naive cropping or scaling.

## 🛠 Features

- Energy map computation using RGB gradient magnitudes.
- Dynamic programming for efficient seam cost propagation.
- Seam backtracking with path recovery.
- Image raster manipulation (custom binary format).
- Modular design with clean abstractions for image I/O and pixel access.

## 🗂 File Structure

```

.
├── seamcarving.c       # Core algorithm (energy, dynamic programming, seam removal)
├── c\_img.c             # Image I/O and raster manipulation
├── seamcarving.h       # Header for seamcarving functions
├── c\_img.h             # Header for image helper functions
├── test/               # Folder for test images and usage examples
└── main.c              # Example main driver (optional, not shown)

````

## 🖼 Input Image Format

Images are stored in a **custom binary format**:
- First two 2-byte integers: `height`, `width`
- Followed by: `height × width × 3` bytes (RGB raster)

Use the `write_img` function to output your own test files in this format.

## 🧪 Compilation & Usage

### Compile with GCC

```bash
gcc -o seamcarve seamcarving.c c_img.c main.c -lm
````

### Example Use

```c
struct rgb_img *im;
read_in_img(&im, "input.img");

struct rgb_img *grad;
calc_energy(im, &grad);

double *energy_map;
dynamic_seam(grad, &energy_map);

int *path;
recover_path(energy_map, grad->height, grad->width, &path);

struct rgb_img *resized;
remove_seam(im, &resized, path);

write_img(resized, "output.img");
```

Or run the compiled binary directly if `main.c` is implemented accordingly.

## 📐 Algorithmic Details

### 1. Energy Calculation

Uses the square of RGB differences in x and y directions:

$$
\text{energy}(x, y) = \sqrt{(R_x^2 + G_x^2 + B_x^2 + R_y^2 + G_y^2 + B_y^2)} / 10
$$

### 2. Seam Cost Propagation

Dynamic programming fills a 2D energy cost array top-down, minimizing local seam energy with respect to adjacent predecessors.

### 3. Seam Path Recovery

Backtracks from the minimum in the bottom row to construct the path of least cumulative energy.

### 4. Seam Removal

Constructs a new image by copying all pixels **except** those on the seam.

## 📁 Example Output

Before:
![Before](https://via.placeholder.com/240x180.png?text=Input+Image)

After 10 seam removals:
![After](https://via.placeholder.com/200x180.png?text=Resized+Image)

## 🧼 Memory Management

All dynamically allocated resources (raster buffers, paths, energy maps) must be freed properly to avoid memory leaks.

## 🧾 Dependencies

* Standard C Libraries: `<stdio.h>`, `<stdlib.h>`, `<stdint.h>`, `<math.h>`
* No external libraries or image processing tools required.

## 📌 Future Extensions

* Horizontal seam support
* Multi-seam batch removal
* PNG/BMP image format conversion
* Real-time visualization using SDL/OpenGL

## 📄 License

This project is licensed under the MIT License.

---

Developed as a focused exercise in low-level image processing and optimization.

---
