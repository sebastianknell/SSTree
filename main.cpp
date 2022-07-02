#include "SSTree.h"

const int height = 720;
const int width = 1080;
const string windowName = "SSTree Visualization";
cv::Mat img(height, width, CV_8UC3, {255, 255, 255});
bool drawing = false;
vector<Point> currentDrawing;
SSTree sstree;

static void clickHandler(int event, int x, int y, int flags, void*) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        cout << x << ", " << y << endl;
        Point point = {double(x), double(y)};
        sstree.insert(point);
        img.setTo(cv::Scalar(255, 255, 255));
        sstree.show(img);
        cv::imshow(windowName, img);
    }
    else if (event == cv::EVENT_RBUTTONDOWN) {
        cout << x << ", " << y << endl;
        Point point = {double(x), double(y)};
        sstree.remove(point);
        img.setTo(cv::Scalar(255, 255, 255));
        sstree.show(img);
        cv::imshow(windowName, img);
    }
}

int main() {
    cv::imshow(windowName, img);
    cv::setMouseCallback(windowName, clickHandler);

    char c;
    do {
        c = (char)cv::waitKey(0);
    } while (c != 'q');

    return 0;
}
