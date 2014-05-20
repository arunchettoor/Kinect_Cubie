#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <opencv/cv.h>

	IplImage *freenect_sync_get_depth_cv(int index);
	IplImage *freenect_sync_get_depth_cv_nw(int index);
	IplImage *freenect_sync_get_rgb_cv(int index);
	void stopKinect();

#ifdef __cplusplus
}
#endif
