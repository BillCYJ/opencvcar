#include<opencv2\opencv.hpp>
#include <opencv2/core/core.hpp> 
#include<iostream>
#include<opencv2/nonfree/features2d.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/nonfree/nonfree.hpp>
#include<opencv2/opencv.hpp>
#include"cv.h"
#include"cxcore.h"
using namespace std;
using namespace cv; 


//��ȡ��ֵ������ֵ
int myOtsu(IplImage *frame)
//int CLICENSE_PLATE_RECONGNITIONDlg::myOtsu(IplImage *frame) 
{
    #define GrayScale 256//frame�Ҷȼ�
    int width = frame->width; 
    int height = frame->height; 
    int pixelCount[GrayScale]={0}; 
    float pixelPro[GrayScale]={0}; 
    int i, j, pixelSum = width * height, threshold = 0; 
    uchar* data = (uchar*)frame->imageData;

    //ͳ��ÿ���Ҷȼ������صĸ���  
    for(i = 0; i < height; i++) 
    { 
        for(j = 0;j < width;j++) 
        { 
            pixelCount[(int)data[i * width + j]]++; 
        } 
    } 

    //����ÿ���Ҷȼ���������Ŀռ����ͼ��ı���  
    for(i = 0; i < GrayScale; i++) 
    { 
        pixelPro[i] = (float)pixelCount[i] / pixelSum; 
    } 
    //�����Ҷȼ�[0,255],Ѱ�Һ��ʵ�threshold 

    float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0; 
    for(i = 0; i < GrayScale; i++) 
    { 
        w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0; 
        for(j = 0; j < GrayScale; j++) 
        { 
            if(j <= i)   //��������  
            { 
                w0 += pixelPro[j]; 
                u0tmp += j * pixelPro[j]; 
            } 
            else   //ǰ������  
            { 
                w1 += pixelPro[j]; 
                u1tmp += j * pixelPro[j]; 
            } 
        } 
        u0 = u0tmp / w0; 
        u1 = u1tmp / w1; 
        deltaTmp = (float)(w0 *w1* pow((u0 - u1), 2)) ; 
        if(deltaTmp > deltaMax) 
        { 
            deltaMax = deltaTmp; 
            threshold = i; 
        } 
    } 
    return threshold;
}


int main()
{
	//ͼ��Ԥ����������ɫ������
	//HSV�Ǹ�����ɫ��ֱ���Դ�����һ����ɫ�ռ䣬H��ɫ����S�Ǳ��Ͷȣ�V�����ȡ�
	char*path = "009.jpg";
	IplImage*SrcImage = cvLoadImage(path);
	if(!SrcImage)return 0;
    IplImage *imgH=NULL,*imgS=NULL,*imgV=NULL,*imgHSV=NULL,*imgGray=NULL;
    imgHSV=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,3);
    imgH=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    imgS=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    imgV=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    cvCvtColor(SrcImage,imgHSV,CV_BGR2HSV); // תΪHSVͼ��
    cvSplit(imgHSV,imgH,imgS,imgV,NULL);    //�����ͨ���ֱ��Ƶ�������ͨ��ͼ����
    cvInRangeS(imgH,cvScalar(94,0,0,0),cvScalar(115,0,0,0),imgH);   //ѡȡ����ͨ��������ֵ��Χ���ڷ�Χ�ڵ���
    cvInRangeS(imgS,cvScalar(90,0,0,0),cvScalar(255,0,0,0),imgS);   //��1��������0������ɫ���ж�ֵ����
    cvInRangeS(imgV,cvScalar(36,0,0,0),cvScalar(255,0,0,0),imgV);
    IplImage *imgTemp=NULL,*imgHsvBinnary=NULL;
    imgTemp=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    imgHsvBinnary=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    cvAnd(imgH,imgS,imgTemp);
    cvAnd(imgTemp,imgV,imgHsvBinnary);  //H,S,V����ͨ���ֱ�λ���룬�����õĵ�ͨ��ͼ�񱣴浽imgHsvBinnary��
    //��̬ѧȥ��
    //����ṹԪ��
    IplConvKernel *element=0;   //�Զ����
    int values[2]={255,255};
    int rows=2,cols=1,anchor_x=0,anchor_y=1;
    element = cvCreateStructuringElementEx(cols,rows,anchor_x,anchor_y,CV_SHAPE_CUSTOM,values); 
    cvDilate(imgHsvBinnary,imgHsvBinnary,element,1);    //���͸�ʴ
    cvErode(imgHsvBinnary,imgHsvBinnary,element,2);     //��θ�ʴ��2�Σ�����������
    cvNamedWindow("imgh1");
    cvShowImage("imgh1",imgHsvBinnary);

    //��ֵ�ָ�
    imgGray=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    cvCvtColor(SrcImage,imgGray,CV_RGB2GRAY);   //��ԭͼתΪ��ͨ���Ļ�ɫͼ��
    IplImage *imgRgbBinnary;
    imgRgbBinnary=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,1);
    int Thresold=myOtsu(imgGray);   //���ô�����ɫͼ����ֵ
    cvThreshold(imgGray,imgRgbBinnary,Thresold,255,CV_THRESH_OTSU); //���ô����ֵ���ж�ֵ��
    cvNamedWindow("imgh2");
    cvShowImage("imgh2",imgRgbBinnary);     //��ʾ��ֵ��ͼ��

	//���ƶ�λ
    //�ж�λ�����ݳ��Ƶ������ͼ���������ж�λ��
    int hop_num=8; //�ַ����������������ֵ
    int num=0;      //��������Ĵ���
    int begin=0;    //�����Ƿ�ʼ
    int mark_Row[2]={0},k=0;//��һ�α�ǳ��ƵĿ�ʼ���������
    int mark_Row1[2]={0};   //�ڶ��α��
    //��һ�ζ�λ
    for(int i=SrcImage->height-1;i>=0;i--)
    {
        num=0;
        for(int j=0;j<SrcImage->width-1;j++)
        {
            if(cvGet2D(imgHsvBinnary,i,j).val[0]!=cvGet2D(imgHsvBinnary,i,j+1).val[0])  //�������е�ֵ��������Ϊһ������
               {
                    num++;
               }
        }
        if(num>hop_num)
            {
                mark_Row[k]=i;
                k=1;
             }
    }
    //cvLine(SrcImage,cvPoint(0,mark_Row[0]),cvPoint(SrcImage->width,mark_Row[0]),CV_RGB(255,255,0));   //��ԭͼ�л�������ǵ�����
    //cvLine(SrcImage,cvPoint(0,mark_Row[1]),cvPoint(SrcImage->width,mark_Row[1]),CV_RGB(255,255,0));
    //�ж�λ
    int mark_col[2]={0},mark_col1[2]={0},num_col=0,k_col=0;
    int a[100]={0},Thresold_col=7;
    for(int j=0;j<SrcImage->width;j++)
    {
        num_col=0;
        for(int i=mark_Row[1];i<mark_Row[0];i++)    //ֻɨ���Ѿ���ǵ�����֮���ͼ��
            if(cvGet2D(imgHsvBinnary,i,j).val[0]>0)
                num_col++;
                if(num_col>Thresold_col)
                {
                    mark_col[k_col]=j;
                    k_col=1;
                }
    }
    int i=0;
    //cvLine(SrcImage,cvPoint(mark_col[0],0),cvPoint(mark_col[0],SrcImage->height),CV_RGB(255,0,0));
    //cvLine(SrcImage,cvPoint(mark_col[1],0),cvPoint(mark_col[1],SrcImage->height),CV_RGB(255,0,0));
    IplImage *imgLicense;
    int license_Width1=(mark_col[1]-mark_col[0]);
    int license_Height1 =mark_Row[0]-mark_Row[1];
    if(license_Width1/license_Height1<3)    //���ݳ��ƵĿ�Ⱥ͸߶ȱȶԳ��������������
    {
        int real_height1 = license_Width1/3;    //���ƵĿ�Ⱥ͸߶ȱȴ��Ϊ3:1
        mark_Row[1] = mark_Row[0]-real_height1;
        license_Height1 = real_height1;
    }
    //�ڶ��ζ�λ���ڵ�һ�ζ�λ�Ļ���֮�ϣ�
    k=0;
    for(int i=mark_Row[0];i>mark_Row[1];i--)
    {
        num=0;
        for(int j=mark_col[0];j<mark_col[1];j++)
        {
            if(cvGet2D(imgHsvBinnary,i,j).val[0]!=cvGet2D(imgHsvBinnary,i,j+1).val[0])  //�������е�ֵ��������Ϊһ������
               {
                    num++;
               }
        }
        if(num>8)
            {
                mark_Row1[k]=i;
                k=1;
             }
    }
    k_col=0;
    for(int j=mark_col[0];j<mark_col[1];j++)
    {
        num_col=0;
        for(int i=mark_Row1[1];i<mark_Row1[0];i++)  //ֻɨ���Ѿ���ǵ�����֮���ͼ��
            if(cvGet2D(imgHsvBinnary,i,j).val[0]>0)
                num_col++;
                if(num_col>6)
                {
                    mark_col1[k_col]=j;
                    k_col=1;
                }
    }
    int license_Width=(mark_col1[1]-mark_col1[0]);
    int license_Height =mark_Row1[0]-mark_Row1[1];
    if(license_Width/license_Height<3)  //���ݿ�Ⱥ͸߶ȱ��ٴ�����
    {
        int real_height = license_Width/3;  //���ƵĿ�Ⱥ͸߶ȱȴ��Ϊ3:1
        mark_Row1[1] = mark_Row1[0]-real_height;
        license_Height = real_height;
    }
	IplImage*SrcLicenseimg1;
	IplImage*SrcLicenseimg2;
    cvSetImageROI(SrcImage,cvRect(mark_col1[0],mark_Row1[1],license_Width,license_Height)); //��������������ΪROI����
	cvSetImageROI(imgRgbBinnary,cvRect(mark_col1[0],mark_Row1[1],license_Width,license_Height));
    cvSetImageROI(imgHsvBinnary,cvRect(mark_col1[0],mark_Row1[1],license_Width,license_Height));
    imgLicense=cvCreateImage(cvGetSize(SrcImage),SrcImage->depth,SrcImage->nChannels);  //������ʾ�ĳ���ͼƬ
	SrcLicenseimg1=cvCreateImage(cvGetSize(imgRgbBinnary),imgRgbBinnary->depth,imgRgbBinnary->nChannels);
    SrcLicenseimg2=cvCreateImage(cvGetSize(imgHsvBinnary),imgHsvBinnary->depth,imgHsvBinnary->nChannels);
    cvCopy(SrcImage,imgLicense,0);
	cvCopy(imgRgbBinnary,SrcLicenseimg1,0); //���������򿽱�����Ӧ��ͼ����
    cvCopy(imgHsvBinnary,SrcLicenseimg2,0);
    //cvNamedWindow("SrcLicenseimg1");  //��ʾ���ƵĶ�ֵ��ͼƬ
    //cvShowImage("SrcLicenseimg1",SrcLicenseimg1);
    //cvNamedWindow("SrcLicenseimg2");  
    //cvShowImage("SrcLicenseimg2",SrcLicenseimg2);
	
	//�ַ��ָ�
    Mat imge1=imread("0000",1);
	Mat imge2=imread("0022");
	vector<vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(imge2, contours, hierarchy,	CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );
	Mat img1;
	Mat img2;
	Mat img3;
	Mat img4;
	Mat img5; 
	Mat img6;
	Mat img7;
	for(int i=0; i<contours.size();i++)
	{
		Rect rect = boundingRect(Mat(contours[i]));
	    rectangle(imge1, rect.tl(), rect.br(),Scalar(255,0,0));
    	Mat roi = imge1(rect);
	    switch(i)
	    {
		    case 1:roi.convertTo(img1, roi.type());
			case 4:roi.convertTo(img2, roi.type());
			case 5:roi.convertTo(img3, roi.type());
			case 7:roi.convertTo(img4, roi.type());
			case 8:roi.convertTo(img5, roi.type());
			case 9:roi.convertTo(img6, roi.type());
			case 10:roi.convertTo(img7, roi.type());
		}
	}
	imshow("img1",img1);
	imshow("img2",img2);
	imshow("img3",img3);
	imshow("img4",img4);
	imshow("img5",img5);
	imshow("img6",img6);
	imshow("img7",img7);
	imshow("contoursImg", imge1);

    cvResetImageROI(SrcImage);  //ȡ��ROI����
 	cvResetImageROI(imgRgbBinnary);
    cvResetImageROI(imgHsvBinnary);
	cvNamedWindow("license");
    cvShowImage("license",imgLicense);
    //cvResize(imgLicense,TheImage_plate);
    //ShowImage(TheImage_plate,IDC_LICENSE_AREA); //��ʾ����
    cvLine(SrcImage,cvPoint(mark_col1[0],mark_Row1[0]),cvPoint(mark_col1[1],mark_Row1[0]),CV_RGB(255,0,0)); //��ԭͼ���л�����������
    cvLine(SrcImage,cvPoint(mark_col1[1],mark_Row1[0]),cvPoint(mark_col1[1],mark_Row1[1]),CV_RGB(255,0,0));
    cvLine(SrcImage,cvPoint(mark_col1[1],mark_Row1[1]),cvPoint(mark_col1[0],mark_Row1[1]),CV_RGB(255,0,0));
    cvLine(SrcImage,cvPoint(mark_col1[0],mark_Row1[1]),cvPoint(mark_col1[0],mark_Row1[0]),CV_RGB(255,0,0));
    cvNamedWindow("src");
    cvShowImage("src",SrcImage);
    //cvResize( SrcImage, TheImage );    //��ʾԭͼ���Ѿ������˳�������
    //ShowImage( TheImage, IDC_VIDEO_SHOW );
	waitKey();
}