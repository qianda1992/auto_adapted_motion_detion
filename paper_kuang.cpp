// paper_kuang.cpp : �������̨Ӧ�ó������ڵ㡣
//

//�˶�ǰ����⡪����������Ӧ��������

#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

#define threshold1 5 //��̬������ֵ
#define Pathfile "video3.avi"
#define SLOTnum 10 //���������С���൥λ
#define SampleNo 50//��ʼ��������
#define PI 3.142 
#define threshold2 0.0047//���ܶȹ����ж��Ƿ�Ϊǰ����ֵ
#define threshold3 0.45


////////////////////////////////////////////////////��ȡ��Ƶ������Ĳ���Ϊ��Ƶ��λ�ü�����///////////////////
VideoCapture ReadFile(string pathfile)
{	
	std::string videoFileName = pathfile;
	cv::VideoCapture capture;
	capture.open(videoFileName);
	if (!capture.isOpened())
	{
		std::cout<<"cannot open video"<<std::endl;
		return -1;
	}
	return capture;

}

/////////////////////////////////////////////////////////ƽ���������У���̬ƽ������////////////////////////////
Mat init_Backgound(string pathfile)
	{
	VideoCapture capture=ReadFile(pathfile);
	Mat grayImg,grayMat,frame;
	capture.read(frame);	
	int height=frame.rows;
	int width=frame.cols;
	Mat matcount=Mat::zeros(height,width,CV_32FC1);	
	//int frameno= capture.get(CV_CAP_PROP_FRAME_COUNT);
	int frametostart=0;
	int frametoend=SampleNo;
	int currentframe=0;
	while(currentframe<frametoend)
	{

	cv::cvtColor(frame, grayImg, CV_BGR2GRAY);
	grayImg.convertTo(grayMat, CV_32FC1);
	matcount=matcount+grayMat;	
	currentframe++;
	}
	
	CvMat cvMat=matcount;
	CvMat *backMat=cvCreateMat(height,width,CV_32FC1);
	cvZero(backMat);	
	for (int i=0;i<height;i++){		
		for (int j=0;j<width;j++)
		{	
								
			cvmSet(backMat,i,j,(cvmGet(&cvMat,i,j)/double(SampleNo)));
					
		}
	
	}
	
	capture.release();
	
	return backMat;

	}

////////////////////////////////////////////////////////////����ƽ��������������ǰ�������ж�/////////////////////////////////////////
int backfilter(Mat init_back,string pathfile)
{
	VideoCapture capture=ReadFile(pathfile);
	Mat frame,grayImg,grayMat,result,resultimg,init_backimg;
	int height=init_back.rows;
	int width=init_back.cols;	
	int framno=0;
	Mat filtermat=Mat::zeros(height,width,CV_32FC1);
	init_back.convertTo(init_backimg,CV_8U);
	CvMat value;
	while (capture.read(frame))
	{	framno++;
		cv::cvtColor(frame, grayImg, CV_BGR2GRAY);
		grayImg.convertTo(grayMat, CV_8U);
		absdiff(init_backimg,grayMat,result);
		result.convertTo(result,CV_32FC1);		
		
		CvMat cvMat=result;
		CvMat cvfiltermat=filtermat;
		for (int i=0;i<height;i++){		
			for (int j=0;j<width;j++)
			{	
				float tmp=cvmGet(&cvMat,i,j);			
				if (tmp>threshold1)
				{	
					
					cvmSet(&cvfiltermat,i,j,255);
				}

				else
				{
					cvmSet(&cvfiltermat,i,j,0);
				}

			}

		}	

		Mat(&cvfiltermat).convertTo(resultimg,CV_8U);
		Mat gaussi_result;
		cv::medianBlur(resultimg,gaussi_result,5);
		//cv::erode(gaussi_result, gaussi_result, cv::Mat());
		// ����
		//cv::dilate(gaussi_result, gaussi_result, cv::Mat());
		cv::imshow("video",frame);
		cv::imshow("foreground", gaussi_result);
		cv::waitKey(10);
		
		}
	capture.release();
	return framno;
		
	}

/////////////////////////////////////////////////////////����ĺ����������ΪҪ�����������ÿһ�д�һ֡���������أ������Ϊÿһ����������𡣡���0��1
CvMat* GrayImageSegmentByKMeans2(CvMat *samples)
{

	//CvMat*samples = cvCreateMat((pImg->width)* (pImg->height),sampleno, CV_32FC1);
	//��������Ǿ���CV_32SF1����λ����ͨ��
	//cout<<samples->width<<endl<<samples->height<<endl;
	CvMat *clusters = cvCreateMat(SLOTnum,1, CV_32SC1);
	//����������ľ���
	int nClusters=2;
	//CvMat *centers = cvCreateMat(nClusters, sampleno, CV_32FC1);
	// ��ԭʼͼ��ת������������	
	
	
	//��ʼ���࣬�����Σ���ֹ���.0
	cvKMeans2(samples, nClusters,clusters, cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,100, 1.0),3,0);	
	//cout<<Mat(clusters)<<endl;
	//cout<<clusters->width<<clusters->height<<endl;

	return clusters;
}    


//����������ǵõ�10֡����ľ���֮��Ĺؼ�����֡�������Ƕ���֡�����ؽ��о��࣬���������getkeyimage���������keyframe��GrayImageSegmentByKMeans2���������Ĺ���
CvMat* GrayImageSegmentByKMeans1(CvMat *samples)
{

	//CvMat*samples = cvCreateMat((pImg->width)* (pImg->height),sampleno, CV_32FC1);
	//��������Ǿ���CV_32SF1����λ����ͨ��
	//cout<<samples->width<<endl<<samples->height<<endl;
	CvMat *clusters = cvCreateMat(SLOTnum,1, CV_32SC1);
	CvMat *sample=cvCreateMat(SLOTnum,1,CV_32FC1);
	CvMat *keyframe = cvCreateMat(2,samples->width+1, CV_32FC1);
	CvMat *cluster=cvCreateMat(SLOTnum,1,CV_32FC1);
	cvZero(keyframe);
	//����������ľ���
	int nClusters=2;
	int class0=0,class1=0;
	//CvMat *centers = cvCreateMat(nClusters, sampleno, CV_32FC1);
	// ��ԭʼͼ��ת������������	

	for(int k=0;k<samples->cols;k++)
	{	int j=0;
		class0=0;
		class1=0;
		while(j<SLOTnum)
		{
			cvmSet(sample,j,0,(cvmGet(samples,j,k)));
			j++;
		}
		cvKMeans2(sample, nClusters,clusters, cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,100, 1.0),3,0);
		
		cvConvert(clusters,cluster);
		for (int i=0;i<SLOTnum;i++){
		if (int(cvmGet(cluster,i,0))==0)
		{
			class0++;		
			double tmp = cvmGet(sample,i,0);
			//cvSet2D(samples,p,k++, s);
			cvmSet(keyframe,0,k,(tmp+cvmGet(keyframe,0,k)));
		

		}
		else
			{
			class1++;
				
			double tmp = cvmGet(sample,i,0);
		//cvSet2D(samples,p,k++, s);
			cvmSet(keyframe,1,k,(tmp+cvmGet(keyframe,1,k)));

			}
		}
		cvmSet(keyframe,0,k,cvmGet(keyframe,0,k)/float(class0));
		cvmSet(keyframe,1,k,cvmGet(keyframe,0,k)/float(class1));	
} 
	cvmSet(keyframe,0,samples->width,0.1);
	cvmSet(keyframe,1,samples->width,0.1);
	return keyframe;
}

//////////////////////////////////////////��ͨ�ú����������������֡�У��õ�����֮��Ĺؼ�֡����ͳ����Ȩ��,�������Ȩ�صĹؼ�֡���д洢//////////////////////////////
CvMat *keyframe(CvMat * clusterMat,CvMat * samples,int row,int coloum)//ͳ�ƾ���֮����ռ�ı���
{
int class0=0,class1=0;
float weight_class0;

CvMat* keyframe = cvCreateMat(2,(row)* (coloum)+1, CV_32FC1);
cvZero(keyframe);
CvMat *cluster=cvCreateMat(SLOTnum,1,CV_32FC1);
cvConvert(clusterMat,cluster);
//Mat(clusterMat).convertTo(cluster, CV_32FC1);
for (int i=0;i<SLOTnum;i++)
{

if (int(cvmGet(cluster,i,0))==0)
{
	class0++;
		
		for(int j=0;j < coloum*row; j++)
		{	
			double tmp = cvmGet(samples,i,j);
			//cvSet2D(samples,p,k++, s);
			cvmSet(keyframe,0,j,(tmp+cvmGet(keyframe,0,j)));
		}
	
}
else
{
	class1++;
	for(int j=0;j < coloum*row; j++)
	{	
		double tmp = cvmGet(samples,i,j);
		//cvSet2D(samples,p,k++, s);		
		cvmSet(keyframe,1,j,(tmp+cvmGet(keyframe,1,j)));
	}

}
}

for(int j=0;j < coloum*row; j++)
{	
	
	cvmSet(keyframe,0,j,cvmGet(keyframe,0,j)/float(class0));
	cvmSet(keyframe,1,j,cvmGet(keyframe,0,j)/float(class1));
}

weight_class0=float(class0)/float(SLOTnum);
cvmSet(keyframe,0,coloum*row,weight_class0/(SampleNo/SLOTnum));///(SampleNo/SLOTnum)//
cvmSet(keyframe,1,coloum*row,(1-weight_class0)/(SampleNo/SLOTnum));///(SampleNo/SLOTnum)//
return keyframe;

}

//////////////////////////////////////////////////////////////////�õ��������������Ĺؼ�֡����Ȩ�ؾ��󣬼�50�������еõ�10���ؼ�֡����Ҫ����keyframe��GrayImageSegmentByKMeans2����
////ͬʱҲ���Ե���GrayImageSegmentByKMeans1��������ɶ����ؼ��ľ��࣬����2֡�ؼ�֡/////////////////////////
CvMat* getkeyimage( string pathfile,int row,int coloum)
{
VideoCapture capture=ReadFile(pathfile);

int frameToStart=0;
int frameToEnd=50;
int totalFrame=capture.get(CV_CAP_PROP_FRAME_COUNT);
Mat frame,grayMat;
CvMat * keymat=cvCreateMat(2*SampleNo/SLOTnum,(row)* (coloum)+1, CV_32FC1);
int currentframe=frameToStart;
CvMat*samples = cvCreateMat(SLOTnum,(row)* (coloum), CV_32FC1);
int p = 0;
CvMat *clusterMat;
CvMat *resultMat;
int line=0;
while(currentframe<frameToEnd)  
{  
	int k=0;
	//��ȡ��һ֡  
	if(!capture.read(frame))  
	{  
		cout<<"��ȡ��Ƶʧ��"<<endl;  
		
	} 
	cv::cvtColor(frame, grayMat, CV_BGR2GRAY);
	grayMat.convertTo(grayMat, CV_32FC1);
	CvMat gray=grayMat;
	CvScalar s;
	for(int i = 0; i < row; i++)
	{
		for(int j=0;j < coloum; j++)
		{	
			s.val[0] = cvmGet(&gray,i,j);
			//cvSet2D(samples,p,k++, s);
			cvmSet(samples,p,k++,s.val[0]);
		}
	}
	currentframe++;
	p++;
	if (currentframe%SLOTnum==0)
	{	p=0;
		
		clusterMat=GrayImageSegmentByKMeans2(samples);
		resultMat=keyframe(clusterMat,samples,row,coloum);//�õ��ؼ�֡�������
		//resultMat=GrayImageSegmentByKMeans1(samples);
		for(int i=0;i<row*coloum+1;i++)
		{
			double tmp0 = cvmGet(resultMat,0,i);
			double tmp1 = cvmGet(resultMat,1,i);
			cvmSet(keymat,line,i,tmp0);
			cvmSet(keymat,line+1,i,tmp1);
		}
				
		line=line+2;
	}
	
}  
capture.release();
return keymat;

}

//////////////////////////////////////////////////////////����ÿ�ζ����ݹؼ�֡������λ����ÿ����һ�Σ�����һ�Σ��������������Ļ�����ʱ��/////////////////////////////////
CvMat * MedianNo_1(CvMat *keyframe)
{

	Mat frame,framemat;
	CvMat *mdianmat=cvCreateMat(SLOTnum-1,keyframe->width-1, CV_32FC1);
	int i=0;
	while(i<SLOTnum-1)
	{	for(int j=0;j<(keyframe->width-1);j++)
	{
		double temp=abs(cvmGet(keyframe,i,j)-cvmGet(keyframe,i+1,j));
		cvmSet(mdianmat,i,j,temp);
	}
	i++;
	}

	//CvMat *medianResult=cvCreateMat(rows*coloums,SampleNo, CV_32FC1);
	Mat medianResult;
	sortIdx(Mat(mdianmat),medianResult,CV_SORT_EVERY_COLUMN+CV_SORT_DESCENDING);

	CvMat *result=cvCreateMat(1,keyframe->width-1, CV_32FC1);

	CvMat median=medianResult;
	CvMat *median_float=cvCreateMat(SLOTnum-1,keyframe->width-1, CV_32FC1);
	cvConvert(&median,median_float);

	for (int i=0;i<keyframe->width-1;i++)
	{
		double temp1 =cvmGet(median_float,SLOTnum/2-1,i);		

		cvmSet(result,0,i,temp1);

	}
	return result;

}
////////////////////////////////////����λ����ÿ�����ص���λ����һ�д洢,ǰ50����������λ��/////////////////////////////
CvMat * MedianNo(int framstart,int frameend,VideoCapture capture,int rows,int coloums)
{
Mat frame,framemat;
Mat previousframe,previousmat;
Mat resultmat,medianResult;
int p=0;
capture.read(previousframe);
cv::cvtColor(previousframe, previousmat, CV_BGR2GRAY);
previousmat.convertTo(previousmat, CV_8U);
CvMat *mdianmat=cvCreateMat(rows*coloums,SampleNo-1, CV_32FC1);
while(framstart<frameend-1)
{	int k=0;
	capture.read(frame);
	cv::cvtColor(frame, framemat, CV_BGR2GRAY);
	framemat.convertTo(framemat, CV_8U);
	absdiff(framemat,previousmat,resultmat);
	resultmat.convertTo(resultmat, CV_32FC1);	
	
	CvMat result=resultmat;
	for(int i = 0; i < rows; i++)
	{
		for(int j=0;j < coloums; j++)
		{	
			double temp = cvmGet(&result,i,j);			
			cvmSet(mdianmat,k++,p,temp);
		}
	}
	p++;
	framstart++;
	
}

//CvMat *medianResult=cvCreateMat(rows*coloums,SampleNo, CV_32FC1);
sortIdx(Mat(mdianmat),medianResult,CV_SORT_EVERY_ROW+CV_SORT_DESCENDING);

CvMat *result1=cvCreateMat(1,rows*coloums, CV_32FC1);

CvMat median=medianResult;

CvMat *cluster=cvCreateMat(rows*coloums,SampleNo-1, CV_32FC1);
cvConvert(&median,cluster);

for (int i=0;i<rows*coloums;i++)
{
	double temp1 =cvmGet(cluster,i,SampleNo/2-1);	
	
	cvmSet(result1,0,i,temp1);

}
return result1;

}

//////////������ܶȹ��Ʊ��ʽ�ĺ������������õ��Ľ����һ�д洢////////////////////////////////
CvMat * calculate_KDE(Mat frame,CvMat *keyframe,CvMat * medianNumber)
{
CvMat frammat=frame;
double coe;//����ָ���ϵ�ϵ��
double weight;//����ָ��ǰ��ı���ϵ��
double kderesult;//���õ���ÿһ��ĺ��ܶ�ֵ
double h;//���ڴ�С
double count=0;//��������յ�ĳһ�����صĸ���ֵ
CvMat *frameline=cvCreateMat(1, frame.rows*frame.cols,CV_32FC1);//�Ѷ�ά֡ת��Ϊһά����
CvMat *kde_result=cvCreateMat(1, frame.rows*frame.cols,CV_32FC1);
int k=0;
for(int i = 0; i < frame.rows; i++)
{
	for(int j=0;j < frame.cols; j++)
	{	
		double temp = cvmGet(&frammat,i,j);			
		cvmSet(frameline,0,k++,temp);
	}
}
//���е����ݣ�ȫ����ת��Ϊһά�ģ���һ�����ݴ���һ��֡��Ϣ 

for(int j=0;j<frame.rows *frame.cols;j++)
{	count=0;
	for(int i=0;i<2*SampleNo/SLOTnum;i++)
	{	double h=(cvmGet(medianNumber,0,j)/0.962);
		coe=abs(cvmGet(frameline,0,j)-cvmGet(keyframe,i,j))*abs(cvmGet(frameline,0,j)-cvmGet(keyframe,i,j));
		coe=-0.5*coe/(h*h);
		weight=(cvmGet(keyframe,i,frame.rows*frame.cols))/sqrt(2*PI*h*h);
		kderesult=exp(coe)*weight;
		count=count+kderesult;
	}
	
	cvmSet(kde_result,0,j,count);
}

return kde_result;
}

//////���������㷨��ÿ���һ֡�����������������Ƚ������ԭ��/////////////////////////
CvMat * Updatebackground_kde(CvMat * backgroundresult,CvMat *keyframe)
{

	for (int i=0;i<backgroundresult->width;i++)
	{
		double temp=cvmGet(backgroundresult,0,i);

		if(temp>threshold2)
		{
			cvmSet(keyframe,0,i,temp);

		}
	}

	CvMat *temp=cvCreateMat(1,keyframe->width,CV_32FC1);
	for (int i=0;i<keyframe->width;i++)
	{
		cvmSet(temp,0,i,cvmGet(keyframe,0,i));

		for (int j=0;j<SLOTnum-1;j++)
		{
			cvmSet(keyframe,j,i,cvmGet(keyframe,j+1,i));
		}

		cvmSet(keyframe,SLOTnum-1,i,cvmGet(keyframe,0,i));
	}

	return keyframe;

}


/////ʼ��ֻ���µ�0�е�����
CvMat * Static_Updatebackground_kde(CvMat * backgroundresult,CvMat *keyframe)
{

	for (int i=0;i<backgroundresult->width;i++)
	{
		double temp=cvmGet(backgroundresult,0,i);

		if(temp>threshold2)
		{
			cvmSet(keyframe,9,i,temp);

		}
	}
	
	return keyframe;

}

///////////////////////////////////////���������㷨��ͻ����£�ÿ�õ�10�������󣬾���õ�2���ؼ�֡�������滻���滻��0����1�е�����//////////
CvMat *keyfrUpdatebackground_cluster(CvMat *frame_10,CvMat *keymat,int rows,int coloums)
{

CvMat *clusterMat;
CvMat *resultMat;
clusterMat=GrayImageSegmentByKMeans2(frame_10);
resultMat=keyframe(clusterMat,frame_10,rows,coloums);//�õ��ؼ�֡�������
//resultMat=GrayImageSegmentByKMeans1(frame_10);
int c_1=0;
int c_2=1;
for(int i=0;i<rows*coloums;i++)
{
	double tmp0 = cvmGet(resultMat,0,i);
	double tmp1 = cvmGet(resultMat,1,i);		
	cvmSet(keymat,c_1,i,tmp0);
	cvmSet(keymat,c_2,i,tmp1);
}
double tmp2 = cvmGet(resultMat,0,rows*coloums);
double tmp3 = cvmGet(resultMat,1,rows*coloums);		
cvmSet(keymat,c_1,rows*coloums,tmp2/3);
cvmSet(keymat,c_2,rows*coloums,tmp3/3);
return keymat;

}


////ȥ����������Ѱ��������������������������С������Ϊ��ɫ�������ȥ�������õ������Ƚϴ�ģ�ȥ��С������///////////////
CvMat * removenoise(CvMat frame)
{
	CvMemStorage* storage = cvCreateMemStorage(0);
	//Mat frame_src;
	Mat frame_src(frame.rows,frame.cols,CV_8U);
	Mat(&frame).convertTo(frame_src,CV_8U);
	CvContourScanner scanner = NULL;     
	//IplImage* img_Clone=cvCloneImage(Mat(&frame))
	IplImage  img_Clone= IplImage(frame_src); 
	scanner = cvStartFindContours(&img_Clone,storage,sizeof(CvContour));     
	//��ʼ����������     
	CvRect rect;
	CvSeq * contour =NULL;
	uchar *pp;
	while (contour=cvFindNextContour(scanner))     
	{     
		double tmparea = fabs(cvContourArea(contour));     
		rect = cvBoundingRect(contour,0);        
		if (tmparea <10)     
		{     
						//����ͨ������ĵ�Ϊ��ɫʱ�����������С���ð�ɫ�������     
			pp=(uchar*)(img_Clone.imageData + img_Clone.widthStep*(rect.y+rect.height/2)+rect.x+rect.width/2);     
			if (pp[0]==255)     
			{     
				for(int y = rect.y;y<rect.y+rect.height;y++)     
				{     
					for(int x =rect.x;x<rect.x+rect.width;x++)     
					{     
						pp=(uchar*)(img_Clone.imageData + img_Clone.widthStep*y+x);     

						if (pp[0]==0)     
						{     
							*(img_Clone.imageData + img_Clone.widthStep*y+x)=0;     
						}     
					}     
				}     
			}     

		}     
	} 
	CvMat *mat = cvCreateMat( frame.rows, frame.cols, CV_32FC1);
	cvConvert( &img_Clone, mat );
	return mat;
}


/*��Ӱ��⣬�����Ҫ��ת��ΪHSV�ռ䣬Ȼ����ݿռ�����ȷ������н��м��㣬�����Ǻ��ܶȽ������һ�д洢���Լ�ԭʼ��֡����ԭʼ֡���пռ�ת�����õ�����Ҫ�����Ӧ������꣬��
��Ӧ���֡�и�������λ��ֵ��Ϊ0
*/
CvMat * detect_hide(Mat currentFrame,CvMat* pFrImg)
{//	cout<<pFrImg->width<<endl;
	IplImage *HSVImg1 = cvCreateImage(cvSize(currentFrame.cols,currentFrame.rows), IPL_DEPTH_32F, 3);
	IplImage *HSVImg=cvCreateImage(cvSize(currentFrame.cols,currentFrame.rows),IPL_DEPTH_32F,3);
	//cout<<currentFrame.channels();
	//cout<<currentFrame.depth();
	//CvMat curr_frame=currentFrame;
	IplImage curr_frame=IplImage(currentFrame);
	//cout<<curr_frame.type<<endl;
	cvConvertScale(&curr_frame,HSVImg1,1.0/255);
	cvCvtColor(HSVImg1, HSVImg, CV_RGB2HSV);
	//cout<<HSVImg->imageData[0]<<endl;
	float total_y_v = 0.0;
	//   int y = 0;
	for ( int y = 0; y < HSVImg->height; y++) {
		float total_v = 0.0;

		for (int x = 0; x < HSVImg->width; x++) {
			int n = HSVImg->width * y + x;
			int v = HSVImg->imageData[n * 3 + 2];
			//cout<<v<<endl;
			total_v +=(v+256)%256;
		}
		total_y_v += (float)total_v / HSVImg->width;
	}
	float avg_v = total_y_v / HSVImg->height;//���ƽ������
	//cout<<avg_v<<endl;
	for (int y = 0; y < HSVImg->height; y++) 
	{
		for (int x = 0; x < HSVImg->width; x++) 
		{
			int n = HSVImg->width * y + x;
			if (cvmGet(pFrImg,y,x)!= 0) 
			{//	cout<<n<<endl;
				//int h = HSVImg->imageData[n * 3];
				//int s = HSVImg->imageData[n * 3 + 1];
				int v = HSVImg->imageData[n * 3 + 2];
				//cout<<h<<endl/*;*/
				if (((v+256)%256)<avg_v/3) 
				{
					cvmSet(pFrImg,y,x,0);
				}
			}
		}
	}
	cvReleaseImage(&HSVImg);
	return pFrImg;
}

////��ʾ���õ��Ľ�����ԻҶ�ͼ����ʽ�����������ȥ���������ĵ��ã���Ӱ���ĵ��ã�ƽ���˲�����ֵ�˲�����ʴ������//////////////////////
int display(CvMat framemat,CvMat *result,Mat frame_time,Mat current_frame)
{
//��result�Ľ����ɶ�ά��,������������ʾ��
CvMat *precision_result=cvCreateMat(framemat.rows,framemat.cols,CV_32FC1);
CvMat *detect_result=cvCreateMat(1,framemat.rows*framemat.cols,CV_32FC1);
cvZero(precision_result);
Mat resultimg;
int row=0;
int coloum=0;

for(int i=0;i<framemat.rows*framemat.cols;i++)
{
if(i%framemat.cols==0 &&i!=0)
	{
	row++;
	coloum=0;
	}
	cvmSet(precision_result,row,coloum,cvmGet(result,0,i));
	coloum++;
}

//CvMat * frame_shade_result=detect_shade(framemat,frame_time,precision_result);

CvMat frame=framemat;

//cout<<Mat(precision_result)<<endl;
for(int i=0;i<framemat.rows;i++)
	for(int j=0;j<framemat.cols;j++)
	{
		if(cvmGet(precision_result,i,j)>threshold2)
		{
			cvmSet(&frame,i,j,0);
		}
		else
			cvmSet(&frame,i,j,255);

	}
	
	//detect_result=detect_hide(current_frame,&frame);
	//CvMat * frame_noise=removenoise(frame);
	//cvSmooth(&frame,&frame, CV_GAUSSIAN, 3, 0, 0);
	Mat(&frame).convertTo(resultimg,CV_8U);		
	Mat gaussi_result;
	cv::medianBlur(resultimg,gaussi_result,7);

	//cv::blur(resultimg,gaussi_result,cv::Size(5,5));
	//cv::medianBlur(resultimg,gaussi_result,7);//��ֵ�˲�ȥ��

	//cv::erode(resultimg, resultimg, cv::Mat());//��̬ѧ��������ʴ������
	// ����
	//cv::dilate(resultimg, resultimg, cv::Mat());	
	cv::imshow("foreground", Mat(gaussi_result));
	cv::waitKey(10);
	return 0;

}

///�ú�������ͻ������ص�ĸ�������������֡���������Ϊ0�����صĸ���////
int cout_change(Mat frame)
{
int count=0;
frame.convertTo(frame,CV_32FC1);
CvMat framemat=frame;
for(int i=0;i<frame.rows;i++)
	for(int j=0;j<frame.cols;j++)
		if (cvmGet(&framemat,i,j)>0)
			count++;
return count;

}

///ͻ����£���10֡�����ѡ��һ֡��������ȫ�滻/////
CvMat *change_update(CvMat *keyframe,CvMat cvframe)
{	int k=0;
	int c_1=rand()%10;
	for(int i = 0; i < cvframe.rows; i++)
	{	
		for(int j=0;j < cvframe.cols; j++)
		{	
			double temp = cvmGet(&cvframe,i,j);			
			cvmSet(keyframe,c_1,k++,temp);
		}
	}

	return keyframe;

}
////������ܶȹ��Ƶ��ܺ�������50֡��ʼ���㣬�����ʾ����������/////////////////////////////////////////////////
int  KDE(CvMat *keyframe,string pathfile,int rows,int coloums,Mat back_sample)
{
	VideoCapture capture=ReadFile(pathfile);
	int totalFrame=capture.get(CV_CAP_PROP_FRAME_COUNT);		
	Mat frame,framemat,previousframe,previousframemat,frame_sub;
	Mat sub_frame;
	int frameToStart=0;
	int frameToEnd=SampleNo;

	int currentframe=frameToEnd;	
	CvMat *MedianNumber;
	CvMat *frame_10=cvCreateMat(10,rows*coloums,CV_32FC1);//�洢10֡	
	MedianNumber=MedianNo(frameToStart,frameToEnd,capture,rows,coloums);
	int frameno=0;
	capture.read(previousframe);
	cv::cvtColor(previousframe, previousframemat, CV_BGR2GRAY);
	//��ñ�����������Ȩֵ,����keyframe�У������λ�����洢��MedianNumber�У�������Ǽ������������ĸ��ʷֲ��ˡ�
	while(currentframe<totalFrame-1)
	{
	int k=0;	
	capture.read(frame);
	cv::cvtColor(frame, framemat, CV_BGR2GRAY);
	framemat.convertTo(framemat, CV_32FC1);
	CvMat cvframe=framemat;
	for(int i = 0; i < frame.rows; i++)
	{
		for(int j=0;j < frame.cols; j++)
		{	
			double temp = cvmGet(&cvframe,i,j);			
			cvmSet(frame_10,frameno,k++,temp);
		}
	}
		
	CvMat * result=calculate_KDE(framemat,keyframe,MedianNumber);		
	framemat.convertTo(frame_sub,CV_8U);
	previousframemat.convertTo(previousframemat,CV_8U);
	absdiff(frame_sub,previousframemat,sub_frame);
	int changeNO=cout_change(sub_frame);//����ͻ��ĸ���

	cv::imshow("video", frame);//��ʾԭ����ͼ��
	display(cvframe,result,back_sample,frame);	
	currentframe++;
	previousframe=frame;
	cv::cvtColor(previousframe, previousframemat, CV_BGR2GRAY);
	frameno++;
	if((currentframe-frameToEnd)%10==0 &&currentframe!=SampleNo)
		{
		//
		//keyframe=keyfrUpdatebackground_cluster(frame_10,keyframe,framemat.rows,frame.cols);//ÿ10֡����һ��
		frameno=0;
		}
	else
		//keyframe=Updatebackground_kde(result,keyframe);
		keyframe=Static_Updatebackground_kde(result,keyframe);//ÿ֡�����£�������֡ʱ������
		

	if(changeNO>rows*coloums/2)//ͻ�����
	{
	//keyframe=change_update(keyframe,cvframe);
	}

	}
	capture.release();
	return 0;

}

//////�ܿ��ƺ������о�̬�Ͷ�̬����///////////////////////////////////////////////////////////

/*
int main(int argc, char** argv)
{	
	cv::Mat back;	
	CvMat *keyframe;
	CvMat *result;
	/////////////////////////////��̬��/////////////////////////////////////////////////
	back=init_Backgound(Pathfile);//�����ʼ����
	//int frameno=backfilter(back,Pathfile);//���㱳����������ʾ��̬������Ч��
	/////////////////////////////////////////��̬����/////////////////////////////////////
	keyframe=getkeyimage(Pathfile,back.rows,back.cols);	
	KDE(keyframe,Pathfile,back.rows,back.cols,back);	
	cout<<"��Ƶ�ѷ�����"<<endl;
	return 0;
}



*/