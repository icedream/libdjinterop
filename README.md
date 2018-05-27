Overview
========

`libdjinterop` is a C++ library that allows access to database formats used to store information about DJ record libraries.

This library currently supports:

* Engine Library, as used on "Prime"-series DJ equipment.

State of Support
================

The library is currently in a very early alpha version, and is almost certainly
not usable for Production systems just yet.  It currently supports only the Engine Library format.

What is supported:

* Track metadata
* Beat grids
* Hot cues
* Loops
* Crates
* Engine library formats for the following database schema versions:
  * Schema 1.6.0 (used by firmware 1.0.0)
  * Schema 1.7.1 (used by firmware 1.0.3)

What is not supported (yet):

* Album art
* Waveforms (overview and performance)
* Playlists
* Play history
* Engine Library formats associated with other firmware versions
* DJ record libraries in formats other than Engine Prime

How Do I Use It?
================

The library is not ready for prime-time yet, but if you are willing to read the
source code, you can get an example application up and running using code similar
to the following:

```c++
#include <chrono>
#include <iostream>
#include <djinterop/enginelibrary.hpp>

namespace c = std::chrono;
namespace el = djinterop::enginelibrary;

int main(int argc, char **argv)
{
    auto db_dir = "Engine Library";
    auto db = el::create_database(db_dir, el::version_1_7_1);

    el::track t;
    t.set_track_number(1);
    t.set_duration(std::chrono::seconds{366});
    t.set_bpm(120);
    t.set_year(1970);
    t.set_title("Some Song");
    t.set_artist("Some Artist");
    t.set_key(el::musical_key::a_minor);
    t.set_path("../01 - Some Artist - Some Song.mp3");
    t.set_filename("01 - Some Artist - Some Song.mp3");
    t.set_file_extension("mp3");
    t.set_bitrate(320);
    t.save(db);

    el::performance_data p{t.id()};
    p.set_sample_rate(44100);
    p.set_total_samples(16140600);
    p.set_key(el::musical_key::a_minor);
    p.set_average_loudness(0.5);
    p.set_default_beat_grid(el::track_beat_grid{
            -4,
            -83316.78,
            812,
            17470734.439});
    std::vector<el::track_hot_cue_point> cues;
    cues.emplace_back(true, "Cue 1", 1377924.5, el::standard_pad_colours::pad_1);
    cues.emplace_back();
    cues.emplace_back(true, "Cue 3", 5508265.964, el::standard_pad_colours::pad_3);
    p.set_hot_cues(std::begin(cues), std::end(cues));
    p.set_adjusted_main_cue_sample_offset(2732);
    p.set_default_main_cue_sample_offset(2732);
    std::vector<el::track_loop> loops;
    loops.emplace_back(
            true, true, "Loop 1",
            1144.012, 345339.134, el::standard_pad_colours::pad_1);
    p.set_loops(std::begin(loops), std::end(loops));
    p.save(db);
    
    el::crate cr;
    cr.set_name("My Example Crate");
    cr.add_track(t.id());
    cr.save(db);

    return 0;
}
```

Dependencies
============

`libdjinterop` makes use of:

* [Boost](https://boost.org)
* [SQLite](https://sqlite.org)
* [SQLite Modern C++ Wrapper](https://github.com/SqliteModernCpp/sqlite_modern_cpp)
* [zlib](http://zlib.net)

# How Do I Compile And Use It?

Assuming you have the above dependencies in place, and the build tools, you can issue the following commands:

```
$ ./autogen.sh
$ make
$ make check               (optional, run unit tests)
# make install
```

The `autogen.sh` script is a wrapper around `configure`, so you can pass parameters into it as you desire, such as:

```
$ ./autogen.sh --disable-shared --prefix=/usr
...
```

