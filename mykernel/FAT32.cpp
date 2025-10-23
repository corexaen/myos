#include "FAT32.h"
#include "disksid.h"
#include "ahci.h"
#include "log.h"
#include "memory.h"
FAT32::FAT32() = default;
FAT32::~FAT32() = default;

void FAT32::init(uint32_t disk_id, uint32_t index, void* buffer) {
	PartitionInfo* p = find_partition(disk_id, index);
	if (!p) {
		return;
	}
	disk = (Disk*)p->disk;
    first_lba = p->first_lba;
	disk->read_bytes(first_lba * 512, &bpb, sizeof(FAT32_BPB));
	uart_print("\nBytes per sector: ");
	uart_print(bpb.BytesPerSector);
	uart_print("\nSectors per cluster: ");
	uart_print(bpb.SectorsPerCluster);
	uart_print("\nReserved sectors: ");
	uart_print(bpb.ReservedSectorCount);
	uart_print("\nNumber of FATs: ");
	uart_print(bpb.NumFATs);
	uart_print("\nFAT size (sectors): ");
	uart_print(bpb.FATSize32);
	uart_print("\nRoot cluster: ");
	uart_print(bpb.RootCluster);
	uart_print("\n");
}
uint32_t FAT32::get_file_size(const char* filename) {
    uint64_t FAT_start = (first_lba + bpb.ReservedSectorCount) * bpb.BytesPerSector;
    uint64_t FAT_size = (uint64_t)bpb.FATSize32 * bpb.BytesPerSector;

    uint32_t data_start = (first_lba
        + bpb.ReservedSectorCount
        + ((uint64_t)bpb.NumFATs * bpb.FATSize32)) * bpb.BytesPerSector;

    // 루트 디렉토리 시작 LBA
    uint32_t cluster = bpb.RootCluster;
    // 한 클러스터 크기
    uint32_t cluster_size = bpb.SectorsPerCluster * bpb.BytesPerSector;

    // 엔트리 수
    int entries = cluster_size / sizeof(FAT32_DirEntry);
    while (cluster < 0x0FFFFFF7) {
        for (int i = 0; i < entries; i++) {
            uint64_t entry_addr = (uint64_t)data_start + (cluster - 2ull) * bpb.SectorsPerCluster * bpb.BytesPerSector + i * sizeof(FAT32_DirEntry);

            FAT32_DirEntry e;
            // 엔트리 한 개씩 직접 채우기
            disk->read_bytes(entry_addr, &e, sizeof(FAT32_DirEntry));

            if (e.Name[0] == 0x00) break;    // 끝
            if (e.Name[0] == (char)0xE5) continue; // 삭제됨
            if (e.Attr == 0x0F) continue;    // LFN 무시

            char name[13] = {0,};
            int j,k;
            for (j = 0; j < 8; j++) {
				if (e.Name[j] == ' ') break;
                name[j] = e.Name[j];
            }
            name[j] = '.';
            for (k = 0; k < 3; k++) {
                if (e.ext[k] == ' ') break;
                name[k + j + 1] = e.ext[k];
            }
            name[k + j + 1] = 0;
            if(strcmp(name, filename) == 0) {
                return e.FileSize;
			}
            /*
            uart_print("Entry: ");
            uart_print(name);
            uart_print("  Size: ");
            uart_print(e.FileSize);
            uart_print("\n");
            uart_print("Attr: ");
            uart_print(e.Attr);
            uart_print("\n");
            uart_print("First cluster: ");
            uint32_t first_cluster = ((uint32_t)e.FstClusHI << 16) | e.FstClusLO;
            uart_print(first_cluster);
            uart_print("\n\n");
            */
        }
        uint64_t FAT_offset = (uint64_t)cluster * 4;
        disk->read_bytes(FAT_start + FAT_offset, &cluster, sizeof(cluster));
        cluster &= 0x0FFFFFFF;
    }
    return 0;
}

void FAT32::read_file(const char* filename, void* buffer, uint32_t size) {
    uint64_t FAT_start = (first_lba + bpb.ReservedSectorCount) * bpb.BytesPerSector;
    uint64_t FAT_size = (uint64_t)bpb.FATSize32 * bpb.BytesPerSector;
    uint32_t data_start = (first_lba
        + bpb.ReservedSectorCount
        + ((uint64_t)bpb.NumFATs * bpb.FATSize32)) * bpb.BytesPerSector;
    // 루트 디렉토리 시작 LBA
    uint32_t cluster = bpb.RootCluster;
    // 한 클러스터 크기
    uint32_t cluster_size = bpb.SectorsPerCluster * bpb.BytesPerSector;
    // 엔트리 수
    int entries = cluster_size / sizeof(FAT32_DirEntry);
    while (cluster < 0x0FFFFFF7) {
        for (int i = 0; i < entries; i++) {
            uint64_t entry_addr = (uint64_t)data_start + (cluster - 2ull) * bpb.SectorsPerCluster * bpb.BytesPerSector + i * sizeof(FAT32_DirEntry);
            FAT32_DirEntry e;
            // 엔트리 한 개씩 직접 채우기
            disk->read_bytes(entry_addr, &e, sizeof(FAT32_DirEntry));
            if (e.Name[0] == 0x00) break;    // 끝
            if (e.Name[0] == (char)0xE5) continue; // 삭제됨
            if (e.Attr == 0x0F) continue;    // LFN 무시
            char name[13] = { 0, };
            int j, k;
            for (j = 0; j < 8; j++) {
                if (e.Name[j] == ' ') break;
                name[j] = e.Name[j];
            }
            name[j] = '.';
            for (k = 0; k < 3; k++) {
				if (e.ext[k] == ' ') break;
                name[k + j + 1] = e.ext[k];
            }
            name[k + j + 1] = 0;
			uart_print("Looking at: "); uart_print(name); uart_print("\n");
			uart_print("result: "); uart_print(strcmp(name, filename)); uart_print("\n");
            if (strcmp(name, filename) == 0) {
                uint32_t first_cluster = ((uint32_t)e.FstClusHI << 16) | e.FstClusLO;
                uint32_t remaining = size;
                uint8_t* buf = (uint8_t*)buffer;
                cluster = first_cluster;
                while (cluster < 0x0FFFFFF7 && remaining > 0) {
                    uint64_t cluster_addr = (uint64_t)data_start + (cluster - 2ull) * bpb.SectorsPerCluster * bpb.BytesPerSector;
                    uint32_t to_read = (remaining < cluster_size) ? remaining : cluster_size;
                    disk->read_bytes(cluster_addr, buf, to_read);
                    buf += to_read;
                    remaining -= to_read;
                    if (remaining == 0) break;
                    uint64_t FAT_offset = (uint64_t)cluster * 4;
                    disk->read_bytes(FAT_start + FAT_offset, &cluster, sizeof(cluster));
                    cluster &= 0x0FFFFFFF;
                }
                return;
            }
        }
        uint64_t FAT_offset = (uint64_t)cluster * 4;
        disk->read_bytes(FAT_start + FAT_offset, &cluster, sizeof(cluster));
        cluster &= 0x0FFFFFFF;
    }
	uart_print("File not found: "); uart_print(filename); uart_print("\n");
	return;
}