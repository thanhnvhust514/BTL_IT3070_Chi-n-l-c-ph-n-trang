
#include<iostream>
#include <iomanip>
#include "TLB.h"
#include "PCB.h"
using namespace std;

bool checkNo(long long int);
void convert_Logical_Addr_to_Physical_Addr(PageTable &pgTable, int log_add, int numPg, int numFr, int choice);
void convert_Logical_Addr_to_Physical_Addr_useTLB(PageTable &pgTable, int log_add, int numPg, int numFr, int choice, TLB & tlb);
void convert_Logical_Addr_to_Physical_Addr_2Level(PageTable &pgTable, int log_add, int numPg, int numFr, int choice, int outer_size);

int main()
{
  
  long long int log_memSize, phy_memSize,pgNum,n;
  int choice;
  // input:
  // Nhập số page trong bộ nhớ logic
  cout<<"Enter the number of pages in logical memory space \n";
  cin>>log_memSize;
  if(!checkNo(log_memSize)) // size < 0 => lỗi
  {
    cout<<"Error: the number of pages entered is invalid\n";
    exit(1);
  }
  // Nhập số frame trong bộ nhớ vật lý
  cout<<"Enter the number of frames in physical memory space \n";
  cin>>phy_memSize;
  if(!checkNo(phy_memSize)) //size<0 = > lỗi
  {
    cout<<"Error: the number of frames entered is invalid\n";
    exit(1);
  }
//  do{
//    cout << "Choose:\n";
//    cout << "0. One level page table\n";
//    cout << "1. Two level page table\n";
//    cout << "2. Exit\n";

 //   char yourChoice;
//    cin >> yourChoice;
//    if(yourChoice == 2)break;
///    if(yourChoice == 1){
//      
//    }
//
//  }while(1);
  PageTable pgTable(log_memSize,phy_memSize); // Khởi tạo bảng trang 
  TLB tlb(5); // Khởi tạo TLB;
  do {
 //   system("cls");
//    pgTable.displayPageFrames(); // test
  //  pgTable.displayPageTable(); // test
    //return 0; // test
    cout<<"Choose a page replacement algorithm for paging simulation (1 - 5):\n"; // lựa chọn chiến lược thay thế trang
    cout<<"1. Least Recently Used\n";
    cout<<"2. First in First Out\n";
    cout<<"3. Least frequently used\n";
    cout<<"4. Not recently used\n";
    cout<<"5. Second Chances\n";
    cout<<"6. Exit\n";
     
    string input1;
    cin>>input1;
    if(input1 != "1" && input1 !="2"&& input1 !="3"&& input1 !="4"&& input1 !="5"&& input1 !="6" ){
        break;
    }
    else {
      choice = int(input1[0]) - 48;
    }
    if(choice == 6) {
      cout << "END";
      return 0;
    }
    pgTable.setPageFaults(0); // khởi tạo chưa có lỗi trang pgFault = 0;
    // nhập số page muốn thể hiện
    cout << "Choose option:\n";
    cout << "1. Convert logicalAddress to physicalAddress\n";
    cout << "2. Convert logicalAddress to physicalAddress (using TLB)\n";
    cout << "3. Convert logicalAddress to physicalAddress (2level_page_table)\n";
    cout << "4. Page replacement simulation\n";
    cout << "5. Page Table\n";
    cout << "6. Back\n";

    int choice_1;
    string input2;
    cin >> input2;
       if(input2 != "1" && input2 !="2"&& input2 !="3"&& input2 !="4"&& input2 !="5"&& input2 !="6" ){
        choice = -1;
        continue;
    }
    else {
      choice_1 = int(input2[0]) - 48;
    }
    
    if(choice_1 >= 6){
      continue;
    }

    if(choice_1 == 5){
      pgTable.displayPageTable();
    }

    if(choice_1 == 4){
          cout << "Clear memory ? Y/N\n";
          cout << "Your answer: ";
          char s = 'Y';
          cin >> s;
          if(s == 'Y');
          cout<<"Enter the number of pages to be refered\n";
          cin>>n;
          long long int pgArr[n];
          // nhập dãy id các trang sẽ được truy cập, các id phải thỏa mãn tử  0 -> (số trang logic)
          cout<<"Enter all the page numbers to be refered each should belong to this range (0 - "<<log_memSize-1<<"):\n";
          for(long long int i=0;i<n;i++)
          {
            cin>>pgArr[i];
            pgNum=pgArr[i];
            if(pgNum<0 || pgNum>log_memSize-1)       
            {
              cout<<"Error! Enter the page number in range (0 - "<<log_memSize-1<<")\n";
              i=i-1;
            }
            else
            {
              pgTable.accessPg(pgNum, choice);
              tlb.update(pgNum, pgTable.getFrame(pgNum));
            }
          } 
        cout<<"The page faults occured are : "<<pgTable.getPageFaults()<<"\n";
    }

    if(choice_1 == 1){
        cout << "Enter the logical address:\n";
        int log_add;
        cin >> std:: hex >> log_add;
        convert_Logical_Addr_to_Physical_Addr(pgTable, log_add,log_memSize, phy_memSize, choice);
    }
    
    if(choice_1 == 2){
      cout << "Enter the logical address:\n";
      int log_add;
      cin >> std:: hex >> log_add;
      convert_Logical_Addr_to_Physical_Addr_useTLB(pgTable, log_add,log_memSize, phy_memSize, choice, tlb);
    }
    if(choice_1 == 3){
      cout << "Enter size of page folder\n";
      int outer_size;
      cin >> outer_size;
      cout << "Enter the logical address:\n";
      int log_add;
      cin >> std:: hex >> log_add;
      convert_Logical_Addr_to_Physical_Addr_2Level(pgTable, log_add,log_memSize, phy_memSize, choice, outer_size);
    }

}while(choice!=6);
cout << "END\n";
  return 0;
}


bool checkNo(long long int size)
{
  if(size<0)
    return false;
    else
      return true;
}

void convert_Logical_Addr_to_Physical_Addr(PageTable &pgTable, int log_add, int numPg, int numFr, int choice){
    int page_size = 4096; // 2^12 B
    int offset_bits = 12;
    int PAGE_NUMBER_MASK = 0xFFFFF000;
    int OFFSET_MASK = 0x00000FFF;
    // Get PageNum from logical adress
    unsigned int pageNumber = (log_add & PAGE_NUMBER_MASK) >> offset_bits;
    if(pageNumber > numPg){
      cout << "This logical Address is invalid\n";
      return;
    }
    // Get offset
    unsigned int offset = log_add & OFFSET_MASK; 
    pgTable.accessPg(pageNumber,choice);
    int frameNumber = pgTable.getFrame(pageNumber);
    unsigned int physicalAddress = (frameNumber<< offset_bits) | offset;
    std::cout << "Physical Address: 0x" << std::hex << physicalAddress << std::endl;
}

void convert_Logical_Addr_to_Physical_Addr_useTLB(PageTable &pgTable, int log_add, int numPg, int numFr, int choice, TLB & tlb){
    int page_size = 4096; // 2^12 B
    int offset_bits = 12;
    unsigned int PAGE_NUMBER_MASK = 0xFFFFF000;
    unsigned int  OFFSET_MASK = 0x00000FFF;
    // Get PageNum from logical adress
    unsigned int pageNumber = (log_add & PAGE_NUMBER_MASK) >> offset_bits;
    if(pageNumber > numPg){
      cout << "This logical Address is invalid\n";
      return;
    }
    // Get offset
    unsigned int offset = log_add & OFFSET_MASK; 
    //
    int physicalFrame = tlb.lookup(pageNumber);

    // Nếu TLB miss, tìm trong bảng trang và cập nhật TLB
    if (physicalFrame == -1) {
        std::cout << "TLB miss" << std::endl; 
        pgTable.accessPg(pageNumber, choice);
        physicalFrame = pgTable.getFrame(pageNumber);
        tlb.update(pageNumber, physicalFrame);
    } else {
        std::cout << "TLB hit" << std::endl;
    }
    unsigned int physicalAddress = (physicalFrame<< offset_bits) | offset;
    std::cout << "Physical Address: 0x" << std::hex << physicalAddress << std::endl;
}

void convert_Logical_Addr_to_Physical_Addr_2Level(PageTable &pgTable, int log_add, int numPg, int numFr, int choice, int outer_size){
    unsigned int offset_bits = 12;
    unsigned int LEVEL1_BITS = 10;
    unsigned int LEVEL2_BITS = 10;
    unsigned int LEVEL1_MASK = 0xFFC00000; // Bits 22-31
    unsigned int LEVEL2_MASK = 0x003FF000; // Bits 12-21
    unsigned int OFFSET_MASK = 0x00000FFF; // Bits 0-11

    // Extract indices and offset
    unsigned int level1Index = (log_add & LEVEL1_MASK) >> (offset_bits + LEVEL2_BITS);
    unsigned int level2Index = (log_add & LEVEL2_MASK) >> offset_bits;
    unsigned int offset = log_add & OFFSET_MASK;

    int pageNumber = level1Index * outer_size + level2Index;
    if(pageNumber > numPg){
      cout << "This logical Address is invalid\n";
      return;
    }
    // Get offset
 //   unsigned int offset = log_add & OFFSET_MASK; 
    pgTable.accessPg(pageNumber,choice);
    int frameNumber = pgTable.getFrame(pageNumber);
    unsigned int physicalAddress = (frameNumber<< offset_bits) | offset;
    std::cout << "Physical Address: 0x" << std::hex << physicalAddress << std::endl;
}