data-parser
=======

data-parser desc TBD

## Exploring

source TBD

## Dependencies
##### Platform agnostic
* **[json-cpp](https://github.com/mrtazz/json-cpp)** - Used for parsing the JSON formatted dataSF data.

## Usage
#### Downloading
To download, simply

	$ git clone https://github.com/fsxfreak/eviction-statistics-sf.git

#### Building

    $ make

#### Running

    $ ./eviction-parser [data.json] -n ["neighborhoods"...] -c [columns...]
    $ ./eviction-parser eviction-notices.json -n "Inner Richmond" "Lone Mountain/USF" "Outer Richmond" -c ILLEGAL NUISANCE > output.csv

#### Usage

TODO

## Author's comments

No comments as of yet.

## License

The MIT License (MIT)

Copyright (c) 2015 Leon Cheung

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.