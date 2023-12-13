# Custom memory pool allocator

### Created by:
- Alexandru Ilies 
- Jorn ten Kate
<br>
Students at **Saxion University of Applied Sciences**

## Warning!
This mini-library is a product of a `research assignment` and it is by no mean
something that should be used in an application in order to `optimize memory allocation`.<br><br>
**This allocator is only meant for the `list datastrucre` (STL forward_list)**

### Description
The library contains a small memory pool allocator used to request a large chunk of memory
that is then divided into smaller pieces handed out to the user on request.
This technique (if done right) is enforced in order to optimize memory allocation and
deallocation for a known number of objects.

### Allocation strategy
- Ask the kernel for a large chunk of memory using `mmap`
- Divide the memory pool in `chunks`
- On allocation keep track of the chunks that are allocated and align the memory
- Deallocate memory by marking the chunk as free (data override)