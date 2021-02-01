// 영상처리 소프트웨어 Ver 0.03
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h> // 픽셀을 출력해주는 기능 포함.
#include <math.h>
#include <io.h>

/////// 함수 선언부 (이름만 나열)
void print_menu();
void openImage();
unsigned char** malloc2D(int, int);
void displayImage();
void freeInputImage();
void freeOutputImage();
void equal_image();
void saveImage();
//화소점처리
void bw_image();
void brightImage();
void reverseImage();
//기하학 처리
void sizeUpImage();
void sizeDownImage();
void rotateImage();
void calculRotate(int rotate);//회전 계산 함수
//영역 처리
void low_high_passFilter();
void medianFilter();
void salt_pepper();//medianFilter를 위해 잡음 추가
void noiseDisplayImage();//잡음 추가된 이미지와 medianFilter로 제거된 이미지 동시 출력
//Morphology
void erosion();//침식 (경계선 처리를 하지 않아 경계선이 생김)
void dilation();//팽창

/////// 전역변수 선언부
unsigned char** inImage = NULL, ** outImage = NULL, ** noiseImage = NULL;
int inH = 0, inW = 0, outH = 0, outW = 0;
char fileName[200];
// 윈도 화면용
HWND  hwnd;  HDC  hdc;
int autoOpen;

/////// 메인코드부
void main() {
	char select = 0;
	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(hwnd);
	while (select != '2') {
		print_menu();
		select = getche();
		switch (select) {
		case  '0':	autoOpen = 0;	openImage();	break;
		case  '1':	saveImage();					break;
		case  '3':	autoOpen = 1;	openImage();	break;
		case  'A':  case 'a':	equal_image();		break;
		case  'B':  case 'b':	brightImage();		break;
		case  'C':  case 'c':	reverseImage();		break;
		case  'D':  case 'd':	bw_image();			break;
		case  'E':  case 'e':	sizeUpImage();		break;
		case  'F':  case 'f':	sizeDownImage();	break;
		case  'G':  case 'g':	rotateImage();		break;
		case  'H':  case 'h':	low_high_passFilter();	break;
		case  'I':  case 'i':	medianFilter();		break;
		case  'J':  case 'j':	erosion();			break;
		case  'K':  case 'k':	dilation();			break;
		default: "키를 잘못 누름...";
		}
		if (noiseImage != NULL) {
			for (int i = 0; i < outH; i++) {
				free(noiseImage[i]);
			}
			free(noiseImage);
			noiseImage = NULL;
		}
	}
	freeInputImage();
	freeOutputImage();
}
////// 공통 함수 정의부
char* findFileList(char* path) {
	char tmpPath[200];
	char retFname[200];
	char fnameList[1000][200];  // 파일 최대 1000개
	int  fCount = 0;
	struct _finddata_t fd;
	long handle;
	int result = 1;
	strcpy(tmpPath, path);
	handle = _findfirst(strcat(tmpPath, "*.*"), &fd);
	while (result != -1)
	{
		strcpy(fnameList[fCount++], fd.name);
		result = _findnext(handle, &fd);
	}
	_findclose(handle);
	srand(time(NULL));
	int pos = rand() % fCount;
	strcpy(retFname, path);
	strcat(retFname, fnameList[pos]);
	return retFname;
}

void print_menu() {
	puts("\t ## 디지털 영상처리 (Beta 2) ##");
	puts("0.파일 이름 입력 받기 1.저장 2.종료 3.랜덤으로 열기");
	puts("A.원본 B.밝게/어둡게 C.반전 D.흑백");
	puts("E.확대 F.축소 G.회전(시계방향으로 회전)");
	puts("H.low or high pass filter I.median filter J.erosion");
	puts("K.dilation");
}
void openImage() {
	// 파일이름을 입력 받기.
	if (autoOpen == 0) {
		strcpy(fileName, "C:\\images\\RAW\\");
		char tmpName[100];
		printf("\n 오픈할 파일명--> ");
		scanf("%s", tmpName);
		strcat(fileName, tmpName);
		strcat(fileName, ".raw");
	}
	else {
		strcpy(fileName, findFileList("c:\\images\\RAW\\"));
	}
	// 파일을 열고, 파일의 크기를 알아내기.
	FILE* rfp;
	rfp = fopen(fileName, "rb");
	if (rfp == NULL) {
		MessageBox(hwnd, L"오늘 점심 뭐 먹지?", L"출력창", NULL);
		return;
	}
	fseek(rfp, 0L, SEEK_END);
	long fsize = ftell(rfp); // 예 : 262144
	fclose(rfp); rfp = fopen(fileName, "rb");
	// 기존에 작업한 것이 있으면 해제
	freeInputImage();
	// 중요! 입력이미지의 높이, 폭 알아내기
	inH = inW = (int)sqrt(fsize);
	inImage = malloc2D(inH, inW);
	for (int i = 0; i < inH; i++) {
		fread(inImage[i], sizeof(unsigned char), inW, rfp);
	}
	fclose(rfp);

	equal_image();
}

unsigned char** malloc2D(int h, int w) {
	unsigned char** retrunPointer;
	retrunPointer = (unsigned char**)malloc(h * sizeof(unsigned char*));
	for (int i = 0; i < h; i++) {
		retrunPointer[i] = (unsigned char*)malloc(w * sizeof(unsigned char));
	}
	return retrunPointer;
}

void displayImage() {
	system("cls");
	unsigned char px;
	for (int i = 0; i < outH; i++) {
		for (int j = 0; j < outW; j++) {
			px = outImage[i][j];
			SetPixel(hdc, j + 80, i + 200, RGB(px, px, px));
		}
	}
}

void freeInputImage() {
	if (inImage == NULL) {
		return;
	}
	for (int i = 0; i < inH; i++) {
		free(inImage[i]);
	}
	free(inImage);
	inImage = NULL;
}

void freeOutputImage() {
	if (outImage == NULL) {
		return;
	}
	for (int i = 0; i < outH; i++) {
		free(outImage[i]);
	}
	free(outImage);
	outImage = NULL;
}

void saveImage() {
	char sfname[200] = "C:\\images\\RAW\\";
	char tmpFname[50];
	printf("저장할 파일명--> ");
	scanf("%s", tmpFname);
	strcat(sfname, tmpFname);
	strcat(sfname, ".raw");
	FILE* wfp = fopen(sfname, "wb");
	for (int i = 0; i < outH; i++) {
		fwrite(outImage[i], sizeof(unsigned char), outW, wfp);
	}

	fclose(wfp);
	printf("%s 으로 저장됨.", sfname);
}

////// 영상처리 함수 정의부
void equal_image() {  // 원본 이미지 출력
	if (inImage == NULL) {
		return;
	}
	// 기존에 작업한 것이 있으면 해제
	freeOutputImage();
	// 중요! 출력이미지의 높이, 폭을 결정  --> 알고리즘에 영향
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	// *** 진짜 영상처리 알고리즘을 구현 ***
	for (int i = 0; i < inH; i++) {
		for (int j = 0; j < inW; j++) {
			outImage[i][j] = inImage[i][j];
		}
	}
	displayImage();
}

void bw_image() {  // 흑백 알고리즘
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] > 127) {
				outImage[i][k] = 255;
			}
			else {
				outImage[i][k] = 0;
			}
		}
	}
	displayImage();
}

void brightImage() {//밝기 조절 알고리즘
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	int bright;
	printf("밝기 입력 (음수 입력시 어둡게): ");
	scanf("%d", &bright);
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] + bright > 255) {
				outImage[i][k] = 255;
			}
			else if (inImage[i][k] + bright < 0) {
				outImage[i][k] = 0;
			}
			else {
				outImage[i][k] = inImage[i][k] + bright;
			}
		}
	}
	displayImage();
}

void reverseImage() {//반전 알고리즘
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	for (int i = 0; i < inH; i++) {
		for (int j = 0; j < inW; j++) {
			outImage[i][j] = 255 - inImage[i][j];
		}
	}
	displayImage();
}

void sizeUpImage() {//확대 알고리즘
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	int mul;
	printf("확대 배수 입력 : ");
	scanf("%d", &mul);
	outH = inH * mul;
	outW = inW * mul;
	outImage = malloc2D(outH, outW);
	for (int i = 0; i < outH; i++) {
		for (int j = 0; j < outW; j++) {
			outImage[i][j] = inImage[i / mul][j / mul];
		}
	}
	displayImage();
}
void sizeDownImage() {//축소 알고리즘
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	int div;
	printf("축소 배수 입력(2의 제곱 단위로 2, 4, 8 ...) : ");
	scanf("%d", &div);
	outH = inH / div;
	outW = inW / div;
	outImage = malloc2D(outH, outW);
	int sum;
	for (int i = 0; i < outH; i++) {//평균값으로 계산
		for (int j = 0; j < outW; j++) {
			sum = 0;
			for (int k = 0; k < div; k++) {
				for (int m = 0; m < div; m++) {
					sum = sum + inImage[i * div + k][j * div + m];
				}
			}
			outImage[i][j] = (unsigned char)(sum / (float)(div * div));
		}
	}
	displayImage();
}
void rotateImage() {//회전 알고리즘
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	int degree;
	printf("회전 각도 입력 (시계방향으로 회전) : ");
	scanf("%d", &degree);
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	calculRotate(degree);//회전 계산
	displayImage();
}

void calculRotate(int degree) {//회전 계산 알고리즘
	int center_w = inW / 2;//중심축 계산
	int center_h = inH / 2;
	int new_w;
	int new_h;
	double pi = 3.141592;
	double seta = pi / (180.0 / degree);
	//회전 알고리즘
	for (int i = 0; i < inH; i++) {
		for (int j = 0; j < inW; j++) {
			new_w = (i - center_h) * sin(seta) + (j - center_w) * cos(seta) + center_w;
			new_h = (i - center_h) * cos(seta) - (j - center_w) * sin(seta) + center_h;
			if (new_w < 0)		continue;
			if (new_w >= inW)	continue;
			if (new_h < 0)		continue;
			if (new_h >= inH)	continue;
			outImage[i][j] = inImage[new_h][new_w];
		}
	}
	//회전 보간법 알고리즘 (hole 채우기)
	int left_pixval = 0;
	int right_pixval = 0;
	for (int i = 0; i < outH; i++) {
		for (int j = 0; j < outW; j++) {
			if (j == 0) {
				right_pixval = outImage[i][j + 1];
				left_pixval = right_pixval;
			}
			else if (j == outW - 1) {
				left_pixval = outImage[i][j - 1];
				right_pixval = left_pixval;
			}
			else {
				left_pixval = outImage[i][j - 1];
				right_pixval = outImage[i][j + 1];
			}
			if (outImage[i][j] == 0 && left_pixval != 0 && right_pixval != 0) {
				outImage[i][j] = (left_pixval + right_pixval) / 2;
			}
		}
	}
}

void low_high_passFilter() {
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	char choice = '1';
	printf("필터 선택 (1:Low pass, 2:High pass) : ");
	choice = getche();
	double lowMask[3][3] = {//low 마스크 설정
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1} };
	double highMask[3][3] = {//high 마스크 설정
		{-1, -1, -1},
		{-1, 8, -1},
		{-1, -1, -1} };
	double sum;
	for (int i = 1; i < inH - 1; i++) {//엣지는 처리하지 않음
		for (int j = 1; j < inW - 1; j++) {
			sum = 0;
			for (int k = 0; k < 3; k++) {
				for (int m = 0; m < 3; m++) {
					if (choice == '1') {
						sum = sum + inImage[i - 1 + k][j - 1 + m] * lowMask[k][m];
					}
					else {
						sum = sum + inImage[i - 1 + k][j - 1 + m] * highMask[k][m];
					}
				}
			}
			if (choice == '1') {
				sum = sum / 9;
			}
			if (sum > 255) {
				sum = 255;
			}
			else if (sum < 0) {
				sum = 0;
			}
			outImage[i][j] = (unsigned char)sum;
		}
	}
	displayImage();
}

void medianFilter() {//노이즈 제거 알고리즘
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	int amount = 10;//잡음 개수 조절
	//가로 x 세로 x (amout / 100)
	int noiseCount = (int)(inH * inW * (amount / (float)100));
	salt_pepper(noiseCount);//영상에 잡음추가
	unsigned char medianSort[9];//정렬을 위한 1차원 배열
	//inImage 값이 아닌 노이즈가 생긴 noiseImage의 값을 가져온다.
	for (int i = 1; i < inH - 1; i++) {//엣지는 처리하지 않음
		for (int j = 1; j < inW - 1; j++) {
			int temp = 0;//임시변수(배열 인덱스 값)
			for (int k = 0; k < 3; k++) {
				for (int m = 0; m < 3; m++) {
					medianSort[temp++] = noiseImage[i - 1 + k][j - 1 + m];
				}
			}
			for (int k = 0; k < 9; k++) {// 버블 정렬
				for (int m = 0; m < 9 - 1; m++) {
					if (medianSort[m] > medianSort[m + 1])
					{
						temp = medianSort[m];
						medianSort[m] = medianSort[m + 1];
						medianSort[m + 1] = temp;
					}
				}
			}
			outImage[i][j] = medianSort[4];//중간 값으로 출력
		}
	}
	noiseDisplayImage();
}

void salt_pepper(int noiseCount) {//영상에 잡음 추가
	srand(time(NULL));
	int salt_or_pepper;
	int row, col;
	noiseImage = malloc2D(inH, inW);

	for (int i = 0; i < outH; i++) {
		for (int j = 0; j < outW; j++) {
			noiseImage[i][j] = inImage[i][j];
		}
	}
	//잡음 추가
	for (int i = 0; i < noiseCount; i++) {
		row = rand() % inH;
		col = rand() % inW;
		// 랜덤하게 0 또는 255, 0이면 후추, 255면 소금
		salt_or_pepper = (rand() % 2) * 255;
		noiseImage[row][col] = salt_or_pepper;
	}
}

void noiseDisplayImage() {
	system("cls");
	unsigned char px;
	for (int i = 0; i < outH; i++) {
		for (int j = 0; j < outW; j++) {
			px = noiseImage[i][j];//잡음 이미지 출력
			SetPixel(hdc, j + 80, i + 220, RGB(px, px, px));
			px = outImage[i][j];//잡음 제거 이미지 출력
			SetPixel(hdc, j + 160 + inH, i + 220, RGB(px, px, px));
		}
	}
}

void erosion() {//침식 알고리즘 (2진화된 영상으로 봐야 뚜렷함)
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	int erosionMask[3][3] = { //마스크 설정
		{0, 1, 0},
		{1, 1, 1},
		{0, 1, 0} };
	int isErosion;
	for (int i = 1; i < inH - 1; i++) {//엣지는 처리하지 않음
		for (int j = 1; j < inW - 1; j++) {
			isErosion = 255;
			for (int k = 0; k < 3; k++) {
				for (int m = 0; m < 3; m++) {
					if (erosionMask[k][m] == 1) {//mask 값이 1이고
						if (inImage[i - 1 + k][j - 1 + m] == 0) {//이미지가 0이라면
							isErosion = 0;//침식
							outImage[i][j] = isErosion;
							break;
						}
					}
				}
				if (isErosion == 0) {
					break;
				}
			}
		}
	}
	displayImage();
}

void dilation() {//팽창 알고리즘 (2진화된 영상으로 봐야 뚜렷함)
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	int dilationMask[3][3] = { //마스크 설정
		{0, 1, 0},
		{1, 1, 1},
		{0, 1, 0} };
	int isDilation;
	for (int i = 1; i < inH - 1; i++) {//엣지는 처리하지 않음
		for (int j = 1; j < inW - 1; j++) {
			isDilation = 0;
			for (int k = 0; k < 3; k++) {
				for (int m = 0; m < 3; m++) {
					if (dilationMask[k][m] == 1) {//mask 값이 1이고
						if (inImage[i - 1 + k][j - 1 + m] == 255) {//이미지가 255라면
							isDilation = 255;//팽창
							outImage[i][j] = isDilation;
							break;
						}
					}
				}
				if (isDilation == 255) {
					break;
				}
			}
		}
	}
	displayImage();
}
