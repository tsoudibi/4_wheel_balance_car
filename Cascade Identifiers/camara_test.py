import cv2

cam = cv2.VideoCapture(0)

while True:
    ret, img = cam.read()
    vis = img.copy()
    cv2.imshow('Camera', vis)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cam.release()
cv2.destroyAllWindows()
#from
##https://clay-atlas.com/blog/2019/07/30/%E4%BD%BF%E7%94%A8-opencv-%E9%80%B2%E8%A1%8C%E7%AD%86%E9%9B%BB%E6%94%9D%E5%BD%B1%E6%A9%9F%E7%9A%84%E8%AA%BF%E7%94%A8-by-python/