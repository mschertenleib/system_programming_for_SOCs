import cv2
import numpy as np

ref = cv2.imread("mandelbrot_fixed_7_25.png")


def compute_diff(image, name: str):
    diff = np.where(np.max(cv2.absdiff(image, ref), axis=-1) > 0, 255, 0)
    cv2.imwrite(name, diff)


compute_diff(cv2.imread("mandelbrot_fixed_6_26.png"), "mandelbrot_fixed_6_26_diff.png")
compute_diff(cv2.imread("mandelbrot_fixed_5_27.png"), "mandelbrot_fixed_5_27_diff.png")
