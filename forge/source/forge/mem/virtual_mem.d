module forge.mem.virtual_mem;

version (linux)
{
    public import forge.mem.linux.virtual_mem;
}
version(windows)
{
    public import forge.mem.windows.virtual_mem;
}