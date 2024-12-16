#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
#include<algorithm>
using namespace std;

int createsplitfile(string inputfile, int sizechuck);
void mergefile(string outputfilename, int sizechunk, int numberfile);

string dongdau;

struct Book {
	string ID;
	string data;
	friend bool operator <(Book a, Book b) {
		if (a.ID == "")return 0; // Neu book id la rong "" thi do la cuoi cung cua 1 file split
		if (b.ID == "")return 1; // va no la book ID max
		return a.ID < b.ID; // Xet xem ID min cua 2 book
	}
	Book() {
		ID =data= "";
	}
	Book(string tmp) {
		data = tmp; // Du lieu cua 1 book
		stringstream ss(tmp);
		getline(ss, ID, ','); // Tim ID cua book truoc dau phay dau tien
	}
};

int createsplitfile(string inputfile, int sizechuck) {
	int numberfile = 0; // Dem xem bao nhieu file dc tach ra voi size_chunke
	const char* inputfilename =inputfile.c_str();
	fstream fin;
	fin.open(inputfilename, ios_base::in | ios_base::binary);
	vector<Book>str;
	// Luu so quyen sach trong 1 chunk
	getline(fin, dongdau);
	// Tach dong dau truong du lieu cua file
	while (1) {
		str.clear(); // Reset lai vector book
		if (!fin.eof()) { // Kiem tra xem co phai cuoi file
			int i = 0;
			for (; i < sizechuck; i++) {
				if (!fin.eof()) {
					string tmp;
					getline(fin, tmp); // Lay du lieu
					Book a(tmp); // Tao 1 book dua tren du lieu da nhan
					str.push_back(a); // Luu du lieu cua 1 book vao vector
				}
				else break;	// du so luong book trong size chunk or file da doc xong
			}
			sort(str.begin(), str.end()); // Sort chunk book
			string s = "temp" + to_string(numberfile)+".csv"; // Ten file chunk
			// Mo va ghi du lieu vao file output
			fstream fout;
			fout.open(s, ios_base::out | ios_base::binary);
			for(int j=0;j<i;j++){
				fout << str[j].data << endl;
			}
			
			fout.close(); // Dong file lai
			numberfile++; // Dem file
		}
		else break; // Neu het du lieu thi break
	}
	fin.close(); // Dong file input
	return numberfile; // Tra so luong file da split ve
}

// Tim ID nho nhat trong vector book
int fineminbook(vector<Book>tmp) {
	Book min = tmp[0];
	int index = 0;
	for (int i = 0; i < tmp.size(); i++) {
		if (tmp[i] < min) {
			min = tmp[i];
			index = i;
		}
	}
	return index; // Tra ve vi tri cua min_book
}

void mergefile(string outputfilename, int sizechunk, int numberfile) {
	// Mo file output
	fstream out;
	out.open(outputfilename, ios_base::out | ios_base::binary);
	out << dongdau << endl;
	// Ghi dong dau id,... ( tieu de cac truong du lieu) vao file moi
	fstream* readsplitfile = new fstream[numberfile];
	// Tao mang gom number_file fstream de luu
	for (int i = 0; i < numberfile; i++) {
		// Tao ten file va mo file de doc voi ten file split tuong ung
		string s = "temp" + to_string(i) + ".csv";
		readsplitfile[i].open(s, ios_base::in | ios_base::binary);
	}
	vector<Book>minbook; // Tao vector minbook
	for (int i = 0; i < numberfile; i++) {
		// Ung voi moi file split book lay ra 1 phan tu book
		// de xet nho nhat de them vao file output
		string tmp;
		getline(readsplitfile[i], tmp);
		Book a(tmp);
		minbook.push_back(a);
	}
	int count = 0; // Dem xem so file split da hoan thanh sau viec lay du lieu
	// Lap den khi du so file split so voi so file da hoan thanh
	while (count != numberfile) {
		int indexminbook = fineminbook(minbook); // Tim vi tri cua min_id book trong vector minbook
		out << minbook[indexminbook].data << endl; // Ghi du lieu min book cua phan tu thu min_id book trong vector minbook
		// Neu file split input con du lieu
		if (!readsplitfile[indexminbook].eof()) {
			string tmp1;
			// Lay du lieu cua book tiep theo tu file split thu indexminbook
			getline(readsplitfile[indexminbook], tmp1);
			Book b(tmp1);
			minbook[indexminbook] = b; // Doi con tro toi phan tiep theo cua book de them vao vector minbook
		}
		else {
			// Neu du lieu cua splitfilebook het
			// thi the vao vi tri cua file da doc het = 1 book co ID max ( ID rong "")
			count++; // Cong 1 vao so file da hoan thanh
			Book c; // Tao 1 book voi ID rong "" ( ID book max)
			minbook[indexminbook] = c;
		}
	}
	for (int i = 0; i < numberfile; i++) {
		readsplitfile[i].close(); // Dong luong doc split file
	}
	out.close(); // Dong file output
	for (int i = 0; i < numberfile; i++) {
		string namefilechunk = "temp" + to_string(i) + ".csv"; 
		remove(namefilechunk.c_str()); // Xoa tat ca cac file split
	}
}

// Ham de goi 2 ham thuc thi tren
void sortbigfile(string namefileinput, string namefileoutput, int sizechunk) {
	int numberfile = createsplitfile(namefileinput, sizechunk); // Dung 1 bien numberfile de luu so file da split
	mergefile(namefileoutput, sizechunk, numberfile); // Gop cac file da split vao chung 1 file output Sorted va xoa cac file split di
}

int main() {
	string inputfilename = "Books_rating.csv"; // File goc can Sort
	string outputfilename = "Books_rating_sorted.csv"; // File da Sort thanh cong
	int chunksize = 50000; // So book toi da trong 1 split file
	sortbigfile(inputfilename, outputfilename, chunksize); // Goi ham thuc thi
}