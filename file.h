

#define ER_OPEN_FILE 	-3


typedef struct {
	uint8_t status;
	uint8_t chs1[3];
	uint8_t type;
	uint8_t chs2[3];
	uint32_t start_sector;
	uint32_t num_sectors;
} PartitionEntry;

typedef struct __attribute__((packed)){
	uint8_t bootstrap[446];
	PartitionEntry partition_entries[4];
	uint16_t magic;
} Mbr;
