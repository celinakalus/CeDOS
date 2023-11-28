#include <stdint.h>
#include "elf.h"
#include "core.h"

#include "file.h"
#include "sched/process.h"

#include "mm/memory.h"

#include "assert.h"

#ifdef DEBUG
#define PRINT_DBG(...) printk("[" __FILE__ "] " __VA_ARGS__)
#else
#define PRINT_DBG(...) {}
#endif

typedef struct {
    uint8_t e_ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t proghead_offset;
    uint32_t secthead_offset;
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_entry_size;
    uint16_t ph_num;
    uint16_t sh_entry_size;
    uint16_t sh_num;
    uint16_t sh_strndx;
} ELF_HEADER;

typedef struct {
    uint32_t type;
    uint32_t offset;
    VIRT_ADDR virt_addr;
    PHYS_ADDR phys_addr;
    uint32_t filesize;
    uint32_t memsize;
    uint32_t flags;
    uint32_t align;
} PROG_HEADER;

typedef enum {
    SHF_WRITE       = 0x01,
    SHF_ALLOC       = 0x02,
    SHF_EXECINSTR   = 0x04
} SH_FLAGS;

typedef struct {
    uint32_t name;
    uint32_t type;
    union {
            SH_FLAGS flags;
            uint32_t value;
        };
    VIRT_ADDR addr;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t addr_align;
    uint32_t entsize;
} SECT_HEADER;

void elf_infodump(VIRT_ADDR elf_pointer, uint32_t size) {
    ELF_HEADER *header = (ELF_HEADER*)elf_pointer;
    
    PRINT_DBG("Reading ELF file from location %p of length %i\n", elf_pointer, size);
    PRINT_DBG("\n");

    PRINT_DBG("ELF header size: %i\n", sizeof(ELF_HEADER));
    assert(sizeof(ELF_HEADER) == 52);
    PRINT_DBG("\n");

    PRINT_DBG("ELF header hexdump:\n");
    memdump(elf_pointer, 52);
    PRINT_DBG("\n");

    PRINT_DBG("ELF header magic number:\n");
    PRINT_DBG("%s\n", &(header->e_ident));
    PRINT_DBG("\n");

    PRINT_DBG("ELF header infodump:\n");
    PRINT_DBG("- machine:       %i\n", header->machine);
    PRINT_DBG("- version:       %i\n", header->version);
    PRINT_DBG("- entry:         %p\n", header->entry);
    PRINT_DBG("\n");
    PRINT_DBG("- program headers:\n");
    PRINT_DBG("  - offset:      %i\n", header->proghead_offset);
    PRINT_DBG("  - entries:     %i\n", header->ph_num);
    PRINT_DBG("  - entry size:  %i\n", header->ph_entry_size);
    PRINT_DBG("\n");
    PRINT_DBG("- section headers:\n");
    PRINT_DBG("  - offset:      %i\n", header->secthead_offset);
    PRINT_DBG("  - entries:     %i\n", header->sh_num);
    PRINT_DBG("  - entry size:  %i\n", header->sh_entry_size);
    PRINT_DBG("\n");

    PRINT_DBG("Enumerating sections:\n");

    int sh_offset = header->secthead_offset;
    SECT_HEADER *sect_headers = (SECT_HEADER*)(elf_pointer + sh_offset);
    
    int num_sections = header->sh_num;
    int section_size = header->sh_entry_size;

    SECT_HEADER sect_names_sh = sect_headers[header->sh_strndx];
    VIRT_ADDR sect_names_addr = elf_pointer + sect_names_sh.offset;

    assert(sizeof(SECT_HEADER) == section_size);

    for (int i = 0; i < num_sections; i++) {
        SECT_HEADER sh = sect_headers[i];
        
        PRINT_DBG("Section:       %s\n", (char*)(sect_names_addr + sh.name));
        PRINT_DBG("- type:        %i\n", sh.type);
        PRINT_DBG("- offset:      %i\n", sh.offset);
        PRINT_DBG("- size:        %i\n", sh.size);
        PRINT_DBG("- addr:        %i\n", sh.addr);
        PRINT_DBG("- addr_align:  %i\n", sh.addr_align);
        PRINT_DBG("\n");
    }
}

PROCESS_ID elf_exec(const char *fname, char *args) {
    crit_enter();
    PRINT_DBG("Loading ELF executable \"%s\".\n", fname);
    // TODO: needs to change when we have other file systems
    int fd = file_open(fname, 0);
    PRINT_DBG("File handle: %i\n", fd);
    
    if (fd == -1) {
        printk("Executable file not found: %s\n", fname);
        return -1;
    }
    //int size = file_read(fd, elf_addr, 0xFFFF);
    //assert(size != 0);

    ELF_HEADER header;
    file_lseek(fd, 0, SEEK_SET);
    int header_size = file_read(fd, (void*)(&header), sizeof(ELF_HEADER));

    if (header_size != sizeof(ELF_HEADER)) {
        printk("Error while reading executable.\n");
        return -1;
    }

    // magic number correct
    assert(*(uint32_t*)(header.e_ident) == 0x464C457F);

    // header size correct
    assert(sizeof(ELF_HEADER) == 52);
    assert(header_size == sizeof(ELF_HEADER));

    // get section table
    int sh_offset = header.secthead_offset;
    int sh_num = header.sh_num;

    assert(header.sh_entry_size == sizeof(SECT_HEADER));

    SECT_HEADER sect_headers[16];
    file_lseek(fd, sh_offset, SEEK_SET);
    int sect_headers_size = file_read(fd, (void*)(&sect_headers), sizeof(SECT_HEADER) * sh_num);

    if (sect_headers_size != sizeof(SECT_HEADER) * sh_num) {
        printk("Error while reading executable.\n");
        return -1;
    }

    int num_sections = header.sh_num;
    int section_size = header.sh_entry_size;

    SECT_HEADER *sect_names_sh = &sect_headers[header.sh_strndx];
    
    char *sect_names = os_kernel_malloc(sect_names_sh->size);

    if (sect_names == NULL) {
        printk("Error while starting executable: Memory allocation failed.\n");
        return -1;
    }

    file_lseek(fd, sect_names_sh->offset, SEEK_SET);
    int sect_names_size = file_read(fd, sect_names, sect_names_sh->size);

    if (sect_names_size != sect_names_sh->size) {
        printk("Error while reading executable.\n");
        return -1;
    }

    assert(sizeof(SECT_HEADER) == section_size);

    // go through all sections and copy/allocate memory as necessary
    PRINT_DBG("Enumerating %i sections:\n", num_sections);
    for (int i = 0; i < num_sections; i++) {
        SECT_HEADER *sh = &sect_headers[i];
        char *name = (char*)(sect_names + sh->name);

        if ((sh->flags & SHF_ALLOC) && (sh->flags & SHF_EXECINSTR)) {
            uint32_t lma = sh->offset;
            uint32_t vma = sh->addr;
            uint32_t sect_size = sh->size;
            PRINT_DBG("%p\n", sh->flags);
            PRINT_DBG("Copying code section %s to its destination ", name);
            PRINT_DBG("(LMA: %p, VMA: %p)\n", lma, vma);

            file_lseek(fd, sh->offset, SEEK_SET);
            int read_size = file_read(fd, vma, sect_size);
            
            if (read_size != sect_size) {
                printk("Error while reading executable.\n");
                return -1;
            }
        } else if (sh->flags & SHF_ALLOC) {
            uint32_t lma = sh->offset;
            uint32_t vma = sh->addr;
            PRINT_DBG("Allocating space for section %s ", name);
            PRINT_DBG("(LMA: %p, VMA: %p)\n", lma, vma);

            /* TODO: alloc */
        } else {
            PRINT_DBG("Skipping section %s\n", name);
        }
    }
    
    PRINT_DBG("\n");

    PRINT_DBG("Entry point: %p\n", header.entry);
    crit_exit();

    // enter the process
    PROCESS_MAIN *entry = (PROCESS_MAIN*)(header.entry);
    entry(args);

    return 0;
}