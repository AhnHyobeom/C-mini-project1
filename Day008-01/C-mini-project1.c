// ����ó�� ����Ʈ���� Ver 0.03
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h> // �ȼ��� ������ִ� ��� ����.
#include <math.h>
#include <io.h>

/////// �Լ� ����� (�̸��� ����)
void print_menu();	void openImage();	unsigned char** malloc2D(int, int);
void displayImage();	void freeInputImage();	void freeOutputImage();
void equal_image();	void saveImage();
//ȭ����ó��
void bw_image();	void brightImage();	void reverseImage();
//������ ó��
void sizeUpImage();	void sizeDownImage();	void rotateImage();
void calculRotate(int rotate);//ȸ�� ��� �Լ�
//���� ó��
void low_high_passFilter();//blurring(�̹����� �ѿ���) && Sharpening(��� ����) 
void medianFilter();
void salt_pepper();//medianFilter�� ���� ���� �߰�
void printBufDisplayImage();//���� �߰��� �̹����� medianFilter�� ���ŵ� �̹��� ���� ���
//Morphology
void erosion();//ħ�� �˰��� ������ ������ �Ͼ�� ���� ����
void calculErosion();
void dilation();//��â �˰��� �Ͼ�� ������ ������ ���� ����
void calculDilation();
void opening();//ħ�� �� ��â (���� �Ͼ�� ��ü ����, �̹��� ����)
void closing();//��â �� ħ�� (���� ������ ��ü ����, �̹��� ����)
void calculOpening();
void calculClosing();

/////// �������� �����
unsigned char** inImage = NULL, ** outImage = NULL, ** printBufImage = NULL;
int inH = 0, inW = 0, outH = 0, outW = 0;
char fileName[200];
// ���� ȭ���
HWND  hwnd;  HDC  hdc;
int autoOpen;

/////// �����ڵ��
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
		case  'L':  case 'l':	opening();			break;
		case  'M':  case 'm':	closing();			break;
		default: "Ű�� �߸� ����...";
		}
		if (printBufImage != NULL) {
			for (int i = 0; i < outH; i++) {
				free(printBufImage[i]);
			}
			free(printBufImage);
			printBufImage = NULL;
		}
	}
	freeInputImage();
	freeOutputImage();
}
////// ���� �Լ� ���Ǻ�
char* findFileList(char* path) {
	char tmpPath[200];
	char retFname[200];
	char fnameList[1000][200];  // ���� �ִ� 1000��
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
	puts("\t ## ������ ����ó�� (Beta 2) ##");
	puts("0.���� �̸� �Է� �ޱ� 1.���� 2.���� 3.�������� ����");
	puts("A.���� B.���/��Ӱ� C.���� D.���");
	puts("E.Ȯ�� F.��� G.ȸ��(�ð�������� ȸ��)");
	puts("H.low or high pass filter I.median filter J.erosion");
	puts("K.dilation L.opening M.closing");
}
void openImage() {
	// �����̸��� �Է� �ޱ�.
	if (autoOpen == 0) {
		strcpy(fileName, "C:\\images\\RAW\\");
		char tmpName[100];
		printf("\n ������ ���ϸ�--> ");
		scanf("%s", tmpName);
		strcat(fileName, tmpName);
		strcat(fileName, ".raw");
	}
	else {
		strcpy(fileName, findFileList("c:\\images\\RAW\\"));
	}
	// ������ ����, ������ ũ�⸦ �˾Ƴ���.
	FILE* rfp;
	rfp = fopen(fileName, "rb");
	if (rfp == NULL) {
		MessageBox(hwnd, L"���� ���� �� ����?", L"���â", NULL);
		return;
	}
	fseek(rfp, 0L, SEEK_END);
	long fsize = ftell(rfp); // �� : 262144
	fclose(rfp); rfp = fopen(fileName, "rb");
	// ������ �۾��� ���� ������ ����
	freeInputImage();
	// �߿�! �Է��̹����� ����, �� �˾Ƴ���
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
	printf("������ ���ϸ�--> ");
	scanf("%s", tmpFname);
	strcat(sfname, tmpFname);
	strcat(sfname, ".raw");
	FILE* wfp = fopen(sfname, "wb");
	for (int i = 0; i < outH; i++) {
		fwrite(outImage[i], sizeof(unsigned char), outW, wfp);
	}

	fclose(wfp);
	printf("%s ���� �����.", sfname);
}

////// ����ó�� �Լ� ���Ǻ�
void equal_image() {  // ���� �̹��� ���
	if (inImage == NULL) {
		return;
	}
	// ������ �۾��� ���� ������ ����
	freeOutputImage();
	// �߿�! ����̹����� ����, ���� ����  --> �˰��� ����
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	// *** ��¥ ����ó�� �˰����� ���� ***
	for (int i = 0; i < inH; i++) {
		for (int j = 0; j < inW; j++) {
			outImage[i][j] = inImage[i][j];
		}
	}
	displayImage();
}

void bw_image() {  // ��� �˰���
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

void brightImage() {//��� ���� �˰���
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	int bright;
	printf("��� �Է� (���� �Է½� ��Ӱ�): ");
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

void reverseImage() {//���� �˰���
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

void sizeUpImage() {//Ȯ�� �˰���
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	int mul;
	printf("Ȯ�� ��� �Է� : ");
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
void sizeDownImage() {//��� �˰���
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	int div;
	printf("��� ��� �Է�(2�� ���� ������ 2, 4, 8 ...) : ");
	scanf("%d", &div);
	outH = inH / div;
	outW = inW / div;
	outImage = malloc2D(outH, outW);
	int sum;
	for (int i = 0; i < outH; i++) {//��հ����� ���
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
void rotateImage() {//ȸ�� �˰���
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	int degree;
	printf("ȸ�� ���� �Է� (�ð�������� ȸ��) : ");
	scanf("%d", &degree);
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	calculRotate(degree);//ȸ�� ���
	displayImage();
}

void calculRotate(int degree) {//ȸ�� ��� �˰���
	int center_w = inW / 2;//�߽��� ���
	int center_h = inH / 2;
	int new_w;
	int new_h;
	double pi = 3.141592;
	// -degree �ݽð� ���� ȸ��
	// degree �ð� ���� ȸ��
	double seta = pi / (180.0 / degree);
	//ȸ�� �˰���
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
	//ȸ�� ������ �˰��� (hole ä���)
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
	printf("���� ���� (1:Low pass, 2:High pass) : ");
	choice = getche();
	double lowMask[3][3] = {//low ����ũ ����
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1} };
	double highMask[3][3] = {//high ����ũ ����
		{-1, -1, -1},
		{-1, 8, -1},
		{-1, -1, -1} };
	double sum;
	for (int i = 1; i < inH - 1; i++) {//������ ó������ ����
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

void medianFilter() {//������ ���� �˰��� ������ �̹��� ������ �������� ����
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	int amount = 10;//���� ���� ����
	//���� x ���� x (amout / 100)
	int noiseCount = (int)(inH * inW * (amount / (float)100));
	salt_pepper(noiseCount);//���� �����߰�
	unsigned char medianSort[9];//������ ���� 1���� �迭
	//inImage ���� �ƴ� ����� ���� noiseImage�� ���� �����´�.
	for (int i = 1; i < inH - 1; i++) {//������ ó������ ����
		for (int j = 1; j < inW - 1; j++) {
			int temp = 0;//�ӽú���(�迭 �ε��� ��)
			for (int k = 0; k < 3; k++) {
				for (int m = 0; m < 3; m++) {
					medianSort[temp++] = printBufImage[i - 1 + k][j - 1 + m];
				}
			}
			for (int k = 0; k < 9; k++) {// ���� ����
				for (int m = 0; m < 9 - 1; m++) {
					if (medianSort[m] > medianSort[m + 1])
					{
						temp = medianSort[m];
						medianSort[m] = medianSort[m + 1];
						medianSort[m + 1] = temp;
					}
				}
			}
			outImage[i][j] = medianSort[4];//�߰� ������ ���
		}
	}
	printBufDisplayImage();
}

void salt_pepper(int noiseCount) {//���� ���� �߰�
	srand(time(NULL));
	int salt_or_pepper;
	int row, col;
	printBufImage = malloc2D(inH, inW);

	for (int i = 0; i < outH; i++) {
		for (int j = 0; j < outW; j++) {
			printBufImage[i][j] = inImage[i][j];
		}
	}
	//���� �߰�
	for (int i = 0; i < noiseCount; i++) {
		row = rand() % inH;
		col = rand() % inW;
		// �����ϰ� 0 �Ǵ� 255, 0�̸� ����, 255�� �ұ�
		salt_or_pepper = (rand() % 2) * 255;
		printBufImage[row][col] = salt_or_pepper;
	}
}

void printBufDisplayImage() {
	system("cls");
	unsigned char px;
	for (int i = 0; i < outH; i++) {
		for (int j = 0; j < outW; j++) {
			px = printBufImage[i][j];//���� �̹��� ���
			SetPixel(hdc, j + 80, i + 220, RGB(px, px, px));
			px = outImage[i][j];//���� ���� �̹��� ���
			SetPixel(hdc, j + 160 + inH, i + 220, RGB(px, px, px));
		}
	}
}

void erosion() {//ħ�� �˰��� (2��ȭ�� �������� ������)
	//������ ������ �Ͼ�� ���� ����
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	calculErosion();
	displayImage();
}

void calculErosion() {
	int erosionMask[5][5] = { //����ũ ����
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1} };
	unsigned char isErosion;
	for (int i = 2; i < inH - 2; i++) {//������ ó������ ����
		for (int j = 2; j < inW - 2; j++) {
			isErosion = 255;
			for (int k = 0; k < 5; k++) {
				for (int m = 0; m < 5; m++) {
					if (inImage[i - 2 + k][j - 2 + m] == 0) {//�̹����� 0�̶��
						isErosion = 0;//ħ��
						outImage[i][j] = isErosion;
						break;
					}
				}
				if (isErosion == 0) {
					break;
				}
			}
			if (isErosion == 255) {//����ũ�� ��� ����ߴٸ�
				outImage[i][j] = isErosion;
			}
		}
	}
}

void dilation() {//��â �˰��� (2��ȭ�� �������� ������)
	//�Ͼ�� ������ ������ ���� ����
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	calculDilation();
	displayImage();
}

void calculDilation() {
	int dilationMask[5][5] = { //����ũ ����
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0} };
	int isDilation;
	for (int i = 2; i < inH - 2; i++) {//������ ó������ ����
		for (int j = 2; j < inW - 2; j++) {
			isDilation = 0;
			for (int k = 0; k < 5; k++) {
				for (int m = 0; m < 5; m++) {
					if (inImage[i - 2 + k][j - 2 + m] == 255) {//�̹����� 255���
						isDilation = 255;//��â
						outImage[i][j] = isDilation;
						break;
					}
				}
				if (isDilation == 255) {
					break;
				}
			}
			if (isDilation == 0) {//����ũ�� ��� ����ߴٸ�
				outImage[i][j] = isDilation;
			}
		}
	}
}

void opening() {//ħ�� �� ��â
	//2���� �̹��� �� ���� �̹����� opening �������� ħ�� �̹���
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	printBufImage = malloc2D(outH, outW);
	calculErosion();
	calculOpening();
	printBufDisplayImage();
}

void calculOpening() { //��â ���� outImage -> printBufImage
	//�̹��� ������ ���� ����ũ�� ���ϰ� ����
	int dilationMask[3][3] = { //����ũ ����
		{0, 1, 0},
		{1, 1, 1},
		{0, 1, 0} };
	int isDilation;
	for (int i = 1; i < inH - 1; i++) {//������ ó������ ����
		for (int j = 1; j < inW - 1; j++) {
			isDilation = 0;
			for (int k = 0; k < 3; k++) {
				for (int m = 0; m < 3; m++) {
					if (dilationMask[k][m] == 1) {//mask ���� 1�̰�
						if (outImage[i - 1 + k][j - 1 + m] == 255) {//�̹����� 255���
							isDilation = 255;//��â
							printBufImage[i][j] = isDilation;
							break;
						}
					}
				}
				if (isDilation == 255) {
					break;
				}
			}
			if (isDilation == 0) {//����ũ�� ��� ����ߴٸ�
				printBufImage[i][j] = isDilation;
			}
		}
	}
}

void closing() {//��â �� ħ��
	//2���� �̹��� �� ���� �̹����� closing �������� ��â �̹���
	if (inImage == NULL) {
		return;
	}
	freeOutputImage();
	outH = inH;
	outW = inW;
	outImage = malloc2D(outH, outW);
	printBufImage = malloc2D(outH, outW);
	calculDilation();
	calculClosing();
	printBufDisplayImage();
}

void calculClosing() {//ħ�� ���� outImage ->  printBufImage
	//�̹��� ������ ���� ����ũ�� ���ϰ� ����

	int erosionMask[3][3] = { //����ũ ����
		{0, 1, 0},
		{1, 1, 1},
		{0, 1, 0} };
	unsigned char isErosion;
	for (int i = 1; i < inH - 1; i++) {//������ ó������ ����
		for (int j = 1; j < inW - 1; j++) {
			isErosion = 255;
			for (int k = 0; k < 3; k++) {
				for (int m = 0; m < 3; m++) {
					if (erosionMask[k][m] == 1) {
						if (outImage[i - 1 + k][j - 1 + m] == 0) {//�̹����� 0�̶��
							isErosion = 0;//ħ��
							printBufImage[i][j] = isErosion;
							break;
						}
					}
				}
				if (isErosion == 0) {
					break;
				}
			}
			if (isErosion == 255) {//����ũ�� ��� ����ߴٸ�
				printBufImage[i][j] = isErosion;
			}
		}
	}
}
