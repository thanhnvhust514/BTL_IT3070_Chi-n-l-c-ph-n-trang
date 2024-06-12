#ifndef _PAGE_TABLE_
#define _PAGE_TABLE_

#include"TLB.h"
#include<iostream>
#include<vector>
#include<iomanip>
using namespace std;

class PageTable
{
 long long int ts;              
 long long int pgFaults;         
  
 long long int clockHand;
 long long int pgReplace(int choice);  
 class PageTableEntry
 {
   public:
     long long int timeStamp;    
     long long int frameNo;      
     int checkFrame;    
     bool referenced; // Cờ tham chiếu
     bool modified;   // Cờ bẩn
     PageTableEntry()
     {
       timeStamp=0;
       frameNo=-1;
       checkFrame=0;
       referenced = false;
       modified = false;
     }
 };
 vector<long long int > frMap; // vector chua thong tin frame
 vector<PageTableEntry> pgMap;   // một vector chứa các đối tượng là thống tin trang logigc
 vector<long long int> emptyFrames;   // các frame trống
 public:
   PageTable(long long int log_memSize,long long int phy_memSize); // Phương thức khởi tạo
   bool containsRef(long long int pgNum);
    // Kiểm tra xem trang có hiện hiện trong bộ nhớ vật lý không ( trả về check frame)
   void accessPg( long int pgNum,int choice);
   // truy cập khung tương ứng với trang 
   void addPg(long long int pgNum,int choice);
   // Thêm một trang vào bộ nhớ vật lý
   void displayPageTable();
   // in ra bảng trang
   long long int getPageFaults();
   // trả về tổng số lỗi trang
   void setPageFaults(int pf);
   // chuyển lỗi trang thành tham số
   void resetReferencedBits(int pf); 
   // đặt lại cờ R
   void displayPageFrames();
   /// in ra thong tin khung trang trong bộ nhớ
  long long int getFrame(long long int pgNum);
   vector<long long int> fifo_framePgs; // vector lưu thứ tự các trang dc sử dụng
   vector<pair<long long int,long long int> > lfu_framePgs;   
};

// Phương thức khởi tạo bảng trang(số trang logic, số trang vật lý)
PageTable::PageTable(long long int log_memSize,long long int phy_memSize)
{
 emptyFrames.clear();
 pgMap.clear();
 fifo_framePgs.clear();
 frMap.clear();
 // khởi tạo các frame trống là tất cả các frame trong bộ nhớ
 for(long long int i=phy_memSize-1;i>=0;i--)
 {
    emptyFrames.push_back(i);
 }
 pgMap.resize(log_memSize);
 frMap.resize(phy_memSize);
 for(int i = 0; i < phy_memSize; i++) frMap[i] = -1;
 ts=1;
 clockHand = 0;                             
}

// hàm kiểm tra xem đã trang logic đã đc đưa vào bộ nhớ chưa
bool PageTable::containsRef(long long int pgNum)
{
 return pgMap[pgNum].checkFrame; // trả
}
// truy cập frame ứng với page  
void PageTable::accessPg( long int pgNum,int choice)
{
  if(!containsRef(pgNum))
  {
    addPg(pgNum, choice);
  }
  long long int frame = pgMap[pgNum].frameNo;
  for(long long int i = 0; i < lfu_framePgs.size(); i++)      
  {
    if(lfu_framePgs[i].first == pgNum)
    {
      long long int freq = lfu_framePgs[i].second;
      lfu_framePgs[i].second = freq + 1; // tần suất sử dụng page tăng lên 1
     }
  }
  pgMap[pgNum].referenced = true;
  pgMap[pgNum].timeStamp = ts; // gán lại thời gian truy cập trang
  displayPageFrames();
  ++ts;  
  cout<<"The page "<<pgNum<<" was found at frame "<<frame<<" in physical memory\n\n";
}

// thêm một trang vào bộ nhớ vật lý
void PageTable::addPg(long long int pgNum,int choice)
{

 if(!emptyFrames.empty()) // nếu còn frame trống
 { 
   long long int frame=emptyFrames.back(); // chọn lấy khung ở cuối
   frMap[frame] = pgNum;
   emptyFrames.pop_back();
   pgMap[pgNum].frameNo=frame; // lưu page ở khung dó
   pgMap[pgNum].checkFrame=1; // đánh  dấu page đã đc lưu trong khung
   pgMap[pgNum].timeStamp=ts; // cập nhật dấu thời gian của page  = ts;
   //cout<<"time "<<ts<<"\n";
   pgMap[pgNum].referenced = true;
   cout<<"Page Fault: Adding "<<pgNum<<" at frame "<<frame<<"\n";
  
 }

 else // nếu ko còn frame trống
 {
   long long int frame=pgReplace(choice); // dùng chiến lược thay thế trang trả về khung trang sẽ được làm trống 
   pgMap[pgNum].frameNo=frame; // lưu page ở frame số ...
   pgMap[pgNum].checkFrame=1; // đánh dấu page đã đc lưu trong frame
   pgMap[pgNum].timeStamp=ts; // cập nhập dấu thời gian của page = ts
   frMap[frame] = pgNum;
   pgMap[pgNum].referenced = true;
   //cout<<"time "<<ts<<"\n";
   cout<<"Page Fault: Adding "<<pgNum<<" at frame "<<frame<<"\n";
 }
 ++pgFaults; // số lượng lỗi trang tăng lên 1
 fifo_framePgs.push_back(pgNum);
 lfu_framePgs.push_back(make_pair(pgNum,1));

}
// chiến lược thay thế trang trả về 1 khung trang trống
long long int PageTable::pgReplace(int choice)
{

 if(choice==1)         //Least Recently Used - lần sử dụng các xa nhất
 {
   long long int min=ts; 
   long long int replacePg;
   // chọn ra page có dấu trang bé nhất ( lần sử dụng xa nhất)
   for(long long int j=0;j<pgMap.size();j++)
   {
     if(pgMap[j].checkFrame==1)    
     {
       if(pgMap[j].timeStamp<min)   
       {
         min=pgMap[j].timeStamp;   
         replacePg=j;             
       }
     }
   }
   long long int frame=pgMap[replacePg].frameNo; // lấy frame đã lưu page này
   pgMap[replacePg].checkFrame=0; // đánh dấu page đã bị đưa ra khỏi bộ nhớ
   pgMap[replacePg].frameNo=-1; // không có frame nào lưu page này nữa 
   return frame;                  
 }

 else if(choice==2)    //First in First Out
 {
   long long int replacePg=fifo_framePgs.at(0);       // trang ở đầu là trang sử dụng đầu tiên
   fifo_framePgs.erase(fifo_framePgs.begin()+0);      // xóa trang đó ra khỏi hàng đợi fifo      
   pgMap[replacePg].checkFrame=0;                     // đánh dấu ko có frame nào  lưu page đó nữa
   long long int frame=pgMap[replacePg].frameNo;
   pgMap[replacePg].frameNo=-1;
   return frame;                  
 }
 else if(choice==3)    //Least frequently used
 {
   long long int min=0;
   // tìm trang có tần xuất sử dụng ít nhất
   for(long long int i=0;i<lfu_framePgs.size();i++)    
   {
     if(lfu_framePgs[i].second<lfu_framePgs[min].second)
       min=i;
   }
   long long int replacePg=lfu_framePgs[min].first; // tìm ra trang sẽ được thay thế
   lfu_framePgs.erase(lfu_framePgs.begin()+min);  // xóa tần suất sử dụng trang đó ra khỏi lfu
   pgMap[replacePg].checkFrame=0;                 // đánh dấu ko có frame lưu page nữa
   long long int frame=pgMap[replacePg].frameNo;  // lấy ra khugng đã từng lưu page này
   pgMap[replacePg].frameNo=-1;
   return frame;                  // trả về khung trống vừa lấy ra
 }
 else if(choice == 4) // NRU
 {
    int indexToReplace = -1;
    for (int category = 0; category <= 3; ++category) {
        for (size_t i = 0; i < pgMap.size(); ++i) {
            if (pgMap[i].checkFrame) {
                bool R = pgMap[i].referenced;
                bool M = pgMap[i].modified;
                int pageCategory = (R << 1) | M;
                if (pageCategory == category) {
                    indexToReplace = i;
                    break;
                }
            }
        }
        if (indexToReplace != -1) {
            break;
        }
    }
    if (indexToReplace != -1) {
        emptyFrames.push_back(pgMap[indexToReplace].frameNo);
        pgMap[indexToReplace].checkFrame = 0;
        pgMap[indexToReplace].frameNo = -1;
        pgMap[indexToReplace].referenced = false;
        pgMap[indexToReplace].modified = false;
    }
    return indexToReplace;
 }
 else if (choice == 5)  /// Second change
 {
  int indexToReplace = -1;  
      while (true)
      {
        if (pgMap[clockHand].checkFrame)
        {
          if (!pgMap[clockHand].referenced)
          {
            indexToReplace = clockHand;
            break;
          } 
          else 
          {
            pgMap[clockHand].referenced = false;
          }
        }
          clockHand = (clockHand + 1) % pgMap.size();
      }
      if (indexToReplace != -1) {
        emptyFrames.push_back(pgMap[indexToReplace].frameNo);
        pgMap[indexToReplace].checkFrame = 0;
        pgMap[indexToReplace].frameNo = -1;
        pgMap[indexToReplace].referenced = false;
        pgMap[indexToReplace].modified = false;
    }
    return indexToReplace;

 }
 return 1;
}
//display the contents of the page table
void PageTable::displayPageTable()
{
 // printing top border
 cout << left << setfill('-') << setw(4) << "+" << setw(4) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(9) << setfill('-') << "-" << right << "+" << setfill(' ')<< setw(21) << "+" << setfill('-') << setw(15) << "-" <<setw(1) <<"+"<< endl;
 // printing table record
 cout << right << setw(1) << "|" << "PageNo" << setfill(' ') << setw(2) << "|" << left << setw(15) << "FrameNo" << setw(1) << "|" << setw(15)  << "Mark" << setw(1) << "|" << setw(5) << "TimeStamp" << setw(1) << "|" << setw(20) << setfill(' ') << " " << "|" << "Empty Frames" << setw(4) << right << "|" << endl;
 // printing bottom border
 cout << left << setfill('-') << setw(1) << "+" << setw(23) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(9) << "-" << setw(1) << "+" << setw(20) << setfill(' ') << " " << setw(1) << setfill(' ') << "+" << setfill('-') << setw(15) << "-" << setw(1) << "+" << endl;
 for (unsigned int i = 0; i < pgMap.size(); i++)
 {
   // printing table record
   cout << "|" << setfill(' ') << setw(6) << i << setfill(' ') << setw(1) << " " << "|" << setw(15) << pgMap.at(i).frameNo
     << setw(1) << "|" << setw(15) << pgMap.at(i).checkFrame << setw(1) << "|" << setw(5)
     << pgMap.at(i).timeStamp << setfill(' ') << setw(4) << " " << setw(1) << "|";
   // conditionally print the empty frames
   if (i < emptyFrames.size()) {
     cout << left << setw(20) << setfill(' ') << " " << "|" << setw(5) << emptyFrames.at(i) << setfill(' ') << setw(10) << " " << setw(1) << "|" <<endl;
     //printing bottom border
     cout << setfill('-') << setw(7) << "+" << setw(17) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(9) << "-" << setw(1) << "+" << setw(20) << setfill(' ') << " " << setw(1) << setfill(' ') << "+" << setfill('-') << setw(15) << "-" << setw(1) << "+" <<endl;
   }
   // if there wasn't a frame printed don't forget to print the bottom border
   else {
     // printing bottom border
     cout << endl << setfill('-') << setw(7) << "+" << setw(17) << "-" << setw(1) << "+" << setw(15) << "-" << setw(1) << "+" << setw(9) << "-" << setw(1) << "+" << endl;
   }
 }
}
void PageTable::displayPageFrames()
{
  //Print Top Border 
   cout << left << setfill('-') << setw(1) << "+" << setw(9) << "-" << setw(1) << "+" << setw(9) << "-" << setw(1) << "+"<< endl;
   // Print Field "Frame" and "Page"
   cout << right<< setw(1) << "|" << "Frame.No" << setfill(' ') << setw(2) << "|" << left << setw(9) <<"Page" <<setw(1) << "|" << endl;
   // Print bottom Border;
   cout << left << setfill('-') << setw(1) << "+" << setw(9) << "-" << setw(1) << "+" << setw(9) << "-" << setw(1) << "+"<< endl;
   for(unsigned int i = 0; i < frMap.size(); i++){
        cout << "|" << setfill(' ') << setw(8) << std :: dec << i << setfill(' ') << setw(1) << " " << "|" << setw(9) << std :: dec <<frMap[i] << setw(1) << "|" <<endl;
        cout << left << setfill('-') << setw(1) << "+" << setw(9) << "-" << setw(1) << "+" << setw(9) << "-" << setw(1) << "+"<< endl;
   }

}
long long int PageTable::getFrame(long long int pgNum)
{
  return pgMap[pgNum].frameNo;
}
void PageTable::setPageFaults(int pf)
{
 pgFaults=pf;
}

long long int PageTable::getPageFaults()
{
 return pgFaults;
}

#endif
