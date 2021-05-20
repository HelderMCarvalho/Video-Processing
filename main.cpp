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
    struct
    {
        int width, height;
        int ntotalframes;
        int fps;
        int nframe;
    } video{};

    // Outros
    std::string str;
    int key = 0;


//    Open Webcam video capture (0 is the camera Id)
    capture.open(0, cv::CAP_DSHOW);
    capture.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);

//    If the capture couldn't open
    if (!capture.isOpened())
    {
        std::cerr << "Error opening video capture!\n";
        return 1;
    }

//    Total number os frames (In camera is always 1)
//    video.ntotalframes = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
//    Frame rate (In camera is always 1)
//    video.fps = (int)capture.get(cv::CAP_PROP_FPS);
//    Capture resolution
    video.width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
    video.height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);

//    Create window to show the capture
    cv::namedWindow("VC - Video", cv::WINDOW_AUTOSIZE);

    cv::Mat frame;
    while (key != 'q') {
//        Read frame
        capture.read(frame);

//        Check frame
        if (frame.empty()) break;

//        Frame number (In camera is always 1)
//        video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);

//        Insert text in the frame
        str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));
        cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
        cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
//        str = std::string("TOTAL DE FRAMES: ").append(std::to_string(video.ntotalframes));
//        cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
//        cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
//        str = std::string("FRAME RATE: ").append(std::to_string(video.fps));
//        cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
//        cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
//        str = std::string("N. DA FRAME: ").append(std::to_string(video.nframe));
//        cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
//        cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);

//        Manipulation code
        //// Cria uma nova imagem IVC
        //IVC *image = vc_image_new(video.width, video.height, 3, 255);
        //// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC
        //memcpy(image->data, frame.data, video.width * video.height * 3);
        //// Executa uma fun��o da nossa biblioteca vc
        //vc_rgb_get_green(image);
        //// Copia dados de imagem da estrutura IVC para uma estrutura cv::Mat
        //memcpy(frame.data, image->data, video.width * video.height * 3);
        //// Liberta a mem�ria da imagem IVC que havia sido criada
        //vc_image_free(image);

        // +++++++++++++++++++++++++

//        Show frame
        cv::imshow("VC - Video", frame);

//        Exit when pressing "q"
        key = cv::waitKey(1);
    }

//    Close window
    cv::destroyWindow("VC - Video");

//    Close capture
    capture.release();

    return 0;
}
