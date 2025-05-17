// 必要なヘッダファイル
#include <librealsense2/rs.hpp>
#include <librealsense2-net/rs_net.hpp>
#include <opencv2/opencv.hpp>
#include "Inference.h"

// マクロ定義
#define IP_ADDR "XXX.XXX.XXX.XXX"
#define WHITE cv::Scalar(255, 255, 255)
#define GREEN cv::Scalar(0, 255, 0)
#define WIDTH 640
#define HEIGHT 480

int main() {
    try {
        // ストリーミング設定
        rs2::config config;
        config.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);
        config.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 0);
        rs2::pipeline pipe;
        pipe.start(config);

        // 初期化とYOLOの設定
        float scale = 1000.0;
        float distance[5];
        int px = 160;
        int py = 120;
        Inference inf("model.engine");

        while (true) {
            // フレーム取得とアライメント
            rs2::frameset frames = pipe.wait_for_frames();
            rs2::align align_to_color(RS2_STREAM_COLOR);
            frames = align_to_color.process(frames);
            rs2::video_frame color_frame = frames.get_color_frame();
            rs2::depth_frame depth = frames.get_depth_frame();

            // 中心距離の取得
            float center_distance = depth.get_distance(WIDTH / 2, HEIGHT / 2);
            std::cout << "中心の距離: " << center_distance << " m" << std::endl;

            // 画像の設定
            cv::Mat color(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
            cv::Mat cropped(color, cv::Rect(px, py, 320, 240));
            cv::resize(cropped, cropped, cv::Size(WIDTH, HEIGHT));

            // YOLO推論
            auto output = inf.runInference(cropped);
            std::cout << "検出数: " << output.size() << std::endl;

            // 車の検出と可視化
            for (const auto& detection : output) {
                if (detection.className == "car") {
                    cv::rectangle(color, detection.box, GREEN, 2);
                    cv::drawMarker(color, cv::Point(detection.box.x + detection.box.width / 2,
                                                    detection.box.y + detection.box.height / 2),
                                   WHITE, cv::MARKER_CROSS, 10, 2);
                    float area_ratio = (float)(detection.box.width * detection.box.height) / (WIDTH * HEIGHT);
                    std::cout << "占有率: " << area_ratio << std::endl;
                }
            }

            // 表示とウィンドウ破棄処理
            cv::imshow("Display", color);
            if (cv::waitKey(1) == 27) {
                cv::destroyAllWindows();
                break;
            }
        }
    } catch (const rs2::error& e) {
        std::cerr << "RealSense error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    }

    return 0;
}
