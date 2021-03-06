// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*
    This example shows how to train a CNN based object detector using dlib's 
    loss_mmod loss layer.  This loss layer implements the Max-Margin Object
    Detection loss as described in the paper:
        Max-Margin Object Detection by Davis E. King (http://arxiv.org/abs/1502.00046).
    This is the same loss used by the popular SVM+HOG object detector in dlib
    (see fhog_object_detector_ex.cpp) except here we replace the HOG features
    with a CNN and train the entire detector end-to-end.  This allows us to make
    much more powerful detectors.

    It would be a good idea to become familiar with dlib's DNN tooling before
    reading this example.  So you should read dnn_introduction_ex.cpp and
    dnn_introduction2_ex.cpp before reading this example program.
    
    Just like in the fhog_object_detector_ex.cpp example, we are going to train
    a simple face detector based on the very small training dataset in the
    examples/faces folder.  As we will see, even with this small dataset the
    MMOD method is able to make a working face detector.  However, for real
    applications you should train with more data for an even better result.
*/


#include <iostream>
#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include <dlib/gui_widgets.h>

#include <pixsense/face_finder.hpp>

using namespace std;
using namespace dlib;

// The first thing we do is define our CNN.  The CNN is going to be evaluated
// convolutionally over an entire image pyramid.  Think of it like a normal
// sliding window classifier.  This means you need to define a CNN that can look
// at some part of an image and decide if it is an object of interest.  In this
// example I've defined a CNN with a receptive field of a little over 50x50
// pixels.  This is reasonable for face detection since you can clearly tell if
// a 50x50 image contains a face.  Other applications may benefit from CNNs with
// different architectures.  
// 
// In this example our CNN begins with 3 downsampling layers.  These layers will
// reduce the size of the image by 8x and output a feature map with
// 32 dimensions.  Then we will pass that through 4 more convolutional layers to
// get the final output of the network.  The last layer has only 1 channel and
// the values in that last channel are large when the network thinks it has
// found an object at a particular location.


// Let's begin the network definition by creating some network blocks.

// A 5x5 conv layer that does 2x downsampling
// template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
// template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

// template <typename SUBNET> using downsampler  = dlib::relu<dlib::bn_con<con5d<32, dlib::relu<dlib::bn_con<con5d<32, dlib::relu<dlib::bn_con<con5d<16,SUBNET>>>>>>>>>;
// template <typename SUBNET> using rcon5  = dlib::relu<dlib::bn_con<con5<45,SUBNET>>>;

// using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<Pixsense::input_grayscale_16bit_image_pyramid<dlib::pyramid_down<6>>>>>>>>;



// template <long num_filters, typename SUBNET> using con5d = con<num_filters,5,5,2,2,SUBNET>;
// template <long num_filters, typename SUBNET> using con3  = con<num_filters,3,3,1,1,SUBNET>;

// template <typename SUBNET> using downsampler  = relu<bn_con<con5d<32, relu<bn_con<con5d<32, relu<bn_con<con5d<32,SUBNET>>>>>>>>>;
// template <typename SUBNET> using rcon3  = relu<bn_con<con3<32,SUBNET>>>;
// // using net_type  = loss_mmod<con<1,6,6,1,1,rcon3<rcon3<rcon3<downsampler<input_rgb_image_pyramid<pyramid_down<6>>>>>>>>;

// template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
// template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

// template <typename SUBNET> using downsampler  = dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16,SUBNET>>>>>>>>>;
// template <typename SUBNET> using rcon5  = dlib::relu<dlib::affine<con5<45,SUBNET>>>;

// using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;
// ----------------------------------------------------------------------------------------

template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

template <typename SUBNET> using downsampler  = dlib::relu<dlib::bn_con<con5d<32, dlib::relu<dlib::bn_con<con5d<32, dlib::relu<dlib::bn_con<con5d<16,SUBNET>>>>>>>>>;
template <typename SUBNET> using rcon5  = dlib::relu<dlib::bn_con<con5<45,SUBNET>>>;

using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<input_grayscale_image_pyramid<dlib::pyramid_down<6>>>>>>>>;

int main(int argc, char** argv) try
{
    // The faces directory contains a training dataset and a separate
    // testing dataset.  The training data consists of 4 images, each
    // annotated with rectangles that bound each human face.  The idea is 
    // to use this training data to learn to identify human faces in new
    // images.  
    // 
    // Once you have trained an object detector it is always important to
    // test it on data it wasn't trained on.  Therefore, we will also load
    // a separate testing set of 5 images.  Once we have a face detector
    // created from the training data we will see how well it works by
    // running it on the testing images. 
    // 
    // So here we create the variables that will hold our dataset.
    // images_train will hold the 4 training images and face_boxes_train
    // holds the locations of the faces in the training images.  So for
    // example, the image images_train[0] has the faces given by the
    // rectangles in face_boxes_train[0].
    std::vector<matrix<unsigned char>> images_train, images_test;
    std::vector<std::vector<mmod_rect>> face_boxes_train, face_boxes_test;

    int cropper_count = 50;
    int progress_threshold = 300;
    int i = 1;
    for (int i = i;i < argc;i++) {
        if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") {
            cout << "Give the path to the examples/faces directory as the argument to this" << endl;
            cout << "program.  For example, if you are in the examples folder then execute " << endl;
            cout << "this program by running: " << endl;
            cout << "   ./trainer --training <training_set> --testing <testing_set> --cropper_count <count> --progress_threshold <count>" << endl;
            cout << endl;
            return 0;
        } else if (std::string(argv[i]) == "--training") {
            std::vector<matrix<unsigned char>>         images_load;
            std::vector<std::vector<mmod_rect>> face_s_load;

            load_image_dataset(images_load, face_s_load, std::string(argv[++i]));

            images_train.insert(    images_train.end(),     images_load.begin(), images_load.end());
            face_boxes_train.insert(face_boxes_train.end(), face_s_load.begin(), face_s_load.end());
        } else if (std::string(argv[i]) == "--testing") {
            std::vector<matrix<unsigned char>> images_load;
            std::vector<std::vector<mmod_rect>> face_s_load;

            load_image_dataset(images_load, face_s_load, std::string(argv[++i]));

            images_test.insert(    images_test.end(),     images_load.begin(), images_load.end());
            face_boxes_test.insert(face_boxes_test.end(), face_s_load.begin(), face_s_load.end());
        }  else if (std::string(argv[i]) == "--cropper_count") {
            cropper_count = atoi(argv[++i]);
        }  else if (std::string(argv[i]) == "--progress_threshold") {
            progress_threshold = atoi(argv[++i]);
        }
    // cout << "num training images: " << images_train.size() << endl;
    // cout << "num testing images:  " << images_test.size() << endl;

    }

    // const std::string training_filename = argv[1];
    // const std::string testing_filename = argv[2];

    // // Now we load the data.  These XML files list the images in each dataset
    // // and also contain the positions of the face boxes.  Obviously you can use
    // // any kind of input format you like so long as you store the data into
    // // images_train and face_boxes_train.  But for convenience dlib comes with
    // // tools for creating and loading XML image datasets.  Here you see how to
    // // load the data.  To create the XML files you can use the imglab tool which
    // // can be found in the tools/imglab folder.  It is a simple graphical tool
    // // for labeling objects in images with boxes.  To see how to use it read the
    // // tools/imglab/README.txt file.
    // load_image_dataset(images_train, face_boxes_train, training_filename);
    // load_image_dataset(images_test, face_boxes_test, testing_filename);


    cout << "num training images: " << images_train.size() << endl;
    cout << "num testing images:  " << images_test.size() << endl;
    cout << "num of crops in cropper: " << cropper_count << endl;
    cout << "num iterations without progress: " << progress_threshold << endl;

    if (images_train.size() == 0) {
        return 0;
    }

    // The MMOD algorithm has some options you can set to control its behavior.  However,
    // you can also call the constructor with your training annotations and a "target
    // object size" and it will automatically configure itself in a reasonable way for your
    // problem.  Here we are saying that faces are still recognizably faces when they are
    // 40x40 pixels in size.  You should generally pick the smallest size where this is
    // true.  Based on this information the mmod_options constructor will automatically
    // pick a good sliding window width and height.  It will also automatically set the
    // non-max-suppression parameters to something reasonable.  For further details see the
    // mmod_options documentation.
    mmod_options options(face_boxes_train, 80,80);
    // The detector will automatically decide to use multiple sliding windows if needed.
    // For the face data, only one is needed however.
    cout << "num detector windows: "<< options.detector_windows.size() << endl;
    for (auto& w : options.detector_windows)
        cout << "detector window width by height: " << w.width << " x " << w.height << endl;
    cout << "overlap NMS IOU thresh:             " << options.overlaps_nms.get_iou_thresh() << endl;
    cout << "overlap NMS percent covered thresh: " << options.overlaps_nms.get_percent_covered_thresh() << endl;

    // Now we are ready to create our network and trainer.  
    net_type net(options);

    // The MMOD loss requires that the number of filters in the final network layer equal
    // options.detector_windows.size().  So we set that here as well.
    net.subnet().layer_details().set_num_filters(options.detector_windows.size());
    dnn_trainer<net_type> trainer(net);
    trainer.set_learning_rate(0.1);
    trainer.be_verbose();
    trainer.set_synchronization_file("mmod_sync", std::chrono::minutes(5));
    trainer.set_iterations_without_progress_threshold(progress_threshold);


    // Now let's train the network.  We are going to use mini-batches of 150
    // images.   The images are random crops from our training set (see
    // random_cropper_ex.cpp for a discussion of the random_cropper). 
    std::vector<matrix<unsigned char>> mini_batch_samples;
    std::vector<std::vector<mmod_rect>> mini_batch_labels; 
    random_cropper cropper;
    cropper.set_chip_dims(200, 200);
    // Usually you want to give the cropper whatever min sizes you passed to the
    // mmod_options constructor, which is what we do here.
    cropper.set_min_object_size(80,80);
    dlib::rand rnd;
    // Run the trainer until the learning rate gets small.  This will probably take several
    // hours.
    while(trainer.get_learning_rate() >= 1e-4)
    {
        cropper(cropper_count, images_train, face_boxes_train, mini_batch_samples, mini_batch_labels);
        // We can also randomly jitter the colors and that often helps a detector
        // generalize better to new images.
        for (auto&& img : mini_batch_samples)
            disturb_colors(img, rnd);

        trainer.train_one_step(mini_batch_samples, mini_batch_labels);
    }
    // wait for training threads to stop
    trainer.get_net();
    cout << "done training" << endl;

    // Save the network to disk
    net.clean();
    serialize("../pixsense/models/mmod_human_face_detector_gray.dat") << net;


    // Now that we have a face detector we can test it.  The first statement tests it
    // on the training data.  It will print the precision, recall, and then average precision.
    // This statement should indicate that the network works perfectly on the
    // training data.
    cout << "training results: " << test_object_detection_function(net, images_train, face_boxes_train) << endl;

    if (images_train.size() == 0) {

        // However, to get an idea if it really worked without overfitting we need to run
        // it on images it wasn't trained on.  The next line does this.   Happily,
        // this statement indicates that the detector finds most of the faces in the
        // testing data.
        cout << "testing results:  " << test_object_detection_function(net, images_test, face_boxes_test) << endl;


        // If you are running many experiments, it's also useful to log the settings used
        // during the training experiment.  This statement will print the settings we used to
        // the screen.
        cout << trainer << cropper << endl;

        // // Now lets run the detector on the testing images and look at the outputs.  
        image_window win;
        for (auto&& img : images_train)
        {
            //pyramid_up(img);
            auto dets = net(img);
            win.clear_overlay();
            win.set_image(img);
            for (auto&& d : dets)
                win.add_overlay(d);
            cin.get();
        }
    }
    return 0;

    // Now that you finished this example, you should read dnn_mmod_train_find_cars_ex.cpp,
    // which is a more advanced example.  It discusses many issues surrounding properly
    // setting the MMOD parameters and creating a good training dataset.

}
catch(std::exception& e)
{
    cout << e.what() << endl;
}




