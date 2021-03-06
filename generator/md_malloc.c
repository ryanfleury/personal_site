// LICENSE AT END OF FILE (MIT).

////////////////////////////////
// TODO(allen): Write commentary for all of this.

#define MD_IMPL_Alloc(ctx,size) MD_MALLOC_Alloc(ctx,size)

static void*
MD_MALLOC_Alloc(void *ctx, MD_u64 size)
{
    MD_Assert(ctx == MD_MALLOC_Alloc);
    return(malloc(size));
}

#define MD_IMPL_GetCtx() MD_MALLOC_GetCtx()

static void*
MD_MALLOC_GetCtx(void)
{
    return((void *)MD_MALLOC_Alloc);
}

/*
Copyright 2021 Dion Systems LLC

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/