#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    // Change these to select images
    source_img.load("scenery.jpg");
    target_img.load("hayden.jpg");
    
    // Variables to tweak
    resolution = 10; // The size of the squares used to recreate the image
    
    // Change this size based on image aspect ratio and desired output size
    resize_size = ofVec2f(350, 650); // SPECIFIC TO TARGET IMAGE ASPECT RATIO - NOTE SOURCE MUST BE LARGER THAN THIS
        
    // Calculate the correct number of squares to divide up images
    split_images_to_grid(source_img, target_img, resolution);
    
    //Set up fbo
    pixel_fbo.allocate(resize_size[0], resize_size[1], GL_RGBA);
    pixel_fbo.begin();
    for (int i = 0; i < target_brightness.size(); i++) {
        for (int j = 0; j < target_brightness[0].size(); j++) {
            
            // X position
            int x_pos = ofMap(i, 0, target_brightness.size(), 0, ofGetWidth()/3);
            x_pos = x_pos + resolution / 2;
            
            // Y Position
            int y_pos = ofMap(j, 0, target_brightness[0].size(), 0, ofGetHeight());
            y_pos = y_pos + resolution / 2;
            
            // Get pixel brightness
            float cur_brightness = target_brightness[i][j];
            cout << cur_brightness << endl;
            ofSetColor((int) cur_brightness);
            ofDrawCircle(x_pos, y_pos, resolution/2);
        }
    }
    pixel_fbo.end();
    
    //
    int k = 0;
    
    
    
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
        vector <float> column_vector;
        int col_pixel_index = ofMap(i, 0, t_cols, 0, t_crop.getWidth());
        //col_pixel_index = col_pixel_index + res/ 2;
        for (int j = 0; j < s_rows; j++) {
            int pixel_index = ofMap(j, 0, t_rows, 0, t_crop.getHeight());
            //pixel_index = pixel_index + res / 2;
            float av_brightness = get_av_brightness(t_crop, ofVec2f(col_pixel_index, pixel_index), res);
            column_vector.push_back(av_brightness);
        }
        target_brightness.push_back(column_vector);
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
