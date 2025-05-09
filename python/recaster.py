import cv2
import numpy as np

# Load calibration homography
H = np.load("osu_to_camera_homography.npy")

# Open video sources
osu_stream = cv2.VideoCapture(1)  # Virtual Osu! camera index
webcam = cv2.VideoCapture(0)      # Webcam (tablet view)
webcam.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
webcam.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

# Read one frame to get dimensions
ret, cam_frame = webcam.read()
if not ret:
    raise RuntimeError("Could not read from webcam")
cam_height, cam_width = cam_frame.shape[:2]
print(f"Cam resolution: ({cam_width}, {cam_height})")
# Correct Osu! resolution (based on your setup)

# Transparency control
alpha = 0.8
# Threshold for black detection
black_threshold = 10  # Adjust this value based on your needs

while True:
    ret_cam, cam_frame = webcam.read()
    ret_osu, osu_frame = osu_stream.read()
    if not ret_cam or not ret_osu:
        break

    # Resize Osu! stream to match calibrated resolution
    osu_frame = cv2.resize(osu_frame, (3440, 1440))

    # Warp Osu! view into camera perspective
    osu_projected = cv2.warpPerspective(osu_frame, H, (cam_width, cam_height))

    # Create a mask for non-black pixels in the projected image
    # Pixels are considered black if all RGB values are below the threshold
    mask = cv2.cvtColor(osu_projected, cv2.COLOR_BGR2GRAY)
    _, mask = cv2.threshold(mask, black_threshold, 255, cv2.THRESH_BINARY)

    # Create overlay by manually combining the images with the mask
    overlay = cam_frame.copy()

    # Only blend where the mask has white pixels (non-black areas of osu)
    for c in range(0, 3):  # Process each color channel
        overlay[:, :, c] = np.where(
            mask == 255,
            (alpha * osu_projected[:, :, c] + (1 - alpha) * cam_frame[:, :, c]).astype(np.uint8),
            cam_frame[:, :, c]
        )

    overlay = cv2.flip(overlay, -1)  # 0 means flipping around the x-axis (vertical flip)

    cv2.imshow("Osu! Augmented Projection", overlay)

    if cv2.waitKey(1) == 27:  # ESC to quit
        break

osu_stream.release()
webcam.release()
cv2.destroyAllWindows()