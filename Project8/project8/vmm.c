#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_NUM 256
#define PAGE_SIZE 256
#define FRAME_NUM 128
#define FRAME_SIZE 256
#define TLB_SIZE 16

int get_frame_TLB(int page_number);
void TLB_update(int page_number, int frame_number);
void delete_TLB(int page_number, int frame_number);

char memory[FRAME_NUM * FRAME_SIZE];

FILE *backing_store;
int total = 0, page_fault = 0, tlb_hit = 0;

int page_table[PAGE_NUM], page_table_inmem[PAGE_NUM];
int frame_count[FRAME_NUM];

void swap_page_out(int frame_number) {
    int page_number = -1;
    
	for (int i = 0; i < PAGE_NUM; i++) {
		if(page_table_inmem[i] > 0 && page_table[i] == frame_number) {
			page_number = i;
			break;
		}
    }
    if (page_number == -1) {
        for (int i = 0; i < PAGE_NUM; i++) {
		
        printf("%d, %d \n", page_table_inmem[i], page_table[i]);
    }
		fprintf(stderr, "[Err] Unexpectedddd Error!\n");
        
		exit(1);
	}
    page_table_inmem[page_number] = 0;

    //TLB delete
    delete_TLB(page_number, frame_number);

    return;
}

int swap_page_in(int page_number) {
    char buf[FRAME_SIZE];
    fseek(backing_store, page_number * FRAME_SIZE, 0);
    fread(buf, sizeof(char), FRAME_SIZE, backing_store);

    int target = -1;
    for (int i = 0; i < FRAME_NUM; i++) {
        if (frame_count[i] == 0) {
            target = i;
            break;
        }
    }
    if (target == -1) {
        for (int i = 0; i < FRAME_NUM; i ++) {
            if (frame_count[i] == FRAME_NUM) {
                target = i;
                break;
            }
        }
        swap_page_out(target);
    }
    for (int i = 0; i < FRAME_SIZE; ++ i) memory[target * FRAME_SIZE + i] = buf[i];
	for (int i = 0; i < FRAME_NUM; ++ i) if (frame_count[i] > 0) frame_count[i]++;
	frame_count[target] = 1;
	return target;
}


int get_val(int frame_number, int offset) {
    int val = (int)memory[frame_number * FRAME_SIZE + offset];

    for (int i = 0; i < FRAME_NUM; ++ i) {
        if (frame_count[i] != 0 && frame_count[i] < frame_count[frame_number])
			frame_count[i]++;
    }
    frame_count[frame_number] = 1;

    return val;
}

int get_frame(int page_number) {

    // TLB
    int tlb_frame = get_frame_TLB(page_number);
    if (tlb_frame != -1) return tlb_frame;

    if (page_table_inmem[page_number] > 0) {

        // TLB
        TLB_update(page_number, page_table[page_number]);

		return page_table[page_number];
	} else {
		page_fault++;
		page_table[page_number] = swap_page_in(page_number);
		page_table_inmem[page_number] = 1;

        // TLB
        TLB_update(page_number, page_table[page_number]);
		return page_table[page_number];
	}
    return -1;
}


// TLB

int TLB_list[TLB_SIZE][2]; // 0 -> page; 1 -> frame
int TLB_count[TLB_SIZE];

int get_frame_TLB(int page_number) {
    int frame = -1, loc = -1;
    for (int i = 0; i < TLB_SIZE; ++ i)
		if (TLB_count[i] != 0 && TLB_list[i][0] == page_number) {
			frame = TLB_list[i][1];
            loc = i;
			break;
		}

    if (frame == -1) return -1; // TLB miss

    tlb_hit++;
    for (int i = 0; i < TLB_SIZE; i++)
        if (TLB_count[i] != 0 && TLB_count[i] < TLB_count[loc]) {
			TLB_count[i]++;
        }
    TLB_count[loc] = 1;

    return frame;
}

void TLB_update(int page_number, int frame_number) {
    int loc = -1;
	for (int i = 0; i < TLB_SIZE; i++)
		if(TLB_count[i] == 0) {
			loc = i;
			break;
		}
	if (loc == -1) {
		for (int i = 0; i < TLB_SIZE; i++)
			if(TLB_count[i] == TLB_SIZE) {
				loc = i;
				break;
			}
	}

    for (int i = 0; i < TLB_SIZE; i++)
		if (TLB_count[i] > 0) TLB_count[i] += 1;

	TLB_count[loc] = 1;
    TLB_list[loc][0] = page_number;
    TLB_list[loc][1] = frame_number;
    return;
}

void delete_TLB(int page_number, int frame_number) {
    int loc = -1;
	for (int i = 0; i < TLB_SIZE; ++ i)
		if(TLB_count[i] == 0) {
			loc = i;
			break;
		}
	if (loc == -1) return; // not find, no need to delete

    for (int i = 0; i < TLB_SIZE; ++ i)
		if (TLB_count[i] > TLB_count[loc]) TLB_count[i] -= 1;

	TLB_count[loc] = 0;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "[Error] Unexpected input!\n");
		return 1;
    }

    backing_store = fopen("BACKING_STORE.bin", "r");

    for (int i = 0; i < PAGE_NUM; i++) {
        page_table[i] = 0;
        page_table_inmem[i] = 0;
    }
    for (int i = 0; i < FRAME_NUM; i++) {
        frame_count[i] = 0;
    }
    for (int i = 0; i < TLB_SIZE; i++) {
        TLB_list[i][0] = 0;
        TLB_list[i][1] = 0;
        TLB_count[i] = 0;
    }

	FILE *stream_in = fopen(argv[1], "r");
	FILE *stream_out = fopen("output.txt", "w");

    int virtual_addr;
    int page_number, offset, frame_number, val;

    while(~fscanf(stream_in, "%d", &virtual_addr)) {
        total += 1;

        virtual_addr = virtual_addr & 0x0000ffff;
        page_number = (virtual_addr >> 8) & 0x000000ff;
		offset = virtual_addr & 0x000000ff;

        frame_number = get_frame(page_number);
        val = get_val(frame_number, offset);

        fprintf(stream_out, "Virtual address: %d Physical address: %d Value: %d\n", virtual_addr, (frame_number << 8) + offset, val);
    }
    fprintf(stdout, "TLB hit rate: %.2f%%      Page fault rate: %.2f%%\n", 100.0 * tlb_hit / total, 100.0 * page_fault / total);

    fclose(stream_in);
    fclose(stream_out);
    
    return 0;
}