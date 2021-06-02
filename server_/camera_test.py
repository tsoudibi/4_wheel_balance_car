import cv2


def camera():
    cam = cv2.VideoCapture(0)

    while True:
        ret, img = cam.read()
        vis = img.copy()
        cv2.imshow('Camera', vis)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cam.release()
    cv2.destroyAllWindows()
