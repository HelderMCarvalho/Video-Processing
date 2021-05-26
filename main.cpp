#include <iostream>
#include <string>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>

extern "C" {
#include "vc.h"
}

#define WIDTH 1280
#define HEIGHT 720

int main() {
    cv::VideoCapture capture;
    struct {
        int width, height;
        int ntotalframes;
        int fps;
        int nframe;
    } video{};

    // Open Webcam video capture (0 is the camera Id)
    capture.open(0, cv::CAP_DSHOW);
    // capture.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
    // capture.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);

    // If the capture couldn't open
    if (!capture.isOpened()) {
        std::cerr << "Error opening video capture!\n";
        return 1;
    }

    // Capture resolution
    video.width = (int) capture.get(cv::CAP_PROP_FRAME_WIDTH);
    video.height = (int) capture.get(cv::CAP_PROP_FRAME_HEIGHT);

    // Create window to show the capture
    cv::namedWindow("VC - Video", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("VC - Video - DEV", cv::WINDOW_AUTOSIZE);

    // Vars
    std::string str;
    int key = 0;
    cv::Mat frame, frameCopy, segmentedFrame, segmentedFrameD, segmentedRedFrame;
    int videoFrameSize3Ch = video.width * video.height * 3, videoFrameSize1Ch = video.width * video.height;

    // Images var to use in processing
    bool no_sign = true, blue_sign = false, red_sign = false;
    int totalAuxRGBImgs = 1, totalAuxBinaryImgs = 7, n_labels, frame_interval = 0, inside_obj_count, area_left, area_right;
    long int pos;
    OVC *blobs;
    IVC *auxRGBImgs[totalAuxRGBImgs], *auxBinaryImgs[totalAuxBinaryImgs], *startRGBImg;
    startRGBImg = vc_image_new(video.width, video.height, 3, 255);

    for (int auxRGBImg = 0; auxRGBImg < totalAuxRGBImgs; auxRGBImg++) {
        auxRGBImgs[auxRGBImg] = vc_image_new(video.width, video.height, 3, 255);
    }
    for (int auxGrayImg = 0; auxGrayImg < totalAuxBinaryImgs; auxGrayImg++) {
        auxBinaryImgs[auxGrayImg] = vc_image_new(video.width, video.height, 1, 255);
    }

    while (key != 'q') {
        // Read frame
        capture.read(frame);

        // Check frame
        if (frame.empty()) break;

        // ---------- Frame Manipulation ----------

        // Copy frame data to auxRGBImg
        memcpy(startRGBImg->data, frame.data, videoFrameSize3Ch);

        // --- Process frame ---

        // Convert image to HSV
        cv::cvtColor(frame, frameCopy, cv::COLOR_BGR2HSV);

        if (no_sign || blue_sign) {
            // Segment HSV Image by Blue color
            cv::inRange(frameCopy, cv::Scalar(90, 90, 90), cv::Scalar(130, 255, 255), segmentedFrame);
            memcpy(auxBinaryImgs[0]->data, segmentedFrame.data, videoFrameSize1Ch);

            // vc_rgb_to_hsv(startRGBImg, auxRGBImgs[0]);
            // vc_hsv_segmentation(auxRGBImgs[0], auxGrayImgs[0], 90, 130, 90, 255, 90, 255);

            // Reduce noise
            vc_binary_close(auxBinaryImgs[0], auxBinaryImgs[1], 5, 5);

            // Fill object
            vc_binary_contour_fill(auxBinaryImgs[1], auxBinaryImgs[2]);

            // Identify blobs
            blobs = vc_binary_blob_labelling(auxBinaryImgs[2], auxBinaryImgs[3], &n_labels);
            vc_binary_blob_info(auxBinaryImgs[3], blobs, n_labels);
            for (int i = 0; i < n_labels; i++) {
                if (blobs[i].area > 5000) {
                    // Here is the sign

                    no_sign = false;
                    blue_sign = true;
                    red_sign = false;

                    // Draw center of mass and bounding box
                    vc_rgb_draw_center_of_mass(startRGBImg, &blobs[i]);
                    vc_rgb_draw_bounding_box(startRGBImg, &blobs[i]);
                }
            }
        }

        if (no_sign || red_sign) {
            // Segment HSV Image by Red color
            cv::inRange(frameCopy, cv::Scalar(0, 90, 90), cv::Scalar(15, 255, 255), segmentedFrame); // STOP
            cv::inRange(frameCopy, cv::Scalar(155, 90, 90), cv::Scalar(180, 255, 255), segmentedFrameD); // Forbidden
            segmentedRedFrame = max(segmentedFrame, segmentedFrameD);
            memcpy(auxBinaryImgs[0]->data, segmentedRedFrame.data, videoFrameSize1Ch);

            // Reduce noise
            vc_binary_close(auxBinaryImgs[0], auxBinaryImgs[1], 5, 5);

            // Fill object
            vc_binary_contour_fill(auxBinaryImgs[1], auxBinaryImgs[2]);

            // Identify blobs
            blobs = vc_binary_blob_labelling(auxBinaryImgs[2], auxBinaryImgs[3], &n_labels);
            vc_binary_blob_info(auxBinaryImgs[3], blobs, n_labels);
            for (int i = 0; i < n_labels; i++) {
                if (blobs[i].area > 5000) {
                    // Here is the sign

                    no_sign = false;
                    blue_sign = false;
                    red_sign = true;

                    // Draw center of mass and bounding box
                    vc_rgb_draw_center_of_mass(startRGBImg, &blobs[i]);
                    vc_rgb_draw_bounding_box(startRGBImg, &blobs[i]);
                }
            }
        }

        if (blue_sign) {
            // If blue sign was found

            // Segment HSV Image by White color
            cv::inRange(frameCopy, cv::Scalar(0, 0, 150), cv::Scalar(180, 76, 255), segmentedFrame);
            memcpy(auxBinaryImgs[4]->data, segmentedFrame.data, videoFrameSize1Ch);

            // Set auxBinaryImgs to White
            memset(auxBinaryImgs[5]->data, 255,
                   auxBinaryImgs[4]->width * auxBinaryImgs[4]->height * auxBinaryImgs[4]->channels);

            area_left = 0, area_right = 0;
            for (int i = 0; i < n_labels; i++) {
                if (blobs[i].area > 5000) {

                    // Count white pixels in the left side of the blob
                    for (int y = blobs[i].y; y < (blobs[i].height + blobs[i].y); y++) {
                        for (int x = blobs[i].x; x < blobs[i].xc; x++) {
                            pos = y * auxBinaryImgs[4]->bytesperline + x * auxBinaryImgs[4]->channels;
                            if (auxBinaryImgs[4]->data[pos] == 255) {
                                area_left++;
                            }
                        }
                    }

                    // Count white pixels in the right side of the blob
                    for (int y = blobs[i].y; y < (blobs[i].height + blobs[i].y); y++) {
                        for (int x = blobs[i].xc; x < (blobs[i].width + blobs[i].x); x++) {
                            pos = y * auxBinaryImgs[4]->bytesperline + x * auxBinaryImgs[4]->channels;
                            if (auxBinaryImgs[4]->data[pos] == 255) {
                                area_right++;
                            }
                        }
                    }

                    // Extract sign to auxBinaryImgs. Extract black pixels from the image.
                    for (int y = blobs[i].y; y < (blobs[i].height + blobs[i].y); y++) {
                        for (int x = blobs[i].x; x < (blobs[i].width + blobs[i].x); x++) {
                            pos = y * auxBinaryImgs[4]->bytesperline + x * auxBinaryImgs[4]->channels;
                            if (auxBinaryImgs[4]->data[pos] == 0) {
                                auxBinaryImgs[5]->data[pos] = auxBinaryImgs[4]->data[pos];
                            }
                        }
                    }
                }
            }

            // Identify blobs inside the sign
            inside_obj_count = 0;
            blobs = vc_binary_blob_labelling(auxBinaryImgs[5], auxBinaryImgs[6], &n_labels);
            vc_binary_blob_info(auxBinaryImgs[6], blobs, n_labels);
            for (int i = 0; i < n_labels; i++) {
                if (blobs[i].area < 15000) {
                    inside_obj_count++;
                }
            }

            // If there is only one object inside the sign then it's either "Turn Left" or "Turn Right".
            if (inside_obj_count == 1) {
                // If there are more white pixels in the left side of the image, then is "Turn Left" sign, otherwise its
                // "Turn Right".
                if (area_left > area_right) {
                    str = std::string("Turn left!");
                } else {
                    str = std::string("Turn right!");
                }
            } else if (inside_obj_count > 3) {
                // If there are more than 3 objects inside the image, then it's "Highway" sign.
                str = std::string("Highway!");
            } else if (inside_obj_count <= 3 && inside_obj_count != 0) {
                // If there are less than or exactly 3 objects inside the image, then it's "Car" sign.
                str = std::string("Car!");
            }

        } else if (red_sign) {
            // Segment HSV Image by White color
            cv::inRange(frameCopy, cv::Scalar(0, 0, 150), cv::Scalar(180, 76, 255), segmentedFrame);
            memcpy(auxBinaryImgs[4]->data, segmentedFrame.data, videoFrameSize1Ch);

            // Set auxBinaryImgs to White
            memset(auxBinaryImgs[5]->data, 255,
                   auxBinaryImgs[4]->width * auxBinaryImgs[4]->height * auxBinaryImgs[4]->channels);

            for (int i = 0; i < n_labels; i++) {
                if (blobs[i].area > 5000) {
                    // Extract sign to auxBinaryImgs. Extract black pixels from the image.
                    for (int y = blobs[i].y; y < (blobs[i].height + blobs[i].y); y++) {
                        for (int x = blobs[i].x; x < (blobs[i].width + blobs[i].x); x++) {
                            pos = y * auxBinaryImgs[4]->bytesperline + x * auxBinaryImgs[4]->channels;
                            if (auxBinaryImgs[4]->data[pos] == 0) {
                                auxBinaryImgs[5]->data[pos] = auxBinaryImgs[4]->data[pos];
                            }
                        }
                    }
                }
            }

            // Identify blobs inside the sign
            inside_obj_count = 0;
            blobs = vc_binary_blob_labelling(auxBinaryImgs[5], auxBinaryImgs[6], &n_labels);
            vc_binary_blob_info(auxBinaryImgs[6], blobs, n_labels);
            for (int i = 0; i < n_labels; i++) {
                if (blobs[i].area < 15000) {
                    inside_obj_count++;
                }
            }

            if (inside_obj_count == 1) {
                // If there is only 1 object inside the sign, then it's "Forbidden" sign.
                str = std::string("Forbidden!");
            } else if (inside_obj_count > 1) {
                // If there are more than 1 object inside the sign, then it's "STOP" sign.
                str = std::string("STOP!");
            }
        }

        // Every 10 frames reset the sign indicator. This allows to have more performance for 10 frames in a row.
        // 1 in 11 frames will check for all types of signs.
        if ((blue_sign || red_sign) && frame_interval <= 10) {
            frame_interval++;
        } else {
            no_sign = true;
            blue_sign = false;
            red_sign = false;
            frame_interval = 0;
        }

        // --- END Process frame ---

        // Copy data to frame
        memcpy(frame.data, startRGBImg->data, videoFrameSize3Ch);

        cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
        cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255),
                    1);
        str.clear();

        // ---------- END Frame Manipulation ----------

        // Show frame
        cv::imshow("VC - Video", frame);
        cv::imshow("VC - Video - DEV", segmentedFrame);

        // Exit when pressing "q"
        key = cv::waitKey(1);
    }

    // Free all memory
    vc_image_free(startRGBImg);
    for (int auxRGBImg = 0; auxRGBImg < totalAuxRGBImgs; auxRGBImg++) {
        vc_image_free(auxRGBImgs[auxRGBImg]);
    }
    for (int auxGrayImg = 0; auxGrayImg < totalAuxBinaryImgs; auxGrayImg++) {
        vc_image_free(auxBinaryImgs[auxGrayImg]);
    }

    // Close window
    cv::destroyWindow("VC - Video");
    cv::destroyWindow("VC - Video - DEV");

    // Close capture
    capture.release();

    return 1;
}
