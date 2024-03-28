#include "ofApp.h"
// Adudio Visual play by Hossein Askari. Final project for PFAD 2024
void ofApp::setup() {
    webcam.setup(512, 512);  
    ofSetFrameRate(30);
    gridCellSize = 8;
    hilbertOrder = pow(2, ceil(log2(512 / gridCellSize)));

    ofSoundStreamSetup(2, 0, this, 44100, 1024, 2);
  
    gui.setup();
    gui.add(minFreqSlider.setup("Min Frequency", 20, 20, 20000));
    gui.add(maxFreqSlider.setup("Max Frequency", 20000, 20, 20000));

}

void ofApp::update() {
    webcam.update();
    if (webcam.isFrameNew()) {
        sortedPixels.clear();
        ofPixels& pixels = webcam.getPixels();
        pixels.resize(512, 512); // both numbers should be in power of 2 in order for the curve to work
        for (int x = 0; x < pixels.getWidth(); x += gridCellSize) {
            for (int y = 0; y < pixels.getHeight(); y += gridCellSize) {
                ofColor color = pixels.getColor(x, y);
                float grayscale = (color.r + color.g + color.b) / 3.0f;
                sortedPixels.push_back({ x, y, grayscale });
            }
        }

        // Sort the pixels based on their Hilbert Curve position
        std::sort(sortedPixels.begin(), sortedPixels.end(), [&](const PixelData& a, const PixelData& b) {
            auto aHilbertPos = hilbertCoordinates(a.x / gridCellSize + a.y / gridCellSize * hilbertOrder, hilbertOrder);
            auto bHilbertPos = hilbertCoordinates(b.x / gridCellSize + b.y / gridCellSize * hilbertOrder, hilbertOrder);
            return aHilbertPos < bHilbertPos;
            });
    }
}

void ofApp::draw() {
    ofClear(0, 0, 0);

    
    for (const auto& pixel : sortedPixels) {
        ofSetColor(pixel.grayscale, pixel.grayscale, pixel.grayscale);
        ofDrawRectangle(pixel.x, pixel.y, gridCellSize, gridCellSize);
    }

    // Visual representation of the frequency on the right side
    for (const auto& pixel : sortedPixels) {
       
        float frequency = mapGrayscaleToFrequency(pixel.grayscale);

       
        float yPos = ofMap(frequency, minFreqSlider, maxFreqSlider, 0, ofGetHeight());

        
        ofSetColor(pixel.grayscale, pixel.grayscale, pixel.grayscale);
        ofDrawCircle(ofGetWidth() - 20, yPos, 2); 
    }

   
    gui.draw();
}


void ofApp::audioOut(ofSoundBuffer& buffer) {
    for (size_t i = 0; i < buffer.getNumFrames(); i++) {
        float frequency = 0;
        if (!sortedPixels.empty()) {
            frequency = mapGrayscaleToFrequency(sortedPixels.front().grayscale);
        }
        float sample = oscillator.sinewave(frequency);
        buffer[i * buffer.getNumChannels()] = sample; 
        buffer[i * buffer.getNumChannels() + 1] = sample; 
    }
}

std::pair<int, int> ofApp::hilbertCoordinates(int index, int order) {
    int x = 0, y = 0;
    for (int s = 1; s < order; s *= 2) {
        int rx = 1 & (index >> 1);
        int ry = 1 & (index ^ rx);
        std::tie(x, y) = rotate(s, { x, y }, rx, ry);
        x += s * rx;
        y += s * ry;
        index /= 4;
    }
    return { x, y };
}

std::pair<int, int> ofApp::rotate(int n, std::pair<int, int> xy, int rx, int ry) {
    if (ry == 0) {
        if (rx == 1) {
            xy.first = n - 1 - xy.first;
            xy.second = n - 1 - xy.second;
        }
        return { xy.second, xy.first };
    }
    return xy;
}

float ofApp::mapGrayscaleToFrequency(float grayscale) {
    float minLogHz = std::log2(minFreqSlider);
    float maxLogHz = std::log2(maxFreqSlider);
    float normalized = grayscale / 255.0;
    return std::pow(2, ofLerp(minLogHz, maxLogHz, normalized));
}