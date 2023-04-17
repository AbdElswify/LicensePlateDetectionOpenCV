/**
 * main.cpp
 *
 * CSS 487 Final project main.
 *
 * This file demonstrates how to use licence_plate_finder.cpp by taking the test images from ./test_input, applying our methods to them, and then outputting the results to ./test_output
 *
 * @authors Trevor Rosenstrom and Abd Elswify
 */

#include "licence_plate_finder.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

void run_all_tests_on_image(const std::string& image_path) {
    std::string input_file_name = image_path.substr(image_path.find_last_of("/\\") + 1);
    cv::Mat input_image = cv::imread(image_path);

    cv::Mat input_with_highlighted_licence_plate = licence_plate_finder::highlight_licence_plate(input_image);
    imwrite("test_output/highlighted_plate_" + input_file_name, input_with_highlighted_licence_plate);

    cv::Mat input_with_isolated_licence_plate = licence_plate_finder::isolate_licence_plate(input_image);
    imwrite("test_output/isolated_plate_" + input_file_name, input_with_isolated_licence_plate);

    cv::Mat input_with_blurred_licence_plate = licence_plate_finder::blur_licence_plate(input_image);
    imwrite("test_output/blurred_plate_" + input_file_name, input_with_blurred_licence_plate);
}

void test_all_test_images() {
    std::vector<std::string> test_input_paths = {
            "test_input/lp1.jpg",
            "test_input/lp2.jpg",
            "test_input/lp3.jpg",
            "test_input/lp4.jpg",
            "test_input/lp5.jpg",
            "test_input/lp6.jpg",
    };

    for (const std::string& test_input_path: test_input_paths) {
        run_all_tests_on_image(test_input_path);
    }
}

int main(int argc, char* argv[]) {
    test_all_test_images();

    //    run_all_tests_on_image("your_path_here"); // Uncomment to test your own input image by path

    return 0;
}
