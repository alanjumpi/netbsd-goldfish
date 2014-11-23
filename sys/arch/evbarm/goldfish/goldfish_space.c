/* XXX: check these includes */ 
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <uvm/uvm_extern.h>

#include <evbarm/goldfish/goldfishvar.h>

/* mostly copied from ifpga/ifpga_io.c */

bs_protos(goldfish);
bs_protos(generic);
bs_protos(generic_armv4);
bs_protos(bs_notimpl);

struct bus_space goldfish_common_bs_tag = {
	/* cookie */
	(void *)0,

	/* mapping/unmapping */
	goldfish_bs_map,
	goldfish_bs_unmap,
	goldfish_bs_subregion,

	/* allocation/deallocation */
	goldfish_bs_alloc,
	goldfish_bs_free,

	/* get kernel virtual address */
	goldfish_bs_vaddr,

	/* mmap */
	bs_notimpl_bs_mmap,

	/* barrier */
	goldfish_bs_barrier,

	/* read (single) */
	generic_bs_r_1,
	generic_armv4_bs_r_2,
	generic_bs_r_4,
	bs_notimpl_bs_r_8,

	/* read multiple */
	generic_bs_rm_1,
	generic_armv4_bs_rm_2,
	generic_bs_rm_4,
	bs_notimpl_bs_rm_8,

	/* read region */
	bs_notimpl_bs_rr_1,
	generic_armv4_bs_rr_2,
	generic_bs_rr_4,
	bs_notimpl_bs_rr_8,

	/* write (single) */
	generic_bs_w_1,
	generic_armv4_bs_w_2,
	generic_bs_w_4,
	bs_notimpl_bs_w_8,

	/* write multiple */
	generic_bs_wm_1,
	generic_armv4_bs_wm_2,
	generic_bs_wm_4,
	bs_notimpl_bs_wm_8,

	/* write region */
	bs_notimpl_bs_wr_1,
	generic_armv4_bs_wr_2,
	generic_bs_wr_4,
	bs_notimpl_bs_wr_8,

	/* set multiple */
	bs_notimpl_bs_sm_1,
	bs_notimpl_bs_sm_2,
	bs_notimpl_bs_sm_4,
	bs_notimpl_bs_sm_8,

	/* set region */
	bs_notimpl_bs_sr_1,
	generic_armv4_bs_sr_2,
	bs_notimpl_bs_sr_4,
	bs_notimpl_bs_sr_8,

	/* copy */
	bs_notimpl_bs_c_1,
	generic_armv4_bs_c_2,
	bs_notimpl_bs_c_4,
	bs_notimpl_bs_c_8,
};

int
goldfish_bs_map(void *t, bus_addr_t bpa, bus_size_t size, int cacheable, bus_space_handle_t *bshp)
{
	bus_addr_t startpa, endpa;
	vaddr_t va;
	const struct pmap_devmap *pd;

	if ((pd = pmap_devmap_find_pa(bpa, size)) != NULL) {
		/* Device was statically mapped. */
		*bshp = pd->pd_va + (bpa - pd->pd_pa);
		return 0;
	}

	/* Round the allocation to page boundries */
	startpa = trunc_page(bpa);
	endpa = round_page(bpa + size);

	/* Get some VM.  */
	va = uvm_km_alloc(kernel_map, endpa - startpa, 0,
	    UVM_KMF_VAONLY | UVM_KMF_NOWAIT);
	if (va == 0)
		return ENOMEM;

	/* Store the bus space handle */
	*bshp = va + (bpa & PGOFSET);

	/* Now map the pages */
	while (startpa < endpa) {
		/* XXX pmap_kenter_pa maps pages cacheable -- not what 
		   we want.  */
		pmap_enter(pmap_kernel(), va, startpa,
			   VM_PROT_READ | VM_PROT_WRITE, 0);
		va += PAGE_SIZE;
		startpa += PAGE_SIZE;
	}
	pmap_update(pmap_kernel());

	return 0;
}

void
goldfish_bs_unmap(void *t, bus_space_handle_t bsh, bus_size_t size)
{
	vaddr_t startva, endva;

	if (pmap_devmap_find_va(bsh, size) != NULL) {
		/* Device was statically mapped; nothing to do. */
		return;
	}

	startva = trunc_page(bsh);
	endva = round_page(bsh + size);

	pmap_remove(pmap_kernel(), startva, endva);
	pmap_update(pmap_kernel());
	uvm_km_free(kernel_map, startva, endva - startva, UVM_KMF_VAONLY);
}

int
goldfish_bs_subregion(void *t, bus_space_handle_t bsh, bus_size_t offset, bus_size_t size, bus_space_handle_t *nbshp)
{
	*nbshp = bsh + offset;
	return 0;
}

int
goldfish_bs_alloc(void *t, bus_addr_t rstart, bus_addr_t rend, bus_size_t size,
	bus_size_t alignment, bus_size_t boundary, int cacheable,
	bus_addr_t *bpap, bus_space_handle_t *bshp)
{
	panic("goldfish_alloc(): not implemented");
}

void    
goldfish_bs_free(void *t, bus_space_handle_t bsh, bus_size_t size)
{
	panic("goldfish_free(): not implemented");
}

void *
goldfish_bs_vaddr(void *t, bus_space_handle_t bsh)
{
	return (void *)bsh;
}

void
goldfish_bs_barrier(void *t, bus_space_handle_t bsh, bus_size_t offset, bus_size_t len, int flags)
{ }
