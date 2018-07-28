#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define SRC_WINDOW_NAME "ԭʼͼ��"
#define GRY_WINDOW_NAME "�Ҷ�ͼ��"
#define DST_WINDOW_NAME "���ͼ��"

Mat srcImg, grayImg, roiImg, dstImg;

int main(void)
{
	//��ȡԭʼͼ��
	srcImg = imread("1.jpg");
	if (srcImg.empty()) { printf("��ȡͼƬʧ�ܣ�\n"); return 0; }
	resize(srcImg, srcImg, Size(), 0.25, 0.25);
	//transpose(srcImg, srcImg);
	//flip(srcImg, srcImg, 1);
	double imgSize = srcImg.rows * srcImg.cols;
	imshow(SRC_WINDOW_NAME, srcImg);

	//ת�Ҷ�,�˲�����ֵ�ָ�
	int thre = 200;
	cvtColor(srcImg, grayImg, COLOR_RGB2GRAY);
	GaussianBlur(grayImg, grayImg, Size(5, 5), 0, 0);
	threshold(grayImg, grayImg, thre, 255, 0);

	//������
	int offset = 10;
	Mat element = getStructuringElement(MORPH_RECT, Size(offset * 2 + 1, offset * 2 + 1), Point(offset, offset));
	morphologyEx(grayImg, grayImg, MORPH_CLOSE, element);

	//Ѱ����Ӷ����
	int epsilon = 50;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarcy;
	findContours(grayImg, contours, hierarcy, 0, CV_CHAIN_APPROX_NONE);

	vector<vector<Point>> contours_poly(1);

	double maxConArea = 0.0, maxIndex;
	for (int i = 0; i<contours.size(); i++)
	{
		double conArea = fabs(contourArea(contours[i], true));
		if (conArea > maxConArea)
		{
			maxConArea = conArea;
			maxIndex = i;
		}
	}
	approxPolyDP(Mat(contours[maxIndex]), contours_poly[0], epsilon, true);

	//��ȡROI
	Mat roi = Mat::zeros(srcImg.size(), srcImg.type());
	drawContours(roi, contours_poly, 0, Scalar::all(255), -1);
	srcImg.copyTo(roiImg, roi);


	//����任����
	vector<Point2f> pts_dst;
	pts_dst.push_back(Point2f(0, 0));
	pts_dst.push_back(Point2f(srcImg.cols - 1, 0));
	pts_dst.push_back(Point2f(srcImg.cols - 1, srcImg.rows - 1));
	pts_dst.push_back(Point2f(0, srcImg.rows - 1));

	vector<Point2f> pts_src;
	pts_src.push_back(Point2f(contours_poly[0][1].x, contours_poly[0][1].y));
	pts_src.push_back(Point2f(contours_poly[0][0].x, contours_poly[0][0].y));
	pts_src.push_back(Point2f(contours_poly[0][3].x, contours_poly[0][3].y));
	pts_src.push_back(Point2f(contours_poly[0][2].x, contours_poly[0][2].y));

	Mat h = findHomography(pts_src, pts_dst);

	warpPerspective(roiImg, dstImg, h, dstImg.size());

	//��ʾ�м�ͽ��ͼ��
	imshow(GRY_WINDOW_NAME, roiImg);
	imshow(DST_WINDOW_NAME, dstImg);

	waitKey(0);
	return 0;
}