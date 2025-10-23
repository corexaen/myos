#ifndef __FAT32_H__
#define __FAT32_H__
#include "size.h"
#include "disk.h"
#include "kernel.h"
struct FAT32_BPB {
    uint8_t  jmpBoot[3];        // ���� ���
    char     OEMName[8];        // OEM �̸�
    uint16_t BytesPerSector;    // ���� ũ��
    uint8_t  SectorsPerCluster; // Ŭ�����ʹ� ���� ��
    uint16_t ReservedSectorCount; // ���� ���� �� (FAT ���۱���)
    uint8_t  NumFATs;           // FAT ���̺� ����
    uint16_t RootEntryCount;    // FAT12/16��, FAT32�� 0
    uint16_t TotalSectors16;    // 16��Ʈ ��ü ���� ��, FAT32�� 0
    uint8_t  Media;             // �̵�� Ÿ��
    uint16_t FATSize16;         // FAT12/16��, FAT32�� 0
    uint16_t SectorsPerTrack;
    uint16_t NumberOfHeads;
    uint32_t HiddenSectors;     // ��Ƽ�� ���� LBA
    uint32_t TotalSectors32;    // FAT32 ��ü ���� ��
    uint32_t FATSize32;         // FAT32 FAT ���̺� ũ��(���� ����)
    uint16_t ExtFlags;
    uint16_t FSVersion;
    uint32_t RootCluster;       // ��Ʈ ���丮 ���� Ŭ������
    uint16_t FSInfo;
    uint16_t BackupBootSector;
    uint8_t  Reserved[12];
    uint8_t  DriveNumber;
    uint8_t  Reserved1;
    uint8_t  BootSignature;
    uint32_t VolumeID;
    char     VolumeLabel[11];
    char     FSType[8];         // "FAT32   "
} __attribute__ ((packed));

struct FAT32_DirEntry {
    char     Name[8];       // 8 ���ϸ�
	char     ext[3];        // Ȯ����
    uint8_t  Attr;           // �Ӽ� (0x10=���丮, 0x20=����)
    uint8_t  NTRes;
    uint8_t  CrtTimeTenth;
    uint16_t CrtTime;
    uint16_t CrtDate;
    uint16_t LstAccDate;
    uint16_t FstClusHI;
    uint16_t WrtTime;
    uint16_t WrtDate;
    uint16_t FstClusLO;
    uint32_t FileSize;
} __attribute__((packed));

class FAT32 {
public:
    FAT32_BPB bpb;
    Disk* disk;
    uint64_t first_lba;
    FAT32();
    ~FAT32();
    void init(uint32_t disk_id, uint32_t index, void* buffer);
	uint32_t get_file_size(const char* filename);
	void read_file(const char* filename, void* buffer, uint32_t size);
};
#endif