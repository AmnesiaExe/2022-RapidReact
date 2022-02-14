#include "Vision.h"

struct Images {
  CJ::Image originImage;
  CJ::Image filterImage;
  CJ::Image contourImage;
  CJ::Image boundingImage;
};

class Capture : public CJ::Layer {
 public:
  Capture(CJ::Application &app, Images &images) : Layer("Camera Layer"), _app(app), _images(images) {
   CJ_PRINT_INFO("Example Layer created");
  }

  void onAttach() override {
    _images.originImage.name = "Input Image"; // Set name for image. (used mainly for debugging)
    _cam.config.port = 0;
    _cam.config.name = "Input Camera"; //Sets the name for camera

    if (_cam.init() !=0) { // Initialize the camera. And stop the program if it fails.
      _app.setRunning(false);
    }
    CJ_PRINT_INFO("Camera Created");
  }

  void onDetach() {
    CJ_PRINT_WARN("Example Layer Detached");
  }

  void onUpdate() {
    _cam.capture(_images.originImage);
  }
private:
  CJ::Application &_app;
  Images &_images;
  CJ::Camera _cam; // Camera instance
};

struct HSV_Options {
  int HL = 21,
  HH = 104,
  SL = 181,
  SH = 255,
  VL = 176,
  VH = 255;

  int erosionSize = 0;
  int dilationSize = 0;
  int blur = 0;
  int binaryThreshold_Low = 0;
  int bindyThreshold_High = 0;

};

class FilterLayer : public CJ::Layer {
  public:
  FilterLayer(CJ::Application &app, Images &images) : Layer("Filter Layer"), _app(app), _images(images) {
    CJ_PRINT_INFO("Filter Layer Created");
  }
  void onAttach() override {
    CJ_PRINT_INFO("Filter Layer Attached");
    _images.filterImage.name = "Filtered Image";

    options.HL = 21; // hue low
    options.HH = 104; //hue highfilter
    
    options.SL = 181; // sat low
    options.SH = 255; // sat high

    options.VL = 176; // value low
    options.VH = 255; // value high

    options.erosionSize = 0;
    options.dilationSize = 2;

    CJ::ColourFilter::createFilterTrackbar(options); 
  }
  void onDetach() override {
    CJ_PRINT_WARN("Filter Layer Detached");
  }
  void onUpdate() override {
    CJ::ColourFilter::filter(_images.originImage, _images.filterImage, options); 
  }
  private:
  CJ::Application &_app;
  Images &_images;

  CJ::ColourFilter::HSV_Options options;
};

class ContoursLayer : public CJ::Layer {
 public:
  ContoursLayer(CJ::Application &app, Images &images) : Layer("Contours Layer"), _app(app), _images(images) {
   CJ_PRINT_INFO("Contours Layer Created"); 
  }

  void onAttach() override {
    CJ_PRINT_INFO("Contours Layer Attached");
    _images.contourImage.name = "Contour Image";
  }

  void onDetach() override {
    CJ_PRINT_WARN("Contours Layer Detached");
  }

  void onUpdate() override {
    CJ::Contours::detectContours(_images.filterImage, _images.contourImage); 
  }

  private:
    CJ::Application &_app;
    Images &_images;
};


class BoundingLayer : public CJ::Layer {
 public:
  BoundingLayer(CJ::Application &app, Images &images) : Layer("Contours Layer"), _app(app), _images(images) {
    CJ_PRINT_INFO("Bounding Layer Created");
  }

  void onAttach() override {
    CJ_PRINT_INFO("Bounding Layer Attacked");
    _images.boundingImage.name = "Bounding Image";
  }

  void onDetach() override {
    CJ_PRINT_WARN("Bounding Layer Detached");
  }

  void onUpdate() override {
    CJ::Bound::drawConvexHull(_images.contourImage, _images.boundingImage); // draw hull onto contourImg
    object_xy = CJ::Bound::drawBoundingBox(_images.boundingImage, _images.boundingImage); // draw bounding box and get center points

   // CJ_PRINT_INFO("Points (x,y): " + std::to_string(object_xy.center_x) + "," + std::to_string(object_xy.center_y));
  }

private:
  CJ::Application &_app;
  Images &_images;

  CJ::BoundingPoints object_xy;
};


class OutputLayer : public CJ::Layer {
 public:
  OutputLayer(CJ::Application &app, Images &images) : Layer("Output Layer"), _app(app), _images(images) {

  }

 
  void onUpdate() override {
    CJ::Output::display(30, _images.originImage, _images.filterImage, _images.contourImage, _images.boundingImage);
  }

 private:
  CJ::Application &_app;
  Images &_images;
};

class VisionTracking : public CJ::Application {
 public:
  VisionTracking() : CJ::Application("Vision App") {
    // Layers creation
    pushLayer(new Capture(get(), _images));
    pushLayer(new FilterLayer(get(), _images));
    pushLayer(new ContoursLayer(get(), _images));
    pushLayer(new BoundingLayer(get(), _images));
    pushLayer(new OutputLayer(get(), _images));
  }
 private:
  Images _images;
};

CJ_CREATE_APPLICATION(VisionTracking);

