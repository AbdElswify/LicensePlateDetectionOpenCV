#ifndef CSS487FINALPROJECT_LICENCE_PLATE_FINDER_H
#define CSS487FINALPROJECT_LICENCE_PLATE_FINDER_H

#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

class licence_plate_finder {
private:
    typedef std::vector<cv::Point> Contour;// Internal typedef for easier reading

    /**
     * Function to sort 2 vector<cv::Point>'s by the area of their contour.
     * @param contour1
     * @param contour2
     * @return true if the area of contour1 is less than the area of contour2
     */
    static bool compare_contour_areas(const Contour& contour1, const Contour& contour2) {
        const double contourArea1 = fabs(cv::contourArea(cv::Mat(contour1)));
        const double contourArea2 = fabs(cv::contourArea(cv::Mat(contour2)));
        return (contourArea1 < contourArea2);
    }

    /**
     * Debug function to quickly show an image with scale
     * @param image Image to show
     * @param name Image/Window name
     */
    static void show_image(const cv::Mat& image, const std::string& name) {
        static const float scale = 1;

        namedWindow(name, cv::WINDOW_NORMAL);
        imshow(name, image);
        cv::resizeWindow(name, (int) ((float) image.cols / scale), (int) ((float) image.rows / scale));
    }


    static std::vector<Contour> find_contours(const cv::Mat& prepared_image);
    static std::vector<cv::Rect> filter_rectangles_from_contours(const std::vector<Contour>& contours);
    static int find_best_licence_plate(const std::vector<cv::Rect>& rectangles);

public:
    constexpr static const double TARGET_ASPECT = 1.0 / 2.0;
    static const int MAX_RECTS_TO_CONSIDER = 10;
    static const int RECT_THRESHOLD = 10000;

    static cv::Mat prepare_image(const cv::Mat& input_image);
    static cv::Mat highlight_licence_plate(const cv::Mat& input_image);
    static cv::Mat isolate_licence_plate(const cv::Mat& input_image);
    static cv::Mat blur_licence_plate(const cv::Mat& input_image);
};


#endif//CSS487FINALPROJECT_LICENCE_PLATE_FINDER_H
