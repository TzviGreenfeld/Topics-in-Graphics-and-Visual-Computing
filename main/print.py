import cv2
import numpy as np

# simple script to read img and print the matrix
# print all the output
np.set_printoptions(threshold=np.inf)


img = cv2.imread('Islands_of_the_Sentinel.png', cv2.cv2.IMREAD_COLOR)
# flat th img matrix to 1d array
img2 = img.flatten()
print(type(img2[2]))
# ma, mi = max(img2), min(img2)
# with open("mat.txt", "w") as f:
#     f.write(f"max={ma}, min={mi}\n")
#     f.write(str(img))
