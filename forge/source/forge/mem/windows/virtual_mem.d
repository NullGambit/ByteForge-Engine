module forge.mem.windows.virtual_mem;

version (windows):

import core.sys.windows.windows;
        
public byte* virtualAlloc(size_t size)
{
    return VirtualAlloc(null, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}