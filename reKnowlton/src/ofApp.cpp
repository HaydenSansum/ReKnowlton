#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    // Change these to select images
    source_img.load("linh.jpg");
    target_img.load("hayden.jpg");
    
    // Variables to tweak
    resolution = 10; // The size of the squares used to recreate the image
    
    // Change this size based on image aspect ratio and desired output size
    resize_size = ofVec2f(350, 650); // SPECIFIC TO TARGET IMAGE ASPECT RATIO - NOTE SOURCE MUST BE LARGER THAN THIS
        
    // Calculate the correct number of squares to divide up images
    split_images_to_grid(source_img, target_img, resolution);
    int w_grids = resize_size[0] / resolution;
    int h_grids = resize_size[1] / resolution;
    
    
    // Match the target image point to the nearest source image point (randomly)
    for (int i = 0; i < target_brightness.size(); i++){
        target_indices.push_back(i);
        target_to_source_indices.push_back(0); //initialize to zeros
    }
    ofRandomize(target_indices);
    
    
    for (int i = 0; i < target_indices.size(); i++) {
        int current_index = target_indices[i];
        target_to_source_indices[current_index] = match_to_nearest_source(current_index);
    }
    
    
    //Set up FBO - just for test drawing
    pixel_fbo.allocate(resize_size[0], resize_size[1], GL_RGBA);
    pixel_fbo.begin();
    for (int i = 0; i < target_brightness.size(); i++) {
            
            ofVec2f position = get_pixel_location_from_index(i, h_grids);
            
            // X position
            int x_pos = ofMap(position[0], 0, w_grids, 0, ofGetWidth()/3);
            x_pos = x_pos + resolution / 2;
            
            // Y Position
            int y_pos = ofMap(position[1], 0, h_grids, 0, ofGetHeight());
            y_pos = y_pos + resolution / 2;
            
            // Get the source pixel index for this given target pixel
            int source_index = target_to_source_indices[i];
            ofVec2f source_position = get_pixel_location_from_index(source_index, h_grids);
        
            // Get the colour from source and set to brightness level
            ofColor cur_color = s_crop.getColor(source_position[0]*resolution-(resolution/2), source_position[1]*resolution-(resolution/2));
            float cur_brightness = source_brightness[source_index];
            cur_color.setBrightness(cur_brightness);
            ofSetColor(cur_color);
            ofDrawCircle(x_pos, y_pos, resolution/2);
    
    }
    pixel_fbo.end();
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
       
    // Draw original and source images
    t_crop.draw(0, 0);
    s_crop.draw(350,0);
    
    // Draw pixel representation
    pixel_fbo.draw(700,0);
}

//--------------------------------------------------------------
int ofApp::match_to_nearest_source(int current_index) {
    
    // Set the matching brightness to the original so it can be modified in place
    source_brightness_matching = source_brightness;
    
    int best_match;
    
    float current_brightness = target_brightness[current_index];
    float cur_difference = 0;
    float min_difference = 99999;
    
    for (int i=0; i < source_brightness.size(); i++) {
        
        cur_difference = abs(current_brightness - source_brightness_matching[i]);
        if(cur_difference < min_difference) {
            min_difference = cur_difference;
            best_match = i;
        }
        
    }
    // Set the brightness value to be a crazy value so it wont match again
    source_brightness_matching[best_match] = 999999;
    
    return best_match;
}


//--------------------------------------------------------------
ofVec2f ofApp::get_pixel_location_from_index(int index, int h_num) {
    
    ofVec2f pix_loc;
    int x_pos = floor((float) index / (float) h_num);
    int y_pos = index % h_num;
    pix_loc = ofVec2f(x_pos, y_pos);
    
    return pix_loc;
}

//--------------------------------------------------------------
void ofApp::split_images_to_grid(ofImage s_img, ofImage t_img, int res) {
    
    // Get sizes
    int s_w = s_img.getWidth();
    int s_h = s_img.getHeight();
    int t_w = t_img.getWidth();
    int t_h = t_img.getHeight();
    
    // Find outstanding pixels that do not divide evenly into
    int sw_remainder = s_w % res;
    int sh_remainder = s_h % res;
    int tw_remainder = t_w % res;
    int th_remainder = t_h % res;
    
    // Crop outstanding pixels
    t_crop = crop_image(t_img, tw_remainder, th_remainder);
    s_crop = crop_image(s_img, sw_remainder, sh_remainder);
    
    // Resize target
    t_crop.resize(resize_size[0], resize_size[1]);
    s_crop.resize(resize_size[0], resize_size[1]);
    
    // Calculate how many squares are available from each image
    int s_cols = s_crop.getWidth() / res;
    int s_rows = s_crop.getHeight() / res;
    int s_n_sqs = s_cols * s_rows;
    
    int t_cols = t_crop.getWidth() / res;
    int t_rows = t_crop.getHeight() / res;
    int t_n_sqs = t_cols * t_rows;
    
    // Find and store the brightness of the target
    for (int i = 0; i < t_cols; i++) {
        int col_pixel_index = ofMap(i, 0, t_cols, 0, t_crop.getWidth());
        for (int j = 0; j < t_rows; j++) {
            int pixel_index = ofMap(j, 0, t_rows, 0, t_crop.getHeight());
            float av_brightness = get_av_brightness(t_crop, ofVec2f(col_pixel_index, pixel_index), res);
            target_brightness.push_back(av_brightness);
        }
    }
    
    // Find and store the brightness of the source - single vector
    for (int i = 0; i < s_cols; i++) {
        int col_pixel_index = ofMap(i, 0, s_cols, 0, s_crop.getWidth());
        for (int j = 0; j < s_rows; j++) {
            int pixel_index = ofMap(j, 0, s_rows, 0, s_crop.getHeight());
            float av_brightness = get_av_brightness(s_crop, ofVec2f(col_pixel_index, pixel_index), res);
            source_brightness.push_back(av_brightness);
        }
    }
}

//--------------------------------------------------------------
float ofApp::get_av_brightness(ofImage img, ofVec2f corner_pixel, int res) {
    
    float sum_brightnesses = 0;
    int total_pixels = res * res;
    
    int x_st = corner_pixel[0];
    int x_end = corner_pixel[0] + res;
    int y_st = corner_pixel[1];
    int y_end = corner_pixel[1] + res;
    
    for (int i=x_st; i<x_end; i++) {
        for (int j=y_st; j<y_end; j++) {
            sum_brightnesses = sum_brightnesses + img.getColor(i,j).getBrightness();
        }
    }
    
    float av_brightness = sum_brightnesses / total_pixels;
    return av_brightness;
}

//--------------------------------------------------------------
ofImage ofApp::crop_image(ofImage img, int w_remainder, int h_remainder) {
    
    ofImage cropped_img;
        
    // CROP WIDTH
    if (w_remainder % 2 == 0) {
        // If there is an even number of outstanding pixels
        cropped_img.cropFrom(img, (w_remainder / 2), 0, img.getWidth() - (w_remainder), img.getHeight());
    } else {
        // If there is an odd number of outstanding pixels
        int w_minus1 = w_remainder - 1;
        cropped_img.cropFrom(img, (w_minus1 / 2), 0, img.getWidth() - (w_minus1) - 1, img.getHeight());
    }
    
    // CROP HEIGHT
    if (h_remainder % 2 == 0) {
        // If there is an even number of outstanding pixels
        cropped_img.crop(0, (h_remainder / 2), cropped_img.getWidth(), cropped_img.getHeight() - (h_remainder));
    } else {
        // If there is an odd number of outstanding pixels
        int h_minus1 = h_remainder - 1;
        cropped_img.crop(0, (h_minus1 / 2), cropped_img.getWidth(), cropped_img.getHeight() - (h_minus1) - 1);
    }
    
    return cropped_img;
}
    
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        save_img.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        save_img.save(ofGetTimestampString() + "screenshot.png");
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
