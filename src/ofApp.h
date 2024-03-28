#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"
#include <vector>
#include <utility>
#include <algorithm>

struct PixelData {
    int x, y;
    float grayscale;
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void audioOut(ofSoundBuffer& buffer);

    ofVideoGrabber webcam;
    int gridCellSize;
    std::vector<PixelData> sortedPixels;
    int hilbertOrder;

    maxiOsc oscillator;  

    ofxPanel gui;
    ofxFloatSlider minFreqSlider;
    ofxFloatSlider maxFreqSlider;


    std::pair<int, int> hilbertCoordinates(int index, int order);
    std::pair<int, int> rotate(int n, std::pair<int, int> xy, int rx, int ry);
    float mapGrayscaleToFrequency(float grayscale);
};