#include <stdint.h>
#include "cedos/elf.h"
#include "cedos/core.h"

#include "cedos/fat.h"
#include "cedos/sched/process.h"

#include "assert.h"

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
    
    printk("Reading ELF file from location %p of length %i\n", elf_pointer, size);
    printk("\n");

    printk("ELF header size: %i\n", sizeof(ELF_HEADER));
    assert(sizeof(ELF_HEADER) == 52);
    printk("\n");

    printk("ELF header hexdump:\n");
    memdump(elf_pointer, 52);
    printk("\n");

    printk("ELF header magic number:\n");
    printk("%s\n", &(header->e_ident));
    printk("\n");

    printk("ELF header infodump:\n");
    printk("- machine:       %i\n", header->machine);
    printk("- version:       %i\n", header->version);
    printk("- entry:         %p\n", header->entry);
    printk("\n");
    printk("- program headers:\n");
    printk("  - offset:      %i\n", header->proghead_offset);
    printk("  - entries:     %i\n", header->ph_num);
    printk("  - entry size:  %i\n", header->ph_entry_size);
    printk("\n");
    printk("- section headers:\n");
    printk("  - offset:      %i\n", header->secthead_offset);
    printk("  - entries:     %i\n", header->sh_num);
    printk("  - entry size:  %i\n", header->sh_entry_size);
    printk("\n");

    printk("Enumerating sections:\n");

    int sh_offset = header->secthead_offset;
    SECT_HEADER *sect_headers = (SECT_HEADER*)(elf_pointer + sh_offset);
    
    int num_sections = header->sh_num;
    int section_size = header->sh_entry_size;

    SECT_HEADER sect_names_sh = sect_headers[header->sh_strndx];
    VIRT_ADDR sect_names_addr = elf_pointer + sect_names_sh.offset;

    assert(sizeof(SECT_HEADER) == section_size);

    for (int i = 0; i < num_sections; i++) {
        SECT_HEADER sh = sect_headers[i];
        char *name = (char*)(sect_names_addr + sh.name);

        printk("Section:       %s\n", name);
        printk("- type:        %i\n", sh.type);
        printk("- offset:      %i\n", sh.offset);
        printk("- size:        %i\n", sh.size);
        printk("- addr:        %i\n", sh.addr);
        printk("- addr_align:  %i\n", sh.addr_align);
        printk("\n");
    }
}

PROCESS_ID elf_exec(const char *fname, char *args) {
    printk("Loading ELF executable \"%s\".\n", fname);
    VIRT_ADDR elf_addr = (VIRT_ADDR*)(0xA0000000);
    // TODO: needs to change when we have other file systems
    int size = FAT_read_file(fname, elf_addr);
    assert(size != 0);

    ELF_HEADER *header = (ELF_HEADER*)(elf_addr);

    // magic number correct
    assert(((uint32_t*)(elf_addr))[0] == 0x464C457F);

    // header size correct
    assert(sizeof(ELF_HEADER) == 52);

    // get section table
    int sh_offset = header->secthead_offset;
    SECT_HEADER *sect_headers = (SECT_HEADER*)(elf_addr + sh_offset);
    
    int num_sections = header->sh_num;
    int section_size = header->sh_entry_size;

    SECT_HEADER sect_names_sh = sect_headers[header->sh_strndx];
    VIRT_ADDR sect_names_addr = elf_addr + sect_names_sh.offset;

    assert(sizeof(SECT_HEADER) == section_size);

    // go through all sections and copy/allocate memory as necessary
    printk("Enumerating %i sections:\n", num_sections);
    for (int i = 0; i < num_sections; i++) {
        SECT_HEADER sh = sect_headers[i];
        char *name = (char*)(sect_names_addr + sh.name);

        if ((sh.flags & SHF_ALLOC) && (sh.flags & SHF_EXECINSTR)) {
            VIRT_ADDR lma = elf_addr + sh.offset;
            VIRT_ADDR vma = sh.addr;
            uint32_t size = sh.size;
            printk("%p\n", sh.flags);
            printk("Copying code section %s to its destination ", name);
            printk("(LMA: %p, VMA: %p)\n", lma, vma);

            memcpy(vma, lma, size);
        } else if (sh.flags & SHF_ALLOC) {
            VIRT_ADDR lma = elf_addr + sh.offset;
            VIRT_ADDR vma = sh.addr;
            printk("Allocating space for section %s ", name);
            printk("(LMA: %p, VMA: %p)\n", lma, vma);

            /* TODO: alloc */
        } else {
            printk("Skipping section %s\n", name);
        }
    }
    
    printk("\n");

    printk("Entry point: %p\n", header->entry);

    // enter the process
    PROCESS_MAIN *entry = header->entry;
    entry(args);

    return 0;
}