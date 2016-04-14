// implement spawn from user space

#include <inc/string.h>
#include <inc/lib.h>


int
spawn(const char *prog, const char **argv)
{
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
	
	// Start the child environment running
	int r;
    if ((r = sys_env_set_status(envid, ENV_RUNNABLE)) < 0)
            panic("sys_env_set_status: %e", r);

    return envid;
}