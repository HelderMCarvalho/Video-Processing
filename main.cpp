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

    // Outros
    std::string str;
    int key = 0;


    // Open Webcam video capture (0 is the camera Id)
    capture.open(1, cv::CAP_DSHOW);
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

    int videoFrameSize3Ch = video.width * video.height * 3, videoFrameSize1Ch = video.width * video.height;

    // Create window to show the capture
    cv::namedWindow("VC - Video", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("VC - Video - DEV", cv::WINDOW_AUTOSIZE);

    cv::Mat frame, frameCopy, segmentedFrame;

    // Images var to use in processing
    bool no_sign = true, blue_sign = false, red_sign = false;
    int totalAuxRGBImgs = 10, totalAuxGrayImgs = 10, n_labels, frame_interval = 0;
    OVC *blobs;
    IVC *auxRGBImgs[totalAuxRGBImgs], *auxGrayImgs[totalAuxGrayImgs], *startRGBImg, *endRGBImg;
    startRGBImg = vc_image_new(video.width, video.height, 3, 255);
    endRGBImg = vc_image_new(video.width, video.height, 3, 255);

    /*auxRGBImgs[0] = vc_read_image((char *) "../Signs/ArrowLeft.ppm");
    auxRGBImgs[1] = vc_read_image((char *) "../Signs/ArrowLeft.ppm");
    for (int auxRGBImg = 2; auxRGBImg < totalAuxRGBImgs; auxRGBImg++) {
        auxRGBImgs[auxRGBImg] = vc_image_new(auxRGBImgs[0]->width, auxRGBImgs[0]->height, 3, 255);
    }
    for (int auxGrayImg = 0; auxGrayImg < totalAuxGrayImgs; auxGrayImg++) {
        auxGrayImgs[auxGrayImg] = vc_image_new(auxRGBImgs[0]->width, auxRGBImgs[0]->height, 1, 255);
    }

    vc_rgb_to_hsv(auxRGBImgs[0], auxRGBImgs[3]);
    vc_hsv_segmentation(auxRGBImgs[3], auxGrayImgs[0], 150, 280, 20, 100, 10, 100);
    vc_binary_contour_fill(auxGrayImgs[0], auxGrayImgs[1]);


    blobs = vc_binary_blob_labelling(auxGrayImgs[1], auxGrayImgs[2], &n_labels);
    vc_binary_blob_info(auxGrayImgs[2], blobs, n_labels);
    for (int i = 0; i < n_labels; i++) {
        *//*if (blobs[i].area > 3000) {
            vc_rgb_draw_center_of_mass(auxRGBImgs[0], &blobs[i]);
            vc_rgb_draw_bounding_box(auxRGBImgs[0], &blobs[i]);
        }*//*
    }

    *//*if (!vc_rgb_get_blue_gray(auxRGBImgs[1])) {
        std::cerr << "Error vc_rgb_get_blue_gray!\n";
    }
    if (!vc_rgb_to_gray(auxRGBImgs[1], auxGrayImgs[0])) {
        std::cerr << "Error vc_rgb_to_gray!\n";
    }
    if (!vc_gray_to_binary_neighborhood_midpoint(auxGrayImgs[0], auxGrayImgs[1], 8)) {
        std::cerr << "Error vc_gray_to_binary_neighborhood_midpoint!\n";
    }
    if (!vc_binary_erode(auxGrayImgs[1], auxGrayImgs[2], 3)){
        std::cerr << "Error vc_binary_erode!\n";
    }
    if (!vc_binary_contour_fill(auxGrayImgs[1], auxGrayImgs[3])) {
        std::cerr << "Error vc_binary_contour_fill!\n";
    }
    if (!vc_rgb_extract_binary(auxRGBImgs[0], auxGrayImgs[3], auxRGBImgs[2])) {
        std::cerr << "Error vc_rgb_extract_binary!\n";
    }*//*

    vc_write_image((char *) "../Out.pgm", auxGrayImgs[1]);
    system("cmd /c start ..\\FilterGear.exe ../Out.pgm");
    vc_write_image((char *) "../Out.ppm", auxRGBImgs[0]);
    system("cmd /c start ..\\FilterGear.exe ../Out.ppm");*/

    for (int auxRGBImg = 0; auxRGBImg < totalAuxRGBImgs; auxRGBImg++) {
        auxRGBImgs[auxRGBImg] = vc_image_new(video.width, video.height, 3, 255);
    }
    for (int auxGrayImg = 0; auxGrayImg < totalAuxGrayImgs; auxGrayImg++) {
        auxGrayImgs[auxGrayImg] = vc_image_new(video.width, video.height, 1, 255);
    }

    while (key != 'q') {
        // Read frame
        capture.read(frame);

        // Check frame
        if (frame.empty()) break;

        // ---------- Frame Manipulation ----------

        // frameCopy = frame.clone();

        // Copy frame data to auxRGBImg
        memcpy(startRGBImg->data, frame.data, videoFrameSize3Ch);
        // memcpy(auxRGBImgs[0]->data, frame.data, videoFrameSize3Ch);

        // --- Process frame ---

        // Lighten the image
        // vc_rgb_lighten(auxRGBImgs[0], 2.5);

        // Convert image to HSV
        cv::cvtColor(frame, frameCopy, cv::COLOR_BGR2HSV);

        if (no_sign || blue_sign) {
            // Segment HSV Image by Blue color
            cv::inRange(frameCopy, cv::Scalar(90, 90, 90), cv::Scalar(130, 255, 255), segmentedFrame);
            memcpy(auxGrayImgs[0]->data, segmentedFrame.data, videoFrameSize1Ch);

            // vc_rgb_to_hsv(startRGBImg, auxRGBImgs[0]);
            // vc_hsv_segmentation(auxRGBImgs[0], auxGrayImgs[0], 90, 130, 90, 255, 90, 255);

            // Reduce noise
            vc_binary_close(auxGrayImgs[0], auxGrayImgs[1], 5, 5);

            // Fill object
            vc_binary_contour_fill(auxGrayImgs[1], auxGrayImgs[2]);

            // Identify blobs
            blobs = vc_binary_blob_labelling(auxGrayImgs[2], auxGrayImgs[3], &n_labels);
            vc_binary_blob_info(auxGrayImgs[3], blobs, n_labels);
            for (int i = 0; i < n_labels; i++) {
                if (blobs[i].area > 5000) {
                    no_sign = false;
                    blue_sign = true;
                    red_sign = false;

                    // Draw center of mass and bounding box
                    vc_rgb_draw_center_of_mass(startRGBImg, &blobs[i]);
                    vc_rgb_draw_bounding_box(startRGBImg, &blobs[i]);
                }
            }
        }




        // --- END Process frame ---

        // vc_gray_to_rgb(auxGrayImgs[0], startRGBImg);

        // Copy data to frame
        endRGBImg = startRGBImg;
        memcpy(frame.data, endRGBImg->data, videoFrameSize3Ch);

        if (blue_sign) {
            // Segment HSV Image by White color
            cv::inRange(frameCopy, cv::Scalar(0, 0, 150), cv::Scalar(180, 76, 255), segmentedFrame);
            memcpy(auxGrayImgs[4]->data, segmentedFrame.data, videoFrameSize1Ch);

            for (int i = 0; i < n_labels; i++) {
                if (blobs[i].area > 5000) {
                    int area_left = 0, area_right = 0;

                    // Count white pixels in the left side of the blob
                    for (int y = blobs[i].y; y < (blobs[i].height + blobs[i].y); y++) {
                        for (int x = blobs[i].x; x < blobs[i].xc; x++) {
                            long int pos = y * auxGrayImgs[4]->bytesperline + x * auxGrayImgs[4]->channels;
                            if (auxGrayImgs[4]->data[pos] == 255) {
                                area_left++;
                            }
                        }
                    }

                    // Count white pixels in the right side of the blob
                    for (int y = blobs[i].y; y < (blobs[i].height + blobs[i].y); y++) {
                        for (int x = blobs[i].xc; x < (blobs[i].width + blobs[i].x); x++) {
                            long int pos = y * auxGrayImgs[4]->bytesperline + x * auxGrayImgs[4]->channels;
                            if (auxGrayImgs[4]->data[pos] == 255) {
                                area_right++;
                            }
                        }
                    }

                    if (area_left > area_right) {
                        str = std::string("Obrigatorio virar a esquerda!");
                    } else {
                        str = std::string("Obrigatorio virar a direita!");
                    }
                    cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
                    cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255),
                                1);
                }
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

        // ---------- END Frame Manipulation ----------

        // Show frame
        cv::imshow("VC - Video", frame);
        cv::imshow("VC - Video - DEV", segmentedFrame);

        // Exit when pressing "q"
        key = cv::waitKey(1);
    }

    // Free all memory
    vc_image_free(startRGBImg);
    vc_image_free(endRGBImg);
    for (int auxRGBImg = 0; auxRGBImg < totalAuxRGBImgs; auxRGBImg++) {
        vc_image_free(auxRGBImgs[auxRGBImg]);
    }
    for (int auxGrayImg = 0; auxGrayImg < totalAuxGrayImgs; auxGrayImg++) {
        vc_image_free(auxGrayImgs[auxGrayImg]);
    }

    // Close window
    cv::destroyWindow("VC - Video");
    cv::destroyWindow("VC - Video - DEV");

    // Close capture
    capture.release();

    return 1;
}
