# gdiextract

This tool aims at extracting the content of Sega GD-ROMs in the `.gdi` + `.bin` + `.raw` format.

## How to use

This tool should work on Windows, macOS and Linux.\
It uses `cmake` as its build system generator.

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Now you can run `gdiextract`:

```bash
$ ./gdiextract
input: 1 argument(s) expected. 0 provided.
Usage: gdiextract [--help] [--version] --output VAR [--debug] [--list] input

Positional arguments:
  input

Optional arguments:
  -h, --help     shows help message and exits
  -v, --version  prints version information and exits
  -o, --output   specify the output directory (to save the extracted files) [nargs=0..1] [default: "extracted"]
  -d, --debug    print more debug information
  -l, --list     tree-list GD-ROM content without extracting
```

### Tested GD-ROM dumps

This tool has been tested on:
- `Disney's Donald Duck: Quack Attack` (3 tracks GD-ROM)
- `Sega Rally 2 (USA)` (multi-tracks GD-ROM)
- `Rayman 2 - The Great Escape (Europe)` (3 tracks GD-ROM with spaces in the filenames)

## The GD-ROM format

This is complicated, so take the following information with care.

### Before you begin

Resources you have to read:
- [the GDI format on dreamcast.wiki](https://dreamcast.wiki/GDI_format)
- [the ISO 9660 file-system on wiki.osdev.org](https://wiki.osdev.org/ISO_9660)

### Important definitions

- **Low density vs High density**: Sega's GD-ROMs can be (partially) read on normal PC-grade CD readers. Of course, the game won't run, and the GD-ROM, when read in such a CD reader, will only show goodies like wallpapers, and an audio track warning the user to put the drive in a Sega console.\
So the CD-readable part of the GD-ROM is called the **low-density** area, while the Sega-only part of the disk, that contains the actual game files, is called the **high-density** area.

- **Sector**: the smallest addressable block of data on a disc.\
The "logical sector" (that is the actual data we want to store) is 2048 bytes. However, the "raw sector" (that is the space occupied by this logical sector on the actual physical disk), is 2352 bytes, structured as follows:
```
[raw 2352 bytes]
  - 16 byte header
  - 2048 bytes user data → ISO9660 content
  - remaining ECC/CRC/empty bytes
```

As a result, when we parse the GD-ROM dumps, we will see raw sectors, that are 2352 bytes long. We will however strip the 16 bytes header, and then read 2048 bytes only, to recover the accurate data only.

- **LBA** (Logical Block Address): It is the index of a sector on the disc, starting from 0.\
On Sega GD-ROM drives, sectors 0 to 44999 are the low-density sectors, while sectors >= 45000 are high-density sectors.

- **ISO9660**: it is the filesystem used to store data on CD-ROMs. The high-density area of our GD-ROMs uses this filesystem to structure the data.

### The multi-tracks structure

As mentioned on [dreamcast.wiki](https://dreamcast.wiki/GDI_format), there are 2 possibles cases for our GD-ROM dumps:
- either there will be 1 `.gdi` file + 3 data tracks
- or there will be 1 `.gdi` files + more than 3 data tracks.

#### 3 tracks case

This case is simple, as only the `track03.bin` will actually contain the game data.\
In this case, the `track03` file contains **raw sectors**, which we have to strip to find **logical sectors** as described above. Once that step is done, we end up with an **ISO9660** filesystem, which we can browse normally (to confirm...).

#### Multi tracks case

This case is more complicated:
- In multi-track GD-ROMs, `track03` contains the **ISO9660** filesystem headers, while the actual file data referenced by that filesystem may reside in the last track
- the last track still contains **raw sectors** again, but only the data

### Code organization

To keep track of this multi-case scenario, the code abstracts the parser of the GD-ROM structure (3 or more data tracks), and the parsing of the actual ISO9660 filesystem.

**Important caveat**: the sectors of the ISO9660 filesystem expect to start at 0, because from the filesystem's point of view, the filesystem headers should start at index 0, and later comes the data.\
But we explained that the GD-ROM disks also store data on low-density areas, which have a sector index (aka LBA) too! As a result, **ISO9660 logical sector 0** corresponds to `track03` sector 0 (disc LBA 45000).\
When the ISO parser requests sector `N`, we read offset `N * sector_size` from `track03`. No subtraction of 45000 is necessary, because track03.bin starts exactly at disc LBA 45000.

```
| LBA 0      \
| LBA 1       \  This is the low density area
| ...         /  storing media and audio
| LBA 44999  /
|-------------
| LBA 45000 | FS sector 0  \
| LBA 45001 | FS sector 1   \ This is the ISO9660 header, always in track03
| ...
| LBA 45XXX | FS sector XXX   // End of the ISO9660 header
|------------- 
| // Depending on the game, this may be in track03 or the last track
| LBA 45XXX+1  | FS sector XXX+1
| LBA 45XXX+2  | FS sector XXX+2
| ... 

```

**Subsequent important caveat**: if the GD-ROM contains more than 3 tracks, we explained that the headers are stored in `track03`, while the actual data is stored in the last track. As a result, when we want to access a data sector, the offset inside the track (as seen as a normal file by C++ code), we have to substract the number of sectors that were stored inside `track03` as the FS headers.