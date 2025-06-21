#include "seamcarving.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void calc_energy(struct rgb_img *im, struct rgb_img **grad)
{
    uint8_t h = im->height;
    uint8_t w = im->width;

    create_img(grad, h, w);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            // edge pixels
            int x_prev = (j == 0) ? w - 1 : j - 1;
            int x_next = (j == w - 1) ? 0 : j + 1;
            int y_prev = (i == 0) ? h - 1 : i - 1;
            int y_next = (i == h - 1) ? 0 : i + 1;

            int r_x = get_pixel(im, i, x_next, 0) - get_pixel(im, i, x_prev, 0);
            int g_x = get_pixel(im, i, x_next, 1) - get_pixel(im, i, x_prev, 1);
            int b_x = get_pixel(im, i, x_next, 2) - get_pixel(im, i, x_prev, 2);
            int r_y = get_pixel(im, y_next, j, 0) - get_pixel(im, y_prev, j, 0);
            int g_y = get_pixel(im, y_next, j, 1) - get_pixel(im, y_prev, j, 1);
            int b_y = get_pixel(im, y_next, j, 2) - get_pixel(im, y_prev, j, 2);

            float delta_y = (float)r_y * (float)r_y + (float)g_y * (float)g_y + (float)b_y * (float)b_y;
            float delta_x = (float)r_x * (float)r_x + (float)g_x * (float)g_x + (float)b_x * (float)b_x;

            float energy = (sqrt(delta_y + delta_x) / (float)10.00);
            set_pixel(*grad, i, j, (uint8_t)energy, (uint8_t)energy, (uint8_t)energy);
        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr)
{
    uint8_t h = grad->height;
    uint8_t w = grad->width;

    *best_arr = (double *)malloc(sizeof(double) * h * w);
    if (*best_arr == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    // top row initialize
    for (int j = 0; j < w; j++)
    {
        (*best_arr)[j] = get_pixel(grad, 0, j, 0);
    }

    // fill rows below, minimum of parents + self
    for (int i = 1; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            double min_energy;

            if (j == 0)
            {
                min_energy = fmin((*best_arr)[(i - 1) * w + j], (*best_arr)[(i - 1) * w + j + 1]);
            }
            else if (j == w - 1)
            {
                min_energy = fmin((*best_arr)[(i - 1) * w + j - 1], (*best_arr)[(i - 1) * w + j]);
            }
            else
            {
                min_energy = fmin((*best_arr)[(i - 1) * w + j - 1],
                                  fmin((*best_arr)[(i - 1) * w + j], (*best_arr)[(i - 1) * w + j + 1]));
            }

            (*best_arr)[i * w + j] = min_energy + get_pixel(grad, i, j, 0);
        }
    }
}

void recover_path(double *best, int height, int width, int **path)
{
    // allocate path memory
    *path = (int *)malloc(sizeof(int) * height);
    if (*path == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    // minimum energy in bottom row must be destination for minimum energy path
    double min_energy = INFINITY;
    int min_idx = 0;

    for (int j = 0; j < width; j++)
    {
        double energy = best[(height - 1) * width + j];
        if (energy < min_energy)
        {
            min_energy = energy;
            min_idx = j;
        }
    }
    (*path)[height - 1] = min_idx;

    // lowest energy above destination must have been passed through
    for (int i = height - 2; i >= 0; i--)
    {
        int j = (*path)[i + 1];

        double up_energy = INFINITY;
        double up_left_energy = INFINITY;
        double up_right_energy = INFINITY;

        up_energy = best[i * width + j];

        if (j > 0)
        {
            up_left_energy = best[i * width + (j - 1)];
        }

        if (j < width - 1)
        {
            up_right_energy = best[i * width + (j + 1)];
        }

        if (up_left_energy <= up_energy && up_left_energy <= up_right_energy && j > 0)
        {
            (*path)[i] = j - 1;
        }
        else if (up_energy <= up_left_energy && up_energy <= up_right_energy)
        {
            (*path)[i] = j;
        }
        else
        {
            (*path)[i] = j + 1;
        }
    }
}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path)
{
    uint8_t sh = src->height;
    uint8_t sw = src->width;

    // make new image 1 pixel skinnier
    *dest = (struct rgb_img *)malloc(sizeof(struct rgb_img));
    create_img(dest, sh, sw - 1);

    for (int i = 0; i < sh; i++)
    {
        int seam_col = path[i];
        int dest_col = 0;

        for (int j = 0; j < sw; j++)
        {
            // pixels on seam path skipped
            if (j == seam_col)
            {
                continue;
            }

            uint8_t r = get_pixel(src, i, j, 0);
            uint8_t g = get_pixel(src, i, j, 1);
            uint8_t b = get_pixel(src, i, j, 2);
            set_pixel(*dest, i, dest_col, r, g, b);

            dest_col++;
        }
    }
}