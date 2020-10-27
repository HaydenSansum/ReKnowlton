#pragma once

#include "ofMain.h"
#include "string.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

        void split_images_to_grid(ofImage c_img, ofImage t_img, int res);
        ofImage crop_image(ofImage img, int w_remainder, int h_remainder);
        float get_av_brightness(ofImage img, ofVec2f corner_pixel, int res);
        ofVec2f get_pixel_location_from_index(int index, int h_num);
        int match_to_nearest_source(int current_index);
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    
    // Image Variables
    ofImage source_img;
    ofImage target_img;
    int s_width; int s_height;
    int t_width; int t_height;
    int resolution;
    ofVec2f resize_size;
   
    // Run mode
    int run_mode;
    
    // Matrix of brightnesses
    vector <float> source_brightness;
    vector <float> target_brightness;
    vector <float> source_brightness_matching;
    
    vector < int > target_indices;
    vector < int > target_to_source_indices;
    
    // For screenshots
    ofImage save_img;
    
    // For comparison
    ofImage t_crop;
    ofImage s_crop;
    
    // Fbo for temporary pixels
    ofFbo pixel_fbo;

    
};
