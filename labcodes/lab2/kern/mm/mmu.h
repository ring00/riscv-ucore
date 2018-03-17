#ifndef __KERN_MM_MMU_H__
#define __KERN_MM_MMU_H__

#ifndef __ASSEMBLER__
#include <defs.h>
#endif

// RISC-V uses 39-bit virtual address to access 56-bit physical address!
//
// Sv39 virtual address
// +-----9------+-----9------+-------9----+-------12--------+
// |   PPN[2]   |   PPN[1]   |   PPN[0]   |   page offset   |
// +------------+------------+-------9----+-----------------+
//
// Sv39 physical address
// +-------26-------+-----9------+-------9----+-------12--------+
// |     PPN[2]     |   PPN[1]   |   PPN[0]   |   page offset   |
// +----------------+------------+-------9----+-----------------+
//
// Sv39 page table entry:
// +-----10-----+-------26-------+-----9------+-----9------+-2-+-------8-------+
// |  Reserved  |     PPN[2]     |   PPN[1]   |   PPN[0]   |RSW|D|A|G|U|X|W|R|V|
// +------------+----------------+------------+------------+---+---------------+
//
// A linear address 'la' has a four-part structure as follows:
// +--------9-------+--------9-------+-------9--------+---------12----------+
// + Page Directory | Page Directory |   Page Table   | Offset within Page  |
// + Pointer Index  |      Index     |     Index      |                     |
// +----------------+----------------+----------------+---------------------+
//  \---PDPX(la)---/ \--- PDX(la) --/ \--- PTX(la) --/ \---- PGOFF(la) ----/
//  \-------------------- PPN(la) -------------------/
//
// The PDPX, PDX, PTX, PGOFF and PPN macros decompose linear addresses as shown.
// To construct a linear address la from PDPX(la), PDX(la), PTX(la), and
// PGOFF(la), use PGADDR(PDPX(la), PDX(la), PTX(la), PGOFF(la)).

// page directory pointer index
#define PDPX(la) ((((uintptr_t)(la)) >> PDPXSHIFT) & 0x1FF)

// page directory index
#define PDX(la) ((((uintptr_t)(la)) >> PDXSHIFT) & 0x1FF)

// page table index
#define PTX(la) ((((uintptr_t)(la)) >> PTXSHIFT) & 0x1FF)

// page number field of address
#define PPN(la) (((uintptr_t)(la)) >> PTXSHIFT)

// offset in page
#define PGOFF(la) (((uintptr_t)(la)) & 0xFFF)

// construct linear address from indexes and offset
#define PGADDR(p, d, t, o) ((uintptr_t)((p) << PDPXSHIFT | (d) << PDXSHIFT | (t) << PTXSHIFT | (o)))

// address in page table or page directory entry
#define PTE_ADDR(pte)   (((uintptr_t)(pte) & 0x3FFFFFFFFFFC00) << 2)
#define PDE_ADDR(pde)   PTE_ADDR(pde)
#define PDPE_ADDR(pdpe) PDE_ADDR(pdpe)

/* page directory and page table constants */
#define NPDPEENTRY      512                     // page directory pointer entries per page directory pointer table
#define NPDEENTRY       512                     // page directory entries per page directory
#define NPTEENTRY       512                     // page table entries per page table

#define PGSIZE          4096                    // bytes mapped by a page
#define PGSHIFT         12                      // log2(PGSIZE)
#define PTSIZE          (PGSIZE * NPTEENTRY)    // bytes mapped by a page directory entry
#define PTSHIFT         22                      // log2(PTSIZE)

#define PTXSHIFT        12                      // offset of PTX in a linear address
#define PDXSHIFT        21                      // offset of PDX in a linear address
#define PDPXSHIFT       30                      // offset of PDPX in a linear address
#define PTE_PPN_SHIFT   10                      // offset of PPN in a physical address

// page table entry (PTE) fields
#define PTE_V     0x001 // Valid
#define PTE_R     0x002 // Read
#define PTE_W     0x004 // Write
#define PTE_X     0x008 // Execute
#define PTE_U     0x010 // User
#define PTE_G     0x020 // Global
#define PTE_A     0x040 // Accessed
#define PTE_D     0x080 // Dirty
#define PTE_SOFT  0x300 // Reserved for Software

#define PAGE_TABLE_DIR (PTE_V)
#define READ_ONLY (PTE_R | PTE_V)
#define READ_WRITE (PTE_R | PTE_W | PTE_V)
#define EXEC_ONLY (PTE_X | PTE_V)
#define READ_EXEC (PTE_R | PTE_X | PTE_V)
#define READ_WRITE_EXEC (PTE_R | PTE_W | PTE_X | PTE_V)

#define PTE_USER (PTE_R | PTE_W | PTE_X | PTE_U | PTE_V)

#endif /* !__KERN_MM_MMU_H__ */
