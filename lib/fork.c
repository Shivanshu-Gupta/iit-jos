// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	addr = ROUNDDOWN(addr, PGSIZE);
	unsigned pn = PGNUM(addr);
	if(!(uvpt[pn] & PTE_COW))
		panic("[%08x] user fault on non-write access va %08x ip %08x\n",
			thisenv->env_id, addr, utf->utf_eip);
	if(!(err && FEC_WR))
		panic("[%08x] user fault on access to non-COW va %08x ip %08x\n",
			thisenv->env_id, addr, utf->utf_eip);

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.
	
	// LAB 4: Your code here.
	if ((r = sys_page_alloc(0, PFTEMP, PTE_P|PTE_U|PTE_W)) < 0)
	    panic("sys_page_alloc: %e", r);
	memcpy(PFTEMP, addr, PGSIZE);
	if ((r = sys_page_map(0, PFTEMP, 0, addr, PTE_P|PTE_U|PTE_W)) < 0)
		panic("sys_page_map: %e", r);

	// panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	// panic("duppage not implemented");
	void *va = (void *)(pn * PGSIZE);
	
	pte_t pte = uvpt[pn];
	int perm = pte & PTE_SYSCALL;
	
	// // map the page onto the child
	// r = sys_page_map(curenv->env_id, va, envid, va, perm & ~(PTE_COW | PTE_W));	
	// if(r < 0) {
	// 	return -1;
	// }
	
	if((perm & PTE_W) || (perm & PTE_COW))
		perm = (perm|PTE_COW) & (~PTE_W);

	if((r = sys_page_map(thisenv->env_id, va, envid, va, perm)) < 0)
		panic("duppage: error mapping page number 0x%x to child: %e",pn, r);
	if(perm & PTE_COW) {
		if((r = sys_page_map(0, va, 0, va, perm)) < 0)
			panic("duppage: error remapping page number 0x%x to parent: %e",pn, r);
	}
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	// panic("fork not implemented");
	set_pgfault_handler(pgfault);
	envid_t envid = sys_exofork();
	if (envid < 0)
        panic("sys_exofork: %e", envid);
	if (envid == 0) {
	        // We're the child.
	        // The copied value of the global variable 'thisenv'
	        // is no longer valid (it refers to the parent!).
	        // Fix it and return 0.
	        thisenv = &envs[ENVX(sys_getenvid())];
	        return 0;
	}

	// We're the parent.
	unsigned pn = 0;
	for(; pn < PGNUM(UTOP); pn++) {
		if(pn != PGNUM(UXSTACKTOP) - 1) {
			if((uvpd[PDX(pn * PGSIZE)] & PTE_P)
				&& (uvpt[pn] & PTE_P)) {
				duppage(envid, pn);
			}
		}
	}
	// allocate page for user exception stack in child
	sys_page_alloc(envid, (void*) (UXSTACKTOP - PGSIZE), PTE_P|PTE_U|PTE_W);
	sys_env_set_pgfault_upcall(envid, thisenv->env_pgfault_upcall);
	// Start the child environment running
	int r;
    if ((r = sys_env_set_status(envid, ENV_RUNNABLE)) < 0)
            panic("sys_env_set_status: %e", r);

    return envid;

}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
