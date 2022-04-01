/* Shane Chiovarou and Jack Tomkiel
Project 3 main_pr1
program with FIFO page replacement */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PAGES 256
#define PAGE_SIZE 256
#define FRAMES 128
#define FRAME_SIZE 256
#define TLB_SIZE 16

typedef struct {
	int pageNum;
	int frameNum;
} tableEntry;

int mem[FRAMES][FRAME_SIZE];
tableEntry pageTable[FRAMES];
tableEntry TLB[TLB_SIZE];



FILE *logicalFP, *physicalFP, *svalFP;

int pageFaults = 0;
int TLBHits = 0;
int frameI = 0;
int pageTableI = 0;
int TLBI = 0;
int TLBhit = 0;


void addTLB(int page, int frame)
{
	if(TLBhit == 0)
	{
		if(TLBI == 0)
		{
			TLB[0].pageNum = page;
			TLB[0].frameNum = frame;
			TLBI++;
		}
		else if(TLBI < (TLB_SIZE - 1))
		{
			
			for(int i  = TLBI+1; i > 0; i--)
			{
				TLB[i] = TLB[i-1];
				
			}
			
			TLB[0].pageNum = page;
			TLB[0].frameNum = frame;
			
			TLBI++;
			
		}
		else
		{
			for(int i = TLBI; i > 0; i--)
			{
				TLB[i] = TLB[i-1];
			}
			
			TLB[0].pageNum = page;
			TLB[0].frameNum = frame;
			
		}
	}
	else //LRU TLB updates
	{
		int index;
		
		for(int i  = 0; i <= TLBI; i++)
		{
			if(TLB[i].pageNum == page)
				index = i;
		}
		
		for(int i = index; i > 0; i--)
		{
			TLB[i] = TLB[i-1];
		}
		TLB[0].pageNum = page;
		TLB[0].frameNum = frame;
	}
}

int getPage(int page){
	int frame;
	int8_t buff[256];
	
	FILE *bStore = fopen("BACKING_STORE.bin","r");
	
	fseek(bStore, page*FRAME_SIZE, SEEK_SET);
	
	fread(buff,sizeof(int8_t), FRAME_SIZE,bStore);
	
	fclose(bStore);
	
	//put the frame we just got into mem and update page table
	if(frameI == 0) // inserting the first frame is easy
	{
		for(int i  = 0; i < FRAME_SIZE; i++)
		{
			mem[frameI][i] = buff[i];
		}
		
		pageTable[pageTableI].pageNum = page;
		pageTable[pageTableI].frameNum = frameI;
		frame = frameI;
		pageTableI++;
		frameI++;
		
	}
	else if(frameI < (FRAMES - 1)) //inserting frames while there are still empty ones
	{
		//still use frameI since that is an empty frame we can use
		for(int i  = 0; i < FRAME_SIZE; i++)
		{
			mem[frameI][i] = buff[i];
		}
		
		//we need to make put this new frame at the begninning of the page table, FIFO
		for(int i  = frameI +1; i > 0; i--)
		{
			pageTable[i] = pageTable[i-1];
		}
		pageTable[0].pageNum = page;
		pageTable[0].frameNum = frameI;
		frame = frameI;
		pageTableI++;
		frameI++;
		
	}
	else //we filled all the frames, we must repalce one
	{
		frame = pageTable[frameI].frameNum;
		for(int i  = 0; i < FRAME_SIZE; i++)
		{
			mem[frame][i] = buff[i];
		}
		for(int i  = frameI; i > 0; i--)
		{
			pageTable[i] = pageTable[i-1];
		}
		pageTable[0].pageNum = page;
		pageTable[0].frameNum = frame;
		
		
	}
	return frame;
	
}

void readAddress(int addr){
	int logAddr = addr;
	uint8_t page = logAddr / PAGES;
	uint8_t offset = logAddr % PAGES;
	TLBhit = 0;
	int frame = -1;
	
	//search TLB
	for(int i = 0; i <= TLBI; i++)
	{
		//look through the TLB, set frame if we find it
		if(TLB[i].pageNum == page)
		{
			TLBHits++;
			frame = TLB[i].frameNum;
			TLBhit = 1;
			break;
		}
	}
	
	//search page table, only if TLB missed
	if(frame == -1)
	{
		for(int i = 0; i < pageTableI; i++)
		{
			//look through the page table, set frame if we find it
			if(pageTable[i].pageNum == page)
			{
				
				frame = pageTable[i].frameNum;
				
				break;
			}
		}
	}
	
	//if page table missed, page fault :(, must go into BACKING_STORE.bin and load the frame in
	if(frame == -1)
	{
		pageFaults++;
		
		//open BACKING_STORE, grab 256*page through (256*(page+1))-1
		//put it in physical mem and note where it is in page table
		frame = getPage(page);
	}
	
	//put the page we just accessed at the front of the TLB queue (FIFO)
	
	
	addTLB(page, frame);
	
	//toss all our values in the output files
	fprintf(logicalFP,"%d\n",logAddr);
	fprintf(physicalFP,"%d\n",FRAME_SIZE*frame+offset);
	fprintf(svalFP,"%d\n",mem[frame][offset]);
}



int main(int argc, char **argv)
{
	
	TLB[0].pageNum = -1;
	TLB[1].pageNum = -1;
	/* check for valid arguments */
	
	if (argc != 2)
	{
		printf("ERROR: Need addresses.txt only with call\n");
		return 1;
	}

	/* open files */
	FILE *inputFP = fopen(argv[1], "r");
	logicalFP = fopen("out1.txt", "w");
	physicalFP = fopen("out2.txt", "w");
	svalFP = fopen("out3.txt", "w");
	/* make sure input file opened */
	if (inputFP == NULL)
	{
		printf("ERROR: Could not open \"%s\"\n",argv[1]);
		return 1;
	}
	
	int numAddr = 0;
	int32_t logAddr;
	fscanf(inputFP, "%d", &logAddr);
	
	while(!feof(inputFP))
	{
		numAddr++;
		readAddress(logAddr);
		fscanf(inputFP, "%d", &logAddr);
	}	
	
	fclose(inputFP);
	fclose(logicalFP);
	fclose(physicalFP);
	fclose(svalFP);
	
	printf("A total number of %d Addresses with:\n",numAddr);
	printf("  %d Page Faults with a rate = %f\n",pageFaults,((double)pageFaults)/((double)numAddr));
	printf("  %d TLB Hits with a rate = %f\n",TLBHits,((double)TLBHits)/((double)numAddr));
	
	return 0;
}
