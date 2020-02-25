//
// Capture images from RealSense camera
//

#pragma once
#include "util/settings.h"
#include "util/globalFuncs.h"
#include "util/globalCalib.h"

#include <sstream>
#include <fstream>
#include <dirent.h>
#include <algorithm>

#include "util/Undistort.h"
#include "IOWrapper/ImageRW.h"

#include <librealsense2/rs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/thread.hpp>

using namespace dso;

class RealsenseCapture
{
public:
	RealsenseCapture(std::string calibFile, std::string gammaFile, std::string vignetteFile) : calibfile(calibFile)
	{
		//this->calibfile = calibFile;

		undistort = Undistort::getUndistorterForFile(calibFile, gammaFile, vignetteFile);

		widthOrg = undistort->getOriginalSize()[0];
		heightOrg = undistort->getOriginalSize()[1];
		width=undistort->getSize()[0];
		height=undistort->getSize()[1];

		// load timestamps if possible.
		// loadTimestamps();
		pipe.start();
	}

	~RealsenseCapture()
	{
		delete undistort;
	};

	Eigen::VectorXf getOriginalCalib()
	{
		return undistort->getOriginalParameter().cast<float>();
	}
	Eigen::Vector2i getOriginalDimensions()
	{
		return  undistort->getOriginalSize();
	}

	void getCalibMono(Eigen::Matrix3f &K, int &w, int &h)
	{
		K = undistort->getK().cast<float>();
		w = undistort->getSize()[0];
		h = undistort->getSize()[1];
	}

	void setGlobalCalibration()
	{
		int w_out, h_out;
		Eigen::Matrix3f K;
		getCalibMono(K, w_out, h_out);
		setGlobalCalib(w_out, h_out, K);
	}

	int getNumImages()
	{
		return 1000;
	}

	double getTimestamp(int id)
	{
		if(timestamps.size()==0) return id*0.1f;
		if(id >= (int)timestamps.size()) return 0;
		if(id < 0) return 0;
		return timestamps[id];
	}

	void prepImage(int id, bool as8U=false)
	{

	}

	// MinimalImageB* getImageRaw(int id)
	// {
	// 	return getImageRaw_internal(id,0);
	// }

	ImageAndExposure* getImage(int id, bool forceLoadDirectly=false)
	{
		return getImage_internal(id, 0);
	}

	inline float* getPhotometricGamma()
	{
		if(undistort==0 || undistort->photometricUndist==0) return 0;
		return undistort->photometricUndist->getG();
	}

	// undistorter. [0] always exists, [1-2] only when MT is enabled.
	Undistort* undistort;
private:

	// Convert rs2::frame to cv::Mat
	cv::Mat frame_to_mat(const rs2::frame& f)
	{
		using namespace cv;
		using namespace rs2;
		auto vf = f.as<video_frame>();
		const int w = vf.get_width();
		const int h = vf.get_height();
		if (f.get_profile().format() == RS2_FORMAT_BGR8)
		{
			auto r = cv::Mat(Size(w, h), CV_8UC3, (void*)f.get_data(), cv::Mat::AUTO_STEP);
			cv::cvtColor(r, r, CV_BGR2GRAY);
			return r;
		}
		else if (f.get_profile().format() == RS2_FORMAT_RGB8)
		{
			auto r = cv::Mat(Size(w, h), CV_8UC3, (void*)f.get_data(), cv::Mat::AUTO_STEP);
			cv::cvtColor(r, r, CV_RGB2GRAY);
			return r;
		}
		throw std::runtime_error("Frame format is not supported yet!");
	}

	MinimalImageB* getImageRaw_internal(int id, int unused)
	{
		rs2::frameset frames = pipe.wait_for_frames();
		rs2::video_frame color = frames.get_color_frame();
		cv::Mat m = frame_to_mat( color );
		if(m.rows*m.cols==0)
		{
			printf("Captured ZERO size image! this may segfault. \n");
			return 0;
		}
		if(m.type() != CV_8U)
		{
			printf("Image depth is not 'unsigned char'! this may segfault. \n");
			return 0;
		}
		MinimalImageB* img = new MinimalImageB(m.cols, m.rows);
		memcpy(img->data, m.data, m.rows*m.cols);
		return img;
	}

	ImageAndExposure* getImage_internal(int id, int unused)
	{
		MinimalImageB* minimg = getImageRaw_internal(id, 0);
		ImageAndExposure* ret2 = undistort->undistort<unsigned char>(
				minimg,
				(exposures.size() == 0 ? 1.0f : exposures[id]),
				(timestamps.size() == 0 ? 0.0 : timestamps[id]));
		delete minimg;
		return ret2;
	}

	std::vector<double> timestamps;
	std::vector<float> exposures;

	int width, height;
	int widthOrg, heightOrg;

	std::string path;
	std::string calibfile;

	rs2::pipeline pipe;
};
