import cv2
import numpy as np
import threading
import keyboard
import pyautogui  # Global mouse position

src_images = []
dst_images = []
src_points = []
dst_points = []
step = 'gameplay'  # 'gameplay' first, then 'webcam'

space_pressed = False

def keyboard_listener():
    global space_pressed
    while True:
        keyboard.wait("space")
        space_pressed = True


def main():
    global space_pressed
    # Capture one frame from both streams
    cap_gameplay = cv2.VideoCapture(1)  # virtual cam (adjust index as needed)
    cap_webcam = cv2.VideoCapture(0)
    cap_webcam.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
    cap_webcam.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

    cv2.namedWindow("Calibration")
    print("Hover over tablet corners and press SPACE (even while focused on Osu!).")
    print("Order: Top-left → Top-right → Bottom-right → Bottom-left")

    threading.Thread(target=keyboard_listener, daemon=True).start()

    while True:
        _, game_frame = cap_gameplay.read()
        _, webcam_frame = cap_webcam.read()
        
        mouse_x, mouse_y = pyautogui.position()

        if space_pressed:
            src_points.append((mouse_x, mouse_y))
            src_images.append(game_frame)
            dst_images.append(webcam_frame)

            print(f"Captured point {len(src_points)}: ({mouse_x}, {mouse_y})")
            space_pressed = False

        if len(src_points) == 4:
            break

    for idx, dst_img in enumerate(dst_images):
        clicked_point = False
        def mouse_callback(event, x, y, flags, param):
            nonlocal clicked_point
            if event == cv2.EVENT_LBUTTONDOWN:
                dst_points.append((x, y))
                clicked_point = True
        
        window_name = "Click the pen point"
        cv2.namedWindow(window_name)
        cv2.setMouseCallback(window_name, mouse_callback)

        cv2.imshow(window_name, dst_img)

        while clicked_point == False:
            if cv2.waitKey(100) & 0xFF == 27:
                cv2.destroyAllWindows()
                break
            
        cv2.destroyWindow(window_name)

    H = cv2.getPerspectiveTransform(np.array(src_points, dtype=np.float32), np.array(dst_points, dtype=np.float32))
    print("\nHomography Matrix:")
    print(H)

    np.save("osu_to_camera_homography.npy", H)
    print("Saved as osu_to_camera_homography.npy")
    cap_webcam.release()
    cap_gameplay.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()




# ret1, gameplay_frame = cap_gameplay.read()
# ret2, webcam_frame = cap_webcam.read()
# cap_gameplay.release()
# cap_webcam.release()

# if not ret1 or not ret2:
#     print("Failed to capture from one or both sources.")
#     exit()

# cv2.imshow("Gameplay - Select 4 corners", gameplay_frame)
# cv2.setMouseCallback("Gameplay - Select 4 corners", click_event)
# cv2.waitKey(0)

# cv2.imshow("Webcam - Select 4 corners", webcam_frame)
# cv2.setMouseCallback("Webcam - Select 4 corners", click_event)
# cv2.waitKey(0)

# # Compute and save homography
# src = np.array(src_points, dtype='float32')
# dst = np.array(dst_points, dtype='float32')
# H, _ = cv2.findHomography(src, dst)

# np.save("homography.npy", H)
# print("Homography matrix saved to homography.npy")





















# osu_width = 2510
# osu_height = 1410
# osu_corners = np.array([[0, 0], [osu_width, 0], [osu_width, osu_height], [0, osu_height]], dtype=np.float32)

# clicked_points = []
# space_pressed = False

# def keyboard_listener():
#     global space_pressed
#     while True:
#         keyboard.wait("space")
#         space_pressed = True

# def main():
#     global space_pressed

#     cap = cv2.VideoCapture(0)
#     cv2.namedWindow("Calibration")

#     print("Hover over tablet corners and press SPACE (even while focused on Osu!).")
#     print("Order: Top-left → Top-right → Bottom-right → Bottom-left")

#     threading.Thread(target=keyboard_listener, daemon=True).start()

#     while True:
#         ret, frame = cap.read()
#         if not ret:
#             break

#         # Get global mouse position (screen coordinates)
#         mouse_x, mouse_y = pyautogui.position()

#         # Show cursor on camera view
#         cv2.circle(frame, (mouse_x, mouse_y), 5, (255, 255, 0), -1)

#         for pt in clicked_points:
#             cv2.circle(frame, pt, 6, (0, 255, 0), -1)

#         cv2.imshow("Calibration", frame)
#         cv2.waitKey(1)

#         if space_pressed and len(clicked_points) < 4:
#             clicked_points.append((mouse_x, mouse_y))
#             print(f"Captured point {len(clicked_points)}: ({mouse_x}, {mouse_y})")
#             space_pressed = False

#         if len(clicked_points) == 4:
#             cam_corners = np.array(clicked_points, dtype=np.float32)
#             H = cv2.getPerspectiveTransform(osu_corners, cam_corners)

#             print("\nHomography Matrix:")
#             print(H)

#             np.save("osu_to_camera_homography.npy", H)
#             print("Saved as osu_to_camera_homography.npy")
#             break

#     cap.release()
#     cv2.destroyAllWindows()

# if __name__ == "__main__":
#     main()