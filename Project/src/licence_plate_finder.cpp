/**
* licence_plate_finder.cpp
*
* The file that contains all our methods used in completing the final project
*
* @authors Trevor Rosenstrom and Abd Elswify
*/

#include "licence_plate_finder.h"

/**
 * Gets the image ready for processing
 *
 * Turn it gray, then apply a bilateral filter
 *
 * @param input_image
 * @return A image with the required transformations applied.
 */
cv::Mat licence_plate_finder::prepare_image(const cv::Mat& input_image) {
    cv::Mat gray;
    cvtColor(input_image, gray, cv::COLOR_BGR2GRAY);
    cv::Mat bilateral;
    bilateralFilter(gray, bilateral, 11, 17, 17);
    cv::Mat blur;
    GaussianBlur(bilateral, blur, cv::Size(5, 5), 0, 0);
    cv::Mat edges;
    Canny(blur, edges, 170, 200);

    if (false) {// Toggle to true to enable outputting debug images
        imwrite("test_output/input_gray.jpg", gray);
        imwrite("test_output/input_gray_bilateral.jpg", bilateral);
        imwrite("test_output/input_gray_bilateral_blur.jpg", blur);
        imwrite("test_output/input_gray_bilateral_blur_edges.jpg", edges);
        show_image(edges, "Edges");
    }

    return edges;
}

/**
 * Uses cv::find_contours to find all the contours in the given prepared image and sorts them by size
 * @param prepared_image The binary input image that has been prepared with licence_plate_finder::prepare_image
 * @return A vector of Contours
 */
std::vector<licence_plate_finder::Contour> licence_plate_finder::find_contours(const cv::Mat& prepared_image) {
    std::vector<Contour> contours;
    cv::findContours(prepared_image, contours, cv::noArray(), cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    std::sort(contours.begin(), contours.end(), compare_contour_areas);
    return contours;
}

/**
 * Filters out given contours for ones that are mostly rectangular
 * @param contours A vector of contours
 * @return A vector of cv::Rect's that represent the bounding boxes of the contours
 */
std::vector<cv::Rect> licence_plate_finder::filter_rectangles_from_contours(const std::vector<Contour>& contours) {
    std::vector<cv::Rect> rectangles;
    for (const Contour& currentCandidate: contours) {
        cv::Rect temp = cv::boundingRect(currentCandidate);
        double difference = temp.area() - cv::contourArea(currentCandidate);
        if (difference < RECT_THRESHOLD) {
            rectangles.push_back(temp);
        } else {
            rectangles.emplace_back();
        }
    }
    return rectangles;
}

/**
 * Finds the best licence plate candidate from list of rectangles based off of size and aspect
 * @param rectangles List of contour bounding boxes
 * @return The index of the best candidate from given rectangles vector
 */
int licence_plate_finder::find_best_licence_plate(const std::vector<cv::Rect>& rectangles) {
    double lowestError = std::numeric_limits<double>::max();
    size_t bestRectIndex = -1;

    for (size_t i = rectangles.size() - 1; i >= std::max(rectangles.size() - MAX_RECTS_TO_CONSIDER, (size_t) 0); i--) {
        cv::Rect rect = rectangles[i];
        double aspect = (double) rect.height / rect.width;
        double error = abs(aspect - TARGET_ASPECT);
        if (error < lowestError) {
            lowestError = error;
            bestRectIndex = i;
        }
    }
    return (int) bestRectIndex;
}

/**
 * Finds a licence plate in the input image and highlights it in a green outline
 * @param input_image Input image with licence plate
 * @return A new image with the found licence plate highlighted
 */
cv::Mat licence_plate_finder::highlight_licence_plate(const cv::Mat& input_image) {
    cv::Mat prepared_image = prepare_image(input_image);
    std::vector<Contour> contours = find_contours(prepared_image);
    std::vector<cv::Rect> rectangles = filter_rectangles_from_contours(contours);

    int best_rect_index = find_best_licence_plate(rectangles);
    if (best_rect_index == -1) {// No valid licence plates were found, just return the input image
        return input_image;
    }

    cv::Mat input_with_highlighted_licence_plate = input_image.clone();
    drawContours(input_with_highlighted_licence_plate, contours, best_rect_index, cv::Scalar(0, 255, 0), 2);
    return input_with_highlighted_licence_plate;
}

/**
 * Find a licence plate and returns an image slice containing it
 * @param input_image Input image with licence plate
 * @return A new image containing only the licence plate
 */
cv::Mat licence_plate_finder::isolate_licence_plate(const cv::Mat& input_image) {
    cv::Mat prepared_image = prepare_image(input_image);
    std::vector<Contour> contours = find_contours(prepared_image);
    std::vector<cv::Rect> rectangles = filter_rectangles_from_contours(contours);

    int best_rect_index = find_best_licence_plate(rectangles);
    if (best_rect_index == -1) {// No valid licence plates were found, just return the input image
        return input_image;
    }

    cv::Rect best_rect = rectangles[best_rect_index];
    cv::Mat input_with_isolated_licence_plate = input_image(best_rect);
    return input_with_isolated_licence_plate;
}

/**
 * Finds a licence plate in the input image and blurs only the licence plate
 * @param input_image Input image with licence plate
 * @return A new image with the found licence plate blurred
 */
cv::Mat licence_plate_finder::blur_licence_plate(const cv::Mat& input_image) {
    cv::Mat prepared_image = prepare_image(input_image);
    std::vector<Contour> contours = find_contours(prepared_image);
    std::vector<cv::Rect> rectangles = filter_rectangles_from_contours(contours);

    int best_rect_index = find_best_licence_plate(rectangles);
    if (best_rect_index == -1) {// No valid licence plates were found, just return the input image
        return input_image;
    }

    cv::Rect best_rect = rectangles[best_rect_index];
    cv::Mat blurred_licence_plate = input_image.clone();

    int blur_amount_x = best_rect.width % 2 == 0 ? best_rect.width - 1 : best_rect.width;
    int blur_amount_y = best_rect.height % 2 == 0 ? best_rect.height - 1 : best_rect.height;
    cv::blur(blurred_licence_plate(best_rect), blurred_licence_plate(best_rect), cv::Size(blur_amount_x, blur_amount_y));

    return blurred_licence_plate;
}
