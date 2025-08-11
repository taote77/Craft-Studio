
提取一个轮廓

···python

    img_src = "resources/S000003_P1.png"
    img_rest = "resources/rest.png"
    # 读取PNG图像
    img = cv2.imread(img_src)
    imgray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

    # canny = cv2.Canny(imgray,127,255,0)
    _, thresh = cv2.threshold(imgray, 150, 255, cv2.THRESH_BINARY)
    # THRESH_BINARY  THRESH_BINARY_INV

    contours, hierarchy = cv2.findContours(thresh,cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_NONE)

    fill_img = np.zeros_like(img)

    for contour in contours:
        cv2.fillPoly(fill_img, [contour], (0, 0, 255))

    # cv2.drawContours(fill_img, contours, -1, (0,0,255), cv2.FILLED)

    cv2.imwrite(img_rest, fill_img)


```


# invert image
def InvertImg(img_src):
    img = cv2.imread(img_src)
    img = cv2.bitwise_not(img)
    return img
