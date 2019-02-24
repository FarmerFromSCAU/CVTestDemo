// OpenCVTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
/*
	步骤：
	1.滤波，避免噪声，这里使用高斯滤波
	2.增强边缘
	3.检测边缘

*/
using namespace cv;
using namespace std;
//原图像，目标图像,灰度图
Mat srcImage, dstImage, grayImage, grayImage1, grayImage2, tmpImage, tmpImage1, tmpImage2;
int ModeValue = 0;
/*
	0 原图像
	1 canny
	2 sobel
	3 拉普拉斯锐化
*/
int GaussianblurSigma = 0;//高斯滤波的标准差，
int CannyLowThreshold = 1;//canny算法低阈值
int CannyHighThreshold = 1;//canny算法高阈值
int SobelKernelSize = 1;//sobel算子核大小
int LaplacianSize = 1;
int LaplacianScale = 1;


void GBSigma(int, void*);//GaussianblurSigma
void ModeType(int, void*);
void CannyType(int, void*);
void SobelType(int, void *);
void Laplacian(int, void *);
void Process();

//将源图像的三个通道分开，推入向量中
vector<Mat>splitImage;
vector<Mat>LaplacianImage;
vector<Mat>LaplacianImageProcessed;

int main(int argc, char* argv[])
{
	int index = 1;//第index次操作
	String indexStr;
	//获取文件名
	String FileName;
	cout << "输入图片名(同目录下或绝对路径)，带后缀，不含空格，如\"lena.png\"\n" << endl;
	cin >> FileName;//输入图片名，带后缀，不含空格，如"lena.png"
	//加载原图像
	srcImage = imread(FileName);
	while (!srcImage.data) {
		cout << "无法读取图片\n" << endl;
		cin >> FileName;
		srcImage = imread(FileName);
	}
	
	//将源图像的三个通道分开，推入向量中
	split(srcImage, splitImage);
	split(srcImage, LaplacianImage);

	//新建窗口
	namedWindow("轮廓检测", 0);
	//把源图像拷贝到目标图像
	dstImage.create(srcImage.size(), srcImage.type());

	//新建窗口
	namedWindow("参数", 0);
	//创建参数滚动条
	//每拖动一次调用一次参数里对应的函数(最后一个参数)
	createTrackbar("模式", "参数", &ModeValue, 3, ModeType);
	createTrackbar("GBS", "参数", &GaussianblurSigma, 600, GBSigma);//Gaussianblur sigma
	createTrackbar("Canny阈值L", "参数", &CannyLowThreshold, 400, CannyType);
	createTrackbar("Canny阈值H", "参数", &CannyHighThreshold, 400, CannyType);
	createTrackbar("Sobel", "参数", &SobelKernelSize, 11, SobelType);
	createTrackbar("LaplacianSize", "参数", &LaplacianSize , 12, Laplacian);
	createTrackbar("LaplacianScale", "参数", &LaplacianScale, 100, Laplacian);

	String pureName;
	String outName;

	//初始化
	ModeType(0, 0);
	//按下ESC退出,按下回车保存
	while (1)
	{
		int key;
		key = waitKey(9);
		if ((char)key == 27)
			break;
		if ((char)key == 13) {
			indexStr += to_string(index);
			pureName = FileName.substr(0, FileName.rfind("."));
			outName = pureName + indexStr + ".jpg";
			imwrite(outName, dstImage);
			index++;
			indexStr.clear();

		}
	}
	return 0;
}
void Process() {
	switch (ModeValue) {
	case 0:
		srcImage.copyTo(dstImage);
		break;
	case 1:
		//转为灰度图
		cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
		GaussianBlur(grayImage, tmpImage, Size(3, 3), GaussianblurSigma / 100);
		Canny(tmpImage, tmpImage, CannyLowThreshold, CannyHighThreshold);
		dstImage = Scalar::all(0);//初始值为0
		srcImage.copyTo(dstImage, tmpImage);
		break;
	case 2:
		Sobel(srcImage, tmpImage, CV_16S, 1, 0, (2 * SobelKernelSize + 1), 1, 1, BORDER_DEFAULT);//求x方向梯度
		convertScaleAbs(tmpImage, tmpImage1);//计算绝对值，并将结果转换成8位
		Sobel(srcImage, tmpImage, CV_16S, 0, 1, (2 * SobelKernelSize + 1), 1, 1, BORDER_DEFAULT);//求y方向梯度
		convertScaleAbs(tmpImage, tmpImage1);//计算绝对值，并将结果转换成8位
		addWeighted(tmpImage1, 0.5, tmpImage1, 0.5, 0, dstImage);
		break;
	case 3://拉普拉斯锐化图像
		/*
		demo
		for (int i = 0; i < 3; i++) {
			GaussianBlur(splitImage[i], LaplacianImage[i], Size(3, 3), GaussianblurSigma / 100);//高斯滤波消除噪声
			Laplacian(LaplacianImage[i], LaplacianImage[i], CV_16S,2*LaplacianSize+1, LaplacianScale);
			convertScaleAbs(LaplacianImage[i], LaplacianImage[i]);
			LaplacianImageProcessed.push_back(LaplacianImage[i] - splitImage[i]);
		}
		LaplacianImageProcessed[1].copyTo(dstImage);
		LaplacianImageProcessed.clear();
		break;
		*/
		GaussianBlur(srcImage, tmpImage, Size(3, 3), GaussianblurSigma);//高斯滤波消除噪声
		//cvtColor(tmpImage, grayImage, COLOR_RGB2GRAY);
		tmpImage.copyTo(grayImage);
		Laplacian(grayImage, tmpImage1, CV_16S, (LaplacianSize * 2 + 1), LaplacianScale/10, 0, BORDER_DEFAULT);
		convertScaleAbs(tmpImage1, tmpImage2);
		dstImage = grayImage - tmpImage2;
		break;
	}
	imshow("轮廓检测", dstImage);
}
void GBSigma(int, void*) {
	Process();
}
void ModeType(int, void*) {
	Process();
}
void CannyType(int, void*) {
	Process();
}
void SobelType(int, void *) {
	Process();
}
void Laplacian(int, void *) {
	Process();
}
