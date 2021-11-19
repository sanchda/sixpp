# sixpp
A tool for visualizing pprof-encoded data on the terminal

Basically:
```
sixpp myprofile.pprof
```
Will visualize `myprofile.pprof` on a sixel-compatible terminal.

```
sixpp myprofile.pprof <dimensions>
```
Will visualize `myprofile.pprof` on a square canvas, and
```
sixpp myprofile.pprof <height> <width>
```
will visualize `myprofile.pprof` on a rectangular canvas.

If you like animations, replace `myprofile.pprof` above with a directory where pprof files will be written and it will iteratively visualize new pprofs as they are written to the file.
